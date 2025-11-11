# For more information about using CMake with Android Studio, read the
# documentation: https://d.android.com/studio/projects/add-native-code.html.
# For more examples on how to use CMake, see https://github.com/android/ndk-samples.

# Sets the minimum CMake version required for this project.
cmake_minimum_required(VERSION 3.22.1)

# Declares the project name. The project name can be accessed via ${ PROJECT_NAME},
# Since this is the top level CMakeLists.txt, the project name is also accessible
# with ${CMAKE_PROJECT_NAME} (both CMake variables are in-sync within the top level
# build script scope).
project("nativeapplication")

# Set C standard to C99
set(CMAKE_C_STANDARD 99)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS OFF)
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# Include raylib as a subdirectories
add_subdirectory(${CMAKE_SOURCE_DIR}/raylib)

# Fetch all source files for your project (recursively), excluding 'raylib' source files
file(GLOB_RECURSE SOURCES "${CMAKE_SOURCE_DIR}/*.c")
list(FILTER SOURCES EXCLUDE REGEX "${CMAKE_SOURCE_DIR}/raylib/.*")

# Add headers directory for android_native_app_glue.c
include_directories(${ANDROID_NDK}/sources/android/native_app_glue/)

# Add android_native_app_glue.c to the source files
list(APPEND SOURCES ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# Create a shared library with game source files
add_library(${CMAKE_PROJECT_NAME} SHARED ${SOURCES})

# Define compiler macros for the library
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE PLATFORM_ANDROID)

# Include raylib header files
target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE "${CMAKE_SOURCE_DIR}/raylib")

# add lib dependencies
target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        raylib)
