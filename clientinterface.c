#include<stdio.h>
#include<stdlib.h>
#include<string.h>
#include"commandparser.h"

int main(int argc, char *argv[])
{
  char* cmds[] = {"LIST","SHOW"};
  registerCommands(cmds,sizeof(cmds)/sizeof(cmds[0]));
  printf("LIST: %d\n",parseCommand("LIST"));
  printf("SHOW: %d\n",parseCommand("SHOW"));
  printf("DIE:  %d\n",parseCommand("DIE"));
}
