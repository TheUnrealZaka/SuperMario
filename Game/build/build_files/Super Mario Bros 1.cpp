#include "raylib.h"
#include <vector>

struct Entity {
    Rectangle rect;
    Vector2 speed;
    bool isAlive = true;
};

int main()
{
    const int screenWidth = 800;
    const int screenHeight = 700;

    InitWindow(screenWidth, screenHeight, "Super Mario Bros Clone - Precise Collisions");
    SetTargetFPS(60);

    // Mario
    Entity mario = { { 250, 300, 32, 32 }, { 0, 0 }, true };
    float gravity = 0.5f;
    bool onGround = false;

    // Bloques
    std::vector<Rectangle> blocks = {
        { 200, 400, 64, 32 },
        { 300, 350, 64, 32 },
        { 400, 300, 64, 32 },
        { 0, 500, 800, 32 }
    };

    // Enemigos
    std::vector<Entity> enemies = {
        { { 500, 400, 32, 32 }, { -1.0f, 0 }, true }
    };

    while (!WindowShouldClose())
    {
        // --- Update ---

        // Movimiento jugador (input)
        if (IsKeyDown(KEY_RIGHT)) mario.speed.x = 2.0f;
        else if (IsKeyDown(KEY_LEFT)) mario.speed.x = -2.0f;
        else mario.speed.x = 0;

        if (onGround && IsKeyPressed(KEY_SPACE)) {
            mario.speed.y = -10.0f;
            onGround = false;
        }

        // Gravedad
        mario.speed.y += gravity;

        // --- Movimiento en X ---
        mario.rect.x += mario.speed.x;

        // Chequeo de colisiones horizontales
        for (Rectangle block : blocks) {
            if (CheckCollisionRecs(mario.rect, block)) {
                if (mario.speed.x > 0) { // Moviéndose a la derecha
                    mario.rect.x = block.x - mario.rect.width;
                }
                else if (mario.speed.x < 0) { // Moviéndose a la izquierda
                    mario.rect.x = block.x + block.width;
                }
                mario.speed.x = 0;
            }
        }

        // --- Movimiento en Y ---
        mario.rect.y += mario.speed.y;

        // Reiniciar onGround
        onGround = false;

        // Chequeo de colisiones verticales
        for (Rectangle block : blocks) {
            if (CheckCollisionRecs(mario.rect, block)) {
                if (mario.speed.y > 0) { // Cayendo
                    mario.rect.y = block.y - mario.rect.height;
                    mario.speed.y = 0;
                    onGround = true;
                }
                else if (mario.speed.y < 0) { // Subiendo
                    mario.rect.y = block.y + block.height;
                    mario.speed.y = 0;
                }
            }
        }

        // --- Movimiento de enemigos ---
        for (auto& enemy : enemies) {
            if (!enemy.isAlive) continue;

            enemy.rect.x += enemy.speed.x;

            for (Rectangle block : blocks) {
                if (CheckCollisionRecs(enemy.rect, block)) {
                    enemy.speed.x *= -1; // cambia dirección
                }
            }

            // Colisión Mario <-> Enemigo
            if (CheckCollisionRecs(mario.rect, enemy.rect)) {
                if (mario.speed.y > 0) { // Mario cae encima
                    enemy.isAlive = false;
                    mario.speed.y = -8.0f; // rebote
                }
                else {
                    mario.isAlive = false; // Mario pierde
                }
            }
        }

        // --- Draw ---
        BeginDrawing();
        ClearBackground(SKYBLUE);

        if (mario.isAlive)
            DrawRectangleRec(mario.rect, RED);
        else
            DrawText("Game Over!", screenWidth / 2 - 50, screenHeight / 2, 20, BLACK);

        for (Rectangle block : blocks)
            DrawRectangleRec(block, DARKGRAY);

        for (auto& enemy : enemies)
            if (enemy.isAlive)
                DrawRectangleRec(enemy.rect, BROWN);

        EndDrawing();
    }

    CloseWindow();
    return 0;
}