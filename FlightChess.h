#ifndef FLIGHT_CHESS_H
#define FLIGHT_CHESS_H

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_ttf.h"

#define WIN_WIDTH 1280
#define WIN_HEIGHT 720

static SDL_Window *Window = NULL;
static SDL_Renderer *Renderer; //画笔（渲染）
static SDL_Surface *MainBackGroundSurface; //表面它是图形
static SDL_Texture *MainBackGroundTexture; //轨迹，让画笔能把它画出来
static SDL_Rect MainBackGroundRect; //Texture的信息，长，宽，高等
static SDL_Surface *PlayBackGroundSurface;
static SDL_Texture *PlayBackGroundTexture;
static SDL_Rect PlayBackGroundRect;

void event_loop(void);
#endif
