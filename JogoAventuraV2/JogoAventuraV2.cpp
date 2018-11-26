// JogoAventuraV2.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"
#include <stdio.h>
#include <locale.h>
#include <string.h>
#include <stdlib.h>
#include <random>
#include <Windows.h>
#include <tchar.h>
#include <conio.h>
#include <strsafe.h>
#include <time.h>
#include <conio.h>
#include <iostream>

#pragma comment( lib, "winmm.lib")

#define MAX_PLAYER_NAME 100
#define PLAYER_INITIAL_ENERGY 100
#define PLAYER_SU_INITIAL_ENERGY 5000
#define PLAYER_INITIAL_LOCATION 0
#define MAX_DESCRIPTION 400
#define MAX_EXPLORE_CHAR 400
#define MAX_CELLS 16
#define MAX_ANSWER_CHAR 10
#define WITCH_LOCATION 12
#define WITCH_ENERGY 300
#define MAX_SAVE_LINE_CHAR 12
#define MAX_FIELD 100
#define MAX_LIN 9
#define MAX_LIN_MAP 1000


struct Player {
	char name[MAX_PLAYER_NAME];
	int energy;
	int location;
	int treasure;
	int godSword;
	int bread;
	int rope;
	int basicSword;
	int suMode;
};

struct Monster {
	int energy;
	int location;
};

struct Witch {
	int energy;
	int location;
};

struct Cell {
	int north;
	int south;
	int west;
	int east;
	int up;
	int down;
	int treasure;
	char description[MAX_DESCRIPTION];
};

void InitPlayer(struct Player *pPlayer);
void ShowPlayer(struct Player *pPlayer);

void InitMonster(struct Monster *pMonster);
void InitWitch(struct Witch *pWitch);

void convertFileBin(struct Cell map[]);
void readFileBin(struct Cell map[]);
int loadMap(struct Cell map[]);
void move();
void introSound();
void eatSound();
void witchSound();
void getswordSound();
void battleSound();
void monsterSound();
void itemcatchSound();
void fightMonster();
void fightWitch();
void help();
void showMap();
void explore();
void gameOver();
void pickup();
void inventory();
void eat();
void invalidCommand();
void ShowProfile();
void save();
void load();
void playerAction();
void monsterAction();

//int InitMap(struct Cell map[]);
HANDLE hMutex1, hMutex2;
struct Cell map[MAX_CELLS];
struct Player player;
struct Monster monster;
struct Witch witch;
int nCells;
char answer[MAX_ANSWER_CHAR];
int randomlocation;
int previousLocation;
int playerDamage;
int monsterDamage;
int witchDamage;
int monsterDead;
int witchDead;
int random;


void PrintToConsole(char text[]) {

	printf("%s", text);
	fflush(stdout);
}

DWORD WINAPI ThreadProc1(LPVOID lpParam)
{
	while ((player.energy > 0 && player.location != monster.location) || monster.energy > 0)
	{
		playerAction();
	}
	return 0;
}


DWORD WINAPI ThreadProc2(LPVOID lpParam)
{
	while ((monster.energy > 0) && (player.location != monster.location))
	{
		srand(time(NULL));
		monsterAction();
		printf("\nMonstro está na sala: %d\n", monster.location);
		Sleep(5000);
	}
	return 0;
}


int main(int argc, char *argv[])
{
	//introSound();
	InitPlayer(&player);
	InitMonster(&monster);
	InitWitch(&witch);
	//nCells = InitMap(map);
	witchDead = 0;
	random = 0;
	nCells = loadMap(map);
	//convertFileBin(map);
	//readFileBin(map);
	ShowPlayer(&player);
	witch.location = 12;
	monster.location = 13;

	hMutex1 = CreateMutex(
		NULL,
		FALSE,
		NULL);

	hMutex2 = CreateMutex(
		NULL,
		FALSE,
		NULL);

	HANDLE hThread1 = CreateThread(
		NULL,
		0,
		ThreadProc1,
		NULL,
		0,
		NULL);

	HANDLE hThread2 = CreateThread(
		NULL,
		0,
		ThreadProc2,
		NULL,
		0,
		NULL);
	if (argc > 1)
	{
		if (strcmp(argv[1], "su") == 0)
		{
			player.suMode = 1;
			player.energy = PLAYER_SU_INITIAL_ENERGY;
		}
	}
	while (player.energy > 0)
	{
		if ((player.location == monster.location) && monsterDead == 0)
			{
				WaitForSingleObject(hMutex1, INFINITE);
				WaitForSingleObject(hMutex2, INFINITE);
				fightMonster();
				ReleaseMutex(hMutex1);
				ReleaseMutex(hMutex2);
			}		
	}
	WaitForSingleObject(hMutex1, INFINITE);
	WaitForSingleObject(hMutex2, INFINITE);
	WaitForSingleObject(hThread1, INFINITE);
	CloseHandle(hThread1);

	WaitForSingleObject(hThread2, INFINITE);
	CloseHandle(hThread2);
	
	return 0;
}

void playerAction()
{
	printf("\n__________________________________________________________________________________________________________________");
	printf("\n\n%s\n", map[player.location].description);
	if (player.suMode == 1)
	{
		showMap();
	}
	printf("\n>>--->");
	scanf("%s", answer);
	if ((strcmp(answer, "north")) != 0 && (strcmp(answer, "n")) != 0 &&
		(strcmp(answer, "south")) != 0 && (strcmp(answer, "s")) != 0 &&
		(strcmp(answer, "west")) != 0 && (strcmp(answer, "w")) != 0 &&
		(strcmp(answer, "east")) != 0 && (strcmp(answer, "e")) != 0 &&
		(strcmp(answer, "up")) != 0 && (strcmp(answer, "u")) != 0 &&
		(strcmp(answer, "down")) != 0 && (strcmp(answer, "d")) != 0 &&
		(strcmp(answer, "profile")) != 0 && (strcmp(answer, "explore")) != 0 &&
		(strcmp(answer, "help")) != 0 && (strcmp(answer, "help")) != 0 &&
		(strcmp(answer, "eat")) != 0 && (strcmp(answer, "pickup")) != 0 &&
		(strcmp(answer, "inventory")) != 0 && (strcmp(answer, "save")) != 0 &&
		(strcmp(answer, "load")) != 0 && (strcmp(answer, "exit")) != 0)
	{
		invalidCommand();
	}
	previousLocation = player.location;
	if ((strcmp(answer, "north")) || (strcmp(answer, "n")) || map[player.location].north != -1 ||
		(strcmp(answer, "south")) || (strcmp(answer, "s")) || map[player.location].south != -1 ||
		(strcmp(answer, "west")) || (strcmp(answer, "w")) || map[player.location].west != -1 ||
		(strcmp(answer, "east")) || (strcmp(answer, "e")) || map[player.location].east != -1 ||
		(strcmp(answer, "up")) || (strcmp(answer, "u")) || map[player.location].west != -1 ||
		(strcmp(answer, "down")) || (strcmp(answer, "d")) || map[player.location].west != -1)
	{
		move();
	}
	if (strcmp(answer, "profile") == 0)
	{
		ShowProfile();
	}
	if (strcmp(answer, "explore") == 0)
	{
		explore();
	}
	if (strcmp(answer, "help") == 0)
	{
		help();
	}
	if (strcmp(answer, "eat") == 0)
	{
		eat();
	}
	if (strcmp(answer, "pickup") == 0)
	{
		pickup();
	}
	if (strcmp(answer, "inventory") == 0)
	{
		inventory();
	}
	if (strcmp(answer, "save") == 0)
	{
		save();
	}
	if (strcmp(answer, "load") == 0)
	{
		load();
	}
	if (strcmp(answer, "exit") == 0)
	{
		exit(1);
	}
	if (witch.location == player.location)
	{
		fightWitch();
	}

}

void monsterAction()
{
	random = rand() % 2;
	monster.location = random + 13;
}

void fightMonster()
{
		monsterSound();
		printf("\n__________________________________________________________________________________________________________________");
		printf("\n\nYOU ARE IN THE SAME ROOM OF THE MONSTER, YOU HAVE TO FIGHT HIM\n\n");
		Sleep(2000);
			while ((player.energy > 0) && (monster.energy > 0))
			{
				if (player.godSword == 1)
				{
					playerDamage = monster.energy;
					monsterDamage = rand() % 30;
				}
				if (player.basicSword == 1)
				{
					playerDamage = monster.energy;
					monsterDamage = rand() % 30;
				}
				if (player.godSword == 0 && player.basicSword == 1)
				{
					playerDamage = rand() % 40;
					monsterDamage = rand() % 20;
				}
				if (player.godSword == 0 && player.basicSword == 0)
				{
					playerDamage = rand() % 10;
					monsterDamage = rand() % 30;
				}
				while ((player.energy > 0) && (monster.energy > 0))
				{
					player.energy = player.energy - monsterDamage;
					printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
					printf("|||                           *Battle!*                       |||\n");
					printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
					battleSound();
					printf("       The monster attacked you, now you have %d of HP\n\n", player.energy);
					monster.energy = monster.energy - playerDamage;
					printf("         You attacked the monster now he has %d of HP\n", monster.energy);
					Sleep(2000);
					if (player.energy <= 0)
					{
						//WaitForSingleObject(hMutex1, INFINITE);
						//WaitForSingleObject(hMutex2, INFINITE);
						gameOver();
					}
					if (monster.energy <= 0)
					{
						printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
						printf("|||       *Good Job Warrior!! You killed the monster!!*       |||\n");
						printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
						printf("__________________________________________________________________________________________________________________");
						printf("\n\n%s\n", map[player.location].description);
						printf("\n>>--->");						
						monster.location = -1;
						monsterDead = 1;
					}
				}
			}
	
}

void invalidCommand()
{
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||                 *That command doesn't exist!              |||\n");
	printf("|||  	      Type help to see the available commands*        |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
}

void fightWitch()
{
	int run;
	char answerFight[MAX_ANSWER_CHAR];
	if (witchDead != 1)
	{
		witchSound();
		printf("\n\nYou are in the same room of the Witch, do you wish to fight him?(yes/no)\n\n\n>>--->");
		scanf("%s", answerFight);
		run = 0;
		if ((strcmp(answerFight, "yes") == 0))
		{
			printf("\n\nYou are fighting the Witch do you wish to attack or run away from the battle warrior?\n\n>>--->");

			while ((player.energy > 0) && (witch.energy > 0) && run == 0)
			{
				if (player.godSword == 1)
				{
					playerDamage = witch.energy;
					witchDamage = 0;
				}
				if (player.godSword == 0 && player.basicSword == 1)
				{
					playerDamage = rand() % 40;
					witchDamage = rand() % 10;
				}
				if (player.godSword == 0 && player.basicSword == 0)
				{
					playerDamage = rand() % 10;
					witchDamage = rand() % 40;
				}

				scanf("%s", answerFight);
				if ((strcmp(answerFight, "run") == 0))
				{
					run = 1;
					printf("\n\nYou run away!\n\n>>--->");
					player.location = previousLocation;
				}

				if ((strcmp(answerFight, "eat") == 0))
				{
					eat();
					printf("/n");
				}

				if ((strcmp(answerFight, "attack") == 0))
				{
					player.energy = player.energy - witchDamage;
					printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
					printf("|||                           *Battle!*                       |||\n");
					printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
					battleSound();
					printf("       The monster attacked you, now you have %d of HP\n\n", player.energy);
					witch.energy = witch.energy - playerDamage;
					printf("         You attacked the monster now he has %d of HP\n", witch.energy);
					if (witch.energy <= 0)
					{
						printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
						printf("|||        *Good Job Warrior!! You killed the Witch!!*        |||\n");
						printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
						witchDead = 1;
					}
				}
			}
		}

		if ((strcmp(answerFight, "no") == 0))
		{
			printf("\n\nYou run away!");
			player.location = previousLocation;
		}
	}
}

/*
int InitMap(struct Cell map[])
{

//CELL 0
map[0].north = 4;
map[0].south = 1;
map[0].west = 2;
map[0].east = 3;
map[0].up = -1;
map[0].down = -1;
map[0].treasure = 0;
strcpy(map[0].description, "You are in front of a big door that leads to a big Castle where there are many difficulties!\nWhat do you wan't to do?");


//CELL 1
map[1].north = 0;
map[1].south = -1;
map[1].west = -1;
map[1].east = -1;
map[1].up = -1;
map[1].down = -1;
map[1].treasure = 1;
strcpy(map[1].description, "You are in the road!\nWhat do you wan't to do?");


//CELL 2
map[2].north = -1;
map[2].south = -1;
map[2].west = -1;
map[2].east = 0;
map[2].up = 10;
map[2].down = -1;
map[2].treasure = 0;
strcpy(map[2].description, "You are in the garden, in  the outside of the castle and you find a big tree!\nWhat do you wan't to do?");


//CELL 3
map[3].north = -1;
map[3].south = -1;
map[3].west = 0;
map[3].east = -1;
map[3].up = -1;
map[3].down = -1;
map[3].treasure = 0;
strcpy(map[3].description, "You are in the garden, in  the outside of the castle and you can see beatiful flowers!\nWhat do you wan't to do?");


//CELL 4
map[4].north = 5;
map[4].south = 0;
map[4].west = -1;
map[4].east = -1;
map[4].up = -1;
map[4].down = -1;
map[4].treasure = 0;
strcpy(map[4].description, "You are inside of the castle, you only have one way in front of you!\nWhat do you wan't to do?");


//CELL 5
map[5].north = 9;
map[5].south = 4;
map[5].west = 6;
map[5].east = 13;
map[5].up = -1;
map[5].down = -1;
map[5].treasure = 0;
strcpy(map[5].description, "You are in the main room of the castle you can go to north or west...\nWhere do you wan't to go?");


//CELL 6
map[6].north = 7;
map[6].south = 8;
map[6].west = -1;
map[6].east = 5;
map[6].up = -1;
map[6].down = -1;
map[6].treasure = 0;
strcpy(map[6].description, "You are in a empty room with a bookshelf, you can go to north or south...\nWhat do you wan't to do?");


//CELL 7
map[7].north = -1;
map[7].south = 6;
map[7].west = -1;
map[7].east = -1;
map[7].up = -1;
map[7].down = 12;
map[7].treasure = 0;
strcpy(map[7].description, "You are in a room with a stairs that go down!\nWhat do you wan't to do?");


//CELL 8
map[8].north = 6;
map[8].south = -1;
map[8].west = -1;
map[8].east = -1;
map[8].up = 11;
map[8].down = -1;
map[8].treasure = 1;
strcpy(map[8].description, "You are in the kitchen with a stairs that go up!\nWhat do you wan't to do?");



//CELL 9
map[9].north = -1;
map[9].south = 5;
map[9].west = -1;
map[9].east = -1;
map[9].up = 15;
map[9].down = -1;
map[9].treasure = 0;
strcpy(map[9].description, "You are in a room with a stairs that go up!\nWhat do you wan't to do?");

//CELL 10
map[10].north = 11;
map[10].south = -1;
map[10].west = -1;
map[10].east = -1;
map[10].up = -1;
map[10].down = 2;
map[10].treasure = 1;
strcpy(map[10].description, "***You have found a secret room***\nWhat do you wan't to do?");

//CELL 11
map[11].north = -1;
map[11].south = 10;
map[11].west = -1;
map[11].east = -1;
map[11].up = -1;
map[11].down = 8;
map[11].treasure = 0;
strcpy(map[11].description, "You are in the castle tower in the armor room!\nYou can see stairs that go down...\nWhat do you wan't to do?");

//CELL 12
map[12].north = -1;
map[12].south = -1;
map[12].west = -1;
map[12].east = -1;
map[12].up = 7;
map[12].down = -1;
map[12].treasure = 0;
strcpy(map[12].description, "You are in the castle lab!\nWhat do you wan't to do?");

//CELL 13
map[13].north = -1;
map[13].south = -1;
map[13].west = 5;
map[13].east = -1;
map[13].up = -1;
map[13].down = 14;
map[13].treasure = 0;
strcpy(map[13].description, "You are in a dark room of the castle but you can see a stairs that go down!\nWhat do you wan't to do?");

//CELL 14
map[14].north = -1;
map[14].south = -1;
map[14].west = 5;
map[14].east = -1;
map[14].up = 13;
map[14].down = -1;
map[14].treasure = 0;
strcpy(map[14].description, "You are in the castle dungeons!\nWhat do you wan't to do?");

//CELL 15
map[15].north = -1;
map[15].south = -1;
map[15].west = -1;
map[15].east = -1;
map[15].up = -1;
map[15].down = 9;
map[15].treasure = 1;
strcpy(map[15].description, "You are in the kings room!\nWhat do you wan't to do?");

return 15;

}
*/

void showMap()
{
	printf("._________________________________________.\n");
	printf("                                           \n");
	printf("            *~MAP~*                        \n");
	printf("                                           \n");
	printf("      [7]     [9]	    UP[2]->[10]    \n");
	printf("       |       |	    UP[8]->[11]    \n");
	printf("      [6]-----[5]-----[13]  UP[9]->[15]    \n");
	printf("       |       |	    DOWN[7]->[12]  \n");
	printf("      [8]     [4]           DOWN[13]->[14] \n");
	printf("               |                           \n");
	printf("      [2]-----[0]-----[3]                  \n");
	printf("               |                           \n");
	printf("              [1]                          \n");
	printf("                                           \n");
	printf("     You are in room:%d                    \n", player.location);
	printf("     Monster location:%d                   \n", monster.location);
	printf("___________________________________________\n");
}

void help()
{
	printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||                    *Commands available!*                  |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||                                                           |||\n");
	printf("|||Direction:                                                 |||\n");
	printf("|||-Go to north: north, n                                     |||\n");
	printf("|||-Go to south: south, s                                     |||\n");
	printf("|||-Go to west: west, w                                       |||\n");
	printf("|||-Go to east: east, e                                       |||\n");
	printf("|||                                                           |||\n");
	printf("|||Random Commands: explore                                   |||\n");
	printf("|||-Explore: explore                                          |||\n");
	printf("|||-Show inventory: inventory                                 |||\n");
	printf("|||-Eat: eat                                                  |||\n");
	printf("|||-Save game: save                                           |||\n");
	printf("|||-Load game: load                                           |||\n");
	printf("|||-Show player profile: profile                              |||\n");
	printf("|||-Catch item: pickup                                        |||\n");
	printf("|||                                                           |||\n");
	printf("|||Battle:                                                    |||\n");
	printf("|||-Accept battle: yes                                        |||\n");
	printf("|||-Negate battle: no                                         |||\n");
	printf("|||-Run from battle: run                                      |||\n");
	printf("|||-Attack: attack                                            |||\n");
	printf("|||                                                           |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
}

void move()
{

	if ((strcmp(answer, "north") == 0) || (strcmp(answer, "n") == 0))
	{
		if (map[player.location].north == -1)
		{
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                  *You can't go there!*                    |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		}
		else
		{
			player.location = map[player.location].north;
		}
	}

	if ((strcmp(answer, "south") == 0) || (strcmp(answer, "s") == 0))
	{
		if (map[player.location].south == -1)
		{
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                  *You can't go there!*                    |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		}
		else
		{
			player.location = map[player.location].south;
		}
	}

	if ((strcmp(answer, "west") == 0) || (strcmp(answer, "w") == 0))
	{
		if (map[player.location].west == -1)
		{
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                  *You can't go there!*                    |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		}
		else
		{
			player.location = map[player.location].west;
		}
	}

	if ((strcmp(answer, "east") == 0) || (strcmp(answer, "e") == 0))
	{
		if (map[player.location].east == -1)
		{
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                  *You can't go there!*                    |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		}
		else
		{
			player.location = map[player.location].east;
		}
	}

	if ((strcmp(answer, "up") == 0) || (strcmp(answer, "u") == 0))
	{
		if (map[player.location].up == -1)
		{
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                  *You can't go there!*                    |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		}
		else
		{
			if (player.location == 2 && player.rope == 0)
			{
				printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
				printf("|||                *You can't climb the tree!*                |||\n");
				printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			}
			else
			{
				if (player.location == 2 && player.rope == 1)
				{
					printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
					printf("|||     *You successfuly climb the tree using your rope!*     |||\n");
					printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
				}
				player.location = map[player.location].up;
			}
		}
	}

	if ((strcmp(answer, "down") == 0) || (strcmp(answer, "d") == 0))
	{
		if (map[player.location].down == -1)
		{
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                  *You can't go there!*                    |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		}
		else
		{
			player.location = map[player.location].down;
		}
	}
}

void ShowPlayer(struct Player *pPlayer)
{
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||                     ~~PLAYER PROFILE~~                    |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("Name: %s\n", pPlayer->name);
	printf("HP: %d\n", pPlayer->energy);
	printf("Location: %d\n", pPlayer->location);
}

void ShowProfile()
{
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||                 ~~PLAYER PROFILE~~                    |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("Name: %s\n", player.name);
	printf("HP: %d\n", player.energy);
	printf("Location: %d\n", player.location);
}

void InitPlayer(struct Player *pPlayer)
{
	printf(".-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-.\n");
	printf("|                                                                       |\n");
	printf("|Hello Warrior!                                                         |\n");
	printf("|Are you ready for this adventure?                                      |\n");
	printf("|In this castle you will find many problems like traps and monsters, but|\n");
	printf("|you can find legendary items too and a big treasure!!!                 |\n");
	printf("|Let's go in this adventure!!                                           |\n");
	printf("|                                                                       |\n");
	printf(".-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-.\n\n\n");
	printf("Oh of course what's your name?\n\n>>--->");
	scanf("%s", pPlayer->name);
	pPlayer->energy = PLAYER_INITIAL_ENERGY;
	pPlayer->location = PLAYER_INITIAL_LOCATION;
	pPlayer->treasure = 0;
	pPlayer->suMode = 0;
}

void InitMonster(struct Monster *pMonster)
{
	pMonster->energy = PLAYER_INITIAL_ENERGY;
	pMonster->location = PLAYER_INITIAL_LOCATION;
}

void InitWitch(struct Witch *pWitch)
{
	pWitch->energy = WITCH_ENERGY;
	pWitch->location = WITCH_LOCATION;
}

void gameOver()
{
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||           ~~*You Died! Better Luck next time*~~           |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	Sleep(1000);
	exit(1);
}

void explore()
{
	if (player.location == 0)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                  *Nothing of interest!*                   |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 1)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||          *This road doesn't lead to anything!*            |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 2)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||               *This tree it's very big!*                  |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");

	}
	if (player.location == 3)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||        *Nice Flowers! They are very collorful!*           |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 4)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                      *Nice Sword!*                        |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 5)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                *Nice castle decoration!*                  |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 6)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                  *Nothing of interest!*                   |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 7)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                *Smells like dark magic!*                  |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 8)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||          *Smells very good! It puts me angry!*            |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 9)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                  *Nothing of interest!*                   |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 10)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||              *You can see too many things!*               |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 11)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                  *Nothing of interest!*                   |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 12)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                      *Nice potions!*                      |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 13)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                    *It's very dark!*                      |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 14)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                    *It's smells bad!*                     |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.location == 15)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                   ***Nice Treasure!!***                   |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
}

void pickup()
{
	if (map[player.location].treasure == 0)
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||              ~~There is nothing to pickup!~~              |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if ((player.location == 10) && (map[10].treasure == 1))
	{
		if (player.godSword == 0)
		{
			map[10].treasure = 0;
			player.godSword = 1;
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||      ~~*You pick up the God Sword(Legendary item)*~~      |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			getswordSound();
		}

	}
	if ((player.location == 4) && (map[4].treasure == 1))
	{
		if (player.basicSword == 0)
		{

			map[4].treasure = 0;
			player.basicSword = 1;
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||             ~~*You pick up a basic Sword*~~               |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			getswordSound();
		}

	}
	if ((player.location == 1) && (map[1].treasure == 1))
	{
		if (player.rope == 0)
		{
			map[1].treasure = 0;
			player.rope = 1;
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                 ~~*You pick up a rope*~~                  |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			itemcatchSound();
		}

	}
	if ((player.location == 8) && (map[8].treasure == 1))
	{
		if (player.bread == 0)
		{
			map[8].treasure = 0;
			player.bread = 1;
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                ~~*You pick up the bread*~~                |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			itemcatchSound();
		}

	}
	if ((player.location == 15) && (map[15].treasure == 1))
	{
		if (player.treasure == 0)
		{
			map[15].treasure = 0;
			player.treasure = 1;
			printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||              ~~*You pick up the Treasure*~~               |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			printf("|||                                                           |||\n");
			printf("|||   *You successfuly completed the game! Congradulations!*  |||\n");
			printf("|||                                                           |||\n");
			printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
			introSound();
			system("pause");
			exit(1);
		}
	}
}

void eat()
{
	if (player.bread == 1)
	{
		eatSound();
		player.energy = player.energy + 50;
		printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||                 <3 *You ate the bread!* <3                |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n\n");
		printf("                       *You have now HP:%d*\n", player.energy);
		player.bread = 0;
	}
	else
	{
		printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
		printf("|||       *You have nothing to eat on your inventory...*      |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
}

void inventory()
{
	printf("\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||                         *INVENTORY!*                      |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||[ItemType][Amount]                 [Item Name]             |||\n");
	if (player.godSword == 1)
	{
		printf("|||  Equip      1                      God Sword              |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.basicSword == 1)
	{
		printf("|||  Equip      1                     Basic Sword             |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	}
	if (player.bread == 1)
	{
		printf("|||   Food      1                        Bread                |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");

	}
	if (player.rope == 1)
	{
		printf("|||   item      1                         rope                |||\n");
		printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");

	}
}

void save()
{
	FILE *f;
	printf("\n~~Name of the file:");
	scanf("%s", answer);
	f = fopen(answer, "w");
	fprintf(f, "%s\n", player.name);
	fprintf(f, "%d\n", player.energy);
	fprintf(f, "%d\n", player.location);
	fprintf(f, "%d\n", player.treasure);
	fprintf(f, "%d\n", player.godSword);
	fprintf(f, "%d\n", player.bread);
	fprintf(f, "%d\n", monster.energy);
	fprintf(f, "%d\n", monster.location);
	fprintf(f, "%d\n", monsterDead);
	fprintf(f, "%d\n", player.suMode);
	fprintf(f, "%d\n", player.rope);
	fprintf(f, "%d\n", player.basicSword);
	fprintf(f, "%d\n", witchDead);
	fclose(f);
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||             *You successfuly saved your game...*          |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
}

void load()
{
	FILE *f;
	char l[MAX_SAVE_LINE_CHAR];
	int i;
	char campo1;
	printf("\n~~Name of the savegame file:");
	scanf("%s", answer);

	f = fopen(answer, "r");
	i = 0;
	while (fgets(l, MAX_LIN, f) != NULL)
	{
		if (i == 0)
		{
			strcpy(player.name, l);
		}
		if (i == 1)
		{
			player.energy = atoi(l);
		}
		if (i == 2)
		{
			player.location = atoi(l);
		}
		if (i == 3)
		{
			player.treasure = atoi(l);;
		}
		if (i == 4)
		{
			player.godSword = atoi(l);;
		}
		if (i == 5)
		{
			player.bread = atoi(l);
		}
		if (i == 6)
		{
			monster.energy = atoi(l);
		}
		if (i == 7)
		{
			monster.location = atoi(l);
		}
		if (i == 8)
		{
			monsterDead = atoi(l);
		}
		if (i == 9)
		{
			player.suMode = atoi(l);
		}
		if (i == 10)
		{
			player.rope = atoi(l);
		}
		if (i == 11)
		{
			player.basicSword = atoi(l);
		}
		if (i == 12)
		{
			witchDead = atoi(l);
		}
		i = i + 1;
	}
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||             *You successfuly load your game...*           |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");

}

int loadMap(struct Cell map[])
{
	FILE *f;
	char l[MAX_LIN_MAP];
	char north[MAX_FIELD];
	char south[MAX_FIELD];
	char west[MAX_FIELD];
	char east[MAX_FIELD];
	char up[MAX_FIELD];
	char down[MAX_FIELD];
	char treasure[MAX_FIELD];
	int i;
	int cell;
	f = fopen("map.txt", "r");
	i = 0;
	cell = 0;
	while (fgets(l, MAX_LIN_MAP, f) != NULL)
	{
		if (i == 3)
		{
			i = 0;
		}
		if (i == 0)
		{
			sscanf(l, "%s %s %s %s %s %s %s", north, south, west, east, up, down, treasure);
			map[cell].north = atoi(north);
			map[cell].south = atoi(south);
			map[cell].west = atoi(west);
			map[cell].east = atoi(east);
			map[cell].up = atoi(up);
			map[cell].down = atoi(down);
			map[cell].treasure = atoi(treasure);
		}
		if (i == 1)
		{
			strcpy(map[cell].description, l);
		}
		if (i == 2)
		{
			cell = cell + 1;
		}

		i++;
	}
	printf("\n\n|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	printf("|||             *You successfuly load the map...*             |||\n");
	printf("|||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||||\n");
	return cell;
}

void convertFileBin(struct Cell map[])
{
	FILE *f;
	loadMap(map);
	f = fopen("map.bin", "w");
	fwrite(map, sizeof(struct Cell), 16, f);
	fclose(f);
}

void readFileBin(struct Cell map[])
{
	FILE *f;
	f = fopen("map.bin", "r");
	fread(map, sizeof(struct Cell), 16, f);
	fclose(f);
}

void introSound()
{
	PlaySound(TEXT("introsong.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void eatSound()
{
	PlaySound(TEXT("eatsong.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void getswordSound()
{
	PlaySound(TEXT("getswordsong.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void battleSound()
{
	PlaySound(TEXT("battlesong.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void monsterSound()
{
	PlaySound(TEXT("monstersong.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void itemcatchSound()
{
	PlaySound(TEXT("itemcatchsong.wav"), NULL, SND_FILENAME | SND_SYNC);
}
void witchSound()
{
	PlaySound(TEXT("witchsong.wav"), NULL, SND_FILENAME | SND_SYNC);
}

