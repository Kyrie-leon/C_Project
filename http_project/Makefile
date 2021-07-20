BIN=http_server
SRC=main.cc
CC=g++
FLAGS=-std=c++11

$(BIN):$(SRC)
	$(CC) -o $@ $^ $(FLAGS)
.PHONY:clean
clean:
	rm -f $(BIN)
