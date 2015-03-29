This is a tiny injector for extracting keys for encrypting DouyuTv URLs.

# Requirements
* A rooted Android device with libdvm.so as the default virtual machine
* A recent version of Linux with Android SDK and Android NDK installed
* [fb-adb](https://github.com/facebook/fb-adb) as a replacement of adb (Optional but recommended)

# How to use

First, clone this repo and update submodules

```Bash
git clone https://github.com/yan12125/douyu-hack
git submodule update --init --recursive
```

Then copy necessary binaries for build ddi and adbi. Connect your Android device and type the following commands:
```Bash
cd ddi/dalvikhook/jni/
mkdir libs && cd libs
adb pull /system/lib/libdl.so
adb pull /system/lib/libdvm.so
```

Then build adbi and ddi respectively:
```Bash
pushd adbi && bash ./build.sh && popd
pushd ddi && bash ./build.sh && popd
```

Finnaly, build douyu-hack:
```Bash
ndk-build
```

Now push necessary files into the device:
```Bash
adb push ./adbi/hijack/libs/armeabi/hijack /data/local/tmp/
adb push ./libs/armeabi/libdouyu-hack.so /data/local/tmp/
```

Run the Douyu App on the phone, click on any of the channels to make sure that the class to inject is already loaded

```Bash
fb-adb shell
su
touch douyu-hack.log
chmod 777 douyu-hack.log
ps | grep douyu
./hijack -d -p PID -l /data/local/tmp/libdouyu-hack.so # PID is got in the previous command
```

Repeat the action of entering a channel, and view douyu-hack.log for the results.

# LICENSE
Licensed under GPLv3. Visit http://opensource.org/licenses/gpl-3.0.html for more information
