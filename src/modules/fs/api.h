#ifndef _FS_API_
#define _FS_API_

int writeToFile(const char *filename, const char *data);
char *readFromFile(const char *filename);
int deleteFile(const char *filename);
int deleteDirectory(const char *dirname);
int exists(const char *path);
int existsFile(const char *filename);
int existsDirectory(const char *dirname);
int createDirectory(const char *dirname);

#endif
