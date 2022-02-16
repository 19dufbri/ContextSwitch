int BlockNum = 0;
int index = 0;
headerPtrPtr freeList;
int totalLength;
addr totalMemory;
int blocksize;

int MagicNumberOff = 0;
int is_freeOff = 1;
int binary_IndexOff = 2;
int sideOff = 3;
int inheritanceOff = 4;
int sizeOff = 5;
int nextOff = 6;
int prevOff = 7;

void _remove(headerPtr h) {
    if (ReadMem(h + prevOff) == NULL && ReadMem(h + nextOff) == NULL) {
        WriteMem(freeList + ReadMem(h + binary_IndexOff), NULL);
    }
    if (ReadMem(h + prevOff) == NULL && ReadMem(h + nextOff) != NULL) {
        WriteMem(freeList + ReadMem(h + binary_IndexOff), ReadMem(h, nextOff));
        WriteMem(ReadMem(h + nextOff) + prevOff, NULL);
        WriteMem(h + nextOff, NULL);
    }
    if (ReadMem(h + prevOff) != NULL && ReadMem(h + nextOff) == NULL) {
        WriteMem(ReadMem(h + prevOff) + nextOff, NULL);
        WriteMem(h + prevOff, NULL);
    }
    if (ReadMem(h + prevOff) != NULL && ReadMem(h + nextOff) != NULL) {
        WriteMem(ReadMem(h + prevOff) + nextOff, ReadMem(h + nextOff));
        WriteMem(ReadMem(h + nextOff) + prevOff, ReadMem(h + prevOff));
        WriteMem(h + prevOff, NULL);
        WriteMem(h + nextOff, NULL);
    }
}

void SplitBlocks(headerPtr h) {
    if (ReadMem(h + MagicNumberOff) == 3028){
        if (ReadMem(h + binary_IndexOff) > 0){
            int b_index = ReadMem(h + binary_IndexOff) - 1;
            int newSize = Multiply(Power(2, b_index), blockSize);
            headerPtr hR = h + newSize;
            _remove(h);
            WriteMem(hR + inheritanceOff, ReadMem(h + inheritanceOff));
            WriteMem(h + inheritanceOff, ReadMem(h + sideOff));

            WriteMem(h + MagicNumberOff, 3028);
            WriteMem(h + is_freeOff, 0);
            WriteMem(h + binary_IndexOff, b_index);
            WriteMem(h + sideOff, 0);
            WriteMem(h + sizeOff, newsize);
            WriteMem(h + nextOff, _hR);

            WriteMem(hR + MagicNumberOff, 3028);
            WriteMem(hR + is_freeOff, 0);
            WriteMem(hR + binary_IndexOff, b_index);
            WriteMem(hR + sideOff, 1);
            WriteMem(hR + sizeOff, newsize);
            WriteMem(hR + prevOff, h);
            
            if (ReadMem(freeList + b_index) == NULL) {
                WriteMem(freeList + b_index, h);
                WriteMem(h + prevOff, NULL);
                WriteMem(hR + next, NULL);
            } else {
                WriteMem(hR + next, ReadMem(freeList + b_index));
                WriteMem(ReadMem(freeList + b_index) + prevOff, hR);
                WriteMem(_h + prev, NULL);
                WriteMem(freeList + b_index, _h);
            }
        }
        else {
            PrintString("error: cannot split last block");
        }
    }
    else {
        PrintString("error: no magic number found");
    }
}

void MergeBlocks(headerPtr h){
    if(ReadMem(h, MagicNumberOff) == 3028){
        if(ReadMem(h, binary_IndexOff) < index){
            int newindex = ReadMem(h, binary_IndexOff) + 1;
            int newsize = Multiply(Power(2,newindex), blocksize);
            headerPtr _h_ = NULL;
            if (ReadMem(_h + side) == 0) {
                headerPtr temp = _h + ReadMem(_h + sizeOff);
                _h_ = temp;
            } else {
                headerPtr temp = _h - ReadMem(_h + sizeOff);
                _h_ = temp;
            }
            if (ReadMem(_h + is_freeOff) == 0) {
                if(ReadMem(_h_ + MagicNumber) == 3028) {
                    if(ReadMem(_h_ + is_free) == 0) {
                        if(ReadMem(_h_ + size) == ReadMem(_h + size)){
                            _remove(_h);
                            _remove(_h_);

                            if (ReadMem(_h + side) == 0) {
                                WriteMem(_h + MagicNumber, 3028);
                                WriteMem(_h + is_free, 0);
                                WriteMem(_h + binary_Index, newindex);
                                WriteMem(_h + side, ReadMem(_h + inheritance));
                                WriteMem(_h + inheritance, ReadMem(_h_ + inheritance));
                                WriteMem(_h + size, newsize);

                                if (ReadMem(freeList + newindex) == NULL) {
                                    WriteMem(freeList + newindex, _h);
                                    WriteMem(_h + prev, NULL);
                                    WriteMem(_h + next, NULL);
                                } else {
                                    WriteMem(_h + nextOff, ReadMem(freeList + newindex));
                                    WriteMem(ReadMem(freeList + newindex) + prevOff, _h);
                                    WriteMem(_h + prevOff, NULL);
                                    WriteMem(ReadMem(freeList + newindex), _h);
                                }
                                MergeBlocks(_h);
                            } else {
                                WriteMem(_h_ + MagicNumberOff, 3028);
                                WriteMem(_h_ + is_freeOff, 0);
                                WriteMem(_h_ + binary_IndexOff, newindex);
                                WriteMem(_h_ + sideOff, ReadMem(_h_ + inheritance));
                                WriteMem(_h_ + inheritanceOff, ReadMem(_h + inheritanceOff));
                                WriteMem(_h_ + sizeOff, newsize);

                                if (ReadMem(freeList + newindex) == NULL) {
                                    WriteMem(freeList + newindex, _h_);
                                    WriteMem(_h_ + prevOff, NULL);
                                    WriteMem(_h_ + nextOff, NULL);
                                } else {
                                    WriteMem(_h_ + nextOff, ReadMem(freeList + newindex));
                                    WriteMem(ReadMem(freeList + newindex) + prevOff, _h_);
                                    WriteMem(_h_ + prevOff, NULL);
                                    WriteMem(freeList + newindex, _h_);
                                }
                                MergeBlocks(_h_);
                            }
                        }
                    }
                }
            }
        }
    }
}

int release_allocator(){
    free(freeList);
    free(totalMemory);
    return 0;
}

int init_allocator(int _length){
    blocksize = 40;
    while (BlockNum < _length / blocksize) {
        index++;
        BlockNum = Power(2, index);
    }
    totalMemory = malloc(BlockNum * blocksize);
    freeList = malloc(index * sizeof(HDR));
    headerPtr a = totalMemory;
    WriteMem(a + MagicNumberOff, 3028);
    WriteMem(a + is_freeOff, 0);
    WriteMem(a + binary_IndexOff, index);
    WriteMem(a + sideOff, 2);
    WriteMem(a + inheritanceOff, 2);
    WriteMem(a + sizeOff, Multiply(BlockNum, blocksize));
    WriteMem(a + nextOff, NULL);
    WriteMem(a + prevOff, NULL);
    WriteMem(freeList + index, a);

    totalLength = Multiply(BlockNum, blocksize);
    return totalLength;

}

int my_malloc(int _length) {
    _length = _length + 40;
    int valid_length = 0;
    int valid_index = 0;
    headerPtr return_hdr;
    if (_length < 40) {
        return NULL;
    }
    else{
        while (valid_length < _length) {
            valid_index = valid_index + 1;
            valid_length = Multiply(Power(2, valid_index), blocksize);
        }
    }

    if (index < valid_index) {
        return NULL;
    } else {
        int _index = valid_index;
        while (ReadMem(freeList + _index) == NULL) {
            _index = _index + 1;
        }
        if (_index > index) {
            return NULL;
        }
        else {
            while (_index > valid_index) {
                SplitBlocks(ReadMem(freeList + _index));
                _index = _index - 1;
            }
            return_hdr = ReadMem(freeList + valid_index);
            _remove(ReadMem(freeList + valid_index));
            if (ReadMem(return_hdr + MagicNumberOff) == 3028) {
                WriteMem(return_hdr + is_freeOff, 1);
            } else {
                return NULL;
            }

            return return_hdr + 8;
        }
    }
}

int my_free(address a) {
    if (a == NULL) {
        PrintString("error: address is NULL");
        return 1;
    } else {
        headerPtr temp = a - 8;

        int newindex = ReadMem(temp + binary_IndexOff);
        WriteMem(temp + is_freeOff, 0);

        if (ReadMem(freeList + newindex) == NULL) {
            WriteMem(freeList + newindex, temp);
            WriteMem(temp + prev, NULL);
            WriteMem(temp + next, NULL);
        } else {
            WriteMem(temp + nextOff, ReadMem(freeList + newindex));
            WriteMem(ReadMem(freeList + newindex) + prevOff, temp);
            WriteMem(temp + prevOff, NULL);
            WriteMem(freeList + newindex, temp);
        }
        MergeBlocks(temp);
    }
    return 0;
}
