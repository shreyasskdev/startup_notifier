CC = gcc
CFLAGS = -std=c11
BIN = main.out

all: $(BIN)

$(BIN): main.c
	$(CC) $(CFLAGS) -o $(BIN) main.c -lcurl

clean:
	rm -rf $(BIN) *.dSYM
