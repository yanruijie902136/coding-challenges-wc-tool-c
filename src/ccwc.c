#include <err.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static int exit_status = EXIT_SUCCESS;

static void print_usage_and_exit(void)
{
        fprintf(stderr, "usage: ccwc [-c] file ...\n");
        exit(EXIT_FAILURE);
}

static char *const *check_usage(int argc, char *const argv[])
{
        int opt;
        while ( (opt = getopt(argc, argv, ":c")) != -1)
        {
                switch (opt)
                {
                case 'c':
                        break;
                case '?':
                        warnx("unrecognized option -- %c", optopt);
                        print_usage_and_exit();
                }
        }

        char *const *paths = argv + optind;
        if (paths[0] == NULL)
        {
                print_usage_and_exit();
        }
        return paths;
}

static void report_file(const char *file)
{
        int fd = open(file, O_RDONLY);
        if (fd < 0)
        {
                warn("%s", file);
                exit_status = EXIT_FAILURE;
                return;
        }

        static char buffer[BUFSIZ];
        size_t nbytes = 0;
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
                        printf("%zu %s\n", nbytes, file);
                        close(fd);
                        return;
                }
                nbytes += nr;
        }
}

int main(int argc, char *const argv[])
{
        char *const *paths = check_usage(argc, argv);
        for (unsigned i = 0; paths[i] != NULL; i++)
        {
                report_file(paths[i]);
        }
        exit(exit_status);
}
