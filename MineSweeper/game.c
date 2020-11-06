#include "game.h"

void Menu()
{
	printf("##########################\n");
	printf("## 1. Play      2. Exit ##\n");
	printf("##########################\n");
	printf("������# ");
}

//����
void SetMines(char mines_board[][COL], int row, int col,int num)
{
	//����һ��������ͳ�����׵ĸ���
	int count = num;

	//ѭ��ʵ�����ף���������Ϊ0�������Ѿ�������
	while (count)
	{
		int x = rand() % 10 + 1;
		int y = rand() % 10 + 1;
		//ֻ��û�б����׵ĵط���������ͬʱ��������һ
		if (mines_board[x][y] == '0')
		{
			mines_board[x][y] = '1';
			count--;
		}
	}
}

//��ʾÿһ�к�ĺ���
void ShowLine(int nums)
{
	printf("---");
	for (int i = 0; i < nums; i++) {
		printf("-");
	}
	printf("\n");
}

//��ʾ10*10���,�˴�����ʾ��Ҫ���ϵ��Լ���
void ShowBoard(char show_board[][COL], int row, int col)
{
	printf("   ");
	//��ӡ�������ǩ
	for (int i = 1; i <row-1; i++)
	{
		printf(" %d  ", i);
	}
	printf("\n");
	//˫��ѭ��������ӡ�б�ǩ���ڲ��ӡÿһ��
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
	//1.������ʾ��������벼������
	char show_board[ROW][COL];
	char mines_board[ROW][COL];

	//2.��ʼ����ʾ����Ϊ*����������Ϊ0
	memset(show_board, '*', sizeof(show_board));
	memset(mines_board, '0', sizeof(mines_board));

	//3.�����NUM���ף����׵�λ����Ϊ�ַ�1,
	srand((unsigned long)time(NULL));
	SetMines(mines_board,ROW,COL,NUM);

	//����һ�����������ڼ��������ʣ��Ŀ�λ����ȥ���߽��������С��ȥ���׵ĸ��� ����-2*����-2-���׵ĸ���
	int count = (ROW - 2)*(COL - 2) - NUM;
	//��������
	int x = 0;
	int y = 0;

	//ѭ��������Ƿ��ſ�
	do
	{
		//��ʾ���
		ShowBoard(show_board,ROW,COL);
		//������������
		printf("������λ�����꣺");
		scanf("%d %d", &x, &y);
		//�ж����������Ƿ�Խ�磬���߽����������ô�1��ʼ��10����
		if (x < 1 || x>10 || y < 1 || y>10)
		{
			printf("�����������Խ�磬���������룡\n");
			continue;
		}
		//�ж����������Ƿ����ų���
		if (show_board[x][y] != '*')
		{
			printf("�������λ���Ѿ��ų������������룡\n");
			continue;
		}
		//�ж�����λ���Ƿ�����
		if (mines_board[x][y] == '1')
		{
			break;
		}


		//���������������£��жϸ�λ���ܱ����ж��ٸ���
		int num = GetMines(mines_board, ROW, COL, x, y);
		//ͳ�Ƹ�����˸������ж�����
		show_board[x][y] = num + '0';
		//��λ��-1
		count--;
		system("cls");
	} while (count>0);

	if (count > 0)
	{
		printf("�㱻ը���ˣ�\n");
	}
	else
	{
		printf("��Ӯ�ˣ�\n");
	}
	printf("�������������Ų�!\n");
	ShowBoard(mines_board, ROW, COL);
}