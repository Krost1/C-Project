#include "../include/inc.h"

char *read_json_file(const char *filename)
{
    FILE *fd = fopen(filename, "r");
    if (fd == NULL)
    {
        printf("Error: Unable to open the file.\n");
        return NULL;
    }

    fseek(fd, 0, SEEK_END);
    size_t file_size = ftell(fd);
    rewind(fd);

    char *buffer = (char *)malloc(file_size + 1);
    if (buffer == NULL)
    {
        printf("Error: Memory allocation failed.\n");
        fclose(fd);
        return NULL;
    }

    size_t result = fread(buffer, 1, file_size, fd);
    if (result != file_size)
    {
        printf("Error: Reading file failed.\n");
        free(buffer);
        fclose(fd);
        return NULL;
    }

    fclose(fd);
    buffer[file_size] = '\0'; // Null-terminate the string

    return buffer;
}
