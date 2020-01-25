#include "unistd.h"
#include "stdio.h"

enum class Mode
{
    NORMAL,
    INSERT
};

int main(char* argv[], int argc) {
    if (argc == 2)
    {
        printf("Got file name : %s\n", argv[1]);
    }

    std::clear_screen(0);

    printf("Welcome to DarkForest's vi! press 'q' to quit\n");

    Mode current_mode = Mode::NORMAL;
    int current_col = 0;
    int current_row = 0;
    for (;;) {
        char c = getchar();

        if (current_mode == Mode::NORMAL)
        {
            if (c == 'h')
            {
                current_col -= 1;
            }
            else if (c == 'j')
            {
                current_row += 1;
            }
            else if (c == 'k')
            {
                current_row -= 1;
            }
            else if (c == 'l')
            {
                current_col += 1;
            }
            else if (c == 'q')
            {
                break;
            }
            else if (c == 'i')
            {
                current_mode = Mode::INSERT;
            }
        }
        else if (current_mode == Mode::INSERT)
        {
            putc(c);
        }

        std::update_cursor(current_col, current_row);
    }

    return 0;
}
