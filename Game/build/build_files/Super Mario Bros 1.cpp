#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <algorithm> 
#include <iostream>
using namespace std;

/*--------------------------------------------------------------------------*/
/*                               GAMESTATES                                 */
/*--------------------------------------------------------------------------*/

enum class GameScreen { LOGO, TITLE, LEVEL1, GAMEPLAY, TIMEOUT, DEATH, ENDING };

/*--------------------------------------------------------------------------*/
/*                            SPRITES AND SOUND                             */
/*--------------------------------------------------------------------------*/



/*--------------------------------------------------------------------------*/
/*                           CONST AND VARIABLES                            */
/*--------------------------------------------------------------------------*/

int GRAVITY = 500;
constexpr float PLAYER_JUMP_SPD = 350.0f;
constexpr float PLAYER_HOR_SPD = 250.0f;
constexpr float PLAYER_RUN_SPD = 250.0f;

int Timer;
int Score = 000000;
int Money = 00;
float elapsedTime = 0.0f;
int contmuerte = 0;

/*--------------------------------------------------------------------------*/
/*                            CLASS DEFINITION                              */
/*--------------------------------------------------------------------------*/

//class Mario { //Yo lo dejaba en structs y a chuparla
//public:
//    Mario(){}
//    
//    Mario(float x, float y) : position{ x, y }, speed(0), canJump(false) {}
//
//    Vector2 position;
//    float speed;
//    bool canJump;
//    bool canJump2;
//    float jumpTime;
//    int alive = 1; //If alive = 0 --> Mario is death
//    int lifes = 3;
//
//    ~Mario() {}
//};

struct Mario {
    Vector2 position;
    float speed;
    bool canJump;
    bool canJump2;
    float jumpTime;
    int alive = 1; //If alive = 0 --> Mario is death
    int lifes = 3;

    Mario(float x, float y) : position{ x, y }, speed(0), canJump(false) {}
};

struct Enemy {
    Vector2 position;
    float speed;
    bool activated;
    bool alive;
    bool death;

    Enemy(float x, float y) : position{ x, y }, speed(0), activated(false), death(false) {}
};

//Structure for objects in the environment
struct EnvElement {
    Rectangle rect;
    bool blocking;
    Color color;

    EnvElement(float x, float y, float width, float height, bool block, Color col)
        : rect{ x, y, width, height }, blocking(block), color(col) {
    } //Esto da los datos pa los bloques del juego
};

struct Flag {
    Vector2 position;
    bool reached;

    Flag(float x, float y) : position{ x, y }, reached(false) {}
};

/*--------------------------------------------------------------------------*/
/*                             MAIN CLASS GAME                              */
/*--------------------------------------------------------------------------*/
class Game {
private:
    //Screen
    constexpr static int screenWidth = 1024;
    constexpr static int screenHeight = 792;

    //Game
    GameScreen currentScreen;
    int framesCounter;
    std::vector<EnvElement> envElements;
    Camera2D camera;
    unsigned int frameCounter;
    unsigned int playFrameCounter;
    unsigned int currentPlayFrame;

    //Player
    Mario player;
    Texture2D mario;

    //Enemies
    Enemy goomba;
    Texture2D Goomba;
    Enemy koopa;

    //Interactive Structures
    Flag flag;
    Texture2D flagTexture;
    /*Falta la tuveria aqui*/

    //Map
    Texture2D castle;

    //Objects
    Texture2D money;

    //Other Textures
    Texture2D logoTexture;
    Texture2D UI;
    Texture2D Level1;
    Texture2D icon_lifes;
    Texture2D icon_money;

    //Typography
    Font marioFont;

public:
    //Initialise the game
    Game() : currentScreen(GameScreen::LOGO), framesCounter(0), player(400, 550), frameCounter(0),
        playFrameCounter(0), currentPlayFrame(0), goomba(700, 280), koopa(700, 330), flag(900, 264) {
        InitWindow(screenWidth, screenHeight, "Super Mario + Screen Manager");
        SetTargetFPS(60);

        //Initialising textures and typography
        logoTexture = LoadTexture("Images/HOME/LogoProyecto1.png");
        UI = LoadTexture("Images/Seleccion Modo/Pantalla_Intro.png");
        money = LoadTexture("Sprites/Items/Monedas.png");
        Level1 = LoadTexture("Images/Seleccion Modo/World 1-1.png");
        mario = LoadTexture("Sprites/MARIO/Mario_RIGHT.png");
        Goomba = LoadTexture("Sprites/Enemies/Goomba.png");
        flagTexture = LoadTexture("Sprites/Tileset/Flag.png");
        castle = LoadTexture("Sprites/Tileset/Castle.png");
        icon_lifes = LoadTexture("Images/Player/Icon_Lifes.png");
        icon_money = LoadTexture("Images/Player/Icon_Money.png");
        marioFont = LoadFont("Fonts/MarioFont.ttf");

        //Blocks
        envElements = {
            {-200, -300, 10000, 10000, false, BLUE},
            {-200, 600, 10000, 200, true, BROWN},
            {300, 200, 50, 50, true, YELLOW},
            {250, 300, 50, 50, true, BROWN},
            {650, 500, 50, 50, true, BROWN},
            { 895, 550, 60, 50, true, YELLOW}
        };

        //Camera of the game
        camera.target = player.position;
        camera.offset = { screenWidth / 2.0f, screenHeight / 2.0f };
        camera.rotation = 0.0f;
        camera.zoom = 1.0f;
    }

    ~Game() {
        //Finalize textures and typography
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

        //All Screens of the Game
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
                player.position = { 400, 550 };
                camera.target.x = player.position.x;
                camera.target.y = 280;
                goomba.position = { 700, 280 };
                Timer = 20;
                Score = 000000;
                Money = 00;
                player.alive = 1;
                elapsedTime = 0.0f;
                contmuerte = 0;
            }
            break;

        case GameScreen::TIMEOUT:
            if (framesCounter == 0) {
                player.lifes--;
                framesCounter++;
                if (player.lifes <= 0) {
                    while (framesCounter < 2999999999) {
                        framesCounter++;
                    }
                    if (framesCounter >= 2999999999) {
                        currentScreen = GameScreen::ENDING;
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
            elapsedTime += GetFrameTime();

            if (elapsedTime >= 3.0f) {
                currentScreen = GameScreen::GAMEPLAY;
                player.position = { 400, 550 };
                camera.target.x = player.position.x;
                camera.target.y = 280;
                goomba.position = { 700, 280 };
                Timer = 20;
                player.alive = 1;
                elapsedTime = 0.0f;
                contmuerte = 0;
            }
            break;

        case GameScreen::GAMEPLAY:
            framesCounter = 0;
            UpdateGameplay();

            if (player.alive == 0) {
                if (contmuerte == 0)
                {
                    player.speed = -PLAYER_JUMP_SPD * 1.2f;
                    player.canJump = false;
                    player.canJump2 = true;
                    player.jumpTime = 0.0f;
                    contmuerte++;
                }
                if (framesCounter == 0) {
                    player.lifes--;
                    framesCounter++;
                    if (player.lifes <= 0) {
                        while (framesCounter < 2999999999) {
                            framesCounter++;
                        }
                        if (framesCounter >= 2999999999) {
                            currentScreen = GameScreen::ENDING;
                        }
                    }
                }
                elapsedTime += GetFrameTime();
                if (elapsedTime >= 10.0f) {
                    currentScreen = GameScreen::DEATH;
                    elapsedTime = 0.0f;
                }
            }

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
                if (elapsedTime >= 10.0f) {
                    currentScreen = GameScreen::TIMEOUT;
                    elapsedTime = 0.0f;
                }
            }
            break;

        case GameScreen::ENDING:

            if (IsKeyPressed(KEY_ENTER)) {
                player.lifes = 3;
                flag.reached = false;
                currentScreen = GameScreen::TITLE;
            }
            break;
        }
    }

    void UpdateGameplay() {

        mario = LoadTexture("Sprites/MARIO/Mario_RIGHT.png");
        float deltaTime = GetFrameTime();
        elapsedTime += deltaTime * 2.5;

        //In-game controls and conditions
        if (IsKeyDown(KEY_RIGHT) && !flag.reached && Timer > 0 && player.alive != 0)
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) && !flag.reached && Timer > 0)
            {
                player.position.x += PLAYER_RUN_SPD * deltaTime;
            }
            player.position.x += PLAYER_HOR_SPD * deltaTime;
        }

        if (IsKeyDown(KEY_LEFT) && player.position.x > camera.target.x - screenWidth / 2.0f && !flag.reached && Timer > 0 && player.alive != 0)
        {
            if (IsKeyDown(KEY_LEFT_SHIFT) && player.position.x > camera.target.x - screenWidth / 2.0f && !flag.reached && Timer > 0)
            {
                player.position.x -= PLAYER_RUN_SPD * deltaTime;
            }
            player.position.x -= PLAYER_HOR_SPD * deltaTime;
        }
        if (player.position.x > camera.target.x && camera.target.x < 1320)
        {
            camera.target.x = player.position.x;
        }

        static constexpr float MAX_JUMP_TIME = 0.3f;
        static constexpr float JUMP_HOLD_FORCE = 500.0f;

        if (IsKeyPressed(KEY_SPACE) && player.canJump && !flag.reached && Timer > 0 && player.alive != 0) {
            player.speed = -PLAYER_JUMP_SPD;
            player.canJump = false;
            player.canJump2 = true; 
            player.jumpTime = 0.0f;
        }

        if (IsKeyDown(KEY_SPACE) && player.canJump2 && player.jumpTime < MAX_JUMP_TIME && !flag.reached && Timer > 0 && player.alive != 0) {
            player.speed -= JUMP_HOLD_FORCE * deltaTime;
            player.jumpTime += deltaTime;
        }

        if (IsKeyReleased(KEY_SPACE) && !flag.reached && Timer > 0 && player.alive != 0) {
            player.canJump2 = false;  //Cut the jump when releasing the key
            player.speed += JUMP_HOLD_FORCE - 300;
        }

        if (Timer <= 0 || player.alive == 0) {
            bool hitObstacle = false;
        }

        bool hitObstacle = false;
        for (auto& element : envElements) {
            if (element.blocking && Timer > 0 && player.alive != 0 &&
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
                player.speed += GRAVITY * 3.0f * deltaTime; // Increase gravity in fall
            }
            else
            {
                player.speed += GRAVITY * deltaTime; // Normal upward gravity
            }
            player.canJump = false;
            player.canJump2 = true;
        }
        else {
            player.canJump = true;
        }

        if (IsKeyPressed(KEY_SPACE) && player.canJump && player.alive != 0) {
            player.speed = -PLAYER_JUMP_SPD;
            player.canJump = false;
        }

        if (goomba.position.y < 600 && goomba.death == false) {
            goomba.position.y += GRAVITY * 2.0f * deltaTime;
        }

        if (goomba.death == true) {
            goomba.position.y += 100 * GetFrameTime(); //The goomba fall
        }

        if (player.position.x - goomba.position.x <= -200 && goomba.death == false && player.alive != 0) {
            goomba.activated = true;
        }

        if (goomba.activated && goomba.death == false && player.alive != 0) {
            goomba.position.x += -150 * deltaTime;
            if (player.position.y == goomba.position.y) {
                goomba.death = true;
            }
        }
        if (!flag.reached && player.position.x >= flag.position.x - 20) { //Flag collision
            flag.reached = true;
            player.position.x = flag.position.x;
            player.speed = 0;
        }

        if (flag.reached) {
            if (!hitObstacle && player.position.y != 550) {
                player.position.y += 1 * 0.01;
            }
            else if (hitObstacle) {
                if (player.position.y >= flag.position.y + 50) {
                    float playerMovementSpeed = 120.0f * GetFrameTime();
                    player.position.x += playerMovementSpeed; 
                }
                if (player.position.x >= flag.position.x + 800) {
                    currentScreen = GameScreen::ENDING;
                }
            }
        }

        if (IsKeyPressed(KEY_R)) {
            player.position = { 400, 550 };
            camera.target.x = player.position.x;
            camera.target.y = 280;
            goomba.position = { 700, 280 };
            Timer = 20;
            Money = 00;
            Score = 000000;
            flag.reached = false;
            player.alive = 1;
            elapsedTime = 0.0f;
            contmuerte = 0;
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

        if (elapsedTime >= 1.0f && Timer > 0 && player.alive == 1 && !flag.reached) {
            Timer--;
            elapsedTime = 0.0f;
        }

        if (IsKeyPressed(KEY_K)) {
            player.lifes = 0;
            currentScreen = GameScreen::ENDING;
        }
    }

    void Draw() {
        BeginDrawing();
        ClearBackground(WHITE);

        int frameWidth = 16; //Each frame measures 16x16 pixels
        int frameHeight = 16;
        Rectangle sourceRec = { 0, 0, (float)frameWidth, (float)frameHeight };

        static float frameTime = 0.0f;
        static int currentFrame = 0;
        float frameSpeed = 1.0f; //Velocity of animation

        //Draw all the Screens
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
            DrawTextEx(marioFont, TextFormat("TIME UP"), { screenWidth / 2 - 110, screenHeight / 2 }, 30, 1, WHITE);
            DrawTextureEx(icon_money, { 343, 50 }, 0, 1, WHITE);
            break;

        case GameScreen::DEATH:

            DrawRectangle(0, 0, screenWidth, screenHeight, BLACK);
            UItest();
            DrawTextEx(marioFont, TextFormat("World 1-1"), { screenWidth / 2 - 150, screenHeight / 2 - 100 }, 35, 1, WHITE);
            DrawTextureEx(icon_lifes, { screenWidth / 2 - 120, screenHeight / 2 - 30 }, 0.0f, 1.5f, WHITE);
            DrawTextEx(marioFont, TextFormat(" x  %d", player.lifes), { screenWidth / 2 - 40, screenHeight / 2 }, 30, 1, WHITE);
            DrawTextureEx(icon_money, { 343, 50 }, 0, 1, WHITE);
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

            DrawTextEx(marioFont, TextFormat("\n x", Money), { 360, 30 }, 32, 1, WHITE);
            DrawTextEx(marioFont, TextFormat("\n  0%d", Money), { 365, 30 }, 32, 1, WHITE);
        }
        if (Money >= 10 && Money < 100) {
            DrawTextEx(marioFont, TextFormat("\n x", Money), { 360, 30 }, 32, 1, WHITE);
            DrawTextEx(marioFont, TextFormat("\n  %d", Money), { 365, 30 }, 32, 1, WHITE);
        }
        if (Money == 100) {
            Money = 0;
            player.lifes++;
        }
        DrawTextEx(marioFont, TextFormat("WORLD\n 1-1 "), { 580, 30 }, 32, 1, WHITE);
        DrawTextEx(marioFont, TextFormat("TIME"), { 800, 30 }, 32, 1, WHITE);
        if (player.lifes > 0)
        {
            if (Timer >= 100) {
                DrawTextEx(marioFont, TextFormat("\n %d", Timer), { 800, 30 }, 32, 1, WHITE);
            }
            if (Timer < 100 && Timer >= 10) {
                DrawTextEx(marioFont, TextFormat("\n 0%d", Timer), { 800, 30 }, 32, 1, WHITE);
            }
            if (Timer < 10 && Timer > 0) {
                DrawTextEx(marioFont, TextFormat("\n 00%d", Timer), { 800, 30 }, 32, 1, WHITE);
            }
            if (Timer == 0) {
                DrawTextEx(marioFont, TextFormat("\n 000", Timer), { 800, 30 }, 32, 1, WHITE);
            }
        }
    }

    void DrawGameplay() {
        BeginMode2D(camera);

        for (const auto& element : envElements) {
            DrawRectangleRec(element.rect, element.color);
        }

        int frameWidth = 16; //Each frame mesure 16x16 pixels
        int frameHeight = 16;

        Rectangle sourceRec = { 0, 0, (float)frameWidth, (float)frameHeight };
        Rectangle sourceRec2 = { 0, 0, (float)frameWidth, (float)frameHeight };

        static float frameTime = 0.0f;
        static int currentFrame = 0;
        frameTime += GetFrameTime();
        float frameSpeed = 0.1f; //Velocity animation

        //Animation of Mario
        if (IsKeyDown(KEY_RIGHT) && Timer > 0 && player.alive != 0 && !flag.reached || flag.reached && camera.target.x < 1320 && player.position.y == 600 || player.position.y == 550) {
            if (IsKeyDown(KEY_LEFT_SHIFT) && !flag.reached) {
                frameSpeed = 0.05f; //Increases running speed
            }
            if (frameTime >= frameSpeed) {
                frameTime = 0.0f;
                currentFrame = (currentFrame + 1) % 3; //Cycling between the 3 walk/run frames
            }
            sourceRec.x = (float)(currentFrame * frameWidth); //Change frame
        }

        //Animation of Enemies
        if (goomba.activated && player.alive != 0) {
            if (frameTime >= frameSpeed) {
                frameTime = 0.0f;
                currentFrame = (currentFrame + 1) % 3;
            }
            sourceRec2.x = (float)(currentFrame * frameWidth);
        }

        else {
            currentFrame = 0; //Return to the first frame if still
            sourceRec.x = 0;
        }

        if (!player.canJump && !flag.reached) { 
            sourceRec.x = frameWidth * 5;
        }

        if (Timer <= 0 || player.alive == 0) {
            sourceRec.x = frameWidth * 6;
        }

        //Draw all entities, structures and objetcs
        DrawTexturePro(Goomba, sourceRec2, { goomba.position.x - 20, goomba.position.y - 48, sourceRec2.width * 3, sourceRec2.height * 3 }, { 0, 0 }, 0, WHITE);
        DrawTextureEx(flagTexture, { flag.position.x, flag.position.y - flagTexture.height }, 0, 3, WHITE);
        DrawTextureEx(castle, { (1200), (360) }, 0.0f, 3, WHITE);
        DrawTexturePro(mario, sourceRec, { player.position.x - 20, player.position.y - 48, sourceRec.width * 3, sourceRec.height * 3 }, { 0, 0 }, 0, WHITE);
       
        if (player.position.x >= 1320) { //Mario arrived to the flag
            camera.target.x = 1320;
            DrawTextureEx(castle, { (1200), (360) }, 0.0f, 3, WHITE);
            UnloadTexture(mario);
        }

        EndMode2D();
        UItest();
    }
};

int main() {
    Game game;
    game.Run();
    return 0;
}