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
	//���ѭ������ÿһ�У��ڲ�ѭ������ÿһ��
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
	//������������x��y
	int x = 0;
	int y = 0;
	while (1) 
	{
		printf("���������λ�ã�");
		scanf("%d %d", &x, &y);
		if (x < 1 || x>3 || y < 1 || y>3)
		{
			printf("���������������\n");
			continue;
		}
		//��Ҫע���û�����������1��ʼ��������������������0��ʼ
		else if (board[x - 1][y - 1] != ' ')
		{
			printf("�������λ���Ѿ���ռ�ã�\n");
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
	//1.����ÿһ���Ƿ����ӳ���
	for (int i = 0; i < row; i++)
	{
		if (board[i][0] == board[i][1] && \
			board[i][1] == board[i][2] && \
			board[i][0] != ' ')
		{
			return board[i][0];
		}
	}

	//2.����ÿһ���Ƿ����ӳ���
	for (int j = 0; j < col; j++)
	{
		if (board[0][j] == board[1][j] && \
			board[1][j] == board[2][j] && \
			board[0][j] != ' ')
		{
			return board[0][j];
		}
	}

	//3.�����Ź���Խ����Ƿ����ӳ���
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
	//4.û�г������ӳ��飬�����̻��п�λ����������
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
	//5.���������������㣬����
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
	//����һ������������ڵ�������
	srand((unsigned long)time(NULL));
	//��������
	char board[ROW][COL];
	//���̳�ʼ��Ϊ��
	memset(board, ' ', sizeof(board));
	//�������result�����ж���Ӯ���
	char result='x';
	do
	{
		ShowBoard(board, ROW, COL);
		PlayerMove(board, ROW, COL);
		result = Judge(board, ROW, COL);
		//�������ӳ��飬����ѭ����������Ӯ���
		if (result != NEXT)
		{
			break;
		}
		ComputerMove(board, ROW, COL);
		result = Judge(board, ROW, COL);
		//�������ӳ������������ѭ����������Ӯ���
		if (result != NEXT)
		{
			break;
		}
	} while (1);

	//��ӡ��Ӯ���
	if (P_COLOR == result)
	{
		printf("��Ӯ�ˣ�\n");
	}
	else if (C_COLOR == result)
	{
		printf("�����ˣ�\n");
	}
	else
	{
		printf("���壡\n");
	}
	//�˴��ٴε�����ʾ���̺�����������ʾ���յ������������������øú���������ֿ��������һ�����ӵ�����
	ShowBoard(board, ROW, COL);
}
