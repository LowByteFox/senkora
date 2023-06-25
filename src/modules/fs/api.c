#define _GNU_SOURCE // Required for DT_DIR
#include "api.h"
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>

int writeToFile(const char *filename, const char *data)
{
    FILE *f = fopen(filename, "wb");
    size_t size = strlen(data);

    if (f == NULL)
    {
        return 0;
    }

    for (int i = 0; i < size; i++)
    {
        fputc(data[i], f);
    }

    fclose(f);

    return 1;
}

char *readFromFile(const char *filename)
{
    FILE *f = fopen(filename, "rb");
    char *str = malloc(1);
    int len = 1;

    if (f == NULL)
    {
        return NULL;
    }

    while (!feof(f))
    {
        str = realloc(str, len + 1);
        str[len - 1] = (char)fgetc(f);
        len++;
    }
    str[len - 2] = '\0';

    fclose(f);
    return str;
}

int deleteFile(const char *filename)
{
    if (unlink(filename) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int deleteDirectory(const char *dirname)
{
    DIR *dir;
    struct dirent *entry;
    char path[PATH_MAX];

    dir = opendir(dirname);

    if (dir == NULL)
    {
        return 0; // Failed to open directory
    }

    while ((entry = readdir(dir)) != NULL)
    {
        if (strcmp(entry->d_name, ".") != 0 && strcmp(entry->d_name, "..") != 0)
        {
            snprintf(path, sizeof(path), "%s/%s", dirname, entry->d_name);

            if (entry->d_type == DT_DIR)
            {
                deleteDirectory(path); // Recursively delete subdirectories
            }
            else
            {
                deleteFile(path); // Delete individual files
            }
        }
    }

    closedir(dir);

    if (rmdir(dirname) == 0)
    {
        return 1; // Successfully deleted the directory
    }
    else
    {
        return 0; // Failed to delete the directory
    }
}

int exists(const char *path)
{
    struct stat s;
    int err = stat(path, &s);

    if (-1 == err) {
        return 0; // Does not exist
    } else {
        return 1; // Exists
    }
}

int existsFile(const char *filename)
{
    struct stat s;
    int err = stat(filename, &s);

    if (-1 == err) {
        return 0; // Does not exist
    } else {
        if (S_ISDIR(s.st_mode))
        {
            return 0; // Exists as a directory
        }
        else
        {
            return 1; // Exists as a file
        }
    }
}

int existsDirectory(const char *dirname)
{
    struct stat s;
    int err = stat(dirname, &s);
    if (-1 == err) {
        return 0; // Does not exist
    } else {
        if (S_ISDIR(s.st_mode))
        {
            return 1; // Exists as a directory
        }
        else
        {
            return 0; // Exists as a file
        }
    }
}

int createDirectory(const char *dirname)
{
    if (mkdir(dirname, 0777) == 0)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}