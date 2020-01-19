#include "stdio.h"

int main(char* argv[], int argc) {
    if (argc == 2)
    {
        printf("Got file name : %s\n", argv[1]);
    }

    printf("Welcome to DarkForest's vi!\n");

    return 0;
}
