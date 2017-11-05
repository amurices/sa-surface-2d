main: Surface/main.cpp
	g++ -std=gnu++17 Surface/main.cpp *.cpp -I /usr/local/include -I ../ -I /usr/local/include/freetype2 -I Outsiders/brkga -lGL -lGLEW -lglfw -lemon -lftgl
