# (x86) JNI Proxy for sniffing library<->jni communication

Useful for reverse engineering / debugging.

## How it works

`JNIEnv` is a pointer to `JNINativeInterface_` which is essentially a table of
functions-pointers that implement the JNI functionality.

The proxy here, creates a fake `JNIEnv`. Due to being too lazy to implement
a proxy function for each JNI function, each function pointer points to a
function defined in `table.s`, that just sets the called index and defers
to `dispatch()` in `jniproxy.c`, which forwards the call to the real jni
implementation.
`dispatch()` can be used to add logging or perform almost arbitrary
other operations during the JNI calls.
It’s a bit fragile, but worked for me.

## Putting it into an APK

Say we want to monitor what `Java_com_example_JniProxy_init` in
`libjniexample.so`. First, rename the original library:

    mv libjniexample.so libexample.so

Now, we add the functions that we want to monitor in `jniproxy.c`
and forward them accordingly.

    # Build the library
    ndk-build NDK_APPLICATION_MK=./Application.mk
    # Rename the library
    mv libs/x86/jniproxy.so libexample.so

Open the APK of interest, and put both files in `lib/x86`.

    # Resign the APK
    ./signapk.sh example.apk
    # Install the updated APK
    adb install -r example.apk
