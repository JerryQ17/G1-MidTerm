#include "FlightChess.h"

int SDL_main(int argc,char *argv[]){
  if (SDL_Init(SDL_INIT_VIDEO)){
    SDL_Log("Cannot init video,%s", SDL_GetError());
    return 1;
  }
  Window = SDL_CreateWindow("飞行棋", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIN_WIDTH, WIN_HEIGHT, SDL_WINDOW_SHOWN);
  if (Window == NULL){
    SDL_Log("Cannot create window,%s", SDL_GetError());
    return 2;
  }
  MainBackGroundSurface = SDL_GetWindowSurface(Window);
  MainBackGroundRect = (SDL_Rect){0, 0, WIN_WIDTH, WIN_HEIGHT};
  SDL_FillRect(MainBackGroundSurface, &MainBackGroundRect, 0xffffffff);
  SDL_UpdateWindowSurface(Window);
  event_loop();
  SDL_DestroyWindow(Window);
  return 0;
}

void event_loop(void){
  while (true){
    SDL_Event event;
    if (SDL_PollEvent(&event)){
      if (event.type == SDL_QUIT) break;
    }
  }
}