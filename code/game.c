#include "FlightChess.h"

void game(void){
  //初始化游戏
  game_init();
  if (record) fprintf(rec_file, "Game Start:\nTotal number = %d\nPlayer number = %d\nAI number = %d\n",
                      current_state.total_number, current_state.player_number, current_state.ai_number);
  //游戏开始
  current_state.round++;
  if (record) fprintf(rec_file, "Round%d %s%s:\n", current_state.round,
                      current_state.player_type == Player ? "Player" : "AI", current_state.color_str);
  draw_text("Round",0,50);
  draw_number(current_state.round, 170, 65);
  //判断当前玩家是否有待起飞的飞机
  bool flag_airport = false;
  for (int i = (int)(current_state.player - 1) * 4; i < current_state.player * 4; i++){
    if (Chess[i].state == AIRPORT) flag_airport = true;
  }

  //跳转至下一玩家
  bool flag_jump = true;
  while (flag_jump) {
    if (current_state.player == current_state.total_number) current_state.player = RED;
    else current_state.player++;
    for (int i = (int) (current_state.player - 1) * 4; i < current_state.player * 4; i++) {
      if (Chess[i].state != FINISH) flag_jump = false;
    }
  }
}

void game_init(void){  //游戏部分的初始化
  //确定游戏人数
  while(set_player());
  //对游戏要用到的部分变量进行初始化
  current_state.round = 0;
  current_state.player = RED;
  game_state_adjust();
  //初始化棋子
  chess_init();
  //渲染gameUI和棋子
  game_render();
}

int set_player(void){  //确定游戏人数
  SDL_Event SetPlayerEvent;
  for (int flag = 0; flag < 2; flag++) {
    main_render();
    if (!flag) draw_text("Please Input Total Player", 360, 569);  //先输入总人数
    else draw_text("Please Input AI Player", 400, 569);           //再输入电脑数
    Again:
    SDL_WaitEvent(&SetPlayerEvent);
    switch (SetPlayerEvent.type) {
      case SDL_QUIT:  //关闭窗口
        if (record) fprintf(rec_file, "SetPlayer:Quit by SDL_QUIT\n");
        quit();
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (SetPlayerEvent.key.keysym.sym) {
          case SDLK_ESCAPE: //Esc
            if (record) fprintf(rec_file, "SetPlayer:Quit by Esc\n");
            quit();
            break;
            //判断键盘输入并赋值
          case SDLK_0:
          case SDLK_KP_0:
            if (flag) current_state.ai_number = 0;
            else return 1;
            draw_text("0", 630, 620);
            break;
          case SDLK_1:
          case SDLK_KP_1:
            if (flag) current_state.ai_number = 1;
            else return 1;
            draw_text("1", 630, 620);
            break;
          case SDLK_2:
          case SDLK_KP_2:
            if (!flag) current_state.total_number = 2;
            else current_state.ai_number = 2;
            draw_text("2", 630, 620);
            break;
          case SDLK_3:
          case SDLK_KP_3:
            if (!flag) current_state.total_number = 3;
            else current_state.ai_number = 3;
            draw_text("3", 630, 620);
            break;
          case SDLK_4:
          case SDLK_KP_4:
            if (!flag) current_state.total_number = 4;
            else current_state.ai_number = 4;
            draw_text("4", 630, 620);
            break;
          default:break;
        }
        SDL_Delay(1000);  //给玩家显示1s输入的值
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (record)
          fprintf(rec_file,
                  "SetPlayer:Mouse button down (%d, %d)\n",
                  SetPlayerEvent.button.x,
                  SetPlayerEvent.button.y);
        break;
        //本函数仍在mainUI里，能看见开始和帮助按钮，只保留帮助功能
      case SDL_MOUSEBUTTONUP:
        if (record)
          fprintf(rec_file,
                  "SetPlayer:Mouse button up (%d, %d)\n",
                  SetPlayerEvent.button.x,
                  SetPlayerEvent.button.y);
        if (SetPlayerEvent.button.x > 555 && SetPlayerEvent.button.x < 722 && SetPlayerEvent.button.y > 463
            && SetPlayerEvent.button.y < 547) { //帮助
          if (record) fprintf(rec_file, "SetPlayer:Press get help button\n");
          int open_readme = system("start README.md");
          if (open_readme) draw_text("Failed to get help:(", 400, 569);
        }
        break;
      default:goto Again;
    }
  }
  //判断玩家输入的数字是否合法
  current_state.player_number = current_state.total_number - current_state.ai_number;
  if (current_state.player_number > current_state.total_number || current_state.ai_number > current_state.total_number
      || current_state.total_number < 2 || current_state.total_number > 4){
    main_render();
    draw_text("Invalid Number", 500, 569);
    if (record) fprintf(rec_file, "SetPlayer:Invalid Number\n");
    SDL_Delay(1000);
    return 1;
  }
  return 0;
}

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

void game_render(void){   //渲染gameUI和棋子
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, GameTexture, NULL, &GameRect);
  for (int i = 0; i < 16; i++) {
    SDL_RenderCopyEx(Renderer,Chess[i].tex,NULL,&(Chess[i].rect),Chess[i].dir,NULL,SDL_FLIP_NONE);
  }
}

void game_state_adjust(void){
  //调整玩家类型
  current_state.player_type = current_state.player > current_state.player_number ? AI : Player;
  //调整颜色字符串
  if (current_state.player == RED){
    memset(current_state.color_str, 7, sizeof(char));
    strcat(current_state.color_str, "Red");
  }else if (current_state.player == GREEN){
    memset(current_state.color_str, 7, sizeof(char));
    strcat(current_state.color_str, "Green");
  }else if (current_state.player == YELLOW){
    memset(current_state.color_str, 7, sizeof(char));
    strcat(current_state.color_str, "Yellow");
  }else if (current_state.player == BLUE){
    memset(current_state.color_str, 7, sizeof(char));
    strcat(current_state.color_str, "Blue");
  }
}

void game_event_loop(void){   //游戏事件循环
  SDL_Event GameEvent;
  while (SDL_WaitEvent(&GameEvent)) {
    switch (GameEvent.type) {
      case SDL_QUIT:  //关闭窗口
        if (record) fprintf(rec_file, "Quit by SDL_QUIT in game\n");
        quit();
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (GameEvent.key.keysym.sym) {
        case SDLK_RETURN:case SDLK_SPACE: //回车和空格都可以掷骰子
            if (record) fprintf(rec_file, "Roll dice by Keydown Enter/Space\n");
            int roll_value = roll();
            draw_dice(roll_value);
            break;
          case SDLK_ESCAPE: //Esc
            if (record) fprintf(rec_file, "Quit by Esc in game\n");
            quit();
            break;
          default:break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (record) fprintf(rec_file, "Mouse button down in game (%d, %d)\n", GameEvent.button.x, GameEvent.button.y);
        printf("Mouse button down in game (%d, %d)\n", GameEvent.button.x, GameEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        if (record) fprintf(rec_file, "Mouse button up in game (%d, %d)\n", GameEvent.button.x, GameEvent.button.y);
        if (GameEvent.button.x > 999 && GameEvent.button.x < WIN_WIDTH && GameEvent.button.y > 0 && GameEvent.button.y < 95){  //返回主界面
          if (record) fprintf(rec_file,"Return to mainUI\n");
          main_event_loop();
        }
        break;
      default:break;
    }
  }
}