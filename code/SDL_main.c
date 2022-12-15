#include "FlightChess.h"

int SDL_main(int argc, char *argv[]) {
  //程序初始化
  int Init_value = Init();
  //判断初始化是否正常
  if (Init_value){
    if (record) {
      fprintf(rec_file, "Init error code %d\n", Init_value);
      fclose(rec_file);
    }
    quit();
    return Init_value;
  }
  //加载主界面UI
  LoadPicture();
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, MainTexture, NULL, &MainRect);
  SDL_RenderPresent(Renderer);
  //进入事件循环
  main_event_loop();
  //退出
  quit();
  return 0;
}

int Init(void) { //程序初始化
  //读取设置文件
  cfg = fopen("code/cfg.txt", "r");
  if (cfg != NULL) {
    fscanf(cfg, "record = %d\ndebug = %d", &record, &debug);
    fclose(cfg);
  }
  //根据设置文件 以及日志文件是否能正常写入 来决定是否记录信息
  if (record == 1){
    rec_file = fopen("code/log.txt", "a+");
    if (rec_file == NULL) record = 0;  //日志文件不能正常写入，不记录信息
    else{
      time_t cur_time = time(NULL);
      fprintf(rec_file, "Program start at %srecord = %d debug = %d\n", ctime(&cur_time), record, debug);
    }
  }
  //SDL初始化
  if (SDL_Init(SDL_INIT_VIDEO)) {
    if (record) fprintf(rec_file, "Cannot init video, %s\n", SDL_GetError());
    else SDL_Log("Cannot init video, %s", SDL_GetError());
    return 1;   //SDL初始化失败则返回1
  }
  Window = SDL_CreateWindow("飞行棋",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIN_WIDTH,WIN_HEIGHT,SDL_WINDOW_SHOWN);
  if (Window == NULL) {
    if (record) fprintf(rec_file, "Cannot create window, %s\n", SDL_GetError());
    else SDL_Log("Cannot create window, %s", SDL_GetError());
    return 2;   //窗口初始化失败则返回2
  }
  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
  if (Renderer == NULL) {
    if (record) fprintf(rec_file, "Cannot create Renderer, %s\n", SDL_GetError());
    else SDL_Log("Cannot create Renderer, %s", SDL_GetError());
    return 3;   //渲染器初始化失败则返回3
  }
  if (TTF_Init()) {
    if (record) fprintf(rec_file, "Cannot init ttf, %s\n", TTF_GetError());
    else SDL_Log("Cannot init ttf, %s", TTF_GetError());
    return 4;   //字体初始化失败则返回4
  }
  Font = TTF_OpenFont("font/calibri.ttf", FONT_SIZE);
  if (Font == NULL) {
    if (record) fprintf(rec_file, "Cannot open font, %s\n", SDL_GetError());
    else SDL_Log("Cannot open font, %s", SDL_GetError());
    return 5;   //字体打开失败则返回5
  }
  return 0;     //初始化正常则返回0
}

void main_event_loop(void) {
  uint32_t begin_time = SDL_GetTicks();
  SDL_Event MainEvent;
  while (SDL_WaitEvent(&MainEvent)) {
    switch (MainEvent.type) {
      case SDL_QUIT:  //关闭窗口
        if (record) fprintf(rec_file, "Quit by SDL_QUIT\n");
        quit();
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (MainEvent.key.keysym.sym) {
          case SDLK_RETURN: //回车
            if (record) fprintf(rec_file, "Game start by Keydown Enter\n");
            game_init();
            break;
          case SDLK_ESCAPE: //Esc
            if (record) fprintf(rec_file, "Quit by Esc\n");
            quit();
            break;
          default:break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (record) fprintf(rec_file, "Mouse button down (%d, %d)\n", MainEvent.button.x, MainEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        if (record) fprintf(rec_file, "Mouse button up (%d, %d)\n", MainEvent.button.x, MainEvent.button.y);
        if (MainEvent.button.x > 555 && MainEvent.button.x < 722 && MainEvent.button.y > 311 && MainEvent.button.y < 404){  //开始
          if (record) fprintf(rec_file, "Game start by start button\n");
          game_init();
        }else if (MainEvent.button.x > 555 && MainEvent.button.x < 722 && MainEvent.button.y > 463 && MainEvent.button.y < 547){ //帮助
          if (record) fprintf(rec_file, "Press get help button\n");
          int open_readme = system("start README.md");
          if (!open_readme) help_fail();
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
  MainSurface = IMG_Load("img/art_mainUI.png");
  MainTexture = SDL_CreateTextureFromSurface(Renderer, MainSurface);
  MainRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
  GameSurface = IMG_Load("img/board.png");
  GameTexture = SDL_CreateTextureFromSurface(Renderer, GameSurface);
  GameRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
}

void help_fail(void){
  FontSurface = TTF_RenderUTF8_Blended(Font, "Failed to get help:(", FontColor);
  FontTexture = SDL_CreateTextureFromSurface(Renderer, FontSurface);
  FontRect = (SDL_Rect){400, 569, FontSurface->w, FontSurface->h};
  SDL_RenderCopy(Renderer, FontTexture, NULL, &FontRect);
  SDL_RenderPresent(Renderer);
  if (record){
    fprintf(rec_file, "Failed to get help\n");
  }
}

void quit(void) {
  TTF_CloseFont(Font);
  SDL_FreeSurface(MainSurface);
  SDL_FreeSurface(GameSurface);
  SDL_FreeSurface(FontSurface);
  SDL_DestroyTexture(MainTexture);
  SDL_DestroyTexture(GameTexture);
  SDL_DestroyTexture(FontTexture);
  SDL_DestroyRenderer(Renderer);
  SDL_DestroyWindow(Window);
  SDL_Quit();
  if (record) {
    time_t cur_time = time(NULL);
    fprintf(rec_file, "Program stop at %s\n", ctime(&cur_time));
    fclose(rec_file);
  }
}