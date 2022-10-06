#include <stdio.h>
#include <stdlib.h>
#include "windef.h"
int main(int argc, char const *argv[])
{
    int a = 1, b = 2;
    printf("%d", max(a++, b++));
    return 0;
}
