all:
	clang -O3 src/*.c -o bzoc

debug:
	clang -g src/*.c -o bzoc