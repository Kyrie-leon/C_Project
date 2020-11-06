#ifndef _GAME_H
#define _GAME_H

#include <stdio.h>
#include <Windows.h>
#include <time.h>
#pragma warning(disable:4996)

//定义扫雷面板大小
#define ROW 12
#define COL 12
//定义埋雷的个数,定义面板大小的五分之一，即10*10*1/5=20
#define NUM 20


void Menu();
void Game();
#endif