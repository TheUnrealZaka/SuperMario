#ifndef TEXTURES_AND_FONTS_H
#define TEXTURES_AND_FONTS_H

#include "raylib.h"

// Bloques
extern Texture2D bloque_int;
extern Texture2D bloque_int_a;
extern Texture2D ladrillo;
extern Texture2D escalera;
extern Texture2D ladrillo_cueva;
extern Texture2D suelo_cueva;

// Interactive Structures
extern Texture2D flagTexture;
extern Texture2D tuberia;
extern class Pipe pipe;
extern class Flag flag;

// Map
extern Texture2D castle;
extern Texture2D fondo;
extern Texture2D azul;
extern Texture2D negro;

// Pipes
extern Texture2D tuberia_s;
extern Texture2D tuberia_m;
extern Texture2D tuberia_b;
extern Texture2D tubo;
extern Texture2D tuberia_cueva;

// Objects
extern Texture2D money;

// Other Textures
extern Texture2D logoTexture;
extern Texture2D UI;
extern Texture2D Level1;
extern Texture2D icon_lifes;
extern Texture2D icon_money;

// Enemigos
extern Texture2D Goomba;
extern Texture2D Goomba_chafado;
extern Texture2D Koopa;
extern Texture2D Shell;
extern Texture2D goomba_sprite;

// Mario
extern Texture2D Mario_Right;
extern Texture2D Mario_Left;
extern Texture2D Mario_Fire_Right;
extern Texture2D Mario_Fire_Left;
extern Texture2D mario_sprite;

// Items
extern Texture2D Mooshroom;
extern Texture2D FireFlower;
extern Texture2D FireBall;

extern Font marioFont;

// Carga
void LoadGameTextures();
void LoadGameTypography();

#endif 
