#include "raylib.h"

int main(void)
{

    const int screenWidth = 1920;
    const int screenHeight = 1080;

    InitWindow(screenWidth, screenHeight, "Inicio de Juego");
    Texture2D Intro = LoadTexture("resources/SI_3DSVC_SuperMarioBros.png");
    SetTargetFPS(60);

    while (!WindowShouldClose())
    {
        BeginDrawing();

        ClearBackground(BLACK);
        
        EndDrawing();
        DrawTexture(Intro, screenWidth / 2 - Intro.width / 2, screenHeight / 2 - Intro.height / 2, WHITE);
    }
    CloseWindow();
    return 0;
    UnloadTexture(Intro);

}