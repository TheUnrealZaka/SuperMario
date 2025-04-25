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
/*                           CONST AND VARIABLES                            */
/*--------------------------------------------------------------------------*/

constexpr float PLAYER_JUMP_SPD = 350.0f;
constexpr float PLAYER_HOR_SPD = 250.0f;
constexpr float PLAYER_RUN_SPD = 250.0f;


const float suelo = 600;
int GRAVITY = 500;

int Timer;
int Score = 000000;
int Money = 00;
float elapsedTime = 0.0f;
int contmuerte = 0;
int conttiempo = 0;

/*--------------------------------------------------------------------------*/
/*                            STRUCTS DEFINITION                            */
/*--------------------------------------------------------------------------*/

struct Mario {
	Vector2 position;
	Rectangle mario_hitbox;
	Vector2 speed;
	const float saltoFuerza = -10.0f;

	bool canJump;
	bool canJump2;
	float jumpTime;

	int alive = 1; //If alive = 0 --> Mario is death
	int lifes = 3;
	bool big;
	bool side; //If side = 0 (Right) ; If side = 1 (Left)

	Mario(float x, float y) : position{ x, y }, speed{ 0, 0 }, canJump(false), big(false) {}
};

struct Enemy {
	Vector2 position;
	Rectangle goomba_hitbox;

	Vector2 speed;
	bool activated;
	bool alive;
	bool death;
	bool side; //If side = 0 (Right) ; If side = 1 (Left)

	Enemy(float x, float y) : position{ x, y }, activated(false), alive(true), death(false), side(true) {}
};
struct PowerUp {
	Vector2 position;
	Rectangle powerup_hitbox;

	Vector2 speed;
	bool active;
	bool side; //If side = 0 (Right) ; If side = 1 (Left)

	PowerUp(float x, float y) : position{ x, y }, active(false), side(true) {}
};
//Structure for objects in the environment
struct EnvElement {
	Rectangle rect;
	Color color;

	EnvElement(float x, float y, float width, float height, Color col)
		: rect{ x, y, width, height, }, color(col) {
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
	Texture2D Koopa;

	//PowerUp
	PowerUp mooshroom;
	Texture2D Mooshroom;

	//Blocks
	Texture2D bloque_int;
	Texture2D ladrillo;
	Texture2D escalera;


	//Interactive Structures
	Flag flag;
	Texture2D flagTexture;
	Texture2D tuberia;

	//Map
	Texture2D castle;
	Texture2D fondo;
	//Pipes
	Texture2D tuberia_s;
	Texture2D tuberia_m;
	Texture2D tuberia_b;
	//Objects
	Texture2D money;

	//Other Textures
	Texture2D logoTexture;
	Texture2D UI;
	Texture2D Level1;
	Texture2D icon_lifes;
	Texture2D icon_money;

	//Musica
	Music musicOverworld;
	Music musicOverworld_hurry;
	Music musicInvencible;
	Music musicInvencible_hurry;

	Sound sfxJumpSmall;
	Sound sfxJumpSuper;
	Sound sfxMushroom;
	Sound sfxFlagpole;
	Sound sfxGameOver;
	Sound sfxDeath;
	Sound sfxCompleted;

	//Typography
	Font marioFont;

	//Blocks
	vector<EnvElement> blocks = {

	{-200, 600, 11000, 200, GREEN}, // SUELO

	{650, 400, 50, 50, GREEN},	//PRIMER ? /MONEDAS

	//Conjuto de bloques
	{850, 400, 50, 50, GREEN},
	{900, 400, 50, 50, GREEN}, // TIENE UN CHAMPIÑON
	{950, 400, 50, 50, GREEN},
	{1000, 400, 50, 50, GREEN}, // MONEDAS
	{1050, 400, 50, 50, GREEN},
		//Bloque superior
	{950, 200, 50, 50, GREEN}, //MONEDAS

	//Tuberias
	{1250, 500, 100, 100, GREEN},
	{1675, 450, 100, 150, GREEN},
	{2075, 400, 100, 200, GREEN},
	{2600, 400, 100, 200, GREEN},

	//Bloque oculto (ya se hará) -Vida extra-
	//Boquete 1

	//Segundo conjuto de bloques
		//Zona inferior
	{3550, 400, 50, 50, GREEN},
	{3600, 400, 50, 50, GREEN}, //Champiñon
	{3650, 400, 50, 50, GREEN},
		//Zona superior
	{3700, 200, 50, 50, GREEN},
	{3750, 200, 50, 50, GREEN},
	{3800, 200, 50, 50, GREEN},
	{3850, 200, 50, 50, GREEN},
	{3900, 200, 50, 50, GREEN},
	{3950, 200, 50, 50, GREEN},
	{4000, 200, 50, 50, GREEN},
	{4050, 200, 50, 50, GREEN},
	//Boquete 2
		//Zona superior 2
	{4250, 200, 50, 50, GREEN},
	{4300, 200, 50, 50, GREEN},
	{4350, 200, 50, 50, GREEN},
	{4400, 200, 50, 50, GREEN},//Monedas
		//Bloque inferior
	{4400, 400, 50, 50, GREEN}, //Moneda

	//Bloques extrella
	{4700, 400, 50, 50, GREEN},
	{4750, 400, 50, 50, GREEN}, //Estrella

	//Bloques ?
		//Inferior
	{5000, 400, 50, 50, GREEN}, //Monedas
	{5125, 400, 50, 50, GREEN}, //Monedas
	{5250, 400, 50, 50, GREEN}, //Monedas
		//Superior
	{5125, 200, 50, 50, GREEN}, //Champiñon

	//Secuencia de bloques
	{5550, 400, 50, 50, GREEN},

	{5700, 200, 50, 50, GREEN},
	{5750, 200, 50, 50, GREEN},
	{5800, 200, 50, 50, GREEN},

	{6000, 200, 50, 50, GREEN},
	{6050, 200, 50, 50, GREEN},
	{6100, 200, 50, 50, GREEN},
	{6150, 200, 50, 50, GREEN},

	{6050, 400, 50, 50, GREEN},
	{6100, 400, 50, 50, GREEN},



	//Primera escalera
	//Primer escalon
	{6310, 550, 50, 50, GREEN},
	{6360, 550, 50, 50, GREEN},
	{6410, 550, 50, 50, GREEN},
	{6460, 550, 50, 50, GREEN},
	//Segundo escalon
	{6360, 500, 50, 50, GREEN},
	{6410, 500, 50, 50, GREEN},
	{6460, 500, 50, 50, GREEN},
	//Tercer escalon
	{6410, 450, 50, 50, GREEN},
	{6460, 450, 50, 50, GREEN},
	//Quarto escalon
	{6460, 400, 50, 50, GREEN},
	//-Invertida-//
	//Primer escalón
	{6590, 550, 50, 50, GREEN},
	{6640, 550, 50, 50, GREEN},
	{6690, 550, 50, 50, GREEN},
	{6740, 550, 50, 50, GREEN},
	//Segundo escalón
	{6590, 500, 50, 50, GREEN},
	{6640, 500, 50, 50, GREEN},
	{6690, 500, 50, 50, GREEN},
	//Tercer escalón
	{ 6590, 450, 50, 50, GREEN},
	{ 6640, 450, 50, 50, GREEN},
	//Quarto escalón
	{ 6590, 400, 50, 50, GREEN},



	//Segunda escalera
	//Primer escalon
	{6980, 550, 50, 50, GREEN},
	{7030, 550, 50, 50, GREEN},
	{7080, 550, 50, 50, GREEN},
	{7130, 550, 50, 50, GREEN},
	//Segundo escalon
	{7030, 500, 50, 50, GREEN},
	{7080, 500, 50, 50, GREEN},
	{7130, 500, 50, 50, GREEN},
	//Tercer escalon
	{7080, 450, 50, 50, GREEN},
	{7130, 450, 50, 50, GREEN},
	//Quarto escalon
	{7130, 400, 50, 50, GREEN},
		//BOQUETE 3
	//-Invertida-//
	//Primer escalón
	{7270, 550, 50, 50, GREEN},
	{7320, 550, 50, 50, GREEN},
	{7370, 550, 50, 50, GREEN},
	{7420, 550, 50, 50, GREEN},
	//Segundo escalón
	{7270, 500, 50, 50, GREEN},
	{7320, 500, 50, 50, GREEN},
	{7370, 500, 50, 50, GREEN},
	//Tercer escalón
	{7270, 450, 50, 50, GREEN},
	{7320, 450, 50, 50, GREEN},
	//Quarto escalón
	{7270, 400, 50, 50, GREEN},



	//Tuberias finales
	{7700, 500, 100, 100, GREEN},
		//Bloques intermedios
		{7950, 400, 50, 50, GREEN},
		{8000, 400, 50, 50, GREEN},
		{8050, 400, 50, 50, GREEN},
		{8100, 400, 50, 50, GREEN},
	{8460, 500, 100, 100, GREEN},


	//ESCALERA FINAL
// Primer escalón (base de 9 bloques)
	{ 8570, 550, 50, 50, GREEN},
	{ 8620, 550, 50, 50, GREEN},
	{ 8670, 550, 50, 50, GREEN},
	{ 8720, 550, 50, 50, GREEN},
	{ 8770, 550, 50, 50, GREEN},
	{ 8820, 550, 50, 50, GREEN},
	{ 8870, 550, 50, 50, GREEN},
	{ 8920, 550, 50, 50, GREEN},
	{ 8970, 550, 50, 50, GREEN},

	// Segundo escalón (8 bloques)
	{ 8620, 500, 50, 50, GREEN},
	{ 8670, 500, 50, 50, GREEN},
	{ 8720, 500, 50, 50, GREEN},
	{ 8770, 500, 50, 50, GREEN},
	{ 8820, 500, 50, 50, GREEN},
	{ 8870, 500, 50, 50, GREEN},
	{ 8920, 500, 50, 50, GREEN},
	{ 8970, 500, 50, 50, GREEN},

	// Tercer escalón (7 bloques)
	{ 8670, 450, 50, 50, GREEN},
	{ 8720, 450, 50, 50, GREEN},
	{ 8770, 450, 50, 50, GREEN},
	{ 8820, 450, 50, 50, GREEN},
	{ 8870, 450, 50, 50, GREEN},
	{ 8920, 450, 50, 50, GREEN},
	{ 8970, 450, 50, 50, GREEN},

	// Cuarto escalón (6 bloques)
	{ 8720, 400, 50, 50, GREEN},
	{ 8770, 400, 50, 50, GREEN},
	{ 8820, 400, 50, 50, GREEN},
	{ 8870, 400, 50, 50, GREEN},
	{ 8920, 400, 50, 50, GREEN},
	{ 8970, 400, 50, 50, GREEN},

	// Quinto escalón (5 bloques)
	{ 8770, 350, 50, 50, GREEN},
	{ 8820, 350, 50, 50, GREEN},
	{ 8870, 350, 50, 50, GREEN},
	{ 8920, 350, 50, 50, GREEN},
	{ 8970, 350, 50, 50, GREEN},

	// Sexto escalón (4 bloques)
	{ 8820, 300, 50, 50, GREEN},
	{ 8870, 300, 50, 50, GREEN},
	{ 8920, 300, 50, 50, GREEN},
	{ 8970, 300, 50, 50, GREEN},

	// Séptimo escalón (3 bloques)
	{ 8870, 250, 50, 50, GREEN},
	{ 8920, 250, 50, 50, GREEN},
	{ 8970, 250, 50, 50, GREEN},

	// Octavo escalón (2 bloques)
	{ 8920, 200, 50, 50, GREEN},
	{ 8970, 200, 50, 50, GREEN},

	//Flag
	{9375, 550, 50, 50, GREEN}
	};

public:
	//Initialise the game
	Game() : currentScreen(GameScreen::LOGO), framesCounter(0), player(50, 600), frameCounter(0),
		playFrameCounter(0), currentPlayFrame(0), goomba(1400, 600), koopa(700, 330), flag(9375, 264), mooshroom(900, 350) {

		InitWindow(screenWidth, screenHeight, "Super Mario + Screen Manager");
		InitAudioDevice();              // Initialize audio device
		SetTargetFPS(60);

		/*--------------------------------------------------------------------------*/
		/*                        Textures and Typography                           */
		/*--------------------------------------------------------------------------*/

		logoTexture = LoadTexture("Resources/Images/HOME/LogoProyecto1.png");
		UI = LoadTexture("Resources/Images/Seleccion Modo/Pantalla_Intro.png");
		money = LoadTexture("Resources/Sprites/Items/Monedas.png");
		Level1 = LoadTexture("Resources/Images/Seleccion Modo/World 1-1.png");
		mario = LoadTexture("Resources/Sprites/MARIO/Mario_Right.png");
		Goomba = LoadTexture("Resources/Sprites/Enemies/Goomba.png");
		icon_lifes = LoadTexture("Resources/Images/Player/Icon_Lifes.png");
		icon_money = LoadTexture("Resources/Images/Player/Icon_Money.png");
		fondo = LoadTexture("Resources/Sprites/Background/Fondo.png");

		marioFont = LoadFont("Resources/Fonts/MarioFont.ttf");
		/*------------------------------------------------------------*/
		/*                           Items                            */
		/*------------------------------------------------------------*/
		Mooshroom = LoadTexture("Resources/Sprites/Items/Mushroom.png");

		/*------------------------------------------------------------*/
		/*                          Bloques                           */
		/*------------------------------------------------------------*/
		bloque_int = LoadTexture("Resources/Sprites/Bloques/Bloque_int.png");
		ladrillo = LoadTexture("Resources/Sprites/Bloques/Ladrillo.png");
		escalera = LoadTexture("Resources/Sprites/Bloques/escalera.png");

		/*------------------------------------------------------------*/
		/*                          Tileset                           */
		/*------------------------------------------------------------*/
		castle = LoadTexture("Resources/Sprites/Tileset/Castle.png");
		flagTexture = LoadTexture("Resources/Sprites/Tileset/Flag.png");
		tuberia_s = LoadTexture("Resources/Sprites/Tileset/Tuberia1.png");
		tuberia_m = LoadTexture("Resources/Sprites/Tileset/Tuberia2.png");
		tuberia_b = LoadTexture("Resources/Sprites/Tileset/Tuberia3.png");

		/*--------------------------------------------------------------------------*/
		/*                            Music and effects                             */
		/*--------------------------------------------------------------------------*/

		musicOverworld = LoadMusicStream("Resources/Audio/Music/Cancion.ogg");
		musicOverworld_hurry = LoadMusicStream("Resources/Audio/Music/CancionHurry.ogg");
		musicInvencible = LoadMusicStream("Resources/Audio/Music/Invencible.ogg");
		musicInvencible_hurry = LoadMusicStream("Resources/Audio/Music/InvencibleHurry.ogg");

		sfxJumpSmall = LoadSound("Resources/Audio/FX/smb_jump-small.wav");
		sfxJumpSuper = LoadSound("Resources/Audio/FX/smb_jump-super.wav");
		sfxMushroom = LoadSound("Resources/Audio/FX/smb_mushroom.wav");
		sfxGameOver = LoadSound("Resources/Audio/FX/smb_gameover.wav");
		sfxFlagpole = LoadSound("Resources/Audio/FX/smb_flagpole.wav");
		sfxDeath = LoadSound("Resources/Audio/Music/Muerte.ogg");
		sfxCompleted = LoadSound("Resources/Audio/FX/smb_stage_clear.wav");

		SetMusicVolume(musicOverworld, 0.5f);
		SetMusicVolume(musicInvencible, 0.5f);
		SetMusicVolume(musicOverworld_hurry, 0.5f);

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
		UnloadAudioAssets();
		


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
			if (framesCounter > 250) {
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
				player.position = { 50, 600 };
				camera.target.x = 400;
				camera.target.y = 280;
				goomba.position = { 1400, 600 };
				mooshroom.position = { 900, 350 };
				Timer = 400;
				Score = 000000;
				Money = 00;
				player.alive = 1;
				goomba.death = false;
				elapsedTime = 0.0f;
				contmuerte = 0;
				conttiempo = 0;

				PlayMusicStream(musicOverworld);
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
						PlaySound(sfxGameOver);
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
				player.position = { 50, 600 };
				camera.target.x = 400;
				camera.target.y = 280;
				goomba.position = { 1400, 600 };
				mooshroom.position = { 900, 350 };
				Timer = 400;
				player.alive = 1;
				goomba.death = false;
				elapsedTime = 0.0f;
				contmuerte = 0;
				conttiempo = 0;

				PlayMusicStream(musicOverworld);
			}
			break;

		case GameScreen::GAMEPLAY:
			framesCounter = 0;
			UpdateGameplay();

			if (player.alive == 0) {
				if (contmuerte == 0)
				{
					StopMusicStream(musicOverworld);
					PlaySound(sfxDeath);
					player.speed.y = -PLAYER_JUMP_SPD * 1.2f;
					player.canJump = false;
					player.canJump2 = true;
					player.jumpTime = 0.0f;
					if (framesCounter == 0) {
						player.lifes--;
						framesCounter++;
					}
					contmuerte++;
				}
				elapsedTime += GetFrameTime();
				if (elapsedTime >= 15.0f) {
					if (player.lifes <= 0) {
						while (elapsedTime < 0.0f) {
							elapsedTime += GetFrameTime();
						}
						if (elapsedTime >= 0.0f) {
							currentScreen = GameScreen::ENDING;
							PlaySound(sfxGameOver);
							break;
						}
					}
					currentScreen = GameScreen::DEATH;
					elapsedTime = 0.0f;
				}
			}
			if (flag.reached) {
				StopMusicStream(musicOverworld);
				if (conttiempo == 0 && player.position.y == 600) {
					PlaySound(sfxCompleted);
					conttiempo++;
				}
			}

			if (Timer <= 0) {
				if (contmuerte == 0)
				{
					PlaySound(sfxDeath);
					player.speed.y = -PLAYER_JUMP_SPD * 1.2f;
					player.canJump = false;
					player.canJump2 = true;
					player.jumpTime = 0.0f;
					contmuerte++;
				}

				elapsedTime += GetFrameTime();
				if (elapsedTime >= 15.0f) {
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

		if (player.big) player.mario_hitbox = { player.position.x, player.position.y, 23,16 };
		if (!player.big) player.mario_hitbox = { player.position.x, player.position.y, 23,16 };
		goomba.goomba_hitbox = { goomba.position.x, goomba.position.y, 16,16 };
		player.mario_hitbox = { player.position.x, player.position.y, 23,16 };
		Rectangle prev_hitbox = player.mario_hitbox;
		player.mario_hitbox.x += player.speed.x;
		player.mario_hitbox.y += player.speed.y;
		mooshroom.powerup_hitbox = { mooshroom.position.x, mooshroom.position.y, 16,16 };

		bool hitObstacleFloor = false;
		bool hitObstacleWall = false;
		bool onGroundEnemy = false;
		bool onGroundPowerUp = false;

		float deltaTime = GetFrameTime();
		elapsedTime += deltaTime * 2.5;

		//In-game controls and conditions
		if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_DOWN) && !flag.reached && Timer > 0 && player.alive != 0)
		{
			if (IsKeyDown(KEY_LEFT_SHIFT) && !flag.reached && Timer > 0 && !hitObstacleWall)
			{
				player.position.x += PLAYER_RUN_SPD * deltaTime;
				player.speed.x = 2.0f;
			}
			player.position.x += PLAYER_HOR_SPD * deltaTime;
			player.speed.x = 2.0f;
		}

		if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_DOWN) && player.position.x > camera.target.x - screenWidth / 2.0f && !flag.reached && Timer > 0 && player.alive != 0)
		{
			if (IsKeyDown(KEY_LEFT_SHIFT) && player.position.x > camera.target.x - screenWidth / 2.0f && !flag.reached && Timer > 0 && !hitObstacleWall)
			{
				player.position.x -= PLAYER_RUN_SPD * deltaTime;
				player.speed.x = -2.0f;
			}
			player.position.x -= PLAYER_HOR_SPD * deltaTime;
			player.speed.x = -2.0f;
		}
		if (player.position.x > camera.target.x && camera.target.x < 9795)
		{
			camera.target.x = player.position.x;
		}

		static constexpr float MAX_JUMP_TIME = 0.3f;
		static constexpr float JUMP_HOLD_FORCE = 500.0f;

		if (IsKeyPressed(KEY_SPACE) && player.canJump && !flag.reached && Timer > 0 && player.alive != 0) {
			player.speed.y = -PLAYER_JUMP_SPD;
			player.canJump = false;
			player.canJump2 = true;
			player.jumpTime = 0.0f;
			PlaySound(sfxJumpSmall);
		}

		if (IsKeyDown(KEY_SPACE) && player.canJump2 && player.jumpTime < MAX_JUMP_TIME && !flag.reached && Timer > 0 && player.alive != 0) {
			player.speed.y -= JUMP_HOLD_FORCE * deltaTime;
			player.jumpTime += deltaTime;
		}

		if (IsKeyReleased(KEY_SPACE) && !flag.reached && Timer > 0 && player.alive != 0) {
			player.canJump2 = false;  //Cut the jump when releasing the key
			player.speed.y += JUMP_HOLD_FORCE - 300;
		}

		if (Timer <= 0 || player.alive == 0) {
			hitObstacleFloor = false;
		}
		//MOOSHROOM
		if (player.position.x >= mooshroom.position.x && player.alive != 0) {
			mooshroom.active = true;
		}
		if (!mooshroom.active && !player.big) mooshroom.position = { 900, 350 };

		mooshroom.speed.x = 1.0f;
		if (mooshroom.active && player.alive != 0 && mooshroom.side) {
			mooshroom.position.x += -120 * deltaTime;
		}

		if (mooshroom.active && player.alive != 0 && !mooshroom.side) {
			mooshroom.position.x += 120 * deltaTime;
		}

		//GOOMBA
		if (player.position.x - goomba.position.x <= -200 && goomba.death == false && player.alive != 0 && Timer > 0) {
			goomba.activated = true;
		}

		goomba.speed.x = 1.0f;
		if (goomba.activated && goomba.death == false && player.alive != 0 && goomba.side) {
			goomba.position.x += -120 * deltaTime;
		}
		if (goomba.activated && goomba.death == false && player.alive != 0 && !goomba.side) {
			goomba.position.x += 120 * deltaTime;
		}

		if (goomba.death == true) {
			if (player.position.y >= goomba.position.y) {
				goomba.position.y = 1000;
			}
		}

		//--------Colisiones de Mario--------\\

		//Suelo
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0
				&& block.rect.x <= player.position.x + player.mario_hitbox.width - 5
				&& block.rect.x + block.rect.width + 10 >= player.position.x
				&& block.rect.y >= player.position.y
				&& block.rect.y <= player.position.y + player.speed.y * deltaTime
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				hitObstacleFloor = true;
				player.speed.y = 0.0f;
				player.position.y = block.rect.y;
			}
		}

		//Techo
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0
				&& block.rect.x <= player.position.x + player.mario_hitbox.width - 5
				&& block.rect.x + block.rect.width + 10 >= player.position.x
				&& block.rect.y + block.rect.height + block.rect.height <= player.position.y
				&& block.rect.y + block.rect.height + block.rect.height >= player.position.y + player.speed.y * deltaTime
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				player.speed.y = 0.0f;
				player.position.y = block.rect.y + block.rect.height + block.rect.height;
			}
		}

		//Lados
		float nextX = player.position.x + player.speed.x * deltaTime; //Calcula la posición futura en X

		//--- COLISIÓN POR LA DERECHA (Mario viene de la izquierda) ---
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 &&
				player.speed.x > 0 &&
				player.position.y > block.rect.y &&
				player.position.y < (block.rect.y + block.rect.height + block.rect.height) &&
				player.position.x - 10 <= block.rect.x &&
				(nextX + player.mario_hitbox.width) >= block.rect.x
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				//¡Choque! Ajustamos posición y detenemos velocidad horizontal
				hitObstacleWall = true;
				player.speed.x = 0;
				player.position.x = block.rect.x - player.mario_hitbox.width;
			}
		}

		//--- COLISIÓN POR LA IZQUIERDA (Mario viene de la derecha) ---
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 &&
				player.speed.x < 0 &&
				player.position.y > block.rect.y &&
				player.position.y < (block.rect.y + block.rect.height + block.rect.height) &&
				player.position.x + 10 >= (block.rect.x + block.rect.width) &&
				(nextX) <= (block.rect.x + block.rect.width + 14)
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				//¡Choque! Ajustamos posición y detenemos velocidad horizontal
				hitObstacleWall = true;
				player.speed.x = 0;
				player.position.x = block.rect.x + block.rect.width + player.mario_hitbox.width - 7;
			}
		}

		if (!hitObstacleFloor) {
			player.position.y += player.speed.y * deltaTime;
			if (player.speed.y > 0)
			{
				player.speed.y += GRAVITY * 3.0f * deltaTime; //Increase gravity in fall
			}
			else
			{
				player.speed.y += GRAVITY * deltaTime; //Normal upward gravity
			}
			player.canJump = false;
			player.canJump2 = true;
		}
		else {
			player.canJump = true;
		}

		//--------Colisiones de Enemigos--------\\

		//Con Mario
		if (goomba.alive && player.position.x + player.mario_hitbox.width + 10 >= goomba.position.x &&
			player.position.x <= goomba.position.x + goomba.goomba_hitbox.width + 20 &&
			player.position.y + player.mario_hitbox.height + 16 >= goomba.position.y && player.position.y <= goomba.position.y + goomba.goomba_hitbox.height)
		{
			if (player.position.y + player.mario_hitbox.height <= goomba.position.y && player.alive) {
				goomba.death = true;
				Score += 100;
				player.speed.y = -PLAYER_JUMP_SPD + 100;
				player.canJump = false;
				player.canJump2 = true;
				player.jumpTime = 0.0f;
			}
			else player.alive = 0;
		}

		//Con el suelo
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 && goomba.activated
				&& block.rect.x <= goomba.position.x + goomba.goomba_hitbox.width - 5
				&& block.rect.x + block.rect.width + 10 >= goomba.position.x
				&& block.rect.y + block.rect.height >= goomba.position.y
				&& block.rect.y <= goomba.position.y
				&& ColorToInt(block.color) != ColorToInt(BLUE)) {
				onGroundEnemy = true;
				goomba.speed.y = 0.0f;
				goomba.position.y = block.rect.y;
			}
		}

		if (!onGroundEnemy && player.alive && Timer > 0) {
			goomba.position.y += (GRAVITY - 300) * deltaTime;
			if (goomba.position.y > 0)
			{
				goomba.position.y += (GRAVITY - 300) * 2.0f * deltaTime; //Increase gravity in fall
			}
			else
			{
				goomba.position.y += (GRAVITY - 300) * deltaTime; //Normal upward gravity
			}
		}

		//Los lados
		float nextXE = goomba.position.x + goomba.speed.x * deltaTime; //Calcula la posición futura en X

		//Derecha
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 && goomba.alive &&
				goomba.activated && !goomba.side &&
				goomba.position.y > block.rect.y &&
				goomba.position.y < (block.rect.y + block.rect.height + block.rect.height) &&
				goomba.position.x - 10 <= block.rect.x &&
				(nextXE + goomba.goomba_hitbox.width) >= block.rect.x - 15
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				goomba.side = true;
			}
		}

		//Izquierda
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 &&
				goomba.activated && goomba.side &&
				goomba.position.y > block.rect.y &&
				goomba.position.y < (block.rect.y + block.rect.height + block.rect.height) &&
				goomba.position.x + 10 >= (block.rect.x + block.rect.width) &&
				(nextXE) <= (block.rect.x + block.rect.width + 20)
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				goomba.side = false;
			}
		}

		//--------Colisiones de Power-Ups--------\\

		//Con Mario
		if (mooshroom.active && player.position.x + player.mario_hitbox.width + 10 >= mooshroom.position.x &&
			player.position.x <= mooshroom.position.x + mooshroom.powerup_hitbox.width + 20 &&
			player.position.y >= mooshroom.position.y && player.position.y <= mooshroom.position.y + mooshroom.powerup_hitbox.height)
		{
			PlaySound(sfxMushroom);
			if (!player.big) player.big = true;
			mooshroom.active = false;
			Score += 1000;
			mooshroom.position.y = 1000;
		}

		//Con el suelo
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 && mooshroom.active
				&& block.rect.x <= mooshroom.position.x + mooshroom.powerup_hitbox.width - 5
				&& block.rect.x + block.rect.width + 10 >= mooshroom.position.x
				&& block.rect.y + block.rect.height >= mooshroom.position.y
				&& block.rect.y <= mooshroom.position.y
				&& ColorToInt(block.color) != ColorToInt(BLUE)) {
				onGroundPowerUp = true;
				mooshroom.speed.y = 0.0f;
				mooshroom.position.y = block.rect.y;
			}
		}

		if (!onGroundPowerUp && player.alive && Timer > 0) {
			mooshroom.position.y += (GRAVITY - 300) * deltaTime;
			if (mooshroom.position.y > 0)
			{
				mooshroom.position.y += (GRAVITY - 300) * 2.0f * deltaTime; //Increase gravity in fall
			}
			else
			{
				mooshroom.position.y += (GRAVITY - 300) * deltaTime; //Normal upward gravity
			}
		}

		//Los lados
		float nextXP = mooshroom.position.x + mooshroom.speed.x * deltaTime; //Calcula la posición futura en X

		//Derecha
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 &&
				mooshroom.active && !mooshroom.side &&
				mooshroom.position.y > block.rect.y &&
				mooshroom.position.y < (block.rect.y + block.rect.height + block.rect.height) &&
				mooshroom.position.x - 10 <= block.rect.x &&
				(nextXP + mooshroom.powerup_hitbox.width) >= block.rect.x - 15
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				mooshroom.side = true;
			}
		}

		//Izquierda
		for (EnvElement block : blocks) {
			if (Timer > 0 && player.alive != 0 &&
				mooshroom.active && mooshroom.side &&
				mooshroom.position.y > block.rect.y &&
				mooshroom.position.y < (block.rect.y + block.rect.height + block.rect.height) &&
				mooshroom.position.x + 10 >= (block.rect.x + block.rect.width) &&
				(nextXP) <= (block.rect.x + block.rect.width + 20)
				&& ColorToInt(block.color) != ColorToInt(BLUE))
			{
				mooshroom.side = false;
			}
		}

		//--------Colision Bandera--------\\
		
		if (!flag.reached && player.position.x >= flag.position.x - 20) {
			PlaySound(sfxFlagpole);
			player.position.x = flag.position.x;
			player.speed.y = 0;
			elapsedTime = 0;
			flag.reached = true;
		}

		if (flag.reached) {
			if (!hitObstacleFloor && player.position.y != 550) {
				player.position.y += 1 * 0.01;
			}
			else if (hitObstacleFloor) {
				if (player.position.y >= flag.position.y + 50) {
					float playerMovementSpeed = 120.0f * GetFrameTime();
					player.position.x += playerMovementSpeed;
				}
				
				if (player.position.x >= flag.position.x + 800) {
					currentScreen = GameScreen::ENDING;
				}
			}
		}

		// FUNCIONES DE PRUEBA //

		if (IsKeyPressed(KEY_R)) {
			player.position = { 50, 600 };
			camera.target.x = 400;
			camera.target.y = 280;
			goomba.position = { 1400, 600 };
			mooshroom.position = { 900, 350 };
			Timer = 400;
			Money = 00;
			Score = 000000;
			flag.reached = false;
			player.alive = 1;
			player.lifes = 3;
			player.big = 0;
			goomba.side = true;
			goomba.death = false;
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

		if (IsKeyPressed(KEY_B)) {
			player.big = 1;
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
			DrawText(" Marc Jimenez", 408, 550, 25, GRAY);
			DrawText(" Ruben Mateo", 410, 600, 25, GRAY);
			DrawText("Sauc Pellejero", 410, 650, 25, GRAY);
			DrawText("Professor: Alejandro Gomez Paris", 310, 725, 25, GRAY);
			DrawText("Replica Super Mario Bros 1 con Raylib", 220, 100, 30, GRAY);

			break;

		case GameScreen::TITLE:

			DrawTextureEx(UI, { (0), (0) }, 0.0f, 1.65f, WHITE);
			break;

		case GameScreen::LEVEL1:

			DrawTextureEx(Level1, { (0), (0) }, 0.0f, 4.0f, WHITE);
			break;

		case GameScreen::GAMEPLAY:

			DrawGameplay();
			AudioGameplay();
			

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
		if (currentScreen == GameScreen::GAMEPLAY)
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

	void AudioGameplay() {
		UpdateMusicStream(musicOverworld);
	}

	void UnloadAudioAssets() {
		UnloadMusicStream(musicOverworld);
		UnloadMusicStream(musicOverworld_hurry);
		UnloadMusicStream(musicInvencible);
		UnloadMusicStream(musicInvencible_hurry);
		
		UnloadSound(sfxJumpSmall);
		UnloadSound(sfxJumpSuper);
		UnloadSound(sfxMushroom);
		UnloadSound(sfxGameOver);
		UnloadSound(sfxFlagpole);
		UnloadSound(sfxDeath);
		UnloadSound(sfxCompleted);

		CloseAudioDevice();
	}

	void DrawGameplay() {
		BeginMode2D(camera);
		ClearBackground(BLUE);

		//Player
		int frameWidthP;
		int frameHeightP;
		
		if (player.big == 0) { //Small Mode
			frameWidthP = 16; //Each frame mesure 16x16 pixels
			frameHeightP = 16;
		}
		if (player.big == 1) { //Big Mode
			frameWidthP = 16;
			frameHeightP = 32;
		}

		if (player.side == 0) mario = LoadTexture("Resources/Sprites/MARIO/Mario_Right.png");
		else if (player.side == 1) mario = LoadTexture("Resources/Sprites/MARIO/Mario_Left.png");

		Rectangle sourceRec = { 0, 0, (float)frameWidthP, (float)frameHeightP };

		static float frameTime = 0.0f;
		static int currentFrame = 0;
		frameTime += GetFrameTime();
		float frameSpeed = 0.1f; //Velocity animation

		//Enemies
		/*--Goomba--*/
		int frameWidthG = 16;
		int frameHeightG = 16;

		if (!goomba.death) {
			int frameWidthG = 16;
			int frameHeightG = 16;
		}
		if (goomba.death) {
			int frameWidthG = 16;
			int frameHeightG = 16;
		}

		Rectangle sourceRec2 = { 0, 0, (float)frameWidthG, (float)frameHeightG };

		/*--Koopa--*/
		int frameWidthK = 16;
		int frameHeightK = 24;
		Rectangle sourceRec3 = { 0, 0, (float)frameWidthK, (float)frameHeightK };

		static float frameTimeE = 0.0f;
		static int currentFrameE = 0;
		frameTimeE += GetFrameTime();
		float frameSpeedE = 0.1f; //Velocity animation

		//Blocks
		/*--?--*/
		int frameWidthInt = 16;
		int frameHeightInt = 16;
		Rectangle sourceRec4 = { 0, 0, (float)frameWidthInt, (float)frameHeightInt };

		static float frameTimeInt = 0.0f;
		static int currentFrameInt = 0;
		frameTimeInt += GetFrameTime();
		float frameSpeedInt = 0.5f; //Velocity animation
		int variant = 1;


		//Animation of Mario
		if (IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && Timer > 0 && player.alive != 0 && !flag.reached || flag.reached && camera.target.x < 9795 && (player.position.y == 600 || player.position.y == 550)) {
			mario = LoadTexture("Resources/Sprites/MARIO/Mario_Right.png");
			player.side = 0;
			if (IsKeyDown(KEY_LEFT_SHIFT) && !flag.reached) {
				frameSpeed = 0.05f; //Increases running speed
			}
			if (IsKeyDown(KEY_DOWN) && Timer > 0 && player.alive != 0 && !flag.reached && !player.big) currentFrame = 0;
			if (frameTime >= frameSpeed && !IsKeyDown(KEY_DOWN)) {
				frameTime = 0.0f;
				currentFrame = (currentFrame + 1) % 4; //Cycling between the 3 walk/run frames
			}
			if (IsKeyDown(KEY_DOWN) && Timer > 0 && player.alive != 0 && !flag.reached && player.big)
			{
				currentFrame = 0;
				sourceRec.x = frameWidthP * 6;
			}
			sourceRec.x = (float)(currentFrame * frameWidthP); //Change frame
		}

		if (IsKeyDown(KEY_LEFT) && !IsKeyDown(KEY_RIGHT) && Timer > 0 && player.alive != 0 && !flag.reached) {
			mario = LoadTexture("Resources/Sprites/MARIO/Mario_Left.png");
			player.side = 1;
			if (IsKeyDown(KEY_LEFT_SHIFT) && !flag.reached) {
				frameSpeed = 0.05f; //Increases running speed
			}
			if (IsKeyDown(KEY_DOWN) && Timer > 0 && player.alive != 0 && !flag.reached && !player.big) currentFrame = 0;
			if (frameTime >= frameSpeed && !IsKeyDown(KEY_DOWN)) {
				frameTime = 0.0f;
				currentFrame = (currentFrame + 1) % 4; //Cycling between the 3 walk/run frames
			}
			if (IsKeyDown(KEY_DOWN) && Timer > 0 && player.alive != 0 && !flag.reached && player.big)
			{
				currentFrame = 0;
				sourceRec.x = frameWidthP * 6;
			}
			sourceRec.x = (float)(currentFrame * frameWidthP); //Change frame
		}

		if (IsKeyDown(KEY_DOWN) && !IsKeyDown(KEY_RIGHT) && !IsKeyDown(KEY_LEFT) && Timer > 0 && player.alive != 0 && !flag.reached && player.big) sourceRec.x = frameWidthP * 6;

		//Animation of Enemies
		if (goomba.activated && player.alive != 0) {
			if (frameTimeE >= frameSpeedE) {
				frameTimeE = 0.0f;
				currentFrameE = (currentFrameE + 1) % 3;
			}
			sourceRec2.x = (float)(currentFrameE * frameWidthG);
		}
		if (!goomba.death) Goomba = LoadTexture("Resources/Sprites/Enemies/Goomba.png");
		if (goomba.death) Goomba = LoadTexture("Resources/Sprites/Enemies/Goomba_chafado.png");

		//Animation of Blocks
		if (player.alive != 0 && Timer > 0) {
			if (currentFrameInt == 3) {
				variant = -1;
			}
			else if (currentFrameInt == 1) {
				variant = 1;
			}
			if (frameTimeInt >= frameSpeedInt) {
				frameTimeInt = 0.0f;
				currentFrameInt = (currentFrameInt + variant) % 3;
			}
			sourceRec4.x = (float)(currentFrameInt * frameWidthInt);
		}

		else {
			currentFrame = 0; //Return to the first frame if still
			sourceRec.x = 0;
		}

		if (!player.canJump && !flag.reached) {
			sourceRec.x = frameWidthP * 5;
		}

		if (Timer <= 0 || player.alive == 0) {
			player.big = 0;
			sourceRec.x = frameWidthP * 6;
		}

		//Draw all entities, structures and objetcs
		
		//Tuberias 
		DrawTextureEx(tuberia_s, { (1242), (500) }, 0.0f, 1.2, WHITE);
		DrawTextureEx(tuberia_m, { (1667), (450) }, 0.0f, 1.2, WHITE);
		DrawTextureEx(tuberia_b, { (2067), (400) }, 0.0f, 1.2, WHITE);
		DrawTextureEx(tuberia_b, { (2592), (400) }, 0.0f, 1.2, WHITE);
		DrawTextureEx(tuberia_b, { (7692), (500) }, 0.0f, 1.2, WHITE);
		DrawTextureEx(tuberia_b, { (8452), (500) }, 0.0f, 1.2, WHITE);

		//All background
		DrawTextureEx(fondo, { (-113), (75) }, 0.0f, 3, WHITE);
		DrawTextureEx(fondo, { (2190), (75) }, 0.0f, 3, WHITE);
		DrawTextureEx(fondo, { (4490), (75) }, 0.0f, 3, WHITE);
		DrawTextureEx(fondo, { (6790), (75) }, 0.0f, 3, WHITE);
		DrawTextureEx(fondo, { (9090), (75) }, 0.0f, 3, WHITE);

		//Bloques ? y ladrillos
		DrawTextureEx(ladrillo, { (850), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (950), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (1050), (400) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (3550), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (3650), (400) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (3700), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (3750), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (3800), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (3850), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (3900), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (3950), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (4000), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (4050), (200) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (4250), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (4300), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (4350), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (4400), (400) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (4700), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (4750), (400) }, 0.0f, 3.2, WHITE); // Estrella

		DrawTextureEx(ladrillo, { (5550), (400) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (5700), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (5750), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (5800), (200) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (6000), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (6050), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (6100), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (6150), (200) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(ladrillo, { (6050), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (6100), (400) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(escalera, { (6310), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6360), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6410), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6460), (550) }, 0.0f, 3.2, WHITE);
		/*----*/
		DrawTextureEx(escalera, { (6360), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6410), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6460), (500) }, 0.0f, 3.2, WHITE);
		/*---*/
		DrawTextureEx(escalera, { (6410), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6460), (450) }, 0.0f, 3.2, WHITE);
		/*--*/
		DrawTextureEx(escalera, { (6460), (400) }, 0.0f, 3.2, WHITE);
		/*-*/

		DrawTextureEx(escalera, { (6590), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6640), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6690), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6740), (550) }, 0.0f, 3.2, WHITE);
		/*----*/
		DrawTextureEx(escalera, { (6590), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6640), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6690), (500) }, 0.0f, 3.2, WHITE);
		/*---*/
		DrawTextureEx(escalera, { (6590), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (6640), (450) }, 0.0f, 3.2, WHITE);
		/*--*/
		DrawTextureEx(escalera, { (6590), (400) }, 0.0f, 3.2, WHITE);
		/*-*/

		DrawTextureEx(escalera, { (6980), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7030), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7080), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7130), (550) }, 0.0f, 3.2, WHITE);
		/*----*/
		DrawTextureEx(escalera, { (7030), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7080), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7130), (500) }, 0.0f, 3.2, WHITE);
		/*---*/
		DrawTextureEx(escalera, { (7080), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7130), (450) }, 0.0f, 3.2, WHITE);
		/*--*/
		DrawTextureEx(escalera, { (7130), (400) }, 0.0f, 3.2, WHITE);
		/*-*/

		DrawTextureEx(escalera, { (7270), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7320), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7370), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7420), (550) }, 0.0f, 3.2, WHITE);
		/*----*/
		DrawTextureEx(escalera, { (7270), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7320), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7370), (500) }, 0.0f, 3.2, WHITE);
		/*---*/
		DrawTextureEx(escalera, { (7270), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (7320), (450) }, 0.0f, 3.2, WHITE);
		/*--*/
		DrawTextureEx(escalera, { (7270), (400) }, 0.0f, 3.2, WHITE);
		/*-*/

		DrawTextureEx(ladrillo, { (7950), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (8000), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(ladrillo, { (8100), (400) }, 0.0f, 3.2, WHITE);

		DrawTextureEx(escalera, { (8570), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8620), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8670), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8720), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8770), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8820), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8870), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (550) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (550) }, 0.0f, 3.2, WHITE);
		/*---------*/
		DrawTextureEx(escalera, { (8620), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8670), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8720), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8770), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8820), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8870), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (500) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (500) }, 0.0f, 3.2, WHITE);
		/*--------*/
		DrawTextureEx(escalera, { (8670), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8720), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8770), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8820), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8870), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (450) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (450) }, 0.0f, 3.2, WHITE);
		/*-------*/
		DrawTextureEx(escalera, { (8720), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8770), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8820), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8870), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (400) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (400) }, 0.0f, 3.2, WHITE);
		/*------*/
		DrawTextureEx(escalera, { (8770), (350) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8820), (350) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8870), (350) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (350) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (350) }, 0.0f, 3.2, WHITE);
		/*-----*/
		DrawTextureEx(escalera, { (8820), (300) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8870), (300) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (300) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (300) }, 0.0f, 3.2, WHITE);
		/*----*/
		DrawTextureEx(escalera, { (8870), (250) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8920), (250) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (250) }, 0.0f, 3.2, WHITE);
		/*---*/
		DrawTextureEx(escalera, { (8920), (200) }, 0.0f, 3.2, WHITE);
		DrawTextureEx(escalera, { (8970), (200) }, 0.0f, 3.2, WHITE);
		/*--*/
		DrawTextureEx(escalera, { (8970), (250) }, 0.0f, 3.2, WHITE);
		/*-*/

		DrawTexturePro(bloque_int, sourceRec4, { 650, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 900, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 1000, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 950, 200, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 3600, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 4400, 200, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 5000, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 5125, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 5250, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 5125, 200, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 6050, 200, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 6100, 200, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(bloque_int, sourceRec4, { 8050, 400, sourceRec4.width * 3.2f, sourceRec4.height * 3.2f }, { 0, 0 }, 0, WHITE);

		


		DrawTexturePro(Goomba, sourceRec2, { goomba.position.x - 20, goomba.position.y - 48, sourceRec2.width * 3, sourceRec2.height * 3 }, { 0, 0 }, 0, WHITE);
		DrawTexturePro(Mooshroom, sourceRec2, { mooshroom.position.x - 20, mooshroom.position.y - 48, sourceRec.width * 3, sourceRec2.height * 3 }, { 0,0 }, 0, WHITE);
		//META Y CASTILLO//
		DrawTextureEx(flagTexture, { 9375, flag.position.y - flagTexture.height }, 0, 3, WHITE);
		DrawTextureEx(castle, { (9675), (360) }, 0.0f, 3, WHITE);
		if (player.big == 0) {
			DrawTexturePro(mario, sourceRec, { player.position.x - 20, player.position.y - 48, sourceRec.width * 3, sourceRec.height * 3 }, { 0, 0 }, 0, WHITE);
		}
		if (player.big == 1) {
			sourceRec.y = 16;
			DrawTexturePro(mario, sourceRec, { player.position.x - 20, player.position.y - 96, sourceRec.width * 3, sourceRec.height * 3 }, { 0, 0 }, 0, WHITE);
		}

		if (player.position.x >= 9795) { //Mario arrived to the flag
			camera.target.x = 9795;
			DrawTextureEx(castle, { (9675), (360) }, 0.0f, 3, WHITE);
			player.big = 0;
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