#include "unistd.h"
#include "stdio.h"

int main(char* argv[], int argc) {
    if (argc == 2)
    {
        printf("Got file name : %s\n", argv[1]);
    }

    printf("Welcome to DarkForest's vi!\n");


    std::update_cursor(12, 12);

    std::sleep_ms(10000);

    return 0;
}
