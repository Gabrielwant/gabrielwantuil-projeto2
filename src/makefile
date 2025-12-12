CC = gcc
CFLAGS = -Wall -g -O0 -fno-stack-protector
LDFLAGS = -lm

SRC = main.c geometria.c lista.c arvore.c visibilidade.c svg.c
OBJ = $(SRC:.c=.o)
TARGET = ted.exe

all: $(TARGET)

$(TARGET): $(OBJ)
	$(CC) -fno-stack-protector $(OBJ) -o $@ $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	del /Q *.o $(TARGET)

.PHONY: all clean