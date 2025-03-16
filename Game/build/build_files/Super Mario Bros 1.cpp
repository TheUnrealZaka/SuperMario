#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm> 
#include <iostream>

using namespace std;

// ENUM para los estados del juego
enum class GameScreen { LOGO, TITLE, GAMEPLAY, ENDING };

// Constantes
constexpr int GRAVITY = 500;
constexpr float PLAYER_JUMP_SPD = 350.0f;
constexpr float PLAYER_HOR_SPD = 250.0f;

// Estructuras de juego
struct Mario {
    Vector2 position;
    float speed;
    bool canJump;

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
    constexpr static int screenHeight = 720;

    GameScreen currentScreen;
    int framesCounter;
    Mario player;  // Usando el nuevo jugador tipo Mario
    std::vector<EnvElement> envElements;
    Camera2D camera;
    Texture2D logoTexture;
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

        if (IsKeyDown(KEY_LEFT)) player.position.x -= PLAYER_HOR_SPD * deltaTime;
        if (IsKeyDown(KEY_RIGHT)) player.position.x += PLAYER_HOR_SPD * deltaTime;
        if (IsKeyDown(KEY_SPACE) && player.canJump) {
            player.speed = -PLAYER_JUMP_SPD;
            player.canJump = false;
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
            player.speed += GRAVITY * deltaTime;
            player.canJump = false;
        }
        else {
            player.canJump = true;
        }

        if (IsKeyPressed(KEY_R)) {
            player = Mario(400, 280);
            camera.target = player.position;
            camera.zoom = 1.0f;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(WHITE);

        switch (currentScreen) {
        case GameScreen::LOGO:
            DrawTextureEx(logoTexture, { (screenWidth - logoTexture.width) / 4.0f, (screenHeight - logoTexture.height) / 3.0f }, 0.0f, 1.3f, WHITE);
            DrawText("LOADING...", 520, 500, 50, GRAY);
            break;

        case GameScreen::TITLE:
            DrawRectangle(0, 0, screenWidth, screenHeight, GREEN);
            DrawText("TITLE SCREEN", 20, 20, 40, DARKGREEN);
            DrawText("PRESS ENTER or TAP to START", 120, 220, 20, DARKGREEN);
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
        EndMode2D();

        DrawText("Controls:", 20, 20, 10, BLACK);
        DrawText("- LEFT | RIGHT: Move", 30, 40, 10, DARKGRAY);
        DrawText("- SPACE: Jump", 30, 60, 10, DARKGRAY);
        DrawText("- R: Reset", 30, 80, 10, DARKGRAY);
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}