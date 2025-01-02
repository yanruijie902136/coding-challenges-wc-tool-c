#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <wchar.h>
#include <wctype.h>

static int exit_status = EXIT_SUCCESS;

enum count_mode
{
        COUNT_BYTES = 0x01,
        COUNT_LINES = 0x02,
        COUNT_WORDS = 0x04,
        COUNT_CHARS = 0x08,
};

struct input_args
{
        char *const *files;
        int modes;
};

static void print_usage_and_exit(void)
{
        fprintf(stderr, "usage: ccwc [-clmw] [file ...]\n");
        exit(EXIT_FAILURE);
}

static struct input_args check_usage(int argc, char *const argv[])
{
        struct input_args args;
        args.modes = 0;

        int opt;
        while ( (opt = getopt(argc, argv, ":clmw")) != -1)
        {
                switch (opt)
                {
                case 'c':
                        args.modes &= ~COUNT_CHARS;
                        args.modes |= COUNT_BYTES;
                        break;
                case 'l':
                        args.modes |= COUNT_LINES;
                        break;
                case 'm':
                        args.modes &= ~COUNT_BYTES;
                        args.modes |= COUNT_CHARS;
                        break;
                case 'w':
                        args.modes |= COUNT_WORDS;
                        break;
                case '?':
                        warnx("unrecognized option -- %c", optopt);
                        print_usage_and_exit();
                }
        }

        args.files = argv + optind;
        if (args.modes == 0)
        {
                args.modes = COUNT_BYTES | COUNT_LINES | COUNT_WORDS;
        }
        return args;
}

static void report_file(const char *file, int modes)
{
        FILE *fp = stdin;
        if (file != NULL && (fp = fopen(file, "r")) == NULL)
        {
                warn("%s", file);
                exit_status = EXIT_FAILURE;
                return;
        }

        size_t nbytes = 0, nchars = 0, nlines = 0, nwords = 0;

        static char buffer[sizeof(wint_t)];
        wint_t prevch = L' ', currch;
        while ( (currch = fgetwc(fp)) != WEOF)
        {
                nchars++;
                nbytes += wctomb(buffer, currch);
                if (currch == L'\n')
                {
                        nlines++;
                }
                if (iswspace(prevch) && !iswspace(currch))
                {
                        nwords++;
                }
                prevch = currch;
        }

        if (fp != stdin)
        {
                fclose(fp);
        }

        if (modes & COUNT_LINES)
        {
                printf("%zu ", nlines);
        }
        if (modes & COUNT_WORDS)
        {
                printf("%zu ", nwords);
        }
        if (modes & COUNT_BYTES)
        {
                printf("%zu ", nbytes);
        }
        if (modes & COUNT_CHARS)
        {
                printf("%zu ", nchars);
        }
        printf("%s\n", file == NULL ? "" : file);
}

int main(int argc, char *const argv[])
{
        setlocale(LC_CTYPE, "");

        struct input_args args = check_usage(argc, argv);
        if (args.files[0] == NULL)
        {
                report_file(NULL, args.modes);
        }
        else
        {
                for (unsigned i = 0; args.files[i] != NULL; i++)
                {
                        report_file(args.files[i], args.modes);
                }
        }

        exit(exit_status);
}
