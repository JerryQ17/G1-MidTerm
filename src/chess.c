#include "FlightChess.h"

void chess_init(void){  //初始化棋子
  int temp = 52;
  for (int i = 0; i < current_state.total_number * 4; i++, temp++){
    if (i % 4 == 0 && i) temp++;
    Chess[i].pos = temp;
    Chess[i].state = AIRPORT;
    Chess[i].color = i / 4 + 1;
    Chess[i].dir = vec[temp].dir;
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
    Chess[i].rect = (SDL_Rect){vec[temp].x, vec[temp].y, Chess[i].sur->w, Chess[i].sur->h};
    recordf("ChessInit: Chess %d \tpos = %d\tstate = %d\tcolor = %d\tdir = %d\n",
            i, Chess[i].pos, Chess[i].state, Chess[i].color, Chess[i].dir);
  }
  recordf("ChessInit: Complete!\n");
}

void chess_move(int num, int step){       //棋子移动
  recordf("ChessMove: Chess %d(%s), pos %d -> %d, dir %d -> %d\n",
                      num, current_state.color_str, Chess[num].pos, Chess[num].pos + step,
                      Chess[num].dir, vec[Chess[num].pos + step].dir);
  //线性移动
  for (int i = 0; i < step; i++){
    if (Chess[num].pos == 51) chess_move_line(num, 0);                                                 //走完一圈
    else if (current_state.player == RED && Chess[num].state == MAIN && Chess[num].pos == 49){               //红进跑道
      chess_move_line(num, 72);
      Chess[num].state = RUNWAY_IN;
    }else if (current_state.player == RED && Chess[num].state == RUNWAY_OUT && Chess[num].pos == 72){        //红反向到头
      chess_move_line(num, 49);
      Chess[num].state = MAIN;
    }else if (current_state.player == GREEN && Chess[num].state == MAIN && Chess[num].pos == 10){            //绿进跑道
      chess_move_line(num, 78);
      Chess[num].state = RUNWAY_IN;
    }else if (current_state.player == GREEN && Chess[num].state == RUNWAY_OUT && Chess[num].pos == 78){      //绿反向到头
      chess_move_line(num, 10);
      Chess[num].state = MAIN;
    }else if (current_state.player == YELLOW && Chess[num].state == MAIN && Chess[num].pos == 23){           //黄进跑道
      chess_move_line(num, 84);
      Chess[num].state = RUNWAY_IN;
    }else if (current_state.player == YELLOW && Chess[num].state == RUNWAY_OUT && Chess[num].pos == 84){     //黄反向到头
      chess_move_line(num, 23);
      Chess[num].state = MAIN;
    }else if (current_state.player == BLUE && Chess[num].state == MAIN && Chess[num].pos == 36){             //蓝进跑道
      chess_move_line(num, 90);
      Chess[num].state = RUNWAY_IN;
    }else if (current_state.player == BLUE && Chess[num].state == RUNWAY_OUT && Chess[num].pos == 90){       //蓝反向到头
      chess_move_line(num, 36);
      Chess[num].state = MAIN;
    }else if ((current_state.player == RED && (Chess[num].pos == 77 || Chess[num].state == RUNWAY_OUT)) ||
        (current_state.player == GREEN && (Chess[num].pos == 83 || Chess[num].state == RUNWAY_OUT)) ||
        (current_state.player == YELLOW && (Chess[num].pos == 89 || Chess[num].state == RUNWAY_OUT)) ||
        (current_state.player == BLUE && (Chess[num].pos == 95 || Chess[num].state == RUNWAY_OUT))){          //反向
      chess_move_line(num, Chess[num].pos - 1);
      Chess[num].state = RUNWAY_OUT;
    }else chess_move_line(num, Chess[num].pos + 1);                                                     //前进一格
  }
  //是否到达终点
  if (Chess[num].pos == 77 || Chess[num].pos == 83 || Chess[num].pos == 89 || Chess[num].pos == 95){
    recordf("ChessMove: Chess %d finish", num);
    draw_text("Congratulations!", 0, 210);
    Chess[num].state = FINISH;
    SDL_Delay(500);
    return;
  }
  //碰撞处理
  chess_crash(num);
  //滑行道处理
  chess_fly(num);
}

void chess_move_line(int num, int vec_t){  //棋子线性移动的动画
  //rect移动
  chess_move_rect(num, vec[vec_t].x, vec[vec_t].y);
  //旋转
  if (vec[vec_t].dir != Chess[num].dir) {
    chess_rotate(num, vec[vec_t].dir);
  }
  //定位position
  Chess[num].pos = vec_t;
  //定位state
  if (vec_t <= 51) Chess[num].state = MAIN;
  else if (vec_t <= 71) Chess[num].state = AIRPORT;
}

void chess_move_rect(int num, int xt, int yt){    //棋子rect移动
  int dx = (int)((xt - vec[Chess[num].pos].x) / FRAME_RATE),
  dy = (int)((yt - vec[Chess[num].pos].y) / FRAME_RATE),
  ex = (int)((xt - vec[Chess[num].pos].x - FRAME_RATE * dx) / ERROR_STEP),
  ey = (int)((yt - vec[Chess[num].pos].y - FRAME_RATE * dy) / ERROR_STEP);
  for (int i = 0; i < FRAME_RATE; i++) {
    uint32_t begin_time = SDL_GetTicks();
    Chess[num].rect.x += dx;
    Chess[num].rect.y += dy;
    if (i % ERROR_RATE == 0){   //过程中误差补偿
      Chess[num].rect.x += ex;
      Chess[num].rect.y += ey;
    }
#ifdef DEBUG
    recordf("ChessMoveRect: (%d)\tChess %d\tx = %d(%d)\ty = %d(%d)\n", i, num, Chess[num].rect.x, xt, Chess[num].rect.y, yt);
#endif
    game_render();
    uint32_t current_time = SDL_GetTicks();
    long long delay_time = (int)(ANIMATION_TIME / FRAME_RATE) - (current_time - begin_time);
    if (delay_time > 0) SDL_Delay(delay_time);
  }
  //末误差补偿
  while (abs(Chess[num].rect.x - xt) > 1 || abs(Chess[num].rect.y - yt) > 1) {
    uint32_t begin_time = SDL_GetTicks();
    if (abs(Chess[num].rect.x - xt) > 1) {
      Chess[num].rect.x < xt ? (Chess[num].rect.x += CHESS_MOVE_SPEED) : (Chess[num].rect.x -= CHESS_MOVE_SPEED);
    }
    if (abs(Chess[num].rect.y - yt) > 1) {
      Chess[num].rect.y < yt ? (Chess[num].rect.y += CHESS_MOVE_SPEED) : (Chess[num].rect.y -= CHESS_MOVE_SPEED);
    }
    game_render();
    uint32_t current_time = SDL_GetTicks();
    long long delay_time = (int)(ANIMATION_TIME / FRAME_RATE) - (current_time - begin_time);
    if (delay_time > 0) SDL_Delay(delay_time);
#ifdef DEBUG
    recordf("ChessMoveRect: Ex\tChess %d\tx = %d(%d)\ty = %d(%d)\tdelay %lld ms\n", num, Chess[num].rect.x, xt, Chess[num].rect.y, yt, delay_time);
#endif
  }
  //定位rect
#ifdef DEBUG
  recordf("ChessMoveRect: Locate Chess %d from (%d,%d) to (%d,%d)", num, Chess[num].rect.x, Chess[num].rect.y, xt, yt);
#endif
  Chess[num].rect.x = xt;
  Chess[num].rect.y = yt;
}

void chess_rotate(int num, double angle_t){   //棋子旋转
  int i = 1;
  recordf("ChessRotate: da = %lf\ta0 = %d\tat = %lf\n", CHESS_ROTATE_SPEED, Chess[num].dir, angle_t);
  while (fabs(Chess[num].dir - angle_t) > 1) {
    uint32_t begin_time = SDL_GetTicks();
    Chess[num].dir < angle_t ? (Chess[num].dir += CHESS_ROTATE_SPEED) : (Chess[num].dir -= CHESS_ROTATE_SPEED);
    game_render();
    uint32_t current_time = SDL_GetTicks();
    long long delay_time = (int)(ANIMATION_TIME / FRAME_RATE) - (current_time - begin_time);
    if (delay_time > 0) SDL_Delay(delay_time);
#ifdef DEBUG
    recordf("ChessRotate: (%d)\tChess %d\tangle = %d(%lf)\tdelay %lld ms\n", i++, num, Chess[num].dir, angle_t, delay_time);
#endif
  }
  //定位direction
#ifdef DEBUG
  recordf("ChessRotate: Locate Chess %d from %d to %lf", num, Chess[num].dir, angle_t);
#endif
  Chess[num].dir = angle_t;
}

void chess_departure(int num){   //棋子起飞
  int take_off_pos = (int)(51 + current_state.player * 5);
  recordf("ChessDeparture: Chess %d (pos = %d) moves to take-off point %d\n", num, Chess[num].pos, take_off_pos);
  chess_move_line(num, take_off_pos);
  int departure_value = dice_roll();
  dice_present(departure_value);
  int departure_pos = (int)(current_state.player * 13 - 12);
  recordf("ChessDeparture: Chess %d\tpos %d -> %d\tdir %d -> %d\n",
          num, Chess[num].pos, departure_pos, Chess[num].dir, vec[departure_pos].dir);
  chess_move_line(num, departure_pos);
  if (departure_value > 1) chess_move(num, departure_value - 1);
}

int chess_click(void) {    //判定点击的是哪个棋子
  if (current_state.player_type == AI){
    srand((unsigned int) time(NULL));
    int value = (int)(current_state.player - 1) * 4 + rand() % 4;
    while (Chess[value].state == FINISH) value = (int)(current_state.player - 1) * 4 + rand() % 4;
    recordf("ChessClick: AI %s chooses chess%d\n", current_state.color_str, value);
    return value;
  }
  SDL_Event ChessClickEvent;
  while (SDL_WaitEvent(&ChessClickEvent)) {
    switch (ChessClickEvent.type) {
      case SDL_QUIT:  //关闭窗口
        recordf("ChessClick: Quit by SDL_QUIT\n");
        quit(EXIT_SUCCESS);
        break;
      case SDL_MOUSEBUTTONDOWN:
        recordf("ChessClick: Mouse button down (%d, %d)\n", ChessClickEvent.button.x, ChessClickEvent.button.y);
        break;
      case SDL_MOUSEBUTTONUP:
        recordf("ChessClick: Mouse button up (%d, %d)\n", ChessClickEvent.button.x, ChessClickEvent.button.y);
        for (int i = 0; i < current_state.total_number * 4; i++) {
          if (ChessClickEvent.button.x >= Chess[i].rect.x
              && ChessClickEvent.button.x <= Chess[i].rect.x + Chess[i].rect.w
              && ChessClickEvent.button.y >= Chess[i].rect.y
              && ChessClickEvent.button.y <= Chess[i].rect.y + Chess[i].rect.h) {
            recordf("ChessClick: Player %s chooses chess%d\n", current_state.color_str, i);
            return i;
          }
        }
        if (ChessClickEvent.button.x > 999 && ChessClickEvent.button.x < WIN_WIDTH &&
            ChessClickEvent.button.y > 0 && ChessClickEvent.button.y < 95) {  //返回主界面
          recordf("ChessClick: Return to mainUI\n");
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
        recordf("ChessCrash: Crash chess %d, pos %d -> %d, dir %d -> %d\n",
                crashed, Chess[crashed].pos, i, Chess[crashed].dir, vec[i]);
        chess_move_line(crashed, i);
        break;
      }
    }
  }else recordf("ChessCrash: No crash\n");
}

void chess_fly(int num){    //滑行道判断与处理
  //判断是否滑行
  if (num < 4 && Chess[num].pos == 17) {  //红色
    chess_fly_crash(num, 17, 86, 29);
    chess_crash(num);
  }else if (num > 3 && num < 8 && Chess[num].pos == 30) {  //绿色
    chess_fly_crash(num, 30, 92, 42);
    chess_crash(num);
  }else if (current_state.total_number > 2 && num > 7 && num < 12 && Chess[num].pos == 43) {   //黄色
    chess_fly_crash(num, 43, 74, 3);
    chess_crash(num);
  }else if (current_state.total_number > 3 && num > 11 && num < 16 && Chess[num].pos == 4) {   //蓝色
    chess_fly_crash(num, 4, 80, 16);
    chess_crash(num);
  }
}

void chess_fly_crash(int num, int depart_pos, int crash_pos, int dest_pos){
  int fly_crashed = 0;
  recordf("ChessFly: Chess %d flies,", num);
  //判断是否碰撞
  bool flag_fly_crash = false;
  for (int i = 0; i < current_state.total_number * 4; i++){
    if (i == num) continue;
    if (Chess[i].pos == crash_pos) {
      flag_fly_crash = true;
      fly_crashed = i;
      break;
    }
  }
  //预旋转
  chess_rotate(num, vec[depart_pos].dir);
  //碰撞处理
  if (flag_fly_crash){    //发生碰撞
    //先到碰撞位
    chess_move_rect(num, vec[crash_pos].x, vec[crash_pos].y - Chess[fly_crashed].rect.y);
    recordf("In-flight crash chess %d\n", fly_crashed);
    //检查机场哪个栏位为空
    for (int i = (int)(Chess[fly_crashed].color * 5 + 47); i < Chess[fly_crashed].color * 5 + 50; i++){
      bool flag_i_empty = true;
      for (int j = (int)((Chess[fly_crashed].color - 1) * 4); j < (int)(Chess[fly_crashed].color * 4); j++){
        if (Chess[j].pos == i) {
          flag_i_empty = false;
          break;
        }
      }
      //两个棋子同步动画
      if (flag_i_empty){
        //平移
        int dx1 = (int)((vec[dest_pos].x - vec[Chess[num].pos].x) / FRAME_RATE),
            dy1 = (int)((vec[dest_pos].y - vec[Chess[num].pos].y) / FRAME_RATE),
            dx2 = (int)((vec[i].x - vec[Chess[fly_crashed].pos].x) / FRAME_RATE),
            dy2 = (int)((vec[i].y - vec[Chess[fly_crashed].pos].y) / FRAME_RATE);
        for (int j = 0; j < FRAME_RATE; j++) {
          uint32_t begin_time = SDL_GetTicks();
          Chess[num].rect.x += dx1;
          Chess[num].rect.y += dy1;
          Chess[fly_crashed].rect.x += dx2;
          Chess[fly_crashed].rect.y += dy2;
          game_render();
          uint32_t current_time = SDL_GetTicks();
          long long delay_time = (int)(ANIMATION_TIME / FRAME_RATE) - (current_time - begin_time);
          if (delay_time > 0) SDL_Delay(delay_time);
        }
        //定位rect
        Chess[num].rect.x = vec[dest_pos].x;
        Chess[num].rect.y = vec[dest_pos].y;
        Chess[fly_crashed].rect.x = vec[i].x;
        Chess[fly_crashed].rect.y = vec[i].y;
        //旋转被碰撞的棋子
        chess_rotate(fly_crashed, vec[i].dir);
        //定位被碰撞的棋子
        Chess[fly_crashed].pos = i;
        Chess[fly_crashed].state = AIRPORT;
      }
    }
  }else{    //不发生碰撞
    recordf("No in-flight crash\n");
    chess_move_rect(num, vec[dest_pos].x, vec[dest_pos].y);
  }
  //定位pos
  Chess[num].pos = dest_pos;
  //末旋转
  chess_rotate(num, vec[dest_pos].dir);
}