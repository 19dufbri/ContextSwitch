void PrintChar(int c);
void PrintString(string str);

void SetMem(int addr, int value);
int ReadMem(int addr);

void AddInts(int addr, int ah, int al, int bh, int bl);
int GetResultMemAddr();

int RotateLeft(int in, int shift);
void PrintHex(int in);

void Multiply(int addr, int inA, int inB);
int GetRawReg(int reg);
