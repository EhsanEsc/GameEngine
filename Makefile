CC = g++
CFLAGS = -std=c++11 -lmingw32 -lSDL2main -lSDL2 -lSDL2_image
OBJECTS = main.o

MyGame.exe: $(OBJECTS)
	$(CC) $(OBJECTS) -o MyGame $(CFLAGS)

main.o: main.cpp
	$(CC) -c main.cpp $(CFLAGS)

clean:
	-rm *.o *~ core paper
