All:  compile run run-ex

compile:
	gcc main.c -o result -lncurses
run:
	./result

run-ex: expanded-compile expanded-run
expanded-compile:
	gcc expanded.c -o res  

expanded-run:
	./res