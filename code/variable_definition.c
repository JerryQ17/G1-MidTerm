#include "FlightChess.h"

//定义全局变量
int GameRound = 0;                          //当前游戏轮数
color cur_player = 5;                       //当前玩家颜色

int record = 0;                             //是否记录日志
int debug = 0;                              //调试模式
FILE *cfg = NULL;                           //配置文件指针
FILE *rec_file = NULL;                      //记录文件指针

SDL_Window *Window = NULL;                  //窗口
SDL_Renderer *Renderer = NULL;              //渲染器

SDL_Surface *MainSurface = NULL;            //主界面
SDL_Texture *MainTexture = NULL;
SDL_Rect MainRect;

SDL_Surface *GameSurface = NULL;            //游戏界面
SDL_Texture *GameTexture = NULL;
SDL_Rect GameRect;

TTF_Font *Font = NULL;                      //字体
SDL_Surface *FontSurface = NULL;
SDL_Texture *FontTexture = NULL;
SDL_Rect FontRect;
SDL_Color FontColor = {0, 0, 0, 255};