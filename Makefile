target:
	g++ -I/usr/local/include/freetype2 *.cpp -std=c++11 -framework OpenGL -lglfw -lGLEW -lftgl -o target
