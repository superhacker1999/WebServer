.PHONY: clean server_build server
SRC_SERVER=Server/Server.cc NetHandler/NetHandler.cc Client/Client.cc ConfigParser/ConfigParser.cc RequestHandler/RequestHandler.cc
CC=gcc
FLAGS=-lstdc++ -Wall -Werror -Wextra
CONF=../ws.conf

clean:
	rm -rf server_build

server_build: clean
	$(CC) $(FLAGS) $(SRC_SERVER) -o server_build

debug: clean
	$(CC) $(FLAGS) -g $(SRC_SERVER) -o server_build

server: server_build
	./server_build $(CONF)

style_check:
	clang-format -i Server/* Client/* NetHandler/* Logger/*
