#include "FlightChess.h"

//定义全局变量

GameState current_state;                    //游戏状态
chess Chess[16];                            //棋盘上的棋子

int record = 0;                             //是否记录日志
int debug = 0;                              //调试模式
FILE *cfg = NULL;                           //配置文件指针
FILE *log_file = NULL;                      //记录文件指针

SDL_Window *Window = NULL;                  //窗口
SDL_Renderer *Renderer = NULL;              //渲染器

SDL_Surface *MainSurface = NULL;            //mainUI
SDL_Texture *MainTexture = NULL;
SDL_Rect MainRect;

SDL_Surface *GameSurface = NULL;            //gameUI
SDL_Texture *GameTexture = NULL;
SDL_Rect GameRect;

TTF_Font *Font = NULL;                      //字体
SDL_Surface *FontSurface = NULL;
SDL_Texture *FontTexture = NULL;
SDL_Rect FontRect;
SDL_Color FontColor = {0, 0, 0, 255};

TTF_Font *NumberFont = NULL;                //数字
SDL_Surface *NumberFontSurface = NULL;
SDL_Texture *NumberFontTexture = NULL;
SDL_Rect NumberFontRect;
SDL_Color NumberFontColor = {0, 0, 0, 255};

SDL_Surface *RedSurface = NULL;             //红色
SDL_Texture *RedTexture = NULL;
SDL_Surface *GreenSurface = NULL;           //绿色
SDL_Texture *GreenTexture = NULL;
SDL_Surface *YellowSurface = NULL;          //黄色
SDL_Texture *YellowTexture = NULL;
SDL_Surface *BlueSurface = NULL;            //蓝色
SDL_Texture *BlueTexture = NULL;

Mix_Music *bgm;                            //BGM

const board vec[96] = {                                                                                                     //棋盘(用函数初始化非常困难，不如打表)
    {BLUE,DL,903,462},{RED,L,860,478},{GREEN,L,820,478},          //-------------
    {YELLOW,DL,780,460},{BLUE,DR,745,500},{RED,D,760,540},        //     |
    {GREEN,D,760,580},{YELLOW,DL,745,620},{BLUE,L,702,637},       //     |
    {RED,L,662,637},{BLANK,L,623,637},{YELLOW,L,583,637},       //     |
    {BLUE,L,544,637},{RED,UL,502,620},{GREEN,U,485,580},       //     |
    {YELLOW,U,485,540},{BLUE,UL,502,500},{RED,DL,465,460},     //     |
    {GREEN,L,425,480},{YELLOW,L,386,480},{BLUE,UL,346,460},    //     |
    {RED,U,328,420},{GREEN,U,328,381},{BLANK,U,328,343},      //     |
    {BLUE,U,328,305},{RED,U,328,266},{GREEN,UR,346,225},       //0 - 51是主棋盘
    {YELLOW,R,385,205},{BLUE,R,424,205},{RED,UR,465,225},      //     |
    {GREEN,UL,502,188},{YELLOW,U,485,147},{BLUE,U,485,107},    //     |
    {RED,UR,502,65},{GREEN,R,543,50},{YELLOW,R,583,50},        //     |
    {BLANK,R,622,50},{RED,R,662,50},{GREEN,R,701,50},           //     |
    {YELLOW,DR,745,65},{BLUE,D,762,106},{RED,D,762,147},       //     |
    {GREEN,DR,745,188},{YELLOW,UR,780,226},{BLUE,R,820,206},   //     |
    {RED,R,860,206},{GREEN,DR,903,226},{YELLOW,D,914,266},     //     |
    {BLUE,D,914,305},{BLANK,D,914,344},{GREEN,D,914,381},        //     |
    {YELLOW,D,914,420},{RED,U,854,576},{RED,U,916,576},        //-------------
    {RED,U,854,635},{RED,U,916,635},{RED,L,957,492},           //52-56是红色机场
    {GREEN,R,331,575},{GREEN,R,390,576},{GREEN,R,331,636},     //57-61是绿色机场
    {GREEN,R,390,636},{GREEN,U,474,677},{YELLOW,D,330,52},     //     |
    {YELLOW,D,391,52},{YELLOW,D,330,111},{YELLOW,D,391,111},   //62-66是黄色机场
    {YELLOW,R,289,196},{BLUE,L,854,51},{BLUE,L,915,51},        //67-71是蓝色机场
    {BLUE,L,854,111},{BLUE,L,915,111},{BLUE,D,771,11},         //-------------
    {BLANK,L,860,343},{BLANK,L,820,343},{BLANK,L,781,343},     //     |
    {BLANK,L,742,343},{BLANK,L,702,343},{BLANK,L,659,343},      //72-77是红色终点
    {BLANK,U,623,580},{BLANK,U,623,541},{BLANK,U,623,501},     //     |
    {BLANK,U,623,462},{BLANK,U,623,423},{BLANK,U,623,381},     //78-83是绿色终点
    {BLANK,R,385,344},{BLANK,R,425,344},{BLANK,R,464,344},     //     |
    {BLANK,R,504,344},{BLANK,R,543,344},{BLANK,R,587,344},     //84-89是黄色终点
    {BLANK,D,622,106},{BLANK,D,622,145},{BLANK,D,622,184},     //     |
    {BLANK,D,622,224},{BLANK,D,622,263},{BLANK,D,622,307}      //90-95是蓝色终点
};                                                                                                                               //-------------