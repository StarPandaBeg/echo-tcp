BUILD_DIR = bin

build:
	mkdir -p $(BUILD_DIR)
	gcc -o bin/server.o server.c
	gcc -o bin/client.o client.c
