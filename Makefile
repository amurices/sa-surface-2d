target:
	g++ -I/usr/local/include/freetype2 -Iinclude/ src/*.cpp -std=c++11 -framework OpenGL -lglfw -lGLEW -lftgl -o target
