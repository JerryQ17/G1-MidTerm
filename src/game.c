#include "FlightChess.h"

void game(void){
  //初始化游戏
  game_init();
  recordf("Game Start:\nTotal number = %d\nPlayer number = %d\nAI number = %d\n\n",
          current_state.total_number, current_state.player_number, current_state.ai_number);
  //游戏开始
  while (true) {
    //判断游戏是否结束
    int flag_break = 0;
    for (int i = 0; i < current_state.total_number; i++){
      if (current_state.win[i]) flag_break++;
    }
    if (flag_break == 3){
      recordf("Game: Game Over\n");
      break;
    }
    //渲染gameUI和棋子
    game_render();
    recordf("Round%d %s %s:\n", current_state.round,
              current_state.player_type == Player ? "Player" : "AI", current_state.color_str);
    game_event();
    //判断玩家是否胜利
    game_judge();
    //记录回合信息
    recordf("Current Situation:\n");
    for (int i = 0; i < current_state.total_number * 4; i++) {
      recordf("Chess%d\tpos = %d\tstate = %d\tdir = %d\n", i, Chess[i].pos, Chess[i].state, Chess[i].dir);
    }
    recordf("Round%d ends;\n\n", current_state.round);
    //跳转至下一玩家
    while (true) {
      if (current_state.player == current_state.total_number) current_state.player = RED;
      else current_state.player++;
      if (!current_state.win[current_state.player]) break;
    }
    game_state_adjust();
    SDL_Delay(1000);
    current_state.round++;
  }
}

void game_init(void){  //游戏部分的初始化
  //确定游戏人数
  while(game_player());
  //对游戏要用到的部分变量进行初始化
  current_state.round = 1;
  current_state.player = RED;
  game_state_adjust();
  for (int i = 0; i < current_state.total_number; i++) current_state.win[i] = false;
  //初始化棋子
  chess_init();
}

int game_player(void){  //确定游戏人数
  SDL_Event GamePlayerEvent;
  for (int flag = 0; flag < 2; flag++) {
    main_render();
    if (!flag) draw_text("Please Input Total Player", 360, 569);  //先输入总人数
    else draw_text("Please Input AI Player", 400, 569);           //再输入电脑数
    GamePlayerLoop:
    SDL_WaitEvent(&GamePlayerEvent);
    switch (GamePlayerEvent.type) {
      case SDL_QUIT:  //关闭窗口
        if (record) fprintf(log_file, "GamePlayer: Quit by SDL_QUIT\n");
        quit(EXIT_SUCCESS);
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (GamePlayerEvent.key.keysym.sym) {
          case SDLK_ESCAPE: //Esc
            if (record) fprintf(log_file, "GamePlayer: Quit by Esc\n");
            quit(EXIT_SUCCESS);
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
        break;
      case SDL_MOUSEBUTTONDOWN:
        recordf("GamePlayer: Mouse button down (%d, %d)\n", GamePlayerEvent.button.x, GamePlayerEvent.button.y);
        break;
        //本函数仍在mainUI里，能看见开始和帮助按钮，只保留帮助功能
      case SDL_MOUSEBUTTONUP:
        recordf("GamePlayer: Mouse button up (%d, %d)\n", GamePlayerEvent.button.x, GamePlayerEvent.button.y);
        if (GamePlayerEvent.button.x > 555 && GamePlayerEvent.button.x < 722 && GamePlayerEvent.button.y > 463
            && GamePlayerEvent.button.y < 547) {    //帮助
          recordf("GamePlayer: Press get help button\n");
          int open_readme = system("start README.md");
          if (open_readme) draw_text("Failed to get help:(", 400, 569);
        }
        break;
      default:goto GamePlayerLoop;
    }
    SDL_Delay(1000);  //给玩家显示1s输入的值
  }
  //判断玩家输入的数字是否合法
  current_state.player_number = current_state.total_number - current_state.ai_number;
  if (current_state.player_number > current_state.total_number || current_state.ai_number > current_state.total_number
      || current_state.total_number < 2 || current_state.total_number > 4){
    main_render();
    draw_text("Invalid Number", 500, 569);
    recordf("SetPlayer: Invalid Number, total = %d, ai = %d\n", current_state.total_number, current_state.ai_number);
    SDL_Delay(1000);
    return 1;
  }
  return 0;
}

void game_render(void){   //渲染gameUI，棋子，Information
  //渲染gameUI
  SDL_RenderClear(Renderer);
  SDL_RenderCopy(Renderer, GameTexture, NULL, &GameRect);
  for (int i = 0; i < 16; i++) {
    if (Chess[i].state != FINISH) {
      SDL_RenderCopyEx(Renderer,Chess[i].tex,NULL,&(Chess[i].rect),Chess[i].dir,NULL,SDL_FLIP_NONE);
    }
  }
  //渲染Information
  draw_text("Round", 0, 50);
  draw_number(current_state.round, 170, 65);
  draw_text(current_state.player_type == Player ? "Player" : "AI", 0, 100);
  draw_text(current_state.color_str, 0, 155);
}

void game_state_adjust(void){   //调整游戏状态
  //调整玩家类型
  current_state.player_type = current_state.player > current_state.player_number ? AI : Player;
  //调整颜色字符串
  memset(current_state.color_str, 0, sizeof(current_state.color_str));
  if (current_state.player == RED) strcat(current_state.color_str, "Red");
  else if (current_state.player == GREEN) strcat(current_state.color_str, "Green");
  else if (current_state.player == YELLOW) strcat(current_state.color_str, "Yellow");
  else if (current_state.player == BLUE) strcat(current_state.color_str, "Blue");
}

void game_event(void){   //游戏事件
  if (current_state.player_type == AI){
    game_round();
    return;
  }
  SDL_Event GameEvent;
  while (SDL_WaitEvent(&GameEvent)) {
    switch (GameEvent.type) {
      case SDL_QUIT:  //关闭窗口
        recordf("GameEvent: Quit by SDL_QUIT in game\n");
        quit(EXIT_SUCCESS);
        break;
      case SDL_KEYDOWN: //按下键盘
        switch (GameEvent.key.keysym.sym) {
        case SDLK_RETURN:case SDLK_SPACE: //回车和空格都可以进入游戏回合
            game_round();
            break;
          case SDLK_ESCAPE: //Esc
            recordf("GameEvent: Quit by Esc in game\n");
            quit(EXIT_SUCCESS);
            break;
          default:break;
        }
        break;
      case SDL_MOUSEBUTTONDOWN:
        recordf("GameEvent: Mouse button down in game (%d, %d)\n", GameEvent.button.x, GameEvent.button.y);
        printf("Mouse button down in game (%d, %d)\n", GameEvent.button.x, GameEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        recordf("GameEvent: Mouse button up in game (%d, %d)\n", GameEvent.button.x, GameEvent.button.y);
        if (GameEvent.button.x > 999 && GameEvent.button.x < WIN_WIDTH && GameEvent.button.y > 0 && GameEvent.button.y < 95){  //返回主界面
          recordf("GameEvent: Return to mainUI\n");
          main_event_loop();
        }
        break;
      default:break;
    }
  }
}

void game_round(void){
  //掷骰子
  int roll_value = dice_roll();
  dice_present(roll_value);
  //判断当前玩家是否有待起飞的飞机
  int flag_airport = 0;
  for (int i = (int) (current_state.player - 1) * 4; i < current_state.player * 4; i++) {
    if (Chess[i].state == AIRPORT) flag_airport++;
  }
  if (flag_airport == 4){   //全在机场
    if (roll_value == 6){   //该玩家第一个棋子起飞
      chess_departure((int)((current_state.player - 1) * 4));
    }
  }else if(flag_airport){   //机场有
    if (roll_value == 6) {
      int chess_clicked = chess_click();
      if (Chess[chess_clicked].state == AIRPORT) chess_departure(chess_clicked);
      else chess_move(chess_clicked, roll_value);
    }
  }else chess_move(chess_click(), roll_value);    //机场没有
}

void game_judge(void){   //判断当前是否有人赢了
  for (int i = 0; i < current_state.total_number; i++) current_state.win[i] = true;
  for (int i = 0; i < current_state.total_number * 4; i++){
    if (Chess[i].state != FINISH) current_state.win[i / 4] = false;
  }
}