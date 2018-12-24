target:
	g++ -I/usr/local/include/freetype2 -Iinclude/ src/*.c* -std=c++11 -framework OpenGL -lglfw -lGLEW -lftgl -o target
