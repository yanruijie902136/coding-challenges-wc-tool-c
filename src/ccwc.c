#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int exit_status = EXIT_SUCCESS;

enum count_mode
{
        COUNT_BYTES = 0x01,
        COUNT_LINES = 0x02,
};

struct input_args
{
        char *const *files;
        int modes;
};

static void print_usage_and_exit(void)
{
        fprintf(stderr, "usage: ccwc [-cl] file ...\n");
        exit(EXIT_FAILURE);
}

static struct input_args check_usage(int argc, char *const argv[])
{
        struct input_args args;
        args.modes = 0;

        int opt;
        while ( (opt = getopt(argc, argv, ":cl")) != -1)
        {
                switch (opt)
                {
                case 'c':
                        args.modes |= COUNT_BYTES;
                        break;
                case 'l':
                        args.modes |= COUNT_LINES;
                        break;
                case '?':
                        warnx("unrecognized option -- %c", optopt);
                        print_usage_and_exit();
                }
        }

        args.files = argv + optind;
        if (args.files[0] == NULL)
        {
                print_usage_and_exit();
        }
        if (args.modes == 0)
        {
                args.modes = COUNT_BYTES | COUNT_LINES;
        }

        return args;
}

static void report_file(const char *file, int modes)
{
        int fd = open(file, O_RDONLY);
        if (fd < 0)
        {
                warn("%s", file);
                exit_status = EXIT_FAILURE;
                return;
        }

        static char buffer[BUFSIZ];
        size_t nbytes = 0, nlines = 0;
        for ( ; ; )
        {
                ssize_t nr = read(fd, buffer, sizeof(buffer));
                if (nr < 0)
                {
                        warn("%s", file);
                        exit_status = EXIT_FAILURE;
                        close(fd);
                        return;
                }
                else if (nr == 0)
                {
                        break;
                }

                for (size_t i = 0; i < nr; i++)
                {
                        nbytes++;
                        if (buffer[i] == '\n')
                        {
                                nlines++;
                        }
                }
        }

        if (modes & COUNT_LINES)
        {
                printf("%zu ", nlines);
        }
        if (modes & COUNT_BYTES)
        {
                printf("%zu ", nbytes);
        }
        printf("%s\n", file);
        close(fd);
}

int main(int argc, char *const argv[])
{
        struct input_args args = check_usage(argc, argv);
        for (unsigned i = 0; args.files[i] != NULL; i++)
        {
                report_file(args.files[i], args.modes);
        }

        exit(exit_status);
}
