#include <string.h>
#include <stdio.h>
#include <jni.h>
#include <android/log.h>
#include <dlfcn.h>
#include <signal.h>

int offset = 0;
void ** realJni;

/**
 * The dispatch function intercepts every jni call.
 * Offset is the jni table index that’s being called.
 * Intercept calls and add logging as needed.
 * Probably does not work for jni functions with more than 5 args.
 */
void * _dispatch(JNIEnv * a, void * b, void * c, void * d, void * e) {
  __android_log_print(ANDROID_LOG_DEBUG, "JniProxy", "[%d]\n", offset);
  switch(offset) {
    case 33: // GetMethodID
      __android_log_print(ANDROID_LOG_DEBUG, "JniProxy", "GetMethodID: '%s', '%s'\n", (char *) c, (char *) d);
      break;
    case 94: // GetFieldID
      __android_log_print(ANDROID_LOG_DEBUG, "JniProxy", "GetFieldId: '%s', '%s'\n", (char *) c, (char *) d);
      break;
    case 173: // GetObjectArrayElement
      __android_log_print(ANDROID_LOG_DEBUG, "JniProxy", "GetObjectArrayElement: %d\n", (unsigned int) c);
      break;
  }
  void * result = ((void *(**)(void**,void*,void*,void*,void*)) *realJni)[offset](realJni, b, c, d, e);
  switch(offset) {
    case 169: // GetStringUTFChars
      __android_log_print(ANDROID_LOG_DEBUG, "JniProxy", "GetStringUTFChars <= '%s'\n", (char *) result);
      break;
    case 24: // IsSameObject
      __android_log_print(ANDROID_LOG_DEBUG, "JniProxy", "IsSameObject <= %d\n", (unsigned int) result);
      break;
  }
  return result;
}
void * (*dispatch)() = _dispatch;

// Setup the mock jni that intercepts the calls.
void (*_jniFuncTable[256])();
void * _fakeJni = &_jniFuncTable;
JNIEnv * fakeJni = (JNIEnv*) &_fakeJni;

extern void * jmpTable;
extern void * jmpTable1;

/** Fills in the jni function table with the corresponding addresses of our jmpTable */
void initFakeJni() {
  int i;
  for(i=0; i<256; i++) {
    _jniFuncTable[i] = ((void (*)()) &jmpTable) + ((&jmpTable1 - &jmpTable) * i);
  }
}

/** Calls a given function in a shared library. */
void * callFn(char * dll, char * name, void *a, void *b, void *c, void *d, void *e) {
  void * (*fn)(void *, void *, void *, void *, void *);
  void *handle = dlopen(dll, RTLD_LOCAL);
  fn = dlsym(handle, name);
  return fn(a, b, c, d, e);
}

/**
 * Reimplement the JNI interface and forward to the original implementation,
 * replacing the JNI with our mock one.
 */
void * Java_com_example_JniProxy_init(JNIEnv* env, void *b, void *c, void *d, void *e)
{
  initFakeJni();
  realJni = (void **) env;
  return callFn("libexample.so", "Java_com_example_JniProxy_init", fakeJni, b, c, d, e);
}
