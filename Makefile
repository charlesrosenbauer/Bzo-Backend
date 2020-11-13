all:
	clang -O3 *.c -o bzoc

debug:
	clang -O3 -g *.c -o bzoc