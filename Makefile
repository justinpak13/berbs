mac: main.c 
	gcc -framework CoreVideo -framework IOKit -framework Cocoa -framework GLUT -I include -framework OpenGL lib/libraylib.a main.c -o my_app_for_mac

linux: main.c 
	gcc main.c -lraylib -lGL -lm -lpthread -ldl -lrt -lX11 -L ./lib -I ./include/ -o my_app_for_linux

