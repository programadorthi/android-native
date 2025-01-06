#!/bin/sh

# Native info
ABI=arm64-v8a
HOST_TAG=darwin-x86_64
NDK_VERSION=27.0.12077973
LIB_NAME=libnativeapplication.so
TARGET=aarch64-linux-android

# SDK location
ANDROID_SDK=${HOME}/Library/Android/sdk

# Build tools version
BUILD_TOOLS_VERSION=34.0.0
BUILD_TOOLS_DIR=${ANDROID_SDK}/build-tools/${BUILD_TOOLS_VERSION}
MIN_SDK_VERSION=24
TARGET_VERSION=34

# Dynamic NDK dir finder to use latest release
NDK_DIR=${ANDROID_SDK}/ndk/${NDK_VERSION}
PREBUILT=${NDK_DIR}/toolchains/llvm/prebuilt/${HOST_TAG}

# Build tools command line
AAPT=${BUILD_TOOLS_DIR}/aapt2
APKS=${BUILD_TOOLS_DIR}/apksigner
ZIPA=${BUILD_TOOLS_DIR}/zipalign
CLANG=${PREBUILT}/bin/clang

# Compile src source files
rm -rf build
COMPILED_SRC_DIR=build/lib/${ABI}
mkdir -p ${COMPILED_SRC_DIR}
${CLANG} \
    --target=${TARGET}${MIN_SDK_VERSION} \
    --sysroot=${PREBUILT}/sysroot \
    -I ${NDK_DIR}/sources/android/native_app_glue \
    -g \
    -DANDROID \
    -fdata-sections \
    -ffunction-sections \
    -funwind-tables \
    -fstack-protector-strong \
    -no-canonical-prefixes \
    -D_FORTIFY_SOURCE=2 \
    -Wformat \
    -Werror=format-security  \
    -fno-limit-debug-info  \
    -fPIC \
    -o ${COMPILED_SRC_DIR}/${LIB_NAME} \
    -c app/src/main/native/main.c

# Compile res files
COMPILED_RES_DIR=build/res
mkdir -p ${COMPILED_RES_DIR}
${AAPT} compile \
    --dir app/src/main/res \
    -o ${COMPILED_RES_DIR} \
    -v

# Getting all flat files compiled previously
FLAT_FILES=""
for filename in ${COMPILED_RES_DIR}/*.flat
do
    FLAT_FILES="${FLAT_FILES}$filename "
done

# Link files and genereate unsigned APK
UNALIGNED_APK=build/unligned.apk
${AAPT} link \
    -I ${ANDROID_SDK}/platforms/android-${TARGET_VERSION}/android.jar ${FLAT_FILES} \
    --manifest app/src/main/AndroidManifest.xml \
    --min-sdk-version ${MIN_SDK_VERSION} \
    --target-sdk-version ${TARGET_VERSION} \
    --version-code 1 \
    --version-name 0.0.1 \
    --debug-mode \
    -o ${UNALIGNED_APK} \
    -v

# Put lib folder to APK file
cd build && zip -D4r unligned.apk lib && cd ..

# Zip and Align the APK
${ZIPA} -f -p 4 ${UNALIGNED_APK} build/aligned.apk

# Sign the APK
${APKS} sign \
    --min-sdk-version ${MIN_SDK_VERSION} \
    --key-pass pass:android \
    --ks-pass pass:android \
    --ks ${HOME}/.android/debug.keystore \
    --out build/app.apk \
    build/aligned.apk
