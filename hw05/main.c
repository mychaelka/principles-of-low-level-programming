#include "sample_printer.h"

#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <pwd.h>
#include <grp.h>
#include <dirent.h>
#include <string.h>

#define USER 1
#define GROUP 2
#define OTHER 3

struct arguments {
    int opt;
    char* file;
    char dir[256];
};

int is_file(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISREG(path_stat.st_mode);
}

int is_directory(const char *path)
{
    struct stat path_stat;
    stat(path, &path_stat);
    return S_ISDIR(path_stat.st_mode)
           && strcmp(path, ".") != 0
           && strcmp(path, "..") != 0;
}

bool load_arguments(int argc, char **argv, struct arguments *args)
{
    args->opt = getopt(argc, argv, "e:i:");
    if (args->opt == '?') {
        return false;
    }
    args->file = optarg;

    if (argc == 4) {
        strcpy(args->dir, argv[3]);
    } else {
        getcwd(args->dir, sizeof(args->dir));
    }
    return true;
}

char* concat_name(char* path, char* subdir)
{
    char* subdir_name = malloc(strlen(path) + strlen(subdir) + 2);
    if (subdir_name == NULL) {
        return NULL;
    }
    strcpy(subdir_name, path);
    strcat(subdir_name, "/");
    strcat(subdir_name, subdir);

    return subdir_name;
}

void print_permissions(struct stat* fstats, int who, FILE* file)
{
    if (who == USER) {
        fprintf(file,"user::");
        fprintf(file, (fstats->st_mode & S_IRUSR) ? "r" : "-");
        fprintf(file, (fstats->st_mode & S_IWUSR) ? "w" : "-");
        fprintf(file, (fstats->st_mode & S_IXUSR) ? "x" : "-");
    } else if (who == GROUP) {
        fprintf(file,"group::");
        fprintf(file, (fstats->st_mode & S_IRGRP) ? "r" : "-");
        fprintf(file, (fstats->st_mode & S_IWGRP) ? "w" : "-");
        fprintf(file, (fstats->st_mode & S_IXGRP) ? "x" : "-");
    } else if (who == OTHER) {
        fprintf(file,"other::");
        fprintf(file, (fstats->st_mode & S_IROTH) ? "r" : "-");
        fprintf(file, (fstats->st_mode & S_IWOTH) ? "w" : "-");
        fprintf(file, (fstats->st_mode & S_IXOTH) ? "x" : "-");
    }
    fputs("\n", file);
}

void print_flags(struct stat* fstats, FILE* file)
{
    if (!(fstats->st_mode & S_ISVTX)
        && !(fstats->st_mode & S_ISGID)
        && !(fstats->st_mode & S_ISUID)) {
        return;
    }
    fprintf(file,"# flags:");
    fprintf(file, (fstats->st_mode & S_ISUID) ? "s" : "-");
    fprintf(file, (fstats->st_mode & S_ISGID) ? "s" : "-");
    fprintf(file, (fstats->st_mode & S_ISVTX) ? "t" : "-");
    fputs("\n", file);
}

void formatted_output(struct stat* fstats, struct dirent* entry, FILE* file, char* path)
{
    struct passwd* user_info = getpwuid(fstats->st_uid);
    struct group* group_info = getgrgid(fstats->st_gid);
    fprintf(file,"# file: %s\n", entry->d_name);
    fprintf(file,"# owner: %s\n", user_info->pw_name);
    fprintf(file,"# group: %s\n", group_info->gr_name);

    print_flags(fstats, file);

    print_permissions(fstats, USER, file);
    print_permissions(fstats, GROUP, file);
    print_permissions(fstats, OTHER, file);
    fputs("\n", file);
}

bool directory_descent(char *path, FILE *file)
{
    struct dirent *entry;
    struct stat fstats;

    DIR* dir = opendir(path);
    if (dir == NULL) {
        fprintf(stderr, "Failed to open directory.\n");
        return false;
    }

    while ((entry = readdir(dir)) != NULL) {
        char* subdir_name = concat_name(path, entry->d_name);
        if (subdir_name == NULL) {
            return false;
        }

        if (stat(subdir_name, &fstats) == -1) {
            perror("stat");
            return false;
        }

        formatted_output(&fstats, entry, file, subdir_name);
        if (strcmp(entry->d_name, "..") != 0 && strcmp(entry->d_name, ".") != 0 && is_directory(subdir_name)) {
            directory_descent(subdir_name, file);
        }
        free(subdir_name);
    }
    closedir(dir);
    return true;
}

int main(int argc, char **argv) {
    if (argc < 3) {
        fprintf(stderr, "The PERMISSIONS_FILE argument is required.\n");
        return EXIT_FAILURE;
    }
    if (argc > 4) {
        fprintf(stderr, "Too many arguments.\n");
        return EXIT_FAILURE;
    }

    struct arguments args = {0, "", ""};

    if (!load_arguments(argc, argv, &args)) {
        fprintf(stderr, "Failed to load arguments.\n");
        return EXIT_FAILURE;
    }


    FILE* permissions_file = fopen(args.file, "w");
    if (!permissions_file) {
        perror("fopen");
        return EXIT_FAILURE;
    }

    //print_sample_output(permissions_file);

    directory_descent(args.dir, permissions_file);
    fclose(permissions_file);

    return EXIT_SUCCESS;
}
