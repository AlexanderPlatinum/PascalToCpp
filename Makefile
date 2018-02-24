CC=g++
CFLAGS=-c -Wall

build: main.o translator.o
	$(CC) main.o translator.o -o translator

main.o: main.cpp
	$(CC) $(CFLAGS) main.cpp

translator.o: translator.cpp
	$(CC) $(CFLAGS) translator.cpp

clean:
	rm -rf *.o translator
