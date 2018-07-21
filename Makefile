CC = gcc
CFLAGS = -Wall -O1

#Definición de los ficheros
SRC = main.c
OBJ = main.o

#Reglas
all: $(OBJ)
	$(CC) $(CFLAGS) -o exif $(OBJ)

clean:
	$(RM) $(OBJ) huf
