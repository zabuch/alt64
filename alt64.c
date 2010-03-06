#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#define VER             "0.1"

#define MIN_TEXT_LEN    0x003
#define MAX_TEXT_LEN    0x400

#define FMT_LINE        0
#define FMT_GREP        1

void logo ();
void help (char* prog);
void alt64 (char* input, size_t len, char* sep);

extern int optopt, optind;

static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
                           "ghijklmnopqrstuvwxyz0123456789+/";

int main (int argc, char** argv)
{
    int     c;
    int     fmt = FMT_LINE;
    char    buf[MAX_TEXT_LEN + 5];
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
            fprintf(stderr, "Error: unexpected getopt return value: %d.\n", c);
            return (2);
        }
    }

    assert(optind <= argc);

    buf[0] = buf[1] = 0;
    input = &buf[2];

    if (optind == argc)
    {
        /* no input string, read from stdin */
        len = fread(input, 1, MAX_TEXT_LEN, stdin);
    }
    else
    {
        input[MAX_TEXT_LEN] = 0;
        strncpy(input, argv[optind], MAX_TEXT_LEN);
        len = strlen(input);
    }

    if (len < MIN_TEXT_LEN)
    {
        fprintf(stderr, "Error: input text too short!\n");
        return (1);
    }
    input[len] = 0;
    input[len + 1] = 0;

    alt64(input, len, fmt == FMT_LINE ? "\n" : "|");

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

/*
** encode_block
**
** encode 3 8-bit binary bytes as 4 '6-bit' characters
*/
void encode_block (unsigned char* in, unsigned char* out, int len)
{
    out[0] = cb64[ in[0] >> 2 ];
    out[1] = cb64[ ((in[0] & 0x03) << 4) | ((in[1] & 0xF0) >> 4) ];
    out[2] = (unsigned char) (len > 1 ? cb64[ ((in[1] & 0x0f) << 2) | ((in[2] & 0xc0) >> 6) ] : '=');
    out[3] = (unsigned char) (len > 2 ? cb64[ in[2] & 0x3f ] : '=');
}

void base64_encode (char* output, unsigned char* input, size_t len)
{
    size_t  i;

    for (i = 0; i < len; i += 3, input += 3, output += 4)
    {
        encode_block(input, output, len - i);
    }
}

void alt64 (char* input, size_t len, char* sep)
{
    char    out[(MAX_TEXT_LEN + 4) * 3 / 4 + 1];
    int     sep_len;

    sep_len = strlen(sep);
    base64_encode(out, input, len);
    fwrite(out, 1, (len / 3) * 4 + (len % 3), stdout);
    fwrite(sep, 1, sep_len, stdout);
}


