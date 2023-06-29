/*
Senkora - JS runtime for the modern age
Copyright (C) 2023  SenkoraJS

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/
#ifndef _FS_API_
#define _FS_API_
#define PATH_MAX 4096

int writeToFile(const char *filename, const char *data);
char *readFromFile(const char *filename);
int deleteFile(const char *filename);
int deleteDirectory(const char *dirname);
int exists(const char *path);
int existsFile(const char *filename);
int existsDirectory(const char *dirname);
int createDirectory(const char *dirname);

#endif
