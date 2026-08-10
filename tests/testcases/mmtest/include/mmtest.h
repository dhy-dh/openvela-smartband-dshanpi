#include <nuttx/config.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <string.h>
#include <dirent.h>
#include <sys/stat.h>

#define IOBUFFERSIZE 1024

typedef struct
{
    unsigned long mem_total;
    unsigned long mem_used;
    unsigned long mem_free;
    unsigned long mem_largest;
} m_info;
