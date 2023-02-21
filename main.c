/*
Name: Atmuri Trinadh kumar
BlazerID: tkatmuri
project: HW02
To compile: make (search: main.c
	gcc -g -o search main.c)
To run: {
    ./search,
    ./search -S,
    ./search -s 1024 -f jpg 1 
}
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include<getopt.h>
#include<sys/stat.h>
#include <sys/types.h>
#include<time.h>

#define MAX_PATH_SIZE 2000

void sizeRestrictedPrint(struct dirent *dirent, int maxFileSize, int tabSpaces, int count, char *path, char *flagStr);
void stringPattern(struct dirent *dirent, int tabSpaces,char *pattern, int dirDepth, char *flagStr, int maxFileSize, int count, char *path);
void printReg(struct dirent *dirent, int tabSpaces, int count);
void printReg(struct dirent *dirent, int tabSpaces, int count);
void traverseDirectory(char *path, int tabSpaces, int maxFileSize, char *flagStr, char *pattern, int dirDepth, int isReg);
char *addStr(char *first, char *second);
void printExtraInfo(struct dirent *dirent, char *path);
void printOnly(struct dirent *dirent, int tabSpaces, char *path, char *type);


char *filetype(unsigned char type) {
  char *str;
  switch(type) {
  case DT_BLK: str = "block device"; break;
  case DT_CHR: str = "character device"; break;
  case DT_DIR: str = "directory"; break;
  case DT_FIFO: str = "named pipe (FIFO)"; break;
  case DT_LNK: str = "symbolic link"; break;
  case DT_REG: str = "regular file"; break;
  case DT_SOCK: str = "UNIX domain socket"; break;
  case DT_UNKNOWN: str = "unknown file type"; break;
  default: str = "UNKNOWN";
  }
  return str;
}

void printOnly(struct dirent *dirent, int tabSpaces, char *path,char *type) {
    char *fType = filetype(dirent->d_type);

    if(strcmp(fType, "regular file") == 0 && strcmp(type,"p") == 0 ) {
        printf("%*s %s (%s)\n",4*tabSpaces," ", dirent->d_name, filetype(dirent->d_type));
    }
    
    if(strcmp(fType, "directory") == 0 && strcmp(type, "d") == 0) {
        printf("%*s %s (%s)\n",4*tabSpaces," ", dirent->d_name, filetype(dirent->d_type));
    }
}

void printReg(struct dirent *dirent, int tabSpaces, int count) {
    printf("%*s[%d] %s (%s)\n", 4 * tabSpaces, " ", count, dirent->d_name, filetype(dirent->d_type)); 

}

void printExtraInfo(struct dirent *dirent, char *path) {
    // char childDir[150];
    struct stat buf;
    stat(path, &buf);

    printf("%s", dirent->d_name);
    off_t fByteSize = buf.st_size;
    printf("\t%lld ",fByteSize);


    if (S_ISDIR(buf.st_mode))
        putchar('d');
    else
        putchar('-');
    if ((buf.st_mode & S_IRUSR) != 0)
        putchar('r');
    else
        putchar('-');
    if ((buf.st_mode & S_IWUSR) != 0)
        putchar('w');
    else
        putchar('-');
    if ((buf.st_mode & S_IXUSR) != 0)
        putchar('x');
    else
        putchar('-');
    if ((buf.st_mode & S_IRGRP) != 0)
        putchar('r');
    else
        putchar('-');
    if ((buf.st_mode & S_IWGRP) != 0)
        putchar('w');
    else
        putchar('-');
    if ((buf.st_mode & S_IXGRP) != 0)
        putchar('x');
    else
        putchar('-');
    if ((buf.st_mode & S_IROTH) != 0)
        putchar('r');
    else
        putchar('-');
    if ((buf.st_mode & S_IWOTH) != 0)
        putchar('w');
    else
        putchar('-');
    if ((buf.st_mode & S_IXOTH) != 0)
        putchar('x');
    else
        putchar('-');

    printf("\t%s", ctime(&buf.st_atime));
}

void sizeRestrictedPrint(struct dirent *dirent, int maxFileSize, int tabSpaces, int count, char *path, char *flagStr) {
    struct stat buf;
    char *x = malloc(500);
    strcpy(x,path);
    strcat(x,"/");
    strcat(x,dirent->d_name);

    stat(x, &buf);
    off_t fByteSize = buf.st_size;
    if(fByteSize < maxFileSize && strstr(flagStr, "S") != NULL) {
        printExtraInfo(dirent, x);
    }
    else if(dirent->d_type == DT_DIR || fByteSize < maxFileSize) {
        printf("%*s[%d] %s (%lld)\n", 4 * tabSpaces, " ", count, dirent->d_name, fByteSize);
    }
   
}

void stringPattern(struct dirent *dirent, int tabSpaces,char *pattern, int dirDepth, char *flagStr, int maxFileSize, int count, char *path) {

    if (strstr(flagStr, "s") != NULL && tabSpaces <= dirDepth && strstr(dirent->d_name, pattern) != NULL) {
        sizeRestrictedPrint(dirent, maxFileSize, tabSpaces, count, path, flagStr);
        // printf("%*s %s (%s)\n", 4 * tabSpaces, " ", dirent->d_name, filetype(dirent->d_type)); 
    }
    else if(strstr(flagStr, "S") != NULL && tabSpaces <= dirDepth && strstr(dirent->d_name, pattern) != NULL) {
        printExtraInfo(dirent, path);
    }
    else if(strstr(dirent->d_name, pattern) != NULL && tabSpaces<=dirDepth) {
        printf("%*s %s (%s)\n", 4 * tabSpaces, " ", dirent->d_name, filetype(dirent->d_type)); 
    }
}

void traverseDirectory(char *path, int tabSpaces, int maxFileSize, char *flagStr, char *pattern, int dirDepth, int isReg) {
    struct dirent *dirent;
    DIR *parentDir;
    // First, we need to open the directory.
    parentDir = opendir(path);
    if (parentDir == NULL) { 
        printf ("Error opening directory '%s'\n", path); 
        exit (-1);
    }
    int count = 1; 
    // After we open the directory, we can read the contents of the directory, file by file.
    while((dirent = readdir(parentDir)) != NULL){ 
        // If the file's name is "." or "..", ignore them. We do not want to  xinfinitely recurse.
        if (strcmp(dirent->d_name, ".") == 0 || strcmp(dirent->d_name, "..") == 0) 
        {
            continue;
        }

        // all flags
        if(strstr(flagStr, "f") != NULL && strstr(flagStr, "s") != NULL && strstr(flagStr, "S") != NULL) {
            stringPattern(dirent, tabSpaces, pattern, dirDepth, flagStr, maxFileSize, count, path);
        }
        // double flags
        else if (strstr(flagStr, "s") != NULL && strstr(flagStr, "f") != NULL && strstr(flagStr, "S") == NULL) {
            stringPattern(dirent, tabSpaces, pattern, dirDepth, flagStr, maxFileSize, count, path);
        } 
        else if(strstr(flagStr, "s") != NULL && strstr(flagStr, "S") != NULL && strstr(flagStr, "f") == NULL) {
            sizeRestrictedPrint(dirent, maxFileSize, tabSpaces, count, path, flagStr);
        }
        else if(strstr(flagStr, "S") != NULL && strstr(flagStr, "f") != NULL && strstr(flagStr, "s") == NULL) {
            stringPattern(dirent, tabSpaces, pattern, dirDepth, flagStr, maxFileSize, count, path);
        }

        // single flags
        else if (strstr(flagStr, "s") != NULL && strstr(flagStr, "f") == NULL && strstr(flagStr, "S") == NULL) {
            sizeRestrictedPrint(dirent, maxFileSize, tabSpaces, count, path, flagStr);
        }
        else if(strstr(flagStr, "f") != NULL && strstr(flagStr, "s") == NULL && strstr(flagStr, "S") == NULL) {
            stringPattern(dirent, tabSpaces, pattern, dirDepth, flagStr, maxFileSize, count, path);
        }
        else if(strstr(flagStr, "S") != NULL && strstr(flagStr, "f") == NULL && strstr(flagStr, "s") == NULL) {
            printExtraInfo(dirent, path);
        }
        else if(isReg != 0) {
            printReg(dirent, tabSpaces, count);
        }

        if(strstr(flagStr, "p") != NULL) {
            printOnly(dirent, tabSpaces, path, "p");
        } else if(strstr(flagStr, "d") != NULL) {
            printOnly(dirent, tabSpaces, path, "d");
        }
   
        // Check to see if the file type is a directory. If it is, recursively call traverseDirectory on it.
        if (dirent->d_type == DT_DIR) {
            // Build the new file path.
            char *subDirPath = (char *) malloc(MAX_PATH_SIZE);
            strcpy(subDirPath, path);
            strcat(subDirPath, "/");
            strcat(subDirPath, dirent->d_name);
            traverseDirectory(subDirPath, tabSpaces + 1, maxFileSize, flagStr, pattern, dirDepth, isReg);
        }
    } 
}

void decryptArguments(int argc, char *argv[], char **pattern, char **path, int *dirDepth, int *maxFileSize, int *onlyFiles, int *extraNeeded, int *sizeLimitation, int *depthNeeded) {
    int gotPath = 0;
    for (int i=1; i<argc; i++) {
        if(argv[i][0] == '-') {
            if(argv[i][1] == 'S'){
                *extraNeeded = 1;
            }
            else if(argv[i][1] == 's'){
                *maxFileSize = atoi(argv[i+1]);
                *sizeLimitation = 1;
                i++;
            }
            else if(argv[i][1] == 'f') {
                *pattern = argv[i+1];
                *dirDepth = atoi(argv[i+2]);
                *depthNeeded = 1;
                i = i+2;
            }
            else if(argv[i][1] == 't') {
                if(argv[i+1] == 'f') {
                    *onlyFiles = 1;
                }
                else {
                    *onlyFiles = 0;
                }
            }

        }
        else {
            *path = argv[i];
            gotPath = 1;
        }
        
    }
    if (gotPath == 0) {
            *path = "./";
        }
}

int main(int argc, char *argv[]) {
    int tabSpaces = 0;
    // Check to see if the user provides at least 2 command-line-arguments.

    char *pattern, *flagStr = malloc(50);
    char *path = NULL;
    int dirDepth = -1, maxFileSize = -1, onlyFiles = -1, extraNeeded = 0, sizeLimitation = 0, depthNeeded = 0, isReg = 1;
    strcpy(flagStr, "");
    
    decryptArguments(argc, argv, &pattern, &path, &dirDepth, &maxFileSize, &onlyFiles, &extraNeeded, &sizeLimitation, &depthNeeded);

    if(extraNeeded == 1) {
        strcat(flagStr, "S");
    }
    if(sizeLimitation == 1) {
        strcat(flagStr, "s");
    }
    if(depthNeeded == 1) {
        strcat(flagStr, "f");
    }
    if(onlyFiles == 1) {
        strcat(flagStr, "p");
    } else if(onlyFiles == 0) {
        strcat(flagStr, "d");
    }


    
    if (argc < 2) { 
        printf("Hello");
        traverseDirectory("./" ,tabSpaces, maxFileSize, flagStr, pattern, dirDepth, isReg);
    }
    else if(onlyFiles == -1) {
        traverseDirectory(path, tabSpaces, maxFileSize, flagStr, pattern, dirDepth, isReg);
    }
    else {
        isReg = 0;
        traverseDirectory("./", tabSpaces, maxFileSize, flagStr, pattern, dirDepth, isReg);
    }
    return 0;
}