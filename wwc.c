#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <wchar.h>
#include <locale.h>

long compute_total_bytes(FILE *fileptr);
long compute_total_words(FILE *fileptr);
long compute_total_chars(FILE *fileptr);
long compute_total_lines(FILE *fileptr);
bool are_flags_valid(const char *flags);

int main(int argc, char *argv[])
{
    /*
    Flags
    -c : number of bytes
    -l : number of lines
    -w : number of words
    -m : number of charcter
    */
    char *has_cflag = NULL;
    char *has_lflag = NULL;
    char *has_wflag = NULL;
    char *has_mflag = NULL;
    char *all_flags[4] = {has_cflag, has_lflag, has_wflag, has_mflag};
    char flag_type[] = {'c', 'l', 'w', 'm'};
    long (*function_array[])(FILE *fileptr) = {
        compute_total_bytes,
        compute_total_lines,
        compute_total_words,
        compute_total_chars,
    };
    bool has_valid_flag = false;
    char *flag = NULL;
    char result_buffer[256] = "";
    FILE *file;
    long results[4] = {0, 0, 0, 0};

    if (argc > 3)
    {
        fprintf(stderr, "Usage: %s <flags> <filename>\n", argv[0]);
        return 1;
    }

    if (argc > 2)
    {
        flag = argv[1];
        has_valid_flag = are_flags_valid(flag);

        if (!has_valid_flag)
        {
            fprintf(stderr, "Flags must start with a hyphen and only m, l, w, c are supported\n");
            return 1;
        }
    }

    if (!has_valid_flag)
    {
        file = fopen(argv[1], "rb");
        if (file == NULL)
        {
            perror("Error opening file");
            return 1;
        }

        // Compute all the flags
        for (int i = 0; i < 4; i++)
        {
            rewind(file); // reset file pointer
            results[i] = function_array[i](file);
            char temp[64]; // temp buffer for each result
            snprintf(temp, sizeof(temp), "%ld ", results[i]);
            strcat(result_buffer, temp); // add the temp result to the buffer
        }
    }

    else if (has_valid_flag)
    {

        file = fopen(argv[2], "rb");
        if (file == NULL)
        {
            perror("Error opening file");
            return 1;
        }

        for (int i = 0; i < 4; i++)
        {
            all_flags[i] = strchr(flag, flag_type[i]);
            if (all_flags[i] != NULL)
            {
                rewind(file); // reset file pointer
                results[i] = function_array[i](file);
                char temp[64]; // temp buffer for each result
                snprintf(temp, sizeof(temp), "%ld ", results[i]);
                strcat(result_buffer, temp); // add the temp result to the buffer
            };
        }
    }

    printf("%s\n", result_buffer);

    fclose(file);
    return 0;
}

long compute_total_bytes(FILE *fileptr)
{
    long file_size;
    fseek(fileptr, 0, SEEK_END);
    file_size = ftell(fileptr);
    fseek(fileptr, 0, SEEK_SET);

    return file_size;
}

long compute_total_lines(FILE *fileptr)
{
    long line_count = 0;
    char line[1024];

    while (fgets(line, sizeof(line), fileptr) != NULL)
    {
        line_count++;

        // Check if the line was too long
        if (line[strlen(line) - 1] != '\n' && !feof(fileptr))
        {
            int ch;
            while ((ch = fgetc(fileptr)) != EOF && ch != '\n')
                ;
        }
    }

    if (ferror(fileptr))
    {
        fprintf(stderr, "An error occurred while reading the file.\n");
        return -1;
    }

    return line_count;
}

long compute_total_chars(FILE *fileptr)
{
    setlocale(LC_ALL, "en_US.UTF-8");

    long char_count = 0;
    int ch;
    mbstate_t state = {0}; // mbstate_t : multibyte state object
    char buf[MB_CUR_MAX];
    size_t nbytes;

    while ((ch = fgetc(fileptr)) != EOF)
    {
        buf[0] = ch;
        nbytes = 1;

        if ((ch & 0xC0) == 0xC0)
        { // 0xC0 = 11000000, checks if the first two bits are 1 for UTF-8 encoding
            while ((ch = fgetc(fileptr)) != EOF && (ch & 0xC0) == 0x80)
            {
                buf[nbytes++] = ch;
            }
            ungetc(ch, fileptr);
        }

        if (mbrtowc(NULL, buf, nbytes, &state) != (size_t)-1)
        {
            char_count++;
        }
    }

    if (ferror(fileptr))
    {
        fprintf(stderr, "An error occurred while reading the file.\n");
        return -1;
    }

    return char_count;
}
long compute_total_words(FILE *fileptr)
{
    long word_count = 0;
    char ch;
    bool in_word = false;

    while ((ch = fgetc(fileptr)) != EOF)
    {
        if (ch == ' ' || ch == '\n' || ch == '\t')
        {
            in_word = false;
        }
        else if (!in_word)
        {
            in_word = true;
            word_count++;
        }
    }

    return word_count;
}

bool are_flags_valid(const char *flags)
{
    if (*flags == '-')
    {
        flags++;
    }

    // Check each character
    while (*flags)
    {
        if (*flags != 'w' && *flags != 'l' && *flags != 'c' && *flags != 'm')
        {
            return false;
        }
        flags++;
    }
    return true;
}