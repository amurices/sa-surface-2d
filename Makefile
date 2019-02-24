target:
	g++ -DGL_SILENCE_DEPRECATION -I/usr/local/include/freetype2 -I/usr/local/include/eigen3 -Iinclude/ext/ -Iinclude/ src/*.cpp -std=c++11 -framework OpenGL -lnanogui -lglfw -lGLEW -lftgl -o target
