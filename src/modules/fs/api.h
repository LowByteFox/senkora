#ifndef _FS_API_
#define _FS_API_

int writeToFile(char *filename, unsigned char *data);
unsigned char *readFromFile(char *filename);

#endif
