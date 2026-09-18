main:
	mkdir -p bin
	g++ -Wall -Wextra -Wpedantic -std=c++20 -O2 -g -I./headers/shaders -I./headers/utils -I./headers/window -isystem ./libraries/include src/main.cpp src/glad.c -o bin/main $(pkg-config --cflags --libs glfw3) -lGL -ldl -lpthread
	
.PHONY: clean

clean: 
	rm -rf ./bin/*