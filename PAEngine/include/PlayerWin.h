/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

class PlayerWin {
public:
  PlayerWin();
  ~PlayerWin();
  void SetWinner(int i);
  const int GetWinner();

  static PlayerWin* globalWin;
private:
  int winner = -1;
};

