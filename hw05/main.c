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

struct perms {
    bool r;
    bool w;
    bool x;
};

struct flags {
    bool setuid;
    bool setgid;
    bool sticky_bit;
};

struct file_info {
    char filename[256];
    char owner[256];
    char group[256];
    unsigned int mask;
    struct perms user_perms;
    struct perms group_perms;
    struct perms other_perms;
    struct flags flags;
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
        args->dir[0] = '.';
        //getcwd(args->dir, sizeof(args->dir));
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
    if (path[strlen(path) - 1] != '/') {  // path is not ended by slash
        strcat(subdir_name, "/");
    }
    strcat(subdir_name, subdir);

    return subdir_name;
}

void read_info(FILE* file, struct file_info* info)  // TODO: delete redundant
{
    fscanf(file, "# file: %s\n", info->filename);
    fscanf(file, "# owner: %s\n", info->owner);
    fscanf(file, "# group: %s\n", info->group);

    unsigned int mask = 0;
    char flags[3];
    fscanf(file, "# flags: %s\n", flags);

    flags[0] == 's' ? mask |= S_ISUID : mask;
    flags[1] == 's' ? mask |= S_ISGID : mask;
    flags[2] == 't' ? mask |= S_ISVTX : mask;

    info->flags.setuid = flags[0] == 's';
    info->flags.setgid = flags[1] == 's';
    info->flags.sticky_bit = flags[2] == 't';

    char usr[3];
    char grp[3];
    char othr[3];

    fscanf(file, "user:: %s\n", usr);
    fscanf(file, "group:: %s\n", grp);
    fscanf(file, "other:: %s\n", othr);

    info->user_perms.r = usr[0] == 'r';
    info->user_perms.w = usr[1] == 'w';
    info->user_perms.x = usr[2] == 'x';

    usr[0] == 'r' ? mask |= S_IRUSR : mask;
    usr[1] == 'w' ? mask |= S_IWUSR : mask;
    usr[2] == 'x' ? mask |= S_IXUSR : mask;

    info->group_perms.r = grp[0] == 'r';
    info->group_perms.w = grp[1] == 'w';
    info->group_perms.x = grp[2] == 'x';

    grp[0] == 'r' ? mask |= S_IRGRP : mask;
    grp[1] == 'w' ? mask |= S_IWGRP : mask;
    grp[2] == 'x' ? mask |= S_IXGRP : mask;

    info->other_perms.r = othr[0] == 'r';
    info->other_perms.w = othr[1] == 'w';
    info->other_perms.x = othr[2] == 'x';

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

void formatted_output(struct stat* fstats, FILE* file, char* path)
{
    struct passwd* user_info = getpwuid(fstats->st_uid);
    struct group* group_info = getgrgid(fstats->st_gid);
    fprintf(file,"# file: %s\n", path);
    fprintf(file,"# owner: %s\n", user_info->pw_name);
    fprintf(file,"# group: %s\n", group_info->gr_name);

    print_flags(fstats, file);

    print_permissions(fstats, USER, file);
    print_permissions(fstats, GROUP, file);
    print_permissions(fstats, OTHER, file);
    fputs("\n", file);
}

bool directory_recursive_read(char *path, FILE *file)
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
        return false;
    }
    formatted_output(&fstats, file, path);
    struct dirent **entrylist;
    int entries;
    if ((entries = scandir(path, &entrylist, NULL, alphasort)) == -1) {
        perror("scandir");
        return false;
    }

    for (int i = 0; i < entries; i++) {
        entry = entrylist[i];
        char* subdir_name = concat_name(path, entry->d_name);
        if (subdir_name == NULL) {
            perror("subdirectory name");
            return false;
        }

        if (stat(subdir_name, &fstats) == -1) {
            perror("stat");
            return false;
        }

        if (strcmp(entry->d_name, "..") != 0
            && strcmp(entry->d_name, ".") != 0
            && is_directory(subdir_name)) {
            directory_recursive_read(subdir_name, file);
        } else if (is_file(subdir_name)) {
            formatted_output(&fstats, file, subdir_name);
        }
        // TODO: add error if file is not regular
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
                0,
                {false, false, false},
                {false, false, false},
                {false, false, false},
                {false, false, false}};

        read_info(file, &info);
        stat(info.filename, &fstats);

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
    if (argc < 3) {
        fprintf(stderr, "The PERMISSIONS_FILE argument is required.\n");
        return EXIT_FAILURE; // TODO: change not to be dependent on argc
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

    FILE* permissions_file = NULL;
    if (args.opt == 'e') {
        permissions_file = fopen(args.file, "w");
        if (!permissions_file) {
            perror("fopen");
            return EXIT_FAILURE;
        }

        directory_recursive_read(args.dir, permissions_file);

    } else {
        permissions_file = fopen(args.file, "r");
        if (!permissions_file) {
            perror("fopen");
            return EXIT_FAILURE;
        }

        if (!change_mods(permissions_file)) {
            return EXIT_FAILURE;
        }
    }

    fclose(permissions_file);

    return EXIT_SUCCESS;
}
