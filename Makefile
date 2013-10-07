all: fm

fm:
	gcc filemanager.c -o filemanager

debug:
	gcc filemanager.c -g -o filemanager

clean:
	rm filemanager
