#include "FlightChess.h"

void game(void){
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, PlayBackGroundTexture, NULL, NULL);
  SDL_RenderPresent(Renderer);
  printf("game\n");
}

void print(void){
  FontSurface = TTF_RenderUTF8_Blended(Font, "114514", FontColor);
  FontTexture = SDL_CreateTextureFromSurface(Renderer, FontSurface);
  //FontRect = (SDL_Rect){580, 38, FontSurface->w, FontSurface->h};
  FontRect.x = 100;
  FontRect.y = 100;
  FontRect.w = FontSurface->w;
  FontRect.h = FontSurface->h;
  SDL_RenderCopy(Renderer, FontTexture, NULL, &FontRect);
  SDL_RenderPresent(Renderer);
}