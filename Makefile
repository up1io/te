.PHONY: build

build:
	clang src/main.c -std=c99 -lncurses \
		-o build/main
