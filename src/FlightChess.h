#ifndef FLIGHT_CHESS_H
#define FLIGHT_CHESS_H

//包含库

#include <math.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include "SDL2/SDL.h"
#include "SDL2/SDL_ttf.h"
#include "SDL2/SDL_image.h"
#include "SDL2/SDL_mixer.h"

//宏定义

//#define DEBUG

#define WIN_WIDTH 1280        //窗口宽度
#define WIN_HEIGHT 720        //窗口高度

#define FONT_SIZE 60          //字体大小
#define NUMBER_SIZE 40        //数字大小

#define CHESS_MOVE_SPEED 2    //棋子移动速度
#define CHESS_ROTATE_SPEED 2.0//棋子旋转速度
#define DICE_ROTATE_TIME 2000 //骰子旋转时间(ms)
#define DICE_ROTATE_SPEED 50  //骰子旋转速度

#define CFG_PATH "cfg/cfg.txt"//配置文件路径
#define LOG_PATH "cfg/log.txt"//日志文件路径
#define BGM_PATH "bgm/bgm.mp3"//背景音乐路径

#define ANIMATION_TIME 1000                         //动画时间(ms)
#define FRAME_RATE (60 * ANIMATION_TIME / 1000)     //动画帧率
#define ERROR_STEP 20                               //误差补偿次数
#define ERROR_RATE (FRAME_RATE / ERROR_STEP)        //误差补偿频率

//类型定义

typedef enum color{           //定义颜色
  RED = 1,                    //红 = 1
  GREEN = 2,                  //绿 = 2
  YELLOW = 3,                 //黄 = 3
  BLUE = 4,                   //蓝 = 4
  BLANK = 5
}color;

typedef enum direction{       //定义棋子指向
  U = 0,                      //上 = 顺时针旋转0°
  UR = 45,                    //右上 = 顺时针旋转45°
  R = 90,                     //右 = 顺时针旋转90°
  DR = 135,                   //右下 = 顺时针旋转135°
  D = 180,                    //下 = 顺时针旋转180°
  DL = 225,                   //左下 = 顺时针旋转225°
  L = 270,                    //左 = 顺时针旋转270°
  UL = 315                    //左上 = 顺时针旋转315°
}direction;

typedef enum ChessState{      //定义棋子的状态
  AIRPORT = 0,                //在机场
  MAIN = 1,                   //在主棋盘上
  RUNWAY_IN = 2,              //在终点线前进
  RUNWAY_OUT = 3,             //在终点线后退
  FINISH = 4                  //已到达终点
}ChessState;

typedef enum PlayerType{      //玩家类型
  Player = 1,                 //真人
  AI = 2                      //电脑
}type;

typedef struct chess{         //定义棋子
  int pos;                    //棋子所在的位置
  ChessState state;           //棋子的状态
  color color;                //棋子的颜色
  direction dir;              //棋子当前的指向
  SDL_Surface *sur;           //棋子的表面
  SDL_Texture *tex;           //棋子的纹理
  SDL_Rect rect;              //棋子的矩形
}chess;

typedef struct board{         //定义棋盘
  color color;                //当前格子的颜色
  direction dir;              //当前格子的指向
  int x;                      //当前格子的x坐标
  int y;                      //当前格子的y坐标
}board;

typedef struct GameState{     //游戏状态
  int total_number;           //游戏总人数
  int player_number;          //玩家人数
  int ai_number;              //电脑人数
  int round;                  //当前游戏轮数
  color player;               //当前玩家颜色
  type player_type;           //当前玩家类型
  char color_str[7];          //当前颜色字符串
  bool win[4];                //当前玩家胜利情况
}GameState;

//声明全局变量

extern GameState current_state;    //游戏状态
extern chess Chess[];              //棋盘上的棋子
extern const board vec[96];        //棋盘

extern int record;                 //是否记录日志
extern int debug;                  //调试模式
extern FILE *cfg;                  //配置文件指针
extern FILE *log_file;             //记录文件指针

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

extern TTF_Font *NumberFont;       //数字
extern SDL_Surface *NumberFontSurface;
extern SDL_Texture *NumberFontTexture;
extern SDL_Rect NumberFontRect;
extern SDL_Color NumberFontColor;

extern SDL_Surface *RedSurface;             //红色
extern SDL_Texture *RedTexture;
extern SDL_Surface *GreenSurface;           //绿色
extern SDL_Texture *GreenTexture;
extern SDL_Surface *YellowSurface;          //黄色
extern SDL_Texture *YellowTexture;
extern SDL_Surface *BlueSurface;            //蓝色
extern SDL_Texture *BlueTexture;

extern Mix_Music *bgm;                      //BGM

//main.c中的函数声明

int main_init(void);
void main_render(void);
void main_event_loop(void);
void main_load(void);
void recordf(const char* format, ...);
void quit(int code);

//game.c中的函数声明

void game(void);
void game_init(void);
int game_player(void);
void game_render(void);
void game_state_adjust(void);
void game_event(void);
void game_round(void);
void game_judge(void);

//dice.c中的函数声明

int dice_roll(void);
void dice_present(int roll);

//chess.c中的函数声明

void chess_init(void);
void chess_move(int num, int step);
void chess_move_line(int num, int vec_t);
void chess_move_rect(int num, int xt, int yt);
void chess_rotate(int num, double angle_t);
void chess_departure(int num);
int chess_click(void);
void chess_crash(int num);
bool chess_fly(int num);
void chess_fly_crash(int num, int depart_pos, int crash_pos, int dest_pos);
void chess_jump(int num);

//draw.c中的函数声明
void draw_text(char *text, int x, int y, bool pre);
void draw_number(int num, int x, int y, bool pre);

#endif