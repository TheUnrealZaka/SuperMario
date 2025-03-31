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
int Timer = 10;
int Score = 000000;
int Money = 00;
float elapsedTime = 0.0f;
int contmuerte = 0;

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
    constexpr static int screenHeight = 792;

    GameScreen currentScreen;
    int framesCounter;
    int framesCounter2;
    Mario player;  // Usando el nuevo jugador tipo Mario
    Enemy goomba;
    std::vector<EnvElement> envElements;
    Camera2D camera;
    Texture2D logoTexture;
    Texture2D UI;
    Texture2D Moneda;
    Texture2D Level1;
    Texture2D mario;
    Texture2D Goomba;
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
        Moneda = LoadTexture("Sprites/Items/SMBCoin.gif");
        Level1 = LoadTexture("Images/Seleccion Modo/World 1-1.png");
        mario = LoadTexture("Sprites/MARIO/Mario_RIGHT.png");
        Goomba = LoadTexture("Sprites/Enemies/Goomba.png");
        marioFont = LoadFont("Fonts/MarioFont.ttf");

        envElements = {
            {-200, -300, 10000, 10000, false, BLUE},
            {-200, 600, 10000, 200, true, BROWN},
            {300, 200, 50, 50, true, YELLOW},
            {250, 300, 50, 50, true, BROWN},
            {650, 300, 50, 50, true, BROWN}
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

            if (elapsedTime >= 3.0f) {  
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
                contmuerte = 0;

            }
            break;

        case GameScreen::GAMEPLAY:
        
            framesCounter = 0;
            // No reinicies framesCounter aquí
            UpdateGameplay();

            // Si el Timer llega a 0, cambia la pantalla a TIMEOUT
            if (Timer <= 0) {
                if (contmuerte == 0)
                {
                    player.speed = -PLAYER_JUMP_SPD * 1.2f;
                    player.canJump = false;
                    player.canJump2 = true;
                    player.jumpTime = 0.0f;
                    contmuerte++;
                }

                elapsedTime += GetFrameTime();

                // Hacer que Mario atraviese el suelo desactivando la colisión
                if (player.position.y < 700)
                {
                    player.position.y += player.speed * 0.2f * 0.5f * 0.2f;
                    player.speed += (GRAVITY * 0.5f) * 0.2f * 2.0f * 0.2f ; // Gravedad más fuerte
                }
                else {
                    player.position.y += player.speed * 0.2f * 0.5f * 0.2f;
                    player.speed += (GRAVITY * 0.5f) * 0.2f * 2.0f * 0.2f; // Gravedad más fuerte
                }

                // Cuando Mario desaparezca por la parte inferior de la pantalla, pasar a TIMEOUT
                if (elapsedTime >= 10.0f) {
                    currentScreen = GameScreen::TIMEOUT;
                    elapsedTime = 0.0f;
                }
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

        if (goomba.position.y < 600) {
            goomba.position.y += GRAVITY * 2.0f * deltaTime;

        }
        if (player.position.x - goomba.position.x <= -200)
        {
            goomba.activated = true;
        }
        
        if (goomba.activated) goomba.position.x += -150  * deltaTime;

       

        if (IsKeyPressed(KEY_R)) {
            player = Mario(400, 280);
            Timer = 300;
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
        ClearBackground(WHITE);

        switch (currentScreen) {
        case GameScreen::LOGO:
            DrawTextureEx(logoTexture, { (screenWidth - logoTexture.width - logoTexture.width + 1000) / 9.0f, (screenHeight - logoTexture.height + 700) / 10.0f }, 0.0f, 1.2f, WHITE);
            DrawText("Project 1 - GDD - CITM", 330, 420, 30, GRAY);
            DrawText("   Members", 420, 500, 25, GRAY);
            DrawText("Sauc Pellejero", 410, 550, 25, GRAY);
            DrawText(" Marc Jimenez", 408, 600, 25, GRAY);
            DrawText(" Ruben Mateo", 410, 650, 25, GRAY);
            break;

        case GameScreen::TITLE:

            DrawTextureEx(UI, { (0), (0) }, 0.0f, 1.65f, WHITE);
            break;

        case GameScreen::LEVEL1:

            DrawTextureEx(Level1, { (0), (0) }, 0.0f, 4.0f, WHITE);
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
        DrawTextureEx(Moneda, { (0), (0) }, 0.0f, 4.0f, WHITE);
        if (Score < 50) {
            DrawTextEx(marioFont, TextFormat("MARIO\n00000%d", Score), { 100, 30 }, 32, 1, WHITE);
        }
        if (Score >= 50 && Score < 100) {
            DrawTextEx(marioFont, TextFormat("MARIO\n0000%d", Score), { 100, 30 }, 32, 1, WHITE);
        }
        if (Score >= 100 && Score < 1000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n000%d", Score), { 100, 30 }, 32, 1, WHITE);
        }
        if (Score >= 1000 && Score < 10000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n00%d", Score), { 100, 30 }, 32, 1, WHITE);
        }
        if (Score >= 10000 && Score < 100000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n0%d", Score), { 100, 30 }, 32, 1, WHITE);
        }
        if (Score > 100000) {
            DrawTextEx(marioFont, TextFormat("MARIO\n%d", Score), { 100, 30 }, 32, 1, WHITE);
        }

        if (Money < 10) {

            DrawTextEx(marioFont, TextFormat("\n x0%d", Money), { 350, 30 }, 32, 1, WHITE);
        }
        if (Money >= 10 && Money < 100) {
            DrawTextEx(marioFont, TextFormat("\n x%d", Money), { 350, 30 }, 32, 1, WHITE);
        }
        if (Money == 100) {
            Money = 0;
            player.lifes++;
        }
        DrawTextEx(marioFont, TextFormat("WORLD\n 1-1 "), { 600, 30 }, 32, 1, WHITE);
        DrawTextEx(marioFont, TextFormat("TIME"), { 830, 30 }, 32, 1, WHITE);
        if (player.lifes > 0)
        {
            if (Timer >= 100) {
                DrawTextEx(marioFont, TextFormat("\n %d", Timer), { 830, 30 }, 32, 1, WHITE);
            }
            if (Timer < 100 && Timer >= 10) {
                DrawTextEx(marioFont, TextFormat("\n 0%d", Timer), { 830, 30 }, 32, 1, WHITE);
            }
            if (Timer < 10 && Timer > 0) {
                DrawTextEx(marioFont, TextFormat("\n 00%d", Timer), { 830, 30 }, 32, 1, WHITE);
            }
            if (Timer == 0) {
                DrawTextEx(marioFont, TextFormat("\n 000", Timer), { 830, 30 }, 32, 1, WHITE);
            }
        }
    }

    void DrawGameplay() {
        BeginMode2D(camera);

        for (const auto& element : envElements) {
            DrawRectangleRec(element.rect, element.color);
        }

        int frameWidth = 48; // Cada frame mide 48x48 píxeles
        int frameHeight = 48;
        Rectangle sourceRec = { 0, 0, (float)frameWidth, (float)frameHeight };

        // Determinar el estado de animación
        static float frameTime = 0.0f;
        static int currentFrame = 0;
        frameTime += GetFrameTime();
        float frameSpeed = 0.1f; // Velocidad de la animación

        if (IsKeyDown(KEY_RIGHT)) {
            if (IsKeyDown(KEY_LEFT_SHIFT)) {
                frameSpeed = 0.05f; // Aumentar la velocidad al correr
            }
            if (frameTime >= frameSpeed) {
                frameTime = 0.0f;
                currentFrame = (currentFrame + 1) % 3; // Ciclar entre los 3 frames de caminar/correr
            }
            sourceRec.x = (float)(currentFrame * frameWidth); // Cambiar el frame
        }
        else {
            currentFrame = 0; // Volver al primer frame si está quieto
            sourceRec.x = 0;
        }

        if (!player.canJump) { // Si está en el aire (saltando o cayendo)
            sourceRec.x = frameWidth * 2; // Suponiendo que el tercer frame es para el salto
        }
        
        DrawTextureRec(mario, sourceRec, { player.position.x - 20, player.position.y - 48 }, WHITE);
        DrawTextureRec(Goomba, sourceRec, { goomba.position.x - 20, goomba.position.y - 48 }, WHITE);
        
        EndMode2D();

        UItest();
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}