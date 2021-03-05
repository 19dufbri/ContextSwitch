all:
	gcc -Iinclude -I"../common/include" -O3 src/* ../common/src/* -o csa-vm

debug:
	gcc -Iinclude -I"../common/include" -g -Og src/* ../common/src/* -o csa-vm

clean:
	rm -rvf bin/ csa-vm