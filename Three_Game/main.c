#include "game.h"

int main()
{
	int select=0;
	int quit = 0;
	while(!quit)
	{
		Menu();
		scanf("%d", &select);
		switch(select)
		{
			case 1:
				Game();
				break;
			case 2:
				quit = 1;
				break;
			default:
				printf("Please input again\n");
				break;
		}
	}

	printf("ByeBye!\n");

	system("pause");
	return 0;
}