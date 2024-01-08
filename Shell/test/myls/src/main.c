#include <assert.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <grp.h>
#include <math.h>
#include <pwd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <termios.h>
#include <time.h>
#include <unistd.h>

struct dirent *ptr;

void option_a(char *direct)
{
  DIR *dir = opendir(direct);
  if (!dir)
  {
    perror("opendir");
    exit(1);
  }

  while ((ptr = readdir(dir)) != NULL)
  {
    printf("%s\n", ptr->d_name);
  }

  if (closedir(dir) == -1)
  {
    perror("close dir");
    exit(1);
  }
}

void modeToLetter(int mode, char *str)
{
  str[0] = '-';

  if (S_ISDIR(mode))
    str[0] = 'd';

  if (S_ISCHR(mode))
    str[0] = 'c';

  if (S_ISBLK(mode))
    str[0] = 'b';

  if (mode & S_IRUSR)
    str[1] = 'r';
  else
    str[1] = '-';

  if (mode & S_IWUSR)
    str[2] = 'w';
  else
    str[2] = '-';

  if (mode & S_IXUSR)
    str[3] = 'x';
  else
    str[3] = '-';

  if (mode & S_IRGRP)
    str[4] = 'r';
  else
    str[4] = '-';

  if (mode & S_IWGRP)
    str[5] = 'w';
  else
    str[5] = '-';

  if (mode & S_IXGRP)
    str[6] = 'x';
  else
    str[6] = '-';

  if (mode & S_IROTH)
    str[7] = 'r';
  else
    str[7] = '-';

  if (mode & S_IWOTH)
    str[8] = 'w';
  else
    str[8] = '-';

  if (mode & S_IXOTH)
    str[9] = 'x';
  else
    str[9] = '-';

  str[10] = '\0';
}

int maxSizeInRep(char *direct)
{
  long int max = 0;
  struct stat fst;
  char *buffer = NULL;
  int sizeBuffer = 0;
  DIR *dir = opendir(direct);

  if (!dir)
  {
    perror("opendir");
    exit(1);
  }

  while ((ptr = readdir(dir)) != NULL)
  {
    if (strlen(direct) + strlen(ptr->d_name) + 1 >= sizeBuffer)
    {
      sizeBuffer = strlen(direct) + strlen(ptr->d_name) + 10;
      buffer = (char *)realloc(buffer, sizeBuffer);
      assert(buffer);
    }

    sprintf(buffer, "%s/%s", direct, ptr->d_name);
    if (fstatat(dirfd(dir), buffer, &fst, AT_SYMLINK_NOFOLLOW) == -1)
    {
      perror("stat");
      exit(1);
    }

    if ((long)fst.st_size > max)
      max = (long)fst.st_size;
  }

  if (closedir(dir) == -1)
  {
    perror("close dir");
    exit(1);
  }

  if (buffer)
    free(buffer);
  return max;
}

void option_l(char *direct)
{
  struct stat fst;
  struct tm *mytime;
  char str[12];
  long int maxSize = maxSizeInRep(direct);
  unsigned length = floor(log10(maxSize)) + 1;
  char *buffer = NULL;
  int sizeBuffer = 0;
  DIR *dir = opendir(direct);

  if (!dir)
  {
    perror("opendir");
    exit(1);
  }

  while ((ptr = readdir(dir)) != NULL)
  {
    if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, ".."))
      continue;

    if (strlen(direct) + strlen(ptr->d_name) + 1 >= sizeBuffer)
    {
      sizeBuffer = strlen(direct) + strlen(ptr->d_name) + 10;
      buffer = (char *)realloc(buffer, sizeBuffer);
      assert(buffer);
    }

    sprintf(buffer, "%s/%s", direct, ptr->d_name);
    if (fstatat(dirfd(dir), buffer, &fst, AT_SYMLINK_NOFOLLOW) == -1)
    {
      perror("fstatat");
      exit(1);
    }

    modeToLetter(fst.st_mode, str);
    printf("%s", str);
    printf(" %ld", fst.st_nlink);
    printf(" %s", getpwuid(fst.st_uid)->pw_name);
    printf(" %s", getgrgid(fst.st_gid)->gr_name);
    printf(" %*ld", length, (long)fst.st_size);
    mytime = localtime(&fst.st_mtime);
    printf(" %d-%02d-%02d %02d:%02d", mytime->tm_year + 1900,
           mytime->tm_mon + 1, mytime->tm_mday, mytime->tm_hour,
           mytime->tm_min);
    printf(" %s", ptr->d_name);
    printf("\n");
  }

  if (closedir(dir) == -1)
  {
    perror("close dir");
    exit(1);
  }

  if (buffer)
    free(buffer);
}

void option_R(char *direct)
{
  struct stat fst;
  char *buffer = NULL;
  int sizeBuffer = 0, count = 0;

  if (stat(direct, &fst) == -1)
  {
    perror("stat");
    exit(1);
  }

  if (!S_ISDIR(fst.st_mode))
    printf("%s\n", direct);
  else
  {
    printf("%s:\n", direct);

    DIR *dir = opendir(direct);
    if (!dir)
    {
      perror("opendir");
      exit(1);
    }

    while ((ptr = readdir(dir)) != NULL)
    {
      if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, ".."))
        continue;

      if (ptr->d_type & DT_DIR)
        count++;
      printf("%s\t ", ptr->d_name);
    }
    printf("\n\n");

    rewinddir(dir);
    while ((ptr = readdir(dir)) != NULL)
    {
      if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, ".."))
        continue;
      if (ptr->d_type & DT_DIR)
      {
        if (strlen(direct) + strlen(ptr->d_name) + 1 >= sizeBuffer)
        {
          sizeBuffer = strlen(direct) + strlen(ptr->d_name) + 10;
          buffer = (char *)realloc(buffer, sizeBuffer);
          assert(buffer);
        }
        sprintf(buffer, "%s/%s", direct, ptr->d_name);
        option_R(buffer);
      }
    }

    if (closedir(dir) == -1)
    {
      perror("close dir");
      exit(1);
    }
  }

  if (buffer)
    free(buffer);
}

void option_aR(char *direct)
{
  DIR *dir = opendir(direct);
  if (!dir)
  {
    perror("opendir");
    exit(1);
  }

  while ((ptr = readdir(dir)) != NULL)
  {
    if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
      continue;

    printf("%s\n", ptr->d_name);

    if (ptr->d_type == DT_DIR)
    {
      char nextDirect[PATH_MAX];
      snprintf(nextDirect, sizeof(nextDirect), "%s/%s", direct, ptr->d_name);
      option_aR(nextDirect);
    }
  }

  if (closedir(dir) == -1)
  {
    perror("close dir");
    exit(1);
  }
}

void option_Ra(char *direct)
{
  struct stat fst;
  DIR *dir = opendir(direct);
  if (!dir)
  {
    perror("opendir");
    exit(1);
  }

  if (stat(direct, &fst) == -1)
  {
    perror("stat");
    exit(1);
  }

  printf("%s:\n", direct);

  while ((ptr = readdir(dir)) != NULL)
  {
    if (strcmp(ptr->d_name, ".") == 0 || strcmp(ptr->d_name, "..") == 0)
      continue;

    printf("%s\n", ptr->d_name);

    if (ptr->d_type == DT_DIR)
    {
      char nextDirect[PATH_MAX];
      snprintf(nextDirect, sizeof(nextDirect), "%s/%s", direct, ptr->d_name);
      option_Ra(nextDirect);
    }
  }

  if (closedir(dir) == -1)
  {
    perror("close dir");
    exit(1);
  }
}

int main(int argc, char *argv[])
{
  assert(argc == 3);
  char *para = argv[1];
  char *direct = argv[2];

  if (!strcmp(para, "-a"))
    option_a(direct);
  if (!strcmp(para, "-l"))
    option_l(direct);
  if (strcmp(para, "-R") == 0)
    option_R(direct);
  if (!strcmp(para, "-aR"))
    option_aR(direct);
  if (!strcmp(para, "-Ra"))
    option_Ra(direct);
  return 0;
}
