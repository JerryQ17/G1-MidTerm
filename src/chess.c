#include "FlightChess.h"

void chess_init(void){  //初始化棋子
  int temp = 52;
  for (int i = 0; i < current_state.total_number * 4; i++, temp++){
    if (i % 4 == 0 && i) temp++;
    Chess[i].num = i % 4 + 1;
    Chess[i].pos = temp;
    Chess[i].state = AIRPORT;
    Chess[i].color = i / 4 + 1;
    Chess[i].dir = board_vec[temp].dir;
    switch (Chess[i].color){
      case RED:
        Chess[i].sur = RedSurface;
        Chess[i].tex = RedTexture;
        break;
      case GREEN:
        Chess[i].sur = GreenSurface;
        Chess[i].tex = GreenTexture;
        break;
      case YELLOW:
        Chess[i].sur = YellowSurface;
        Chess[i].tex = YellowTexture;
        break;
      case BLUE:
        Chess[i].sur = BlueSurface;
        Chess[i].tex = BlueTexture;
        break;
      default:break;
    }
    Chess[i].rect = (SDL_Rect){board_vec[temp].x, board_vec[temp].y, Chess[i].sur->w, Chess[i].sur->h};
  }
}

