main:
	mkdir -p bin
	g++ -std=c++20 -O2 -Wall -Wextra -Wpedantic \
	src/glad.c \
	src/main.cpp \
	-Ivendor/libraries/include \
	-Iheaders \
	-Iheaders/meshes \
	-Iheaders/UI \
	-Iheaders/shaders \
	-Iheaders/utils \
	-Iheaders/window \
	-Iglad/include \
	-lglfw -lGL -ldl \
	-o bin/main

run:
	bin/main

.PHONY: clean

clean: 
	rm -rf bin/*