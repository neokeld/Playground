CC=gcc
CFLAGS= -W -Wall -Wextra -g
LDFLAGS=
EXEC=graph_test test_parser test_commandParser test_cfgparser controler

all: $(EXEC)

graph_test: graph_test.o graph.o

test_parser: test_parser.o graph.o utils.o parser.o

test_commandParser: test_commandParser.o graph.o utils.o parser.o commandParser.o

test_cfgparser: test_cfgparser.o utils.o cfgparser.o

controler: controler.o commandParser.o graph.o utils.o parser.o cfgparser.o

.PHONY: clean mrproper

clean:
	rm -rf *.o

mrproper: clean
	rm $(EXEC)
