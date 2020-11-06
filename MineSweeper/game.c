#include "game.h"

void Menu()
{
	printf("##########################\n");
	printf("## 1. Play      2. Exit ##\n");
	printf("##########################\n");
	printf("请输入# ");
}

//埋雷
void SetMines(char mines_board[][COL], int row, int col,int num)
{
	//定义一个计数器统计埋雷的个数
	int count = num;

	//循环实现埋雷，当计数器为0表明雷已经被埋完
	while (count)
	{
		int x = rand() % 10 + 1;
		int y = rand() % 10 + 1;
		//只有没有被埋雷的地方才能埋雷同时计数器减一
		if (mines_board[x][y] == '0')
		{
			mines_board[x][y] = '1';
			count--;
		}
	}
}

//显示每一行后的横线
void ShowLine(int nums)
{
	printf("---");
	for (int i = 0; i < nums; i++) {
		printf("-");
	}
	printf("\n");
}

//显示10*10面板,此处的显示需要不断调试计算
void ShowBoard(char show_board[][COL], int row, int col)
{
	printf("   ");
	//打印横坐标标签
	for (int i = 1; i <row-1; i++)
	{
		printf(" %d  ", i);
	}
	printf("\n");
	//双层循环，外层打印列标签，内层打印每一列
	for (int i = 1; i <row-1; i++)
	{
		printf("%2d ", i);
		for (int j = 1; j < col - 1; j++)
		{
			printf(" %c |", show_board[i][j]);
		}
		printf("\n");
		ShowLine(2 * col + col + 4);
	}

}

int GetMines(char mines_board[][COL], int row, int col, int x, int y)
{
	return mines_board[x - 1][y - 1] + mines_board[x - 1][y] + mines_board[x + 1][y + 1] + mines_board[x][y + 1] \
		+ mines_board[x + 1][y + 1] + mines_board[x + 1][y] + mines_board[x + 1][y - 1] + mines_board[x][y - 1]\
		- 8 * '0';
}

void Game()
{
	//1.定义显示面板数组与布雷数组
	char show_board[ROW][COL];
	char mines_board[ROW][COL];

	//2.初始化显示数组为*，布雷数组为0
	memset(show_board, '*', sizeof(show_board));
	memset(mines_board, '0', sizeof(mines_board));

	//3.随机埋NUM个雷，有雷的位置设为字符1,
	srand((unsigned long)time(NULL));
	SetMines(mines_board,ROW,COL,NUM);

	//定义一个计数器用于计数面板中剩余的空位，即去除边界后的数组大小减去埋雷的个数 行数-2*列数-2-埋雷的个数
	int count = (ROW - 2)*(COL - 2) - NUM;
	//定义坐标
	int x = 0;
	int y = 0;

	//循环检测雷是否排空
	do
	{
		//显示面板
		ShowBoard(show_board,ROW,COL);
		//输入坐标排雷
		printf("请输入位置坐标：");
		scanf("%d %d", &x, &y);
		//判断输入坐标是否越界，填充边界后的数组正好从1开始，10结束
		if (x < 1 || x>10 || y < 1 || y>10)
		{
			printf("您输入的坐标越界，请重新输入！\n");
			continue;
		}
		//判断输入坐标是否已排除过
		if (show_board[x][y] != '*')
		{
			printf("您输入的位置已经排除，请重新输入！\n");
			continue;
		}
		//判断输入位置是否有雷
		if (mines_board[x][y] == '1')
		{
			break;
		}


		//不满足上述条件下，判断该位置周边埋有多少个雷
		int num = GetMines(mines_board, ROW, COL, x, y);
		//统计该坐标八个方向有多少雷
		show_board[x][y] = num + '0';
		//空位数-1
		count--;
		system("cls");
	} while (count>0);

	if (count > 0)
	{
		printf("你被炸死了！\n");
	}
	else
	{
		printf("你赢了！\n");
	}
	printf("下面是雷区的排布!\n");
	ShowBoard(mines_board, ROW, COL);
}