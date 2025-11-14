#include <android/log.h>
#include <android_native_app_glue.h>

#include "raylib.h"

#define HELLO "Hello Android"
#define FONT_SIZE 80

int main(void)
{
    InitWindow(0, 0, HELLO);
    SetTargetFPS(60);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(WHITE);
        int w = GetScreenWidth();
        int h = GetScreenHeight();
        int text_size = MeasureText(HELLO, FONT_SIZE);
        int posX = (w / 2) - (text_size / 2);
        DrawText(HELLO, posX, h / 2, FONT_SIZE, MAGENTA);
        EndDrawing();
    }

    CloseWindow();
    return 0;
}