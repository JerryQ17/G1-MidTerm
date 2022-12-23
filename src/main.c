#include "FlightChess.h"

int SDL_main(int argc, char *argv[]) {
  //程序初始化
  int init_value = main_init();
  //判断初始化是否正常
  if (init_value){
    if (record) {
      recordf("SDL_main: MainInit Error Code %d\n\n", init_value);
      fclose(log_file);
    }
    quit(init_value);
  }
  //加载所有表面和纹理
  main_load();
  //进入主事件循环
  main_event_loop();
  return 0;
}

int main_init(void) {   //程序初始化
  //读取设置文件
  cfg = fopen(CFG_PATH, "r");
  if (cfg != NULL) {
    if (fscanf(cfg, "record=%d\ndebug=%d", &record, &debug) < 2){
      //读取设置失败，不影响游戏运行，只需要报告错误
      fprintf(stderr, "MainInit: Error occurred when loading configs, ");
      if (feof(cfg)) fprintf(stderr, "EOF\n");
      if (ferror(cfg)) fprintf(stderr, "Read Error\n");
      else fprintf(stderr, "Match Error\n");
    }
    fclose(cfg);
  }else fprintf(stderr, "MainInit: Fail to find cfg.txt\n");
  //根据设置文件 以及日志文件是否能正常写入 来决定是否记录信息
  if (record){
    log_file = fopen(LOG_PATH, "a+");
    if (log_file == NULL) {   //日志文件不能正常写入，不记录信息
      record = 0;
      fprintf(stderr, "Failed to get log");
    }
    else{
      time_t cur_time = time(NULL);
      recordf("MainInit: Program start at %srecord = %d debug = %d\n", ctime(&cur_time), record, debug);
    }
  }
  //SDL初始化
  if (SDL_Init(SDL_INIT_VIDEO)) {
    recordf("MainInit: Cannot init video, %s\n", SDL_GetError());
    SDL_Log("MainInit: Cannot init video, %s\n", SDL_GetError());
    return 1;   //SDL初始化失败则返回1
  }
  Window = SDL_CreateWindow("飞行棋",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIN_WIDTH,WIN_HEIGHT,SDL_WINDOW_SHOWN);
  if (Window == NULL) {
    recordf("MainInit: Cannot create window, %s\n", SDL_GetError());
    SDL_Log("MainInit: Cannot create window, %s\n", SDL_GetError());
    return 2;   //窗口初始化失败则返回2
  }
  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
  if (Renderer == NULL) {
    recordf("MainInit: Cannot create Renderer, %s\n", SDL_GetError());
    SDL_Log("MainInit: Cannot create Renderer, %s\n", SDL_GetError());
    return 3;   //渲染器初始化失败则返回3
  }
  if (TTF_Init()) {
    recordf("MainInit: Cannot init ttf, %s\n", TTF_GetError());
    SDL_Log("MainInit: Cannot init ttf, %s\n", TTF_GetError());
    return 4;   //字体初始化失败则返回4
  }
  Font = TTF_OpenFont("font/calibri.ttf", FONT_SIZE);
  NumberFont = TTF_OpenFont("font/calibri.ttf", NUMBER_SIZE);
  if (Font == NULL || NumberFont == NULL) {
    recordf("MainInit: Cannot open font, %s\n", SDL_GetError());
    SDL_Log("MainInit: Cannot open font, %s\n", SDL_GetError());
    return 5;   //字体打开失败则返回5
  }
  if (Mix_OpenAudio(MIX_DEFAULT_FREQUENCY, MIX_DEFAULT_FORMAT, MIX_DEFAULT_CHANNELS, 4096) == -1) {
    recordf("MainInit: Cannot open audio, %s\n", SDL_GetError());
    SDL_Log("MainInit: Cannot open audio, %s\n", SDL_GetError());
    return 6;   //音频打开失败则返回6
  }
  return 0;     //初始化正常则返回0
}

void main_render(void){   //渲染mainUI
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, MainTexture, NULL, &MainRect);
  SDL_RenderPresent(Renderer);
}

void main_event_loop(void) {  //主事件循环
  //播放音乐
  Mix_PlayMusic(bgm, -1);
  //主事件循环
  SDL_Event MainEvent;
  while (SDL_WaitEvent(&MainEvent)) {
    //渲染mainUI
    main_render();
    switch (MainEvent.type) {
      case SDL_QUIT:  //关闭窗口
        recordf("MainEventLoop: Quit by SDL_QUIT\n");
        quit(EXIT_SUCCESS);
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (MainEvent.key.keysym.sym) {
        case SDLK_RETURN:case SDLK_SPACE: //回车或空格开始游戏
            recordf("MainEventLoop: Game start by Keydown Enter/Space\n\n");
            game();
            break;
          case SDLK_ESCAPE: //Esc
            recordf("MainEventLoop: Quit by Esc\n");
            quit(EXIT_SUCCESS);
            break;
          default:break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        recordf("MainEventLoop: Mouse button down (%d, %d)\n", MainEvent.button.x, MainEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        recordf("MainEventLoop: Mouse button up (%d, %d)\n", MainEvent.button.x, MainEvent.button.y);
        if (MainEvent.button.x > 555 && MainEvent.button.x < 722 && MainEvent.button.y > 311 && MainEvent.button.y < 404){  //开始
          recordf("MainEventLoop: Game start by start button\n\n");
          game();
        }else if (MainEvent.button.x > 555 && MainEvent.button.x < 722 && MainEvent.button.y > 463 && MainEvent.button.y < 547){ //帮助
          recordf("MainEventLoop: Press get help button\n");
          int open_readme = system("start README.md");
          if (open_readme) {
            main_render();
            draw_text("Failed to get help:(", 400, 569);
            recordf("MainEventLoop: Failed to open readme\n");
          }
        }
        break;
      default:break;
    }
  }
}

void main_load(void){   //加载所有资源
  //mainUI
  MainSurface = IMG_Load("img/art_mainUI.png");
  MainTexture = SDL_CreateTextureFromSurface(Renderer, MainSurface);
  MainRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
  //gameUI
  GameSurface = IMG_Load("img/gameUI.png");
  GameTexture = SDL_CreateTextureFromSurface(Renderer, GameSurface);
  GameRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
  //chess
  RedSurface = IMG_Load("img/RedChess.png");
  RedTexture = SDL_CreateTextureFromSurface(Renderer, RedSurface);
  GreenSurface = IMG_Load("img/GreenChess.png");
  GreenTexture = SDL_CreateTextureFromSurface(Renderer, GreenSurface);
  YellowSurface = IMG_Load("img/YellowChess.png");
  YellowTexture = SDL_CreateTextureFromSurface(Renderer, YellowSurface);
  BlueSurface = IMG_Load("img/BlueChess.png");
  BlueTexture = SDL_CreateTextureFromSurface(Renderer, BlueSurface);
  recordf("LoadPicture: Complete!\n");
  //bgm
  bgm = Mix_LoadMUS(BGM_PATH);
  Mix_VolumeMusic(45);
}

void recordf(const char* format, ...){    //向日志文件中记录信息
  if (record){
    va_list ap;
    va_start(ap, format);
    vfprintf(log_file, format, ap);
    va_end(ap);
  }
}

void quit(int code) {   //销毁各指针并退出程序
  //close music
  Mix_HaltMusic();
  Mix_FreeMusic(bgm);
  Mix_Quit();
  //close font
  TTF_CloseFont(Font);
  TTF_CloseFont(NumberFont);
  //close surface
  SDL_FreeSurface(MainSurface);
  SDL_FreeSurface(GameSurface);
  SDL_FreeSurface(FontSurface);
  SDL_FreeSurface(NumberFontSurface);
  SDL_FreeSurface(RedSurface);
  SDL_FreeSurface(GreenSurface);
  SDL_FreeSurface(YellowSurface);
  SDL_FreeSurface(BlueSurface);
  //destroy texture
  SDL_DestroyTexture(MainTexture);
  SDL_DestroyTexture(GameTexture);
  SDL_DestroyTexture(FontTexture);
  SDL_DestroyTexture(NumberFontTexture);
  SDL_DestroyTexture(RedTexture);
  SDL_DestroyTexture(GreenTexture);
  SDL_DestroyTexture(YellowTexture);
  SDL_DestroyTexture(BlueTexture);
  //destroy renderer
  SDL_DestroyRenderer(Renderer);
  //destroy window
  SDL_DestroyWindow(Window);
  //quit
  SDL_Quit();
  //record stop time
  if (record) {
    time_t cur_time = time(NULL);
    recordf("Quit: Program stop at %s\n", ctime(&cur_time));
    fclose(log_file);
  }
  exit(code);
}