Project Homepage:[https://aenu.cc/aps3e](https://aenu.cc/aps3e)



Precompiled libraries LLVM, ffmpeg, glslang need to be compiled in Termux using [android-ndk-r27b-aarch64](https://github.com/lzhiyong/termux-ndk)

Compile LLVM 19.1.7:

cmake -S llvm/llvm -B build_android -DCMAKE_INSTALL_PREFIX=~/llvm_out -DCMAKE_TOOLCHAIN_FILE=~/android-ndk-r27b/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 -DLLVM_BUILD_RUNTIME=OFF -DLLVM_BUILD_TOOLS=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_DOCS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_TOOLS=OFF -DLLVM_INCLUDE_UTILS=OFF -DLLVM_ENABLE_BACKTRACES=OFF -DLLVM_TARGETS_TO_BUILD="AArch64" -DCMAKE_BUILD_TYPE=Release


Compile ffmpeg 6.1.2

./configure --arch=aarch64 --target-os=android --as=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/aarch64-linux-android28-clang --cc=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/aarch64-linux-android28-clang --cxx=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/aarch64-linux-android28-clang++ --nm=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-nm --ar=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-ar --strip=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-strip --ranlib=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-ranlib --prefix=$HOME/ffmpeg_out --sysroot=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/sysroot --disable-vulkan --enable-pic  --disable-asm


Compile glslang 15.2.0

cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=~/android-ndk-r27b/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=android-28 -DANDROID_ABI=arm64-v8a -DCMAKE_INSTALL_PREFIX=~/glslang_out -DGLSLANG_TESTS_DEFAULT=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_EXTERNAL=OFF -DENABLE_SPVREMAPPER=OFF -DENABLE_GLSLANG_BINARIES=OFF -DENABLE_HLSL=OFF -DENABLE_RTTI=OFF -DENABLE_EXCEPTIONS=OFF -DENABLE_PCH=OFF -DENABLE_OPT=OFF ..


Compilation Steps (Windows using Android Studio):

Rename app/build.gradle.bak to app/build.gradle  and complete the signing information.
 
Copy the compiled outputs llvm_out, ffmpeg_out,and glslang_out to app/src/main/cpp directory.

LICENSE:

Please check the LICENSE file under the appropriate file header and directory for detailed information.


--------

项目主页:[https://aenu.cc/aps3e](https://aenu.cc/aps3e)



预编译库 LLVM,ffmpeg,glslang,需要在termux中使用[android-ndk-r27b-aarch64](https://github.com/lzhiyong/termux-ndk)编译。

编译LLVM 19.1.7:

cmake -S llvm/llvm -B build_android -DCMAKE_INSTALL_PREFIX=~/llvm_out -DCMAKE_TOOLCHAIN_FILE=~/android-ndk-r27b/build/cmake/android.toolchain.cmake -DANDROID_ABI=arm64-v8a -DANDROID_PLATFORM=android-28 -DLLVM_BUILD_RUNTIME=OFF -DLLVM_BUILD_TOOLS=OFF -DLLVM_INCLUDE_BENCHMARKS=OFF -DLLVM_INCLUDE_DOCS=OFF -DLLVM_INCLUDE_EXAMPLES=OFF -DLLVM_INCLUDE_TESTS=OFF -DLLVM_INCLUDE_TOOLS=OFF -DLLVM_INCLUDE_UTILS=OFF -DLLVM_ENABLE_BACKTRACES=OFF -DLLVM_TARGETS_TO_BUILD="AArch64" -DCMAKE_BUILD_TYPE=Release


编译ffmpeg 6.1.2

./configure --arch=aarch64 --target-os=android --as=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/aarch64-linux-android28-clang --cc=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/aarch64-linux-android28-clang --cxx=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/aarch64-linux-android28-clang++ --nm=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-nm --ar=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-ar --strip=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-strip --ranlib=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/bin/llvm-ranlib --prefix=$HOME/ffmpeg_out --sysroot=$HOME/android-ndk-r27b/toolchains/llvm/prebuilt/linux-aarch64/sysroot --disable-vulkan --enable-pic  --disable-asm

编译glslang 15.2.0


cmake -DCMAKE_BUILD_TYPE=Release -DCMAKE_TOOLCHAIN_FILE=~/android-ndk-r27b/build/cmake/android.toolchain.cmake -DANDROID_PLATFORM=android-28 -DANDROID_ABI=arm64-v8a -DCMAKE_INSTALL_PREFIX=~/glslang_out -DGLSLANG_TESTS_DEFAULT=OFF -DBUILD_SHARED_LIBS=OFF -DBUILD_EXTERNAL=OFF -DENABLE_SPVREMAPPER=OFF -DENABLE_GLSLANG_BINARIES=OFF -DENABLE_HLSL=OFF -DENABLE_RTTI=OFF -DENABLE_EXCEPTIONS=OFF -DENABLE_PCH=OFF -DENABLE_OPT=OFF ..

编译步骤(Windows下，使用Android Studio)

1:
重命名 app/build.gradle.bak为app/build.gradle，并补全签名信息

2:
复制llvm_out,ffmpeg_out,glslang_out到app/src/amin/cpp目录下
