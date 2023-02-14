int main()
{
    int y = 300;
    int x = 399;
    while (y)
    {
        while (x)
        {
            DisplayPixel(x+y, 1, 0);
            x = x - 1;
        }
        DisplayPixel(x+y, 1, 1);
        x = 399;
        y = y - 1;
    }
    while (1) {}
    return;
}
