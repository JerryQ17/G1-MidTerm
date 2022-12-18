#include "FlightChess.h"

int SDL_main(int argc, char *argv[]) {
  //程序初始化
  int Init_value = main_init();
  //判断初始化是否正常
  if (Init_value){
    if (record) {
      fprintf(rec_file, "main_init error code %d\n", Init_value);
      fclose(rec_file);
    }
    quit();
    return Init_value;
  }
  //加载所有表面和纹理
  LoadPicture();
  //进入主事件循环
  main_event_loop();
  return 0;
}

int main_init(void) {   //程序初始化
  //读取设置文件
  cfg = fopen(CFG_PATH, "r");
  if (cfg != NULL) {
    if (fscanf(cfg, "record = %d\ndebug = %d", &record, &debug) < 2){
      //读取设置失败，不影响游戏运行，只需要报告错误
      printf("MainInit: Error occurred when loading configs, ");
      if (feof(cfg)) printf("EOF\n");
      if (ferror(cfg)) printf("Read Error\n");
      else printf("Match Error\n");
    }
    fclose(cfg);
  }else fprintf(stderr, "MainInit: Fail to find cfg.txt\n");
  //根据设置文件 以及日志文件是否能正常写入 来决定是否记录信息
  if (record == 1){
    rec_file = fopen(LOG_PATH, "a+");
    if (rec_file == NULL) record = 0;   //日志文件不能正常写入，不记录信息
    else{
      time_t cur_time = time(NULL);
      fprintf(rec_file, "MainInit: Program start at %srecord = %d debug = %d\n", ctime(&cur_time), record, debug);
    }
  }
  //SDL初始化
  if (SDL_Init(SDL_INIT_VIDEO)) {
    if (record) fprintf(rec_file, "MainInit: Cannot init video, %s\n", SDL_GetError());
    else SDL_Log("MainInit: Cannot init video, %s", SDL_GetError());
    return 1;   //SDL初始化失败则返回1
  }
  Window = SDL_CreateWindow("飞行棋",SDL_WINDOWPOS_CENTERED,SDL_WINDOWPOS_CENTERED,WIN_WIDTH,WIN_HEIGHT,SDL_WINDOW_SHOWN);
  if (Window == NULL) {
    if (record) fprintf(rec_file, "MainInit: Cannot create window, %s\n", SDL_GetError());
    else SDL_Log("MainInit: Cannot create window, %s", SDL_GetError());
    return 2;   //窗口初始化失败则返回2
  }
  Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED);
  if (Renderer == NULL) {
    if (record) fprintf(rec_file, "MainInit: Cannot create Renderer, %s\n", SDL_GetError());
    else SDL_Log("MainInit: Cannot create Renderer, %s", SDL_GetError());
    return 3;   //渲染器初始化失败则返回3
  }
  if (TTF_Init()) {
    if (record) fprintf(rec_file, "MainInit: Cannot init ttf, %s\n", TTF_GetError());
    else SDL_Log("MainInit: Cannot init ttf, %s", TTF_GetError());
    return 4;   //字体初始化失败则返回4
  }
  Font = TTF_OpenFont("font/calibri.ttf", FONT_SIZE);
  NumberFont = TTF_OpenFont("font/calibri.ttf", NUMBER_SIZE);
  if (Font == NULL || NumberFont == NULL) {
    if (record) fprintf(rec_file, "MainInit: Cannot open font, %s\n", SDL_GetError());
    else SDL_Log("MainInit: Cannot open font, %s", SDL_GetError());
    return 5;   //字体打开失败则返回5
  }
  return 0;     //初始化正常则返回0
}

void main_render(void){   //渲染mainUI
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, MainTexture, NULL, &MainRect);
  SDL_RenderPresent(Renderer);
}

void main_event_loop(void) {  //主事件循环
  //渲染mainUI
  main_render();
  //主事件循环
  SDL_Event MainEvent;
  while (SDL_WaitEvent(&MainEvent)) {
    switch (MainEvent.type) {
      case SDL_QUIT:  //关闭窗口
        if (record) fprintf(rec_file, "MainEventLoop: Quit by SDL_QUIT\n");
        quit();
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (MainEvent.key.keysym.sym) {
          case SDLK_RETURN: //回车
            if (record) fprintf(rec_file, "MainEventLoop: Game start by Keydown Enter\n");
            game();
            break;
          case SDLK_ESCAPE: //Esc
            if (record) fprintf(rec_file, "MainEventLoop: Quit by Esc\n");
            quit();
            break;
          default:break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (record) fprintf(rec_file, "MainEventLoop: Mouse button down (%d, %d)\n", MainEvent.button.x, MainEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        if (record) fprintf(rec_file, "MainEventLoop: Mouse button up (%d, %d)\n", MainEvent.button.x, MainEvent.button.y);
        if (MainEvent.button.x > 555 && MainEvent.button.x < 722 && MainEvent.button.y > 311 && MainEvent.button.y < 404){  //开始
          if (record) fprintf(rec_file, "MainEventLoop: Game start by start button\n");
          game();
        }else if (MainEvent.button.x > 555 && MainEvent.button.x < 722 && MainEvent.button.y > 463 && MainEvent.button.y < 547){ //帮助
          if (record) fprintf(rec_file, "MainEventLoop: Press get help button\n");
          int open_readme = system("start README.md");
          if (open_readme) {
            main_render();
            draw_text("Failed to get help:(", 400, 569);
          }
        }
        break;
      default:break;
    }
  }
}

void LoadPicture(void){   //加载所有表面和纹理
  if (record) fprintf(rec_file, "Loading picture...");
  //mainUI
  MainSurface = IMG_Load("img/art_mainUI.png");
  MainTexture = SDL_CreateTextureFromSurface(Renderer, MainSurface);
  MainRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
  //gameUI
  GameSurface = IMG_Load("img/board.png");
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
  if (record) fprintf(rec_file, "Loaded\n");
}

void draw_text(char *text, int x, int y){   //根据参数渲染文本
  FontSurface = TTF_RenderUTF8_Blended(Font, text, FontColor);
  FontTexture = SDL_CreateTextureFromSurface(Renderer, FontSurface);
  FontRect = (SDL_Rect){x, y, FontSurface->w, FontSurface->h};
  SDL_RenderCopy(Renderer, FontTexture, NULL, &FontRect);
  SDL_RenderPresent(Renderer);
  if (record) fprintf(rec_file, "DrawText(%d,%d): %s\n", x, y, text);
}

void draw_number(int num, int x, int y){   //根据参数渲染数字
  //处理数字
  int flag = 1, text_num = 0, number[11] = {0};
  char text[11] = {0};
  for (int i = 0; i < 11; i++){
    number[i] = num / (int)pow(10, 10 - i);
    num -= number[i] * (int)pow(10, 10 - i);
  }
  for (int i = 0; i < 11; i++){
    if (number[i] && flag) flag = 0;
    if (!flag) text[text_num++] = (char)(number[i] + 48);
  }
  //渲染数字
  NumberFontSurface = TTF_RenderUTF8_Blended(NumberFont, text, NumberFontColor);
  NumberFontTexture = SDL_CreateTextureFromSurface(Renderer, NumberFontSurface);
  NumberFontRect = (SDL_Rect){x, y, NumberFontSurface->w, NumberFontSurface->h};
  SDL_RenderCopy(Renderer, NumberFontTexture, NULL, &NumberFontRect);
  SDL_RenderPresent(Renderer);
  if (record) fprintf(rec_file, "DrawNumber(%d,%d): %s\n", x, y, text);
}

void draw_rect(int x, int y, int w, int h, Uint32 color){
  if (record) fprintf(rec_file, "DrawRect(%d,%d,%d,%d,%x)\n", x, y, w, h, color);
}

void quit(void) {   //销毁各指针并退出程序
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
    fprintf(rec_file, "Quit: Program stop at %s\n", ctime(&cur_time));
    fclose(rec_file);
  }
}