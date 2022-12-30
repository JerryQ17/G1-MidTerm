#include "FlightChess.h"

void draw_text(char *text, int x, int y, bool pre){   //根据参数渲染文本
  FontSurface = TTF_RenderUTF8_Blended(Font, text, FontColor);
  FontTexture = SDL_CreateTextureFromSurface(Renderer, FontSurface);
  FontRect = (SDL_Rect){x, y, FontSurface->w, FontSurface->h};
  SDL_RenderCopy(Renderer, FontTexture, NULL, &FontRect);
  if (pre) SDL_RenderPresent(Renderer);
}

void draw_number(int num, int x, int y, bool pre){   //根据参数渲染数字
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
  if (!text[0]) text[0] = '0';
  //渲染数字
  NumberFontSurface = TTF_RenderUTF8_Blended(NumberFont, text, NumberFontColor);
  NumberFontTexture = SDL_CreateTextureFromSurface(Renderer, NumberFontSurface);
  NumberFontRect = (SDL_Rect){x, y, NumberFontSurface->w, NumberFontSurface->h};
  SDL_RenderCopy(Renderer, NumberFontTexture, NULL, &NumberFontRect);
  if (pre) SDL_RenderPresent(Renderer);
}