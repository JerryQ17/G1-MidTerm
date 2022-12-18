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

void chess_move(int num, int step){       //棋子移动
  if (record) fprintf(rec_file, "ChessMove: Chess %d(%s), pos %d -> %d, dir %d -> %d, ",
                      num, current_state.color_str, Chess[num].pos, Chess[num].pos + step,
                      Chess[num].dir, board_vec[Chess[num].pos + step].dir);
  //线性移动
  for (int i = 0; i < step; i++) chess_move_line(num, Chess[num].pos + 1);
  //碰撞处理
  chess_crash(num);
  //判断滑行道
  int flag_fly = 0;
  if (num < 4 && Chess[num].pos == 17) {
    //TODO
    chess_move_line(num, 29);
  }
  else if (num > 3 && num < 8 && Chess[num].pos == 30) flag_fly = GREEN;
  else if (current_state.total_number > 2 && num > 7 && num < 12 && Chess[num].pos == 43) flag_fly = YELLOW;
  else if (current_state.total_number > 3 && num > 11 && num < 16 && Chess[num].pos == 4) flag_fly = BLUE;
  //滑行

  //滑行道碰撞处理

  //碰撞处理
  chess_crash(num);
}

void chess_move_line(int num, int vec_t){  //棋子线性移动的动画
  //移动
  for (int i = 0; i < FRAMERATE; i++) {
    uint32_t begin_time = SDL_GetTicks();
    Chess[num].rect.x += ((board_vec[vec_t].x - board_vec[Chess[num].pos].x) / FRAMERATE);
    Chess[num].rect.y += ((board_vec[vec_t].y - board_vec[Chess[num].pos].y) / FRAMERATE);
    game_render();
    uint32_t current_time = SDL_GetTicks();
    long long delay_time = (ANIMATION_TIME / FRAMERATE) - (current_time - begin_time);
    if (delay_time > 0) SDL_Delay(delay_time);
  }
  //定位rect
  Chess[num].rect.x = board_vec[vec_t].x;
  Chess[num].rect.y = board_vec[vec_t].y;
  //旋转,可能存在bug
  if (board_vec[vec_t].dir != Chess[num].dir) {
    double delta_angle = (double) (board_vec[vec_t].dir - Chess[num].dir) / FRAMERATE;
    for (int i = 0; i < FRAMERATE; i++) {
      uint32_t begin_time = SDL_GetTicks();
      Chess[num].dir += delta_angle;
      game_render();
      uint32_t current_time = SDL_GetTicks();
      long long delay_time = (ANIMATION_TIME / FRAMERATE) - (current_time - begin_time);
      if (delay_time > 0) SDL_Delay(delay_time);
    }
    //定位direction
    Chess[num].dir = board_vec[vec_t].dir;
  }
  //定位position
  Chess[num].pos = vec_t;
  //定位state
  if (vec_t <= 51) Chess[num].state = MAIN;
  else if (vec_t <= 71) Chess[num].state = AIRPORT;
  else if (vec_t <= 95) Chess[num].state = RUNWAY;
}

void chess_departure(int chess_number){   //棋子起飞
  chess_move_line(chess_number, (int) (51 + current_state.player * 5));
  int departure_value = dice_roll();
  dice_present(departure_value);
  chess_move_line(chess_number, (int) (current_state.player * 13 - 11));
  for (int step = 1; step < departure_value; step++){
    chess_move_line(chess_number, Chess[chess_number].pos + 1);
  }
}

int chess_click(void) {    //判定点击的是哪个棋子
  SDL_Event ChessClickEvent;
  while (SDL_WaitEvent(&ChessClickEvent)) {
    switch (ChessClickEvent.type) {
      case SDL_QUIT:  //关闭窗口
        if (record) fprintf(rec_file, "ChessClick: Quit by SDL_QUIT\n");
        quit();
        break;
      case SDL_MOUSEBUTTONDOWN:
        if (record)
          fprintf(rec_file,
                  "ChessClick: Mouse button down (%d, %d)\n",
                  ChessClickEvent.button.x,
                  ChessClickEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        if (record)
          fprintf(rec_file,
                  "ChessClick: Mouse button up (%d, %d)\n",
                  ChessClickEvent.button.x,
                  ChessClickEvent.button.y);
        for (int i = 0; i < current_state.total_number * 4; i++) {
          if (ChessClickEvent.button.x >= Chess[i].rect.x
              && ChessClickEvent.button.x <= Chess[i].rect.x + Chess[i].rect.w
              && ChessClickEvent.button.y >= Chess[i].rect.y
              && ChessClickEvent.button.y <= Chess[i].rect.y + Chess[i].rect.h) {
            return i;
          }
        }
        if (ChessClickEvent.button.x > 999 && ChessClickEvent.button.x < WIN_WIDTH &&
            ChessClickEvent.button.y > 0 && ChessClickEvent.button.y < 95) {  //返回主界面
          if (record) fprintf(rec_file, "ChessClick: Return to mainUI\n");
          main_event_loop();
        }
        break;
      default:break;
    }
  }
}

void chess_crash(int num){ //碰撞处理
  //检查是否碰撞
  bool flag_crash = false;
  int crashed = -1;
  for (int i = 0; i < current_state.total_number * 4; i++){
    if (i == num) continue;
    if (Chess[i].pos == Chess[num].pos){
      flag_crash = true;
      crashed = i;
      break;
    }
  }
  //碰撞处理
  if (flag_crash){
    for (int i = (int)(Chess[crashed].color * 5 + 47); i < Chess[crashed].color * 5 + 50; i++){
      bool flag_i_empty = true;
      for (int j = (int)((Chess[crashed].color - 1) * 4); j < (int)(Chess[crashed].color * 4); j++){
        if (Chess[j].pos == i) {
          flag_i_empty = false;
          break;
        }
      }
      if (flag_i_empty){
        if (record) fprintf(rec_file, "Crash chess %d\n", crashed);
        chess_move_line(crashed, i);
        break;
      }
    }
  }else{
    if (record) fprintf(rec_file, "No crash\n");
  }
}