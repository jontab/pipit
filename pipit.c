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

#include <errno.h>
#include <getopt.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <termios.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

#define CLEAR_SCREEN L"\x1b[2J"
#define CURSOR_HOME L"\x1b[H"

#define CTRL_KEY(K) (K) & 0x1f

#define DIE_IF(E)    \
    do               \
    {                \
        if (E)       \
            die(#E); \
    } while (0)

/******************************************************************************/
/* Data                                                                       */
/******************************************************************************/

struct
{
    const char *host;
    const char *port;
    const char *filename;
    bool server;

    struct termios termios;
    wint_t key;
} E;

/******************************************************************************/
/* Terminal                                                                   */
/******************************************************************************/

void die(const char *s)
{
    wprintf(CLEAR_SCREEN);
    wprintf(CURSOR_HOME);

    fwprintf(stderr, L"%s: %s\n", s, strerror(errno));
    exit(EXIT_FAILURE);
}

void noraw(void)
{
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &E.termios);
}

void raw(void)
{
    DIE_IF(tcgetattr(STDIN_FILENO, &E.termios) < 0);
    DIE_IF(atexit(noraw) < 0);

    struct termios to = E.termios;
    to.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    to.c_oflag &= ~(OPOST);
    to.c_cflag |= CS8;
    to.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);

    DIE_IF(tcsetattr(STDIN_FILENO, TCSAFLUSH, &to) < 0);
}

wint_t getch(void)
{
    wint_t key = fgetwc(stdin);
    DIE_IF(key < 0);
    return key;
}

/******************************************************************************/
/* Output                                                                     */
/******************************************************************************/

void refresh(void)
{
    wprintf(CLEAR_SCREEN);
    wprintf(CURSOR_HOME);

    if (iswcntrl(E.key))
    {
        wprintf(L"%d", E.key);
    }
    else
    {
        wprintf(L"%d ('%lc')", E.key, E.key);
    }
}

/******************************************************************************/
/* Input                                                                      */
/******************************************************************************/

void process_input(void)
{
    wint_t key = getch();
    switch (key)
    {
    case CTRL_KEY('d'):
        wprintf(CLEAR_SCREEN);
        wprintf(CURSOR_HOME);
        exit(EXIT_SUCCESS);

    default:
        E.key = key;
        break;
    }
}

/******************************************************************************/
/* Parsing                                                                    */
/******************************************************************************/

void usage(FILE *file, const char *program)
{
    fwprintf(file, L"Usage: %s [OPTION] FILENAME\n", program);
    fwprintf(file, L"\n");
    fwprintf(file, L"A collaborative text editor for the terminal.\n");
    fwprintf(file, L"\n");
    fwprintf(file, L"Options:\n");
    fwprintf(file, L"     -i HOST     Host of server (default: " DEFAULT_HOST ")\n");
    fwprintf(file, L"     -p PORT     Port of server (default: " DEFAULT_PORT ")\n");
    fwprintf(file, L"     -s          Run as server (default: false)\n");
    fwprintf(file, L"     -h          Show this help message and exit\n");
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
        fwprintf(stderr, L"%s: missing operand\n", *argv);
    more:
        fwprintf(stderr, L"Try '%s -h' for more information.\n", *argv);
        exit(EXIT_FAILURE);
    }
}

/******************************************************************************/
/* Main                                                                       */
/******************************************************************************/

int main(int argc, char **argv)
{
    setlocale(LC_ALL, "");
    parse(argc, argv);
    raw();

    while (1)
    {
        refresh();
        process_input();
    }

    return EXIT_SUCCESS;
}
