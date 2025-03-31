#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm> 
#include <iostream>

using namespace std;

// ENUM para los estados del juego
enum class GameScreen { LOGO, TITLE, LEVEL1, GAMEPLAY, TIMEOUT, DEATH, ENDING };

// Constantes
int GRAVITY = 500;
constexpr float PLAYER_JUMP_SPD = 350.0f;
constexpr float PLAYER_HOR_SPD = 250.0f;
constexpr float PLAYER_RUN_SPD = 250.0f;
int Timer = 300;
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

struct Enemy {
    Vector2 position;
    float speed;
    bool activated;
    bool alive;

    Enemy(float x, float y) : position{ x, y }, speed(100), activated(false) {}
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
    constexpr static int screenWidth = 1024;
    constexpr static int screenHeight = 815;

    GameScreen currentScreen;
    int framesCounter;
    int framesCounter2;
    Mario player;  // Usando el nuevo jugador tipo Mario
    Enemy goomba;
    std::vector<EnvElement> envElements;
    Camera2D camera;
    Texture2D logoTexture;
    Texture2D UI;
    Texture2D Level1;
    Texture2D mario;
    Font marioFont;
    unsigned int frameCounter;
    unsigned int playFrameCounter;
    unsigned int currentPlayFrame;

public:
    Game()
        : currentScreen(GameScreen::LOGO), framesCounter(0), framesCounter2(0), player(400, 280),
        frameCounter(0), playFrameCounter(0), currentPlayFrame(0), goomba(400, 280) {
        InitWindow(screenWidth, screenHeight, "Super Mario + Screen Manager");
        SetTargetFPS(60);
        logoTexture = LoadTexture("Images/HOME/LogoProyecto1.png");
        UI = LoadTexture("Images/Seleccion Modo/Pantalla_Intro.png");
        Level1 = LoadTexture("Images/Seleccion Modo/World 1-1.png");
        mario = LoadTexture("Images/Player/Mario.png");
        marioFont = LoadFont("Fonts/MarioFont.ttf");

        envElements = {
            {-200, 600, 10000, 200, true, GRAY},
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
        UnloadTexture(Level1);
        UnloadTexture(mario);
        UnloadFont(marioFont);

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
            framesCounter = 0;
            if (IsKeyPressed(KEY_ENTER)) {
                currentScreen = GameScreen::LEVEL1;
            }
            break;
        case GameScreen::LEVEL1:
            framesCounter++;
            if (framesCounter >= 120) {
                currentScreen = GameScreen::GAMEPLAY;
            }
            break;
        case GameScreen::TIMEOUT:
        
            if (framesCounter == 0) {  // Solo restar una vida al entrar en TIMEOUT
                player.lifes--;
                framesCounter++;
                if (player.lifes <= 0) {
                    while (framesCounter < 2999999999) {
                        framesCounter++;
                    }
                    if (framesCounter >= 2999999999) {
                        currentScreen = GameScreen::ENDING; // Fin del juego si no hay más vidas
                    }
                }
            }
            elapsedTime += GetFrameTime();

            if (elapsedTime >= 3.0f) {  // Ejemplo: 3 segundos en pantalla de TIMEOUT
                currentScreen = GameScreen::DEATH;
                elapsedTime = 0.0f;
            }
            break;

        case GameScreen::DEATH:
        
            // Esperar un tiempo o presionar tecla para reiniciar
            elapsedTime += GetFrameTime();

            if (elapsedTime >= 3.0f) {
                currentScreen = GameScreen::GAMEPLAY;
                player.position = { 400, 280 };
                camera.target = player.position;
                Timer = 10;  // Reiniciar el temporizador
                player.alive = 1;
                elapsedTime = 0.0f;  // Reiniciar tiempo de espera
            }
            break;

        case GameScreen::GAMEPLAY:
        
            framesCounter = 0;
            // No reinicies framesCounter aquí
            UpdateGameplay();

            // Si el Timer llega a 0, cambia la pantalla a TIMEOUT
            if (Timer <= 0) {
                currentScreen = GameScreen::TIMEOUT;
                elapsedTime = 0.0f;
            }
            break;


        case GameScreen::ENDING:
        
            if (IsKeyPressed(KEY_ENTER)) {
                player.lifes = 3; // Restablecer vidas
                Timer = 10; // Restablecer el temporizador
                currentScreen = GameScreen::TITLE; // Volver al menú de inicio
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

        if (goomba.position.y <= 520) {
            goomba.position.y += goomba.speed * 2 * deltaTime;

        }
        if (player.position.x - goomba.position.x <= -200)
        {
            goomba.activated = true;
        }
        
        if (goomba.activated) goomba.position.x += -100 * deltaTime;

       

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

        if (IsKeyPressed(KEY_K)) {
            player.lifes = 0;
            currentScreen = GameScreen::ENDING;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(BLUE);

        switch (currentScreen) {
        case GameScreen::LOGO:
            DrawTextureEx(logoTexture, { (screenWidth - logoTexture.width) / 4.0f, (screenHeight - logoTexture.height) / 3.0f }, 0.0f, 1.3f, WHITE);
            DrawText("Proyecte 1 - GDD - CITM", 470, 550, 25, GRAY);
            DrawText("   Members\n \nSauc Pellejero\nMarc Jimenez\nRuben Mateo", 520, 600, 25, GRAY);
            break;

        case GameScreen::TITLE:

            DrawTextureEx(UI, { (screenWidth - UI.width - UI.width) / 9.0f, (screenHeight - UI.height - UI.height + 133) / 10.0f }, 0.0f, 1.7f, WHITE);
            break;

        case GameScreen::LEVEL1:

            DrawTextureEx(Level1, { (screenWidth - Level1.width - Level1.width) / 5.0f, (screenHeight - Level1.height - Level1.height) / 5.0f }, 0.0f, 2.0f, WHITE);
            break;

        case GameScreen::GAMEPLAY:
        
            DrawGameplay();
            break;
        case GameScreen::TIMEOUT:
        
            DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
            UItest();
            DrawTextEx(marioFont, TextFormat("TIME UP"), { screenWidth / 2 - 150, screenHeight / 2 }, 30, 1, WHITE);
            break;

        case GameScreen::DEATH:
        
            DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
            UItest();
            DrawTextEx(marioFont, TextFormat(" x  %d", player.lifes), { screenWidth / 2 - 150, screenHeight / 2 }, 30, 1, WHITE);
            break;
        case GameScreen::ENDING:
        
            DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
            UItest();
            DrawTextEx(marioFont, TextFormat("GAME OVER"), { screenWidth / 2 - 150, screenHeight / 2 }, 30, 1, WHITE);
            break;
        }
        EndDrawing();
    }

    void UItest() {
        if (Score < 50) {
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
            player.lifes++;
        }
        DrawTextEx(marioFont, TextFormat("WORLD\n 1-1 "), { 700, 220 }, 30, 1, RED);
        DrawTextEx(marioFont, TextFormat("TIME"), { 1000, 220 }, 30, 1, RED);
        if (player.lifes > 0)
        {
            if (Timer >= 100) {
                DrawTextEx(marioFont, TextFormat("\n %d", Timer), { 1000, 220 }, 30, 1, RED);
            }
            if (Timer < 100 && Timer >= 10) {
                DrawTextEx(marioFont, TextFormat("\n 0%d", Timer), { 1000, 220 }, 30, 1, RED);
            }
            if (Timer < 10 && Timer > 0) {
                DrawTextEx(marioFont, TextFormat("\n 00%d", Timer), { 1000, 220 }, 30, 1, RED);
            }
            if (Timer == 0) {
                DrawTextEx(marioFont, TextFormat("\n 000", Timer), { 1000, 220 }, 30, 1, RED);
                //if (framesCounter2 <= 120)
                //{
                //    framesCounter2++;
                //}
                //if (framesCounter2 >= 120) {
                //    currentScreen = GameScreen::TIMEOUT;
                //}
            }
        }
    }

    void DrawGameplay() {
        BeginMode2D(camera);

        for (const auto& element : envElements) {
            DrawRectangleRec(element.rect, element.color);
        }

        
        DrawTextureEx(mario, { player.position.x - 20, player.position.y - 62 }, 0, 4, WHITE);

        DrawRectangle(goomba.position.x, goomba.position.y, 80, 80, BROWN);
        EndMode2D();

        
        


        UItest();
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