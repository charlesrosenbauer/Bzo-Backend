all:
	clang -O3 src/*.c -o bzoc

debug:
	clang -g -Og src/*.c -o bzoc