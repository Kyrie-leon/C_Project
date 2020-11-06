#include "game.h"

int main()
{
	int quit = 0;
	int select = 0;
	while (!quit)
	{
		Menu();
		scanf("%d", &select);
		switch (select)
		{
		case 1:
			Game();
			printf("要不要在来一把?\n");
			break;
		case 2:
			quit = 1;
			break;
		default:
			printf("你输入的选择有误，请重新输入！\n");
			break;
		}
	}
	printf("ByeBye\n");
	system("pause");
	return 0;
}