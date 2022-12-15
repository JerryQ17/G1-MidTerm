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

#define WIN_WIDTH 1280  //窗口宽度
#define WIN_HEIGHT 720  //窗口高度
#define FRAMERATE 60    //动画帧率
#define FONT_SIZE 64    //字体大小

//类型定义
typedef enum color{           //定义颜色
  RED = 1,                    //红 = 1
  GREEN = 2,                  //绿 = 2
  YELLOW = 3,                 //黄 = 3
  BLUE = 4                    //蓝 = 4
}color;

typedef enum direction{       //定义棋子指向
  U = 1,                      //上 = 1
  UR = 2,                     //右上 = 2
  R = 3,                      //右 = 3
  DR = 4,                     //右下 = 4
  D = 5,                      //下 = 5
  DL = 6,                     //左下 = 6
  L = 7,                      //左 = 7
  UL = 8                      //左上 = 8
}direction;

typedef struct plane_chess{   //定义棋子
  int pos;                    //棋子所在的位置
  color color;                //棋子的颜色
  direction dir;              //棋子当前的指向
}chess;

//声明全局变量
extern int GameRound;              //当前游戏轮数
extern color cur_player;           //当前玩家颜色

extern int record;                 //是否记录日志
extern int debug;                  //调试模式
extern FILE *cfg;                  //配置文件指针
extern FILE *rec_file;             //记录文件指针

extern SDL_Window *Window;         //窗口
extern SDL_Renderer *Renderer;     //渲染器

extern SDL_Surface *MainSurface;   //主界面
extern SDL_Texture *MainTexture;
extern SDL_Rect MainRect;

extern SDL_Surface *GameSurface;   //游戏界面
extern SDL_Texture *GameTexture;
extern SDL_Rect GameRect;

extern TTF_Font *Font;             //字体
extern SDL_Surface *FontSurface;
extern SDL_Texture *FontTexture;
extern SDL_Rect FontRect;
extern SDL_Color FontColor;

//SDL_main.c中的函数声明
void main_event_loop(void);
int Init(void);
void LoadPicture(void);
void help_fail(void);
void quit(void);

//game_init.c中的函数声明
void game_init(void);
void game_event_loop(void);
int roll(void);
void die_present(int num);

#endif