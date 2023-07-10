#Nome do programa
TARGET = app

#diretorios dos arquivos
SRC = ./src
OBJ = ./obj# guarda os objetos criados na compilacao
BIN = ./bin# guarda os executaveis

#compilador
CC = gcc

#arquivos
SOURCES := $(shell find $(SRC) -name '*.c')
OBJS := $(patsubst $(SRC)/%.c, %.o, $(SOURCES))
#OBJS = main.o header.o ...
# compila os arquivos e 
all: | $(OBJ) $(BIN) $(OBJS)
	$(CC) $(OBJ)/*.o -o $(BIN)/$(TARGET)

$(OBJ):
	@echo "Folder $(OBJ) does not exist"
	mkdir -p $@

$(BIN):
	@echo "Folder $(BIN) does not exist"
	mkdir -p $@

main.o: $(SRC)/main.c
	$(CC) -c $(SRC)/main.c -o $(OBJ)/main.o

net.o: $(SRC)/net.c
	$(CC) -c $(SRC)/net.c -o $(OBJ)/net.o

file.o: $(SRC)/file.c
	$(CC) -c $(SRC)/file.c -o $(OBJ)/file.o

run:
	$(BIN)/$(TARGET)

clean:
	rm $(OBJ)/*.o
	rm $(BIN)/*
