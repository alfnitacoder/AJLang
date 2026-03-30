# Makefile for AJLANG interpreter

CC = gcc
CFLAGS = -Wall -g -Iinclude

SOURCES = src/main.c src/tokenizer.c src/parser.c src/evaluator.c
OBJECTS = $(SOURCES:.c=.o)
EXEC = ajlang

all: $(EXEC)

$(EXEC): $(OBJECTS)
	$(CC) $(CFLAGS) -o $@ $^

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f $(OBJECTS) $(EXEC)
