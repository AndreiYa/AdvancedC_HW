CC = gcc
CFLAGS = -Wall -Wextra -std=c11
TARGET = program

all: $(TARGET)

$(TARGET): main.o
	$(CC) $(CFLAGS) -o $(TARGET) main.o -lm

main.o: main.c
	$(CC) $(CFLAGS) -c main.c -o main.o

clean:
	rm -f *.o $(TARGET)