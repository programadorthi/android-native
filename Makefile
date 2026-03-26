#**************************************************************************************************
#
#   raylib makefile for Android project (APK building)
#
#   Copyright (c) 2017-2025 Ramon Santamaria (@raysan5)
#
#   This software is provided "as-is", without any express or implied warranty. In no event
#   will the authors be held liable for any damages arising from the use of this software.
#
#   Permission is granted to anyone to use this software for any purpose, including commercial
#   applications, and to alter it and redistribute it freely, subject to the following restrictions:
#
#     1. The origin of this software must not be misrepresented; you must not claim that you
#     wrote the original software. If you use this software in a product, an acknowledgment
#     in the product documentation would be appreciated but is not required.
#
#     2. Altered source versions must be plainly marked as such, and must not be misrepresented
#     as being the original software.
#
#     3. This notice may not be removed or altered from any source distribution.
#
#**************************************************************************************************

# Define Android architecture (armeabi-v7a, arm64-v8a, x86, x86-64) and API version
# Starting in 2019 using ARM64 is mandatory for published apps,
# Starting on August 2025, minimum required target API is Android 15 (API level 35)
ANDROID_ARCH                ?= x86_64
ANDROID_API_VERSION         ?= 35
ANDROID_BUILD_TOOLS_VERSION ?= $(ANDROID_API_VERSION).0.1

ifeq ($(ANDROID_ARCH),ARM)
    ANDROID_ARCH_NAME   = armeabi-v7a
endif
ifeq ($(ANDROID_ARCH),ARM64)
    ANDROID_ARCH_NAME   = arm64-v8a
endif
ifeq ($(ANDROID_ARCH),x86)
    ANDROID_ARCH_NAME   = x86
endif
ifeq ($(ANDROID_ARCH),x86_64)
    ANDROID_ARCH_NAME   = x86_64
endif

# Required path variables
# NOTE: JAVA_HOME must be set to JDK (using OpenJDK 25)
JAVA_HOME              ?= /usr/lib/jvm/java-25-openjdk-amd64
ANDROID_HOME           ?= $(HOME)/android_sdk
ANDROID_BUILD_TOOLS    ?= $(ANDROID_HOME)/build-tools/$(ANDROID_BUILD_TOOLS_VERSION)
ANDROID_PLATFORM_TOOLS  = $(ANDROID_HOME)/platform-tools

# Android project configuration variables
PROJECT_NAME           ?= native_application
PROJECT_LIBRARY_NAME   ?= native-application
PROJECT_BUILD_PATH     ?= build

# Android app configuration variables
APP_KEY_ALIAS          ?= android
APP_KEYSTORE           ?= $(HOME)/.android/debug.keystore
APP_KEYSTORE_PASS      ?= android

# Android APK building process... some steps required...
# NOTE: typing 'make' will invoke the default target entry called 'all',
all: clear \
     create_temp_project_dirs \
     generate_apk_keystore \
     config_project_package \
     compile_project_code \
     create_project_apk_package \
     zipalign_project_apk_package \
     sign_project_apk_package

# Clear old files and directories that needs to be removed before building
clear:
	rm -rf $(PROJECT_BUILD_PATH)/bin

# Create required temp directories for APK building
create_temp_project_dirs:
	mkdir $(PROJECT_BUILD_PATH) 
	mkdir $(PROJECT_BUILD_PATH)/src
	mkdir $(PROJECT_BUILD_PATH)/bin

# Generate storekey for APK signing: $(APP_KEYSTORE)
# NOTE: Configure here your Distinguished Names (-dname) if required!
generate_apk_keystore: 
ifeq (,$(wildcard $(APP_KEYSTORE)))
	$(JAVA_HOME)/bin/keytool -genkeypair -validity 10000 -dname "CN=Thiago Santos,O=programadorthi,C=BR" -keystore $(APP_KEYSTORE) -storepass $(APP_KEYSTORE_PASS) -keypass $(APP_KEYSTORE_PASS) -alias $(APP_KEY_ALIAS) -keyalg RSA
endif
	
# Config project package and resource using AndroidManifest.xml and res/values/strings.xml
# NOTE: Generates resources file: src/package_name/R.java
config_project_package:
	$(ANDROID_BUILD_TOOLS)/aapt package -f -m -S res -J $(PROJECT_BUILD_PATH)/src -M AndroidManifest.xml -I $(ANDROID_HOME)/platforms/android-$(ANDROID_API_VERSION)/android.jar

compile_project_code:
	ndk-build NDK_DEBUG=1 && mv libs $(PROJECT_BUILD_PATH)/lib
	
# Create Android APK package: bin/$(PROJECT_NAME).unaligned.apk
# NOTE: There is no compiled classes.dex. Only lib$(PROJECT_LIBRARY_NAME).so
# NOTE: Use -A resources to define additional directory in which to find raw asset files
create_project_apk_package:
	$(ANDROID_BUILD_TOOLS)/aapt package -f -M AndroidManifest.xml -S res -I $(ANDROID_HOME)/platforms/android-$(ANDROID_API_VERSION)/android.jar -F $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).unaligned.apk $(PROJECT_BUILD_PATH)/bin
	cd $(PROJECT_BUILD_PATH) && $(ANDROID_BUILD_TOOLS)/aapt add bin/$(PROJECT_NAME).unaligned.apk lib/$(ANDROID_ARCH_NAME)/lib$(PROJECT_LIBRARY_NAME).so $(PROJECT_SHARED_LIBS)

# Create zip-aligned APK package: bin/$(PROJECT_NAME).aligned.apk 
zipalign_project_apk_package:
	$(ANDROID_BUILD_TOOLS)/zipalign -p -f 4 $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).unaligned.apk $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).aligned.apk

# Create signed APK package using generated Key: $(PROJECT_NAME).apk 
sign_project_apk_package:
	$(ANDROID_BUILD_TOOLS)/apksigner sign --ks $(APP_KEYSTORE) --ks-pass pass:$(APP_KEYSTORE_PASS) --key-pass pass:$(APP_KEYSTORE_PASS) --out $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).apk --ks-key-alias $(APP_KEY_ALIAS) $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).aligned.apk

# Install $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).apk to default emulator/device
# NOTE: Use -e (emulator) or -d (device) parameters if required
install:
	$(ANDROID_PLATFORM_TOOLS)/adb install $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).apk

# Check supported ABI for the device (armeabi-v7a, arm64-v8a, x86, x86_64)
check_device_abi:
	$(ANDROID_PLATFORM_TOOLS)/adb shell getprop ro.product.cpu.abi

# Monitorize output log coming from device, only raylib tag
logcat:
	$(ANDROID_PLATFORM_TOOLS)/adb logcat -c
	$(ANDROID_PLATFORM_TOOLS)/adb logcat dev.programadorthi:V
	#$(ANDROID_PLATFORM_TOOLS)/adb logcat *:V

# Install and monitorize $(PROJECT_BUILD_PATH)/bin/$(PROJECT_NAME).apk to default emulator/device
deploy: install logcat

# Clean everything
clean:
	ndk-build clean
	rm -rf $(PROJECT_BUILD_PATH)
	rm -rf obj/
	@echo Cleaning done

