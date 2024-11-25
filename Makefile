raycaster:
	gcc main.c -o main -lm -lGL -lGLU -lglut

editor:
	gcc -o mapper map_editor.c -lncurses -lpanel -lmenu -lform
