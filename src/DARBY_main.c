/*
DARBY: Read the Word of God from your terminal

License: Public domain
*/

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <readline/readline.h>
#include <readline/history.h>
#include <sys/ioctl.h>

#include "DARBY_config.h"
#include "DARBY_data.h"
#include "DARBY_ref.h"
#include "DARBY_render.h"
#include "strutil.h"

const char *
usage = "usage: DARBY [flags] [reference...]\n"
    "\n"
    "Flags:\n"
    "  -A num  show num verses of context after matching verses\n"
    "  -B num  show num verses of context before matching verses\n"
    "  -W num  enable line wrap to a specific width\n"
    "  -C      show matching verses in context of the chapter\n"
    "  -e      highlighting of chapters and verse numbers\n"
    "          (default when output is a TTY)\n"
    "  -p      output to less with chapter grouping, spacing, indentation,\n"
    "          and line wrapping\n"
    "          (default when output is a TTY)\n"
    "  -b      have a blank line after each verse\n"
    "  -i      remove all highlighting\n"
    "  -l      list books\n"
    "  -h      show help\n"
    "\n"
    "Reference:\n"
    "    <Book>\n"
    "        Individual book\n"
    "    <Book>:<Chapter>\n"
    "        Individual chapter of a book\n"
    "    <Book>:<Chapter>:<Verse>[,<Verse>]...\n"
    "        Individual verse(s) of a specific chapter of a book\n"
    "    <Book>:<Chapter>-<Chapter>\n"
    "        Range of chapters in a book\n"
    "    <Book>:<Chapter>:<Verse>-<Verse>\n"
    "        Range of verses in a book chapter\n"
    "    <Book>:<Chapter>:<Verse>-<Chapter>:<Verse>\n"
    "        Range of chapters and verses in a book\n"
    "\n"
    "    /<Search>\n"
    "        All verses that match a pattern\n"
    "    <Book>/<Search>\n"
    "        All verses in a book that match a pattern\n"
    "    <Book>:<Chapter>/<Search>\n"
    "        All verses in a chapter of a book that match a pattern\n";

int
main(int argc, char *argv[])
{
    bool is_atty = isatty(STDOUT_FILENO) == 1;
    DARBY_config config = {
        .highlighting = is_atty,
        .pretty = is_atty,

        .maximum_line_length = 80,

        .context_before = 0,
        .context_after = 0,
        .context_chapter = false,
        .blank_line_after_verse = false,
        .change_line_width = true,
    };

    bool list_books = false;

    opterr = 0;
    for (int opt; (opt = getopt(argc, argv, "A:B:W:Cbepilh")) != -1; ) {
        char *endptr;
        switch (opt) {
        case 'A':
            config.context_after = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "DARBY: invalid flag value for -A\n\n%s", usage);
                return 1;
            }
            break;
        case 'B':
            config.context_before = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "DARBY: invalid flag value for -B\n\n%s", usage);
                return 1;
            }
            break;
        case 'W':
            config.maximum_line_length = strtol(optarg, &endptr, 10);
            if (endptr[0] != '\0') {
                fprintf(stderr, "DARBY: invalid flag value for -W\n\n%s", usage);
                return 1;
            }
            config.change_line_width = false;
            break;
        case 'C':
            config.context_chapter = true;
            break;
        case 'b':
            config.blank_line_after_verse = true;
            break;
        case 'e':
            config.highlighting = true;
            break;
        case 'i':
            config.highlighting = false;
            break;
        case 'p':
            config.pretty = true;
            break;
        case 'l':
            list_books = true;
            break;
        case 'h':
            printf("%s", usage);
            return 0;
        case '?':
            fprintf(stderr, "DARBY: invalid flag -%c\n\n%s", optopt, usage);
            return 1;
        }
    }

    if (list_books) {
        for (int i = 0; i < DARBY_books_length; i++) {
            DARBY_book *book = &DARBY_books[i];
            printf("%s (%s)\n", book->name, book->abbr);
        }
        return 0;
    }

    struct winsize ttysize;
    if (config.change_line_width && ioctl(STDOUT_FILENO, TIOCGWINSZ, &ttysize) == 0 && ttysize.ws_col > 0) {
        config.maximum_line_length = ttysize.ws_col;
    }

    signal(SIGPIPE, SIG_IGN);

    if (argc == optind) {
        using_history();
        while (true) {
            char *input = readline("DARBY> ");
            if (input == NULL) {
                break;
            }
            add_history(input);
            DARBY_ref *ref = DARBY_newref();
            int success = DARBY_parseref(ref, input);
            free(input);
            if (success == 0) {
                DARBY_render(ref, &config);
            }
            DARBY_freeref(ref);
        }
    } else {
        char *ref_str = str_join(argc-optind, &argv[optind]);
        DARBY_ref *ref = DARBY_newref();
        int success = DARBY_parseref(ref, ref_str);
        free(ref_str);
        if (success == 0) {
            DARBY_render(ref, &config);
        }
        DARBY_freeref(ref);
    }

    return 0;
}

