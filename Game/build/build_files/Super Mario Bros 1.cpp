#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm> 
#include <iostream>

using namespace std;

// ENUM para los estados del juego
enum class GameScreen { LOGO, TITLE, GAMEPLAY, ENDING };

// Constantes
int GRAVITY = 500;
constexpr float PLAYER_JUMP_SPD = 350.0f;
constexpr float PLAYER_HOR_SPD = 250.0f;
constexpr float PLAYER_RUN_SPD = 250.0f;
int Timer = 400;
int Score = 000000;
int Money = 00;
float elapsedTime = 0.0f;

// Estructuras de juego
struct Mario {
    Vector2 position;
    float speed;
    bool canJump;
    bool canJump2;
    float jumpTime;
    int alive = 1; // If alive = 0 --> Mario is death
    int lifes = 3;

    Mario(float x, float y) : position{ x, y }, speed(0), canJump(false) {}
};

// Estructura para los objetos del entorno
struct EnvElement {
    Rectangle rect;
    bool blocking;
    Color color;

    EnvElement(float x, float y, float width, float height, bool block, Color col)
        : rect{ x, y, width, height }, blocking(block), color(col) {}
};

// Clase Principal del Juego
class Game {
private:
    constexpr static int screenWidth = 1280;
    constexpr static int screenHeight = 950;

    GameScreen currentScreen;
    int framesCounter;
    Mario player;  // Usando el nuevo jugador tipo Mario
    std::vector<EnvElement> envElements;
    Camera2D camera;
    Texture2D logoTexture;
    Texture2D UI;
    Texture2D mario;
    Font marioFont;
    unsigned int frameCounter;
    unsigned int playFrameCounter;
    unsigned int currentPlayFrame;

public:
    Game()
        : currentScreen(GameScreen::LOGO), framesCounter(0), player(400, 280),
        frameCounter(0), playFrameCounter(0), currentPlayFrame(0) {
        InitWindow(screenWidth, screenHeight, "Super Mario + Screen Manager");
        SetTargetFPS(60);
        logoTexture = LoadTexture("Images/HOME/LogoProyecto1.png");
        UI = LoadTexture("Images/Seleccion Modo/Pantalla_Intro.png");
        mario = LoadTexture("Images/Player/Mario.png");
        marioFont = LoadFont("Fonts/MarioFont.ttf");

        envElements = {
            {0, 0, 1000, 400, false, BLACK},
            {0, 400, 1000, 200, true, GRAY},
            {300, 200, 400, 10, true, GRAY},
            {250, 300, 100, 10, true, GRAY},
            {650, 300, 100, 10, true, GRAY}
        };

        camera.target = player.position;
        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    }

    ~Game() {
        UnloadTexture(logoTexture);
        UnloadTexture(UI);

        CloseWindow();
    }

    void Run() {
        while (!WindowShouldClose()) {
            Update();
            Draw();
        }
    }

private:
    void Update() {
        switch (currentScreen) {
        case GameScreen::LOGO:
            framesCounter++;
            if (framesCounter > 120) {
                currentScreen = GameScreen::TITLE;
            }
            break;
        case GameScreen::TITLE:
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = GameScreen::GAMEPLAY;
            }
            break;
        case GameScreen::GAMEPLAY:
            UpdateGameplay();
            break;
        case GameScreen::ENDING:
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = GameScreen::TITLE;
            }
            break;
        }
    }

    void UpdateGameplay() {
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime * 2.5;

        if (IsKeyDown(KEY_RIGHT))
        {
            if (IsKeyDown(KEY_LEFT_SHIFT))
            {
                player.position.x += PLAYER_RUN_SPD * deltaTime;
            }
            player.position.x += PLAYER_HOR_SPD * deltaTime;
        }

        if (IsKeyDown(KEY_LEFT) && player.position.x > camera.target.x - screenWidth / 2.0f)
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) && player.position.x > camera.target.x - screenWidth / 2.0f)
            {
                player.position.x -= PLAYER_RUN_SPD * deltaTime;
            }
            player.position.x -= PLAYER_HOR_SPD * deltaTime;
        }
        if (player.position.x > camera.target.x)
        {
            camera.target.x = player.position.x;
        }

        static constexpr float MAX_JUMP_TIME = 0.3f;  // Tiempo máximo que puede durar el salto
        static constexpr float JUMP_HOLD_FORCE = 500.0f;  // Fuerza extra si se mantiene presionado

        if (IsKeyPressed(KEY_SPACE) && player.canJump) {
            player.speed = -PLAYER_JUMP_SPD;
            player.canJump = false;
            player.canJump2 = true;  // Permitir extensión del salto
            player.jumpTime = 0.0f;  // Reiniciar el tiempo de salto
        }

        if (IsKeyDown(KEY_SPACE) && player.canJump2 && player.jumpTime < MAX_JUMP_TIME) {
            player.speed -= JUMP_HOLD_FORCE * deltaTime;
            player.jumpTime += deltaTime;
        }

        if (IsKeyReleased(KEY_SPACE)) {
            player.canJump2 = false;  // Cortar el salto al soltar la tecla
            player.speed += JUMP_HOLD_FORCE - 300 ;
        }

        bool hitObstacle = false;
        for (auto& element : envElements) {
            if (element.blocking &&
                element.rect.x <= player.position.x &&
                element.rect.x + element.rect.width >= player.position.x &&
                element.rect.y >= player.position.y &&
                element.rect.y <= player.position.y + player.speed * deltaTime) {
                hitObstacle = true;
                player.speed = 0.0f;
                player.position.y = element.rect.y;
            }
        }

        if (!hitObstacle) {
            player.position.y += player.speed * deltaTime;
            if (player.speed > 0) 
            {
                player.speed += GRAVITY * 3.0f * deltaTime; // Aumentar la gravedad en caída
            }
            else 
            {
                player.speed += GRAVITY * deltaTime; // Gravedad normal al subir
            }
            player.canJump = false;
            player.canJump2 = true;
        }
        else {
            player.canJump = true;
        }

        if (IsKeyPressed(KEY_R)) {
            player = Mario(400, 280);
            camera.target = player.position;
            camera.zoom = 1.0f;
        }

        if (IsKeyPressed(KEY_P)) {
            player.alive = 0;
        }
        if (IsKeyPressed(KEY_L)) {
            Score += 50;
        }
        if (IsKeyPressed(KEY_I)) {
            Score += 10000;
        }
        if (IsKeyPressed(KEY_O)) {
            Money++;
        }

        if (elapsedTime >= 1.0f && Timer > 0 && player.alive == 1) {
            Timer--;
            elapsedTime = 0.0f;  // Reiniciar el contador de tiempo
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(WHITE);

        switch (currentScreen) {
        case GameScreen::LOGO:
            DrawTextureEx(logoTexture, { (screenWidth - logoTexture.width) / 4.0f, (screenHeight - logoTexture.height) / 3.0f }, 0.0f, 1.3f, WHITE);
            DrawText("Proyecte 1 - GDD - CITM", 470, 550, 25, GRAY);
            DrawText("   Members\n \nSauc Pellejero\nMarc Jimenez\nRuben Mateo", 520, 600, 25, GRAY);
            break;

        case GameScreen::TITLE:

            DrawTextureEx(UI, { (screenWidth - UI.width - UI.width) / 10.0f, (screenHeight - UI.height - UI.height) / 10.0f }, 0.0f, 2.0f, WHITE);
            break;

        case GameScreen::GAMEPLAY:
            DrawGameplay();
            break;

        case GameScreen::ENDING:
            DrawRectangle(0, 0, screenWidth, screenHeight, BLUE);
            DrawText("ENDING SCREEN", 20, 20, 40, DARKBLUE);
            DrawText("PRESS ENTER to RETURN to TITLE SCREEN", 120, 220, 20, DARKBLUE);
            break;
        }

        EndDrawing();
    }

    void DrawGameplay() {
        BeginMode2D(camera);

        for (const auto& element : envElements) {
            DrawRectangleRec(element.rect, element.color);
        }

        DrawRectangleRec({ player.position.x - 20, player.position.y - 40, 40, 40 }, RED);
        DrawTextureEx(mario, { player.position.x - 20, player.position.y - 62 }, 0, 4, WHITE);
        EndMode2D();
        
        if (Score < 50){
            DrawTextEx(marioFont, TextFormat("MARIO\n00000%d", Score), { 200, 220 }, 30, 1, RED);
        }
        if (Score >= 50 && Score < 100) {
            DrawTextEx(marioFont, TextFormat("MARIO\n0000%d", Score), { 200, 220 }, 30, 1, RED);
        }
        if (Score >= 100 && Score < 1000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n000%d", Score), { 200, 220 }, 30, 1, RED);
        }
        if (Score >= 1000 && Score < 10000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n00%d", Score), { 200, 220 }, 30, 1, RED);
        }
        if (Score >= 10000 && Score < 100000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n0%d", Score), { 200, 220 }, 30, 1, RED);
        }
        if (Score > 100000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n%d", Score), { 200, 220 }, 30, 1, RED);
        }

        if (Money < 10) {
            DrawTextEx(marioFont, TextFormat("\n x0%d", Money), { 450, 220 }, 30, 1, RED);
        }
        if (Money >= 10 && Money < 100) {
            DrawTextEx(marioFont, TextFormat("\n x%d", Money), { 450, 220 }, 30, 1, RED);
        }
        if (Money == 100) {
            Money = 0;
        }
        DrawTextEx(marioFont, TextFormat("WORLD\n 1-1 "), { 700, 220 }, 30, 1, RED);
        DrawTextEx(marioFont, TextFormat("TIME\n %d", Timer), { 1000, 220 }, 30, 1, RED);
        DrawText("Controls:", 20, 20, 10, BLACK);
        DrawText("- LEFT | RIGHT: Move", 30, 40, 10, DARKGRAY);
        DrawText("- SPACE: Jump", 30, 60, 10, DARKGRAY);
        DrawText("- R: Reset", 30, 80, 10, DARKGRAY);
        DrawText("- SHIFT: Run", 30, 100, 10, DARKGRAY);
        DrawText("- P: Muerte", 30, 120, 10, DARKGRAY);
        DrawText("- L: Sumar 50 puntuacion", 30, 140, 10, DARKGRAY);
        DrawText("- I: Sumar 10000 puntuacion", 30, 160, 10, DARKGRAY);
        DrawText("- O: Sumar 1 moneda", 30, 180, 10, DARKGRAY);
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}