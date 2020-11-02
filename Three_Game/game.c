#include "game.h"

void Menu()
{
	printf("################################\n");
	printf("####  1.Play        2.Exit  ####\n");
	printf("################################\n");
	printf("Please input your selection#: ");
}

void ShowBoard(char board[][COL],int row, int col)
{
	printf("   | 1 | 2 | 3 |\n");
	printf("----------------\n");
	//外层循环控制每一行，内层循环控制每一列
	for (int i = 0; i < ROW; i++)
	{
		printf(" %d | ",i+1);
		for (int j = 0; j < COL; j++) 
		{
			printf("%c | ", board[i][j]);
		}
		printf("\n----------------\n");
	}
}

void PlayerMove(char board[][COL], int row, int col)
{
	//定义输入坐标x，y
	int x = 0;
	int y = 0;
	while (1) 
	{
		printf("请输入你的位置：");
		scanf("%d %d", &x, &y);
		if (x < 1 || x>3 || y < 1 || y>3)
		{
			printf("你输入的坐标有误！\n");
			continue;
		}
		//需要注意用户输入的坐标从1开始，而程序定义的棋盘坐标从0开始
		else if (board[x - 1][y - 1] != ' ')
		{
			printf("你输入的位置已经被占用！\n");
			continue;
		}
		else
		{
			board[x - 1][y - 1] = P_COLOR;
			break;
		}
	}
}

char Judge(char board[][COL], int row, int col)
{
	//1.遍历每一行是否三子成珠
	for (int i = 0; i < row; i++)
	{
		if (board[i][0] == board[i][1] && \
			board[i][1] == board[i][2] && \
			board[i][0] != ' ')
		{
			return board[i][0];
		}
	}

	//2.遍历每一列是否三子成珠
	for (int j = 0; j < col; j++)
	{
		if (board[0][j] == board[1][j] && \
			board[1][j] == board[2][j] && \
			board[0][j] != ' ')
		{
			return board[0][j];
		}
	}

	//3.遍历九宫格对角线是否三子成珠
	if (board[0][0] == board[1][1] && \
		board[1][1] == board[2][2] && \
		board[0][0] != ' ')
	{
		return board[0][0];
	}

	if (board[0][2] == board[1][1] && \
		board[1][1] == board[2][0] && \
		board[0][2] != ' ')
	{
		return board[0][2];
	}
	//4.没有出现三子成珠，且棋盘还有空位，继续下棋
	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < col; j++)
		{
			if (board[i][j] == ' ')
			{
				return NEXT;
			}
		}
	}
	//5.上述条件都不满足，和棋
	return DRAW;
}

void ComputerMove(char board[][COL], int row, int col)
{
	while (1)
	{
		int x = rand() % row;
		int y = rand() % col;
		if (board[x][y] == ' ')
		{
			board[x][y] = C_COLOR;
			break;
		}
	}
}

void Game()
{
	//定义一个随机种子用于电脑落子
	srand((unsigned long)time(NULL));
	//定义棋盘
	char board[ROW][COL];
	//棋盘初始化为空
	memset(board, ' ', sizeof(board));
	//定义变量result用于判断输赢情况
	char result='x';
	do
	{
		ShowBoard(board, ROW, COL);
		PlayerMove(board, ROW, COL);
		result = Judge(board, ROW, COL);
		//出现三子成珠，跳出循环，给出输赢结果
		if (result != NEXT)
		{
			break;
		}
		ComputerMove(board, ROW, COL);
		result = Judge(board, ROW, COL);
		//出现三子成珠情况，跳出循环，给出输赢结果
		if (result != NEXT)
		{
			break;
		}
	} while (1);

	//打印输赢结果
	if (P_COLOR == result)
	{
		printf("你赢了！\n");
	}
	else if (C_COLOR == result)
	{
		printf("你输了！\n");
	}
	else
	{
		printf("和棋！\n");
	}
	//此处再次调用显示棋盘函数，用于显示最终的落子情况，如果不调用该函数，会出现看不到最后一次落子的棋盘
	ShowBoard(board, ROW, COL);
}
