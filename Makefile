target:
	g++ -DGL_SILENCE_DEPRECATION -I/usr/local/include/eigen3 -Inanogui/ext/nanovg/src -Inanogui/include -Iinclude src/*.cpp -std=c++11 -framework OpenGL -lnanogui -o target
