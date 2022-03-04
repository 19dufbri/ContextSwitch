#include "./PrintHead.h"

int main() {
    int result = GetResultMemAddr();
    CMultiply(result, 0xABCD, 0x5678);
    PrintString("0x");
    PrintHex(ReadMem(result));
    PrintHex(ReadMem(result+1));
    
    PrintLine(" ");
    
    if (ReadMem(result) == 0x3A07) {
        if (ReadMem(result+1) == 0x6618) {
            PrintLine("We're good!");
        } else {
            PrintLine("Bad Low Short");
        }
    } else {
        PrintLine("Bad High Short");
    }
	return 0x00;
}

void CMultiply(int result, int inA, int inB) {
    int tmpHigh = 0;
    int tmpLow = inB;
    int i = 16;
    int ptr = 1;
    int b = GetRawReg(6);
    b = b - 7;
    
    SetMem(result, 0);
    SetMem(result + 1, 0);
    
    while (i) {
        if (ptr & inA) {
            AddInts(result, ReadMem(result), ReadMem(result+1), ReadMem(b), ReadMem(b+1));
        }
        i = i - 1;
        ptr = ptr + ptr;
        AddInts(b, ReadMem(b), ReadMem(b+1), ReadMem(b), ReadMem(b+1));
    }
    
    return;
}
