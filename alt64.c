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
void alt64 (char* input, size_t len, int fmt);

extern int optopt, optind;

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

void base64_encode (char* out, unsigned char* in, size_t len)
{
    static const char cb64[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdef"
                               "ghijklmnopqrstuvwxyz0123456789+/";

    size_t  i;

    for (i = 0; i < len; i += 3, in += 3)
    {
        *out++ = cb64[in[0] >> 2];
        *out++ = cb64[((in[0] & 0x03) << 4) | (in[1] >> 4)];
        *out++ = cb64[((in[1] & 0x0F) << 2) | (in[2] >> 6)];
        *out++ = cb64[in[2] & 0x3F];
    }
}

#define exact_out_len(_len) ((_len) / 3) * 4 + ((_len) % 3)

/* input buffer *MUST* have two bytes accessable before and after
   the relevant data 
   */
void alt64 (char* input, size_t len, int fmt)
{
    char    out[(MAX_TEXT_LEN + 4) * 3 / 4 + 1];
    int     sep_len, sep_last;
    char*   sep;

    switch (fmt)
    {
    case FMT_LINE:  sep = "\n"; sep_last = 1; break;
    case FMT_GREP:  sep = "|";  sep_last = 0; break;
    default:
        fprintf(stderr, "BUG: bad format %d.\n", fmt);
        exit(2);
    }
    sep_len = strlen(sep);

    base64_encode(out, input, len);
    fwrite(out, 1, exact_out_len(len), stdout);
    fwrite(sep, 1, sep_len, stdout);

    base64_encode(out, input - 1, len + 1);
    fwrite(&out[2], 1, exact_out_len(len + 1) - 2, stdout);
    fwrite(sep, 1, sep_len, stdout);

    base64_encode(out, input - 2, len + 2);
    fwrite(&out[3], 1, exact_out_len(len + 2) - 3, stdout);
    if (sep_last) fwrite(sep, 1, sep_len, stdout);
}


