typedef struct {
    unsigned int key;
    char *message_hash;
    char *message;
} message_rec;

int record_open(char *filename);
void record_close(int fd);
int insert_record(int fd, message_rec *rec);
int get_record(int fd, message_rec *rec, int key);
