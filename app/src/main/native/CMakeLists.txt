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

# build native_app_glue as a static lib
set(${CMAKE_C_FLAGS}, "${CMAKE_C_FLAGS}")
add_library(native_app_glue STATIC
        ${ANDROID_NDK}/sources/android/native_app_glue/android_native_app_glue.c)

# now build app's shared lib
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror")

# Export ANativeActivity_onCreate(),
# Refer to: https://github.com/android-ndk/ndk/issues/381.
set(CMAKE_SHARED_LINKER_FLAGS
        "${CMAKE_SHARED_LINKER_FLAGS} -u ANativeActivity_onCreate")

# ==== LVGL ====
set(LVGL_ROOT_DIR ../../../../lvgl)

# Option to define LV_LVGL_H_INCLUDE_SIMPLE, default: ON
option(LV_LVGL_H_INCLUDE_SIMPLE
        "Use #include \"lvgl.h\" instead of #include \"../../lvgl.h\"" ON)

# Option to define LV_CONF_INCLUDE_SIMPLE, default: ON
option(LV_CONF_INCLUDE_SIMPLE
        "Use #include \"lv_conf.h\" instead of #include \"../../lv_conf.h\"" ON)

file(GLOB_RECURSE SOURCES ${LVGL_ROOT_DIR}/src/*.c ${LVGL_ROOT_DIR}/src/*.S)

# Build LVGL library
add_library(lvgl ${SOURCES})
add_library(lvgl::lvgl ALIAS lvgl)

target_compile_definitions(
        lvgl PUBLIC $<$<BOOL:${LV_LVGL_H_INCLUDE_SIMPLE}>:LV_LVGL_H_INCLUDE_SIMPLE>
        $<$<BOOL:${LV_CONF_INCLUDE_SIMPLE}>:LV_CONF_INCLUDE_SIMPLE>)

target_include_directories(lvgl PUBLIC ${LVGL_ROOT_DIR} ${CMAKE_CURRENT_SOURCE_DIR})
# ==== LVGL ====

# ==== CLAY ====
set(CLAY_ROOT_DIR ../../../../clay)

file(GLOB_RECURSE SOURCES ${CLAY_ROOT_DIR}/*.h)

add_library(clay ${SOURCES})

set_target_properties(clay PROPERTIES LINKER_LANGUAGE C)

target_include_directories(clay PUBLIC ${CLAY_ROOT_DIR})
# ==== CLAY ====

add_library(${CMAKE_PROJECT_NAME} SHARED main.c render.h render.c types.h)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
        ${ANDROID_NDK}/sources/android/native_app_glue)

# add lib dependencies
target_link_libraries(${CMAKE_PROJECT_NAME}
        android
        log
        native_app_glue
        lvgl
        clay)
