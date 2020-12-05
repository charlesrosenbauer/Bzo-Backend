all:
	clang -O3 *.c -o bzoc

debug:
	clang -O2 -g *.c -o bzoc
