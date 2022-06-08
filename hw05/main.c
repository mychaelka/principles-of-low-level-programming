#include "sample_printer.h"

#define __USE_MISC        1  // for S_ISVTX

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
#include <errno.h>

#define USER      1
#define GROUP     2
#define OTHER     3

struct arguments {
    int opt;
    char* file;
    char dir[256];
};

struct file_info {
    char filename[256];
    char owner[256];
    char group[256];
    unsigned int mask;
};


bool load_arguments(int argc, char **argv, struct arguments *args)
{
    args->opt = getopt(argc, argv, "e:i:");
    if (args->opt == '?') {
        return false;
    }
    args->file = optarg;

    if (optind == argc - 1) {
        strcpy(args->dir, argv[optind]);
    }

    if (optind == argc) {
        args->dir[0] = '.';
        return true;
    }
    if (optind == argc - 1) {
        strcpy(args->dir, argv[optind]);
        return true;
    }

    fputs("Too many arguments\n", stderr);
    return false;
}

char* concat_name(char* path, char* subdir)
{
    char* subdir_name = malloc(strlen(path) + strlen(subdir) + 2);
    if (subdir_name == NULL) {
        return NULL;
    }
    strcpy(subdir_name, path);
    if (strcmp(path, "") == 0) {
        strcat(subdir_name, subdir);
        return subdir_name;
    }
    if (path[strlen(path) - 1] != '/') {  // path is not ended by slash
        strcat(subdir_name, "/");
    }
    strcat(subdir_name, subdir);

    return subdir_name;
}

void read_info(FILE* file, struct file_info* info)
{
    fscanf(file, "# file: %s\n", info->filename);
    fscanf(file, "# owner: %s\n", info->owner);
    fscanf(file, "# group: %s\n", info->group);

    unsigned int mask = 0;
    char flags[3] = {' ', ' ', ' '};
    fscanf(file, "# flags: %s\n", flags);

    flags[0] == 's' ? mask |= S_ISUID : mask;
    flags[1] == 's' ? mask |= S_ISGID : mask;
    flags[2] == 't' ? mask |= S_ISVTX : mask;

    char usr[3] = {' ', ' ', ' '};
    char grp[3] = {' ', ' ', ' '};
    char othr[3] = {' ', ' ', ' '};

    fscanf(file, "user:: %s\n", usr);
    fscanf(file, "group:: %s\n", grp);
    fscanf(file, "other:: %s\n", othr);

    usr[0] == 'r' ? mask |= S_IRUSR : mask;
    usr[1] == 'w' ? mask |= S_IWUSR : mask;
    usr[2] == 'x' ? mask |= S_IXUSR : mask;

    grp[0] == 'r' ? mask |= S_IRGRP : mask;
    grp[1] == 'w' ? mask |= S_IWGRP : mask;
    grp[2] == 'x' ? mask |= S_IXGRP : mask;

    othr[0] == 'r' ? mask |= S_IROTH : mask;
    othr[1] == 'w' ? mask |= S_IWOTH : mask;
    othr[2] == 'x' ? mask |= S_IXOTH : mask;

    info->mask = mask;
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

void formatted_output(struct stat* fstats, FILE* file, char* path, bool initial)
{
    if (!initial) {
        fputs("\n", file);
    }
    struct passwd* user_info = getpwuid(fstats->st_uid);
    struct group* group_info = getgrgid(fstats->st_gid);
    fprintf(file,"# file: %s\n", path);
    fprintf(file,"# owner: %s\n", user_info->pw_name);
    fprintf(file,"# group: %s\n", group_info->gr_name);

    print_flags(fstats, file);

    print_permissions(fstats, USER, file);
    print_permissions(fstats, GROUP, file);
    print_permissions(fstats, OTHER, file);
}

bool directory_recursive_read(char *path, FILE *file, bool initial)
{
    struct dirent *entry;
    struct stat fstats;

    DIR* dir = opendir(path);
    if (dir == NULL) {
        perror("Failed to open directory.\n");
        return false;
    }

    if (stat(path, &fstats) == -1) {
        perror("stat");
        closedir(dir);
        return false;
    }
    formatted_output(&fstats, file, path, initial);
    struct dirent **entrylist;
    int entries;
    if ((entries = scandir(path, &entrylist, NULL, alphasort)) == -1) {
        perror("scandir");
        closedir(dir);
        return false;
    }

    for (int i = 0; i < entries; i++) {
        entry = entrylist[i];
        char* subdir_name;
        if (strcmp(path, ".") != 0) {
            subdir_name = concat_name(path, entry->d_name);
        }
        else {
            subdir_name = concat_name("", entry->d_name);
        }

        if (subdir_name == NULL) {
            fputs("Could not parse subdirectory name\n", stderr);
            free(entry);
            free(entrylist);
            closedir(dir);
            return false;
        }

        if (stat(subdir_name, &fstats) == -1) {
            perror("stat");
            free(subdir_name);
            free(entry);
            free(entrylist);
            closedir(dir);
            return false;
        }

        if (!S_ISREG(fstats.st_mode) && !S_ISDIR(fstats.st_mode)) {
            perror("ISREG || ISDIR");
            free(subdir_name);
            free(entry);
            free(entrylist);
            closedir(dir);
            return false;
        }
        if (strcmp(entry->d_name, "..") != 0
            && strcmp(entry->d_name, ".") != 0
            && S_ISDIR(fstats.st_mode)) {
            directory_recursive_read(subdir_name, file, false);
        } else if (S_ISREG(fstats.st_mode)) {
            formatted_output(&fstats, file, subdir_name, false);
        }
        free(subdir_name);
        free(entry);
    }
    free(entrylist);
    closedir(dir);
    return true;
}

int file_peek(FILE* file)
{
    int c = getc(file);
    return c == EOF ? EOF : ungetc(c, file);
}

bool change_mods(FILE *file)
{
    while (file_peek(file) != EOF) {
        struct stat fstats;
        struct file_info info = {
                "",
                "",
                "",
                0};

        read_info(file, &info);
        if (stat(info.filename, &fstats) != 0) {
            fputs("Could not load file stats\n", stderr);
            return false;
        }

        struct passwd* user_info = getpwuid(fstats.st_uid);
        struct group* group_info = getgrgid(fstats.st_gid);
        bool incorrect_usr_grp = false;

        if (strcmp(user_info->pw_name, info.owner) != 0) {
            fprintf(stderr, "User of file %s is incorrect\n", info.filename);
            incorrect_usr_grp = true;
        }
        if (strcmp(group_info->gr_name, info.group) != 0) {
            fprintf(stderr, "Group of file %s is incorrect\n", info.filename);
            incorrect_usr_grp = true;
        }

        if (incorrect_usr_grp) {
            continue;
        }

        if (chmod(info.filename, info.mask) != 0) {
            perror(strerror(errno));
            continue;
        }
    }
    return true;
}


int main(int argc, char **argv) {

    struct arguments args = {0, "", ""};

    if (!load_arguments(argc, argv, &args)) {
        puts("Arguments need to be passed in format \"<-e|-i> <PERMISSIONS_FILE> [DIRECTORY_TO_CHECK]\"");
        return EXIT_FAILURE;
    }

    FILE* permissions_file = NULL;
    if (args.opt == 'e') {
        permissions_file = fopen(args.file, "w");
        if (!permissions_file) {
            fputs("Could not open permissions file\n", stderr);
            return EXIT_FAILURE;
        }

        if (chdir(args.dir) != 0) {
            fputs("Could not change directory\n", stderr);
            return EXIT_FAILURE;
        }

        if (!directory_recursive_read(".", permissions_file, true)) {
            fclose(permissions_file);
            return EXIT_FAILURE;
        }

    } else {
        permissions_file = fopen(args.file, "r");
        if (!permissions_file) {
            perror("fopen");
            return EXIT_FAILURE;
        }

        if (chdir(args.dir) != 0) {
            fputs("Could not change directory\n", stderr);
            return EXIT_FAILURE;
        }

        if (!change_mods(permissions_file)) {
            return EXIT_FAILURE;
        }
    }

    fclose(permissions_file);
    return EXIT_SUCCESS;
}
