all:
	clang -O3 src/*.c

debug:
	clang -g src/*.c