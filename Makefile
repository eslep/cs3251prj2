all:
	gcc commandparser.c clientinterface.c -o client

fm:
	gcc filemanager.c -o filemanager

debug:
	gcc filemanager.c -g -o filemanager

