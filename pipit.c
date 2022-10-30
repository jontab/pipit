/**
 * MIT License
 *
 * Copyright (c) 2022 Jonathan Abbott
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 *
 */

#define DEFAULT_HOST "localhost"
#define DEFAULT_PORT "8080"

#include <getopt.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

/******************************************************************************/
/* Data                                                                       */
/******************************************************************************/

struct
{
    const char *host;
    const char *port;
    const char *filename;
    bool server;
} E;

/******************************************************************************/
/* Parsing                                                                    */
/******************************************************************************/

void usage(FILE *file, const char *program)
{
    fprintf(file, "Usage: %s [OPTION] FILENAME\n", program);
    fprintf(file, "\n");
    fprintf(file, "A collaborative text editor for the terminal.\n");
    fprintf(file, "\n");
    fprintf(file, "Options:\n");
    fprintf(file, "     -i HOST     Host of server (default: " DEFAULT_HOST ")\n");
    fprintf(file, "     -p PORT     Port of server (default: " DEFAULT_PORT ")\n");
    fprintf(file, "     -s          Run as server (default: false)\n");
    fprintf(file, "     -h          Show this help message and exit\n");
}

void parse(int argc, char **argv)
{
    E.host = DEFAULT_HOST;
    E.port = DEFAULT_PORT;
    E.server = false;

    int result;
    while ((result = getopt(argc, argv, "i:p:sh")) >= 0)
    {
        switch (result)
        {
        case 'i':
            E.host = optarg;
            break;

        case 'p':
            E.port = optarg;
            break;

        case 's':
            E.server = true;
            break;

        case 'h':
            usage(stdout, *argv);
            exit(EXIT_SUCCESS);

        default:
            goto more;
        }
    }

    if (optind < argc)
    {
        E.filename = argv[optind];
    }
    else
    {
        fprintf(stderr, "%s: missing operand\n", *argv);
    more:
        fprintf(stderr, "Try '%s -h' for more information.\n", *argv);
        exit(EXIT_FAILURE);
    }
}

/******************************************************************************/
/* Main                                                                       */
/******************************************************************************/

int main(int argc, char **argv)
{
    parse(argc, argv);

    printf("E.host = %s\n", E.host);
    printf("E.port = %s\n", E.port);
    printf("E.filename = %s\n", E.filename);
    printf("E.server = %s\n", E.server ? "true" : "false");

    return EXIT_SUCCESS;
}
