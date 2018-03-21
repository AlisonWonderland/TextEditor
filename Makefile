all: editor.c 
	gcc -g -Wall -o editor editor.c

  clean: 
	  $(RM) editor