all:
	gcc -Wall -Iinclude -I"../common/include" -O3 src/* ../common/src/* -o csa-asm

debug:
	gcc -Wall -Iinclude -I"../common/include" -g -Og src/* ../common/src/* -o csa-asm

clean:
	rm -rvf bin/ csa-asm