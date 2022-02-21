/*
Name: Neelesh Palaparthi
Blazer ID: palaparn
project#: Homework 2
To compile:<gcc -Wall -o p_hw2 p_hw2.c>
To run:<./p_hw2 /home/UAB/palaparn/CS532 -t f>
      :<./p_hw2 /home/UAB/palaparn/CS532 -t d>
      :<./p_hw2 /home/UAB/palaparn/CS532 -S>
      :<./p_hw2 /home/UAB/palaparn/CS532 -S 1000>
      :<./p_hw2 /home/UAB/palaparn/CS532 -f c>
      :<./p_hw2 /home/UAB/palaparn/CS532 -t f>
*/

#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

// returns file type string in human readable way from the actual directory type
char *getFileTypeString(unsigned char type)
{
  switch (type)
  {
  case DT_BLK:
    return "block device";

  case DT_CHR:
    return "character device";
  case DT_DIR:
    return "directory";

  case DT_FIFO:
    return "named pipe (FIFO)";

  case DT_LNK:
    return "symbolic link";

  case DT_REG:
    return "regular file";

  case DT_SOCK:
    return "UNIX domain socket";

  case DT_UNKNOWN:
    return "unknown file type";

  default:
    return "UNKNOWN";
  }
}

//------------------- command line arguments
int sizeArgument;

int fileTypeArgument;
char fileTypeValue;

char *fileExtensionValue;
int fileExtensionArgument;

int minSizeArgument;
long long minSizeValue;

// parses size argument and min size value
void parseSizeArgument(char *arg)
{
  minSizeArgument = 1;
  minSizeValue = atoll(arg);
}

// parses file extension argument and it's value
void parseFileExtensionArgument(char *exte)
{
  fileExtensionValue = (char *)malloc(1 + strlen(exte));
  strcpy(fileExtensionValue, ".");
  strcat(fileExtensionValue, exte);
  fileExtensionArgument = 1;
}

// parses file type argument and either files or directories to be printed
void parseFileTypeArgument(char *fileType)
{
  fileTypeArgument = 1;
  fileTypeValue = fileType[0];
  if (fileTypeValue != 'd' && fileTypeValue != 'f')
  {
    printf("After -t you should pass either f or d");
    exit(-1);
  }
}

// this will parse the command line arguments
void parseCommandLineArguments(int argc, char *argv[])
{
  int i;
  for (i = 1; i < argc; i++)
  {
    if (strcmp(argv[i], "-S") == 0)
    {
      sizeArgument = 1;
    }
    else if (strcmp(argv[i], "-s") == 0)
    {
      if (argv[i + 1] == NULL)
      {
        printf("After -s argument you should pass the max file size value to be filtered");
        exit(-1);
      }
      else
      {
        parseSizeArgument(argv[i + 1]);
        i = i + 1;
      }
    }
    else if (strcmp(argv[i], "-f") == 0)
    {
      if (argv[i + 1] == NULL)
      {
        printf("After -f you should pass the file format to be filtered");
        exit(-1);
      }
      else
      {
        parseFileExtensionArgument(argv[i + 1]);
        i = i + 1;
      }
    }
    else if (strcmp(argv[i], "-t") == 0)
    {
      if (argv[i + 1] == NULL)
      {
        printf("After -t you should pass either f or d as filter");
        exit(-1);
      }
      else
      {
        parseFileTypeArgument(argv[i + 1]);
        i = i + 1;
      }
    }
  }
}

//------------------------- command line arguments

int count = 1;

// returns the size of a file in bytes
off_t getFilesizeLL(const char *path)
{
  struct stat fStat;
  if (!lstat(path, &fStat))
    return fStat.st_size;
  else
    return 0;
}

void printDirsAndFiles(char *dirToOpen, int nested)
{
  struct dirent *dirent;
  DIR *dir;
  char *file_path;
  off_t t_file_size;

  // normal - 1
  // with size - 2
  // only dirs/files - 3 with size
  // only dirs/files - 4 without size
  int printType = 0;

  if (!(dir = opendir(dirToOpen)))
    return;
  if (!(dirent = readdir(dir)))
    return;

  do
  {
    char path[1024];
    int len = snprintf(path,
                       sizeof(path) - 1, "%s/%s",
                       dirToOpen,
                       dirent->d_name);

    path[len] = 0;

    if (sizeArgument == 0 && minSizeArgument == 0 && fileTypeArgument == 0 && fileExtensionArgument == 0)
    {
      // if no filters are passed print normally
      printType = 1;
    }
    else
    {
      // filters for -s & -S
      if (sizeArgument == 1 || minSizeArgument == 1)
      {
        if (dirent->d_type != DT_DIR)
        {

          // required memory allocation for file path
          file_path = (char *)malloc(1 + strlen(dirToOpen) + 1 + strlen(dirent->d_name));
          strcpy(file_path, dirToOpen);
          strcat(file_path, "/");
          strcat(file_path, dirent->d_name);
          t_file_size = getFilesizeLL(file_path);

          // free memory allocated
          free(file_path);

          // if file size is less than min size or max size
          if ((minSizeArgument == 1 && t_file_size > minSizeValue))
            printType = 2;
          else
            // do not print
            printType = 4;
        }
        else
        {
          printType = 1;
        }
      }

      if (sizeArgument == 1 && minSizeArgument != 1 && printType != 4)
      {
        printType = 2;
      }

      // check for file or directory
      if (fileTypeArgument == 1 && printType != 4)
      {

        if (fileTypeValue == 'd' && dirent->d_type == DT_DIR)
        {
          // if it is a directory and size argument was passed
          if (printType == 2)
          {
            printType = 2;
          }
          else
          {
            // print normally
            printType = 1;
          }
        }
        else if (fileTypeValue == 'f' && dirent->d_type == DT_REG)
        {
          // if it is a file and size argument was passed
          if (printType == 2)
          {
            printType = 2;
          }
          else
          {
            // print normally
            printType = 1;
          }
        }
      }

      // print only files with specific extension
      if (fileExtensionArgument == 1)
      {
        char *end = strrchr(dirent->d_name, '.');
        if (end && strcmp(end, fileExtensionValue) == 0)
          if (printType == 2)
            printType = 2;
          else
            printType = 1;
        else
          printType = 4;
      }
    }

    switch (printType)
    {
      // print without size
    case 1:
      printf("[%d]  \t\t%*s%s (%s)", count++, nested * 2, "|", dirent->d_name, getFileTypeString(dirent->d_type));
      printf("\n");
      break;
      // print with size
    case 2:
      printf("[%d]  \t\t%*s%s (%s) (%ld bytes)", count++, nested * 2, "|", dirent->d_name, getFileTypeString(dirent->d_type), t_file_size);
      printf("\n");
      break;
    default:
      break;
    }

    printType = 0;

    // skip entering directory if . or ..
    if (strcmp(dirent->d_name, ".") != 0 &&
        strcmp(dirent->d_name, "..") != 0)
      printDirsAndFiles(path, nested + 1);
  } while ((dirent = readdir(dir)) != NULL);
  closedir(dir);
}

// main function which drives the program
int main(int argc, char **argv)
{
  DIR *parentDir;
  if (argc < 2)
  {
    printf("Usage: %s <dirname>\n", argv[0]);
    exit(-1);
  }

  parseCommandLineArguments(argc, argv);

  parentDir = opendir(argv[1]);
  if (parentDir == NULL)
  {
    printf("Error opening directory '%s'\n", argv[1]);
    exit(-1);
  }

  closedir(parentDir);
  printDirsAndFiles(argv[1], 0);
  return 0;
}
