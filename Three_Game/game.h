#ifndef _GAME_H
#define _GAME_H

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#pragma warning(disable:4996)

//定义棋盘大小
#define ROW 3
#define COL 3
//定义玩家与电脑棋子颜色
#define P_COLOR 'X'
#define C_COLOR 'O'
#define NEXT 'N'
#define DRAW 'D'


void Menu();
void Game();

#endif
