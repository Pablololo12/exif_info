CC = gcc
CFLAGS = -g -Wall -O3

#Definición de los ficheros
SRC = main.c
OBJ = main.o

#Reglas
all: $(OBJ)
	$(CC) $(CFLAGS) -o exif $(OBJ)

clean:
	$(RM) $(OBJ) huf
