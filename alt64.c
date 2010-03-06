#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define VER             "0.01"

#define MIN_TEXT_LEN    0x003
#define MAX_TEXT_LEN    0x400

#define FMT_LINE        0
#define FMT_GREP        1

void logo ();
void help (char* prog);
void alt64 (char* input, size_t len, int fmt);

extern int optopt, optind;

int main (int argc, char** argv)
{
    int     c;
    int     fmt = FMT_LINE;
    char    buf[MAX_TEXT_LEN + 1];
    char*   input;
    size_t  len; 

    while ((c = getopt(argc, argv, "+gvh")) != -1)
    {
        switch (c)
        {
        case 'h':
            help(argv[0]);
            return (0);

        case 'v':
            logo();
            return (0);

        case 'g':
            fmt = FMT_GREP;
            break;

        case '?':
            // fprintf(stderr, "Error: unknown option '-%c'.\n", optopt);
            return (1);

        default:
            printf("%d - oopt\n", c);
            return (2);
        }
    }

    assert(optind <= argc);

    if (optind == argc)
    {
        /* no input string, read from stdin */
        len = fread(buf, 1, MAX_TEXT_LEN, stdin);
        input = buf;
    }
    else
    {
        input = argv[optind];
        len = strlen(input);
    }

    if (len < MIN_TEXT_LEN)
    {
        fprintf(stderr, "Error: input text too short!\n");
        return (1);
    }

    alt64(input, len, fmt);

    return (0);
}

void logo ()
{
    printf("alt64 - ver " VER "\n");
}

void help (char* prog)
{
    logo();
    printf("Usage: %s -hvg [TEXT]\n", prog);
}

void alt64 (char* input, size_t len, int fmt)
{
    printf("processing %d bytes...\n", len);
}

