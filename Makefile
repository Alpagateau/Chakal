CFLAGS:=-Wall -Wextra -std=c99 -g -lm
INCLUDE:=-I./src
TEST_INCLUDE:=-I./lib/munit

SRC:=$(wildcard src/*.c)
OBJ:=$(filter-out build/main.o, $(patsubst src/%.c, build/%.o, $(SRC)))

all: build/test build/types_test build/libchakal.a

build/libchakal.a: build/chakal_types.o build/chakal_linked_list.o
	ar rcs build/libchakal.a build/chakal_types.o build/chakal_linked_list.o

build/test: $(OBJ)
	gcc $(OBJ) src/main.c -o build/test $(CFLAGS) $(INCLUDE)

build/%.o: src/%.c
	gcc $(CFLAGS) $< -c -o $@

build/linked_test: test/linked_test.c build/munit.o
	gcc $(TEST_INCLUDE)  $(INCLUDE) $(CFLAGS) test/linked_test.c -o build/linked_test

build/types_test: test/chakal_types_test.c build/munit.o
	gcc $(TEST_INCLUDE)  $(INCLUDE) $(CFLAGS) test/chakal_types_test.c $(OBJ) build/munit.o -o build/types_test

build/munit.o: ./lib/munit/munit.c build
	gcc $(CFLAGS) ./lib/munit/munit.c -c -o build/munit.o

build:
	mkdir -p build

.PHONY: all
