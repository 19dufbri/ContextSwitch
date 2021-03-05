all:
	gcc -Iinclude -I"../common/include" -O3 src/* ../common/src/* -o csa-comp

debug:
	gcc -Iinclude -I"../common/include" -g -Og src/* src/* ../common/src/* -o csa-comp

clean:
	rm -rvf bin/ csa-asm