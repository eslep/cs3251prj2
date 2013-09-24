/*
 * CommandParser.c
 * Code for the CommandParser
 */
#include<stdio.h>
#include<stdlib.h>
#include<string.h>

char** commands;
int numCommands;

void registerCommands(char* newCmds[], int newNum)
{
	commands = newCmds;
	numCommands = newNum;
	printf("numCommands: %d\n",numCommands);
}

int parseCommand(char* cmd)
{
	int i=0;
	while(i<numCommands && strcmp(commands[i],cmd)!=0)
	{
		i++;
	}
	if(i==numCommands)
		return -1;
	else
		return i;
}
