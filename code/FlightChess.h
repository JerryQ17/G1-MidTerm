#ifndef FLIGHT_CHESS_H
#define FLIGHT_CHESS_H

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define DEBUG           //调试模式
#define WIN_WIDTH 1280  //窗口宽度
#define WIN_HEIGHT 720  //窗口高度
#define FRAMERATE 60    //动画帧率
#define FONT_SIZE 16    //字体大小

//全局变量
static SDL_Window *Window = NULL;                  //窗口
static SDL_Renderer *Renderer = NULL;              //渲染器

static SDL_Surface *MainBackGroundSurface = NULL;  //主界面
static SDL_Texture *MainBackGroundTexture = NULL;
static SDL_Rect MainBackGroundRect;

static SDL_Surface *PlayBackGroundSurface = NULL;  //游戏界面
static SDL_Texture *PlayBackGroundTexture = NULL;
static SDL_Rect PlayBackGroundRect;

static TTF_Font *Font;                             //字体
static SDL_Surface *FontSurface = NULL;
static SDL_Texture *FontTexture = NULL;
static SDL_Rect FontRect;
static SDL_Color FontColor = {255, 255, 255, 255};

//SDL_main.c中的函数声明
void event_loop(void);
int load(void);
void LoadPicture(void);
void quit(void);

//game.c中的函数声明
void game(void);
void print(void);
#endif