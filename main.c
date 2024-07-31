#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

int main(int argc, char *argv[])
{
    bool has_flag = false;

    if (argc > 2)
    {
        has_flag = true;
    }

    char* flag;
    if (has_flag)
    {
        flag = argv[1];
    }

    // number of bytes
    const char c_flag = 'c';
    // number of lines
    const char l_flag = 'l';
    // number of words
    const char w_flag = 'w';
    // number of characters
    const char m_flag = 'm';

    printf("%s\n", flag);
    printf("%d\n", has_flag);

    return 0;
}

int num_bytes(FILE *fileptr)
{
    long file_size;
    fseek(fileptr, 0, SEEK_END);
    file_size = ftell(fileptr);   
    fseek(fileptr, 0, SEEK_SET);

    return file_size;
}

