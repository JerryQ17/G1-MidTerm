#include "FlightChess.h"

void game_init(void){  //游戏部分的初始化
  //对游戏部分要用到的变量进行初始化
  GameRound = 0;
  cur_player = 5;
  //渲染游戏UI
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, GameTexture, NULL, &GameRect);
  SDL_RenderPresent(Renderer);
  //决定哪一方先开始
  while (cur_player > 4) cur_player = roll();
  switch (cur_player) {
    case RED:
      if (record) fprintf(rec_file, "Red is the starter\n");
      break;
    case GREEN:
      if (record) fprintf(rec_file, "Green is the starter\n");
      break;
    case YELLOW:
      if (record) fprintf(rec_file, "Yellow is the starter\n");
      break;
    case BLUE:
      if (record) fprintf(rec_file, "Blue is the starter\n");
      break;
  }
  game_event_loop();
}

void game_event_loop(void){

}

int roll(void){   //debug = 1时根据键盘输入得到骰子的点数，debug = 0时取随机数，返回值是骰子的点数
  //debug = 1
  if (debug) {
    SDL_Event RollEvent;
    while (SDL_WaitEvent(&RollEvent)) {
      if (RollEvent.type == SDL_KEYDOWN) {
        switch (RollEvent.key.keysym.sym) { //Keydown1-6为对应点数，空格则随机得到一个点数，其它操作无效
          case SDLK_1:case SDLK_KP_1:
            if (record) fprintf(rec_file, "Debug:dice 1\n");
            return 1;
          case SDLK_2:case SDLK_KP_2:
            if (record) fprintf(rec_file, "Debug:dice 2\n");
            return 2;
          case SDLK_3:case SDLK_KP_3:
            if (record) fprintf(rec_file, "Debug:dice 3\n");
            return 3;
          case SDLK_4:case SDLK_KP_4:
            if (record) fprintf(rec_file, "Debug:dice 4\n");
            return 4;
          case SDLK_5:case SDLK_KP_5:
            if (record) fprintf(rec_file, "Debug:dice 5\n");
            return 5;
          case SDLK_6:case SDLK_KP_6:
            if (record) fprintf(rec_file, "Debug:dice 6\n");
            return 6;
          case SDLK_SPACE:
            if (record) fprintf(rec_file, "Debug:");
            goto random;
          default: break;
        }
      } else if (RollEvent.type == SDL_QUIT) {  //等待操作时防止出现不能退出程序的bug
        if (record) fprintf(rec_file, "Quit by SDL_QUIT\n");
        quit();
        exit(0);
      }
    }
  }
  //debug = 0
  random:
  srand((unsigned int) time(NULL));
  int value = rand() % 7;
  while (value == 0) value = rand() % 7;
  if (record) fprintf(rec_file, "random dice %d\n", value);
  return value;
}

void die_present(int num){  //加载roll函数给出的骰子点数对应的动画
  //处理要加载的文件名
  num += 48;
  char file_name[] = {'i','m','g','/',(char)num,'.','g','i','f',0};
  //加载并渲染对应文件
  SDL_Surface *DieSurface = IMG_Load(file_name);
  SDL_Texture *DieTexture = SDL_CreateTextureFromSurface(Renderer, DieSurface);
  SDL_Rect DieRect = {100, 600, DieSurface->w, DieSurface->h};
  SDL_RenderCopy(Renderer, DieTexture, NULL, &DieRect);
  SDL_RenderPresent(Renderer);
  //销毁表面和纹理
  SDL_FreeSurface(DieSurface);
  SDL_DestroyTexture(DieTexture);
}