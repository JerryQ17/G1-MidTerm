#include "FlightChess.h"

int SDL_main(int argc, char *argv[]) {
  //SDL各部分初始化
  int load_value = load();
  if (load_value) return load_value;
  //加载主界面背景图片
  LoadPicture();
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, MainBackGroundTexture, NULL, &MainBackGroundRect);
  SDL_RenderPresent(Renderer);
  //进入事件循环
  event_loop();
  //退出
  quit();
  return 0;
}

int load(void) { //SDL各部分初始化
  if (SDL_Init(SDL_INIT_VIDEO)) {
    SDL_Log("Cannot init video,%s", SDL_GetError());
    return 1;
  }
  Window = SDL_CreateWindow("飞行棋",
                            SDL_WINDOWPOS_CENTERED,
                            SDL_WINDOWPOS_CENTERED,
                            WIN_WIDTH,
                            WIN_HEIGHT,
                            SDL_WINDOW_SHOWN);
  if (Window == NULL) {
    SDL_Log("Cannot create window,%s", SDL_GetError());
    return 2;
  }
  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
  if (Renderer == NULL) {
    SDL_Log("Cannot create Renderer,%s", SDL_GetError());
    return 3;
  }
  if (TTF_Init()) {
    SDL_Log("Cannot init ttf,%s", TTF_GetError());
    return 4;
  }
  Font = TTF_OpenFont("font/calibri.ttf", FONT_SIZE);
  if (Font == NULL) {
    SDL_Log("Cannot open font,%s", SDL_GetError());
    return 5;
  }
  return 0;
}

void event_loop(void) {
  uint32_t begin_time = SDL_GetTicks();
  SDL_Event event;
  while (SDL_WaitEvent(&event)) {
    switch (event.type) {
      case SDL_QUIT:  //关闭窗口
        quit();
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (event.key.keysym.sym) {
          case SDLK_RETURN: //回车
            game();
            break;
          case SDLK_ESCAPE: //Esc
            quit();
            break;
          default:break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        printf("(%d, %d)\n", event.button.x, event.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        if (event.button.x > 556 && event.button.x < 721 && event.button.y > 312 && event.button.y < 403){
          print();
        }
        break;
      default:break;
    }
  }

  uint32_t current_time = SDL_GetTicks();
  long long delay_time = (1000 / FRAMERATE) - (current_time - begin_time);
  if (delay_time > 0) SDL_Delay(delay_time);
}

void LoadPicture(void){
  MainBackGroundSurface = IMG_Load("img/art_mainUI.png");
  MainBackGroundTexture = SDL_CreateTextureFromSurface(Renderer, MainBackGroundSurface);
  MainBackGroundRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
  PlayBackGroundSurface = IMG_Load("img/board.png");
  PlayBackGroundTexture = SDL_CreateTextureFromSurface(Renderer, PlayBackGroundSurface);
  PlayBackGroundRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
}

void quit(void) {
  TTF_CloseFont(Font);
  SDL_FreeSurface(MainBackGroundSurface);
  SDL_FreeSurface(PlayBackGroundSurface);
  SDL_FreeSurface(FontSurface);
  SDL_DestroyTexture(MainBackGroundTexture);
  SDL_DestroyTexture(PlayBackGroundTexture);
  SDL_DestroyTexture(FontTexture);
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_Quit();
}