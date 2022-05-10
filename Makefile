all:
	g++ -Isrc/include -Lsrc/lib -o main main.cpp painter.cpp snake.cpp PlayGround.cpp Gallery.cpp -lmingw32 -lSDL2main -lSDL2 -lSDL2_image