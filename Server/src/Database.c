#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>

#include <sys/stat.h>
#include <sys/types.h>

#include "Database.h"

//https://www.youtube.com/watch?v=SEenaPQXxFs
int record_open(char *filename)
{
    int fd = open(filename, O_CREAT | O_RDWR | O_APPEND, 0664);
    return fd;
}

void record_close(int fd)
{
    close(fd);
}

int insert_record(int fd, message_rec *rec)
{
    return write(fd, rec, sizeof(message_rec));
}

int get_record(int fd, message_rec *rec, int key)
{
    int ret;
    while ( (ret = read(fd, rec, sizeof(message_rec))) != -1)
    {
        if (ret == 0)
        {
            memset(rec, 0, sizeof(message_rec));
            return ret;
        }
        else if ((unsigned int)key == rec->key)
        {
            return ret;
        }
    }
    memset(rec, 0, sizeof(message_rec));
    return ret;
}

int delete_record(int fd, int key)
{
    int ret;
    message_rec *rec;
    off_t pos;

    pos = lseek(fd, 0, SEEK_SET);

    while ( (ret = read(fd, rec, sizeof(message_rec))) != -1)
    {
        if (ret == 0)
        {
            return ret;
        }
        else if ((unsigned int)key == rec->key)
        {
            lseek(fd, pos, SEEK_SET);
            rec->key = 0;
            return write(fd, &rec, sizeof(message_rec));
        }
        pos = lseek(fd, 0, SEEK_CUR);
    }
    return ret;
}
