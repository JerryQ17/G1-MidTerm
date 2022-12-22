#include "FlightChess.h"

int dice_roll(void){   //debug = 1时根据键盘输入得到骰子的点数，debug = 0时取随机数，返回值是骰子的点数
  //debug = 1
  if (debug) {
    SDL_Event RollEvent;
    while (SDL_WaitEvent(&RollEvent)) {
      if (RollEvent.type == SDL_KEYDOWN) {
        switch (RollEvent.key.keysym.sym) { //Keydown1-6为对应点数，空格则随机得到一个点数，其它操作无效
          case SDLK_1:case SDLK_KP_1:
            recordf("DiceRoll: dice 1(debug)\n");
            return 1;
          case SDLK_2:case SDLK_KP_2:
            recordf("DiceRoll: dice 2(debug)\n");
            return 2;
          case SDLK_3:case SDLK_KP_3:
            recordf("DiceRoll: dice 3(debug)\n");
            return 3;
          case SDLK_4:case SDLK_KP_4:
            recordf("DiceRoll: dice 4(debug)\n");
            return 4;
          case SDLK_5:case SDLK_KP_5:
            recordf("DiceRoll: dice 5(debug)\n");
            return 5;
          case SDLK_6:case SDLK_KP_6:
            recordf("DiceRoll: dice 6(debug)\n");
            return 6;
          case SDLK_SPACE:
            recordf("DiceRoll: goto random(debug);\n");
            goto random;
          default: break;
        }
      } else if (RollEvent.type == SDL_QUIT) {  //等待操作时防止出现不能退出程序的bug
        recordf("DiceRoll: Quit by SDL_QUIT\n");
        quit(EXIT_SUCCESS);
      }
    }
  }
  //debug = 0
  random:
  srand((unsigned int) time(NULL));
  int value = rand() % 7;
  while (value == 0) value = rand() % 7;
  recordf("DiceRoll: dice %d(random)\n", value);
  return value;
}

void dice_present(int roll){  //加载roll函数给出的骰子点数对应的动画
  //处理要加载的文件名
  roll += 48;
  char file_name[] = {'i', 'm', 'g', '/', (char)roll, '.', 'p', 'n', 'g', 0};
  //加载并渲染对应文件
  SDL_Surface *DieSurface = IMG_Load(file_name);
  SDL_Texture *DieTexture = SDL_CreateTextureFromSurface(Renderer, DieSurface);
  SDL_Rect DieRect = {100, 550, DieSurface->w, DieSurface->h};
  //骰子旋转的动画
  double angle = 0;
  uint32_t start = SDL_GetTicks(), cur = SDL_GetTicks();
  while(cur - start <= DICE_ROTATE_TIME){   //通过旋转和翻转6.png来模拟骰子的旋转
    uint32_t begin = SDL_GetTicks();
    SDL_Texture *RotateTexture = IMG_LoadTexture(Renderer, "img/6.png");
    SDL_RenderCopyEx(Renderer, RotateTexture, NULL, &DieRect, angle, NULL, SDL_FLIP_HORIZONTAL);
    angle += DICE_ROTATE_SPEED;
    SDL_RenderPresent(Renderer);
    SDL_DestroyTexture(RotateTexture);
    cur = SDL_GetTicks();
    long long delay_time = (long long)(ANIMATION_TIME / FRAME_RATE) - (cur - begin);
    if (delay_time > 0) SDL_Delay(delay_time);
  }
  //渲染最终的骰子图片
  SDL_RenderCopy(Renderer, DieTexture, NULL, &DieRect);
  SDL_RenderPresent(Renderer);
  //销毁表面和纹理
  SDL_FreeSurface(DieSurface);
  SDL_DestroyTexture(DieTexture);
}
