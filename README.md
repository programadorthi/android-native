# Android Native

A basic template project for people that want skip Java or Kotlin as must as possible.

There is a build by Gradle but is to have a easily integration with NDK and CMake. It takes seconds or milliseconds because don't have to download dependencies or compile Java/Kotlin code.

> Yes, I know that even a hello world Gradle project is fast. But until you finish the first sync it will take minutes 🙆‍♂️. 

But there is a script if you want to avoid Gradle. Manual script don't requires you have a folder structure as used by default Gradle Android projects.

## About UI

Until now I saw that developing UI using NDK is not an easy task. There are some C libraries to develop UI: [LVGL](https://lvgl.io/), [Raylib](https://www.raylib.com/index.html) and [clay](https://github.com/nicbarker/clay).

Raylib is more related to game development and I'll use it in the future maybe.

I'm using LVGL because is more related to a GUI framework and have support to themes, styles and a lot of widgets ready to use. You can see it working in the branch [lvgl](https://github.com/programadorthi/android-native/tree/lvgl).

Clay is a new project. It brings features that you find in composition layout like React, Vue, Compose Multiplatform, etc. You can see it working in the branch [clay-lvgl](https://github.com/programadorthi/android-native/tree/clay-lvgl).
