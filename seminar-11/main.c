#include <stdio.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>


int print_file(const char *path)
{
    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror(path);
        return 1;
    }

    char buffer[1024] = {'\0'};
    size_t size = 0;

    while ((size = read(file, buffer, 1024)) > 0) {
        fwrite(buffer, 1, size, stdout);
    }

    if (close(file) != 0) {
        perror(path);
        return 1;
    }

    return 0;
}

int print_stats(const char *path)
{
    int file = open(path, O_RDONLY);
    if (file == -1) {
        perror(path);
        return 1;
    }

    struct stat buff;
    if (fstat(file, &buff) != 0) {
        return 1;
    }

    printf("Velikost souboru: %ld\n", buff.st_size);
    printf("Datum posledniho pristupu: %s", ctime(&buff.st_ctim.tv_sec));
    printf("UID majitele: %u\n", buff.st_uid);
    printf("GID majitele: %u\n", buff.st_gid);

    if (close(file) != 0) {
        perror(path);
        return 1;
    }
    return 0;
}

int read_directory(const char *path, int (*func)(const char *));

int main(int argc, char *argv[])
{
	if (argc < 3) {
		fprintf(stderr, "Invalid amount of arguments\n");
		return EXIT_FAILURE;
	}

    if (strcmp(argv[1], "-s") == 0) {
        for (int i = 2; i < argc; i++) {
            print_stats(argv[i]);
        }
    }
    if (strcmp(argv[1], "-p") == 0) {
        for (int i = 2; i < argc; i++) {
            print_file(argv[i]);
        }
    }


	return EXIT_SUCCESS;
}
