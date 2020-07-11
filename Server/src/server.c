#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>
#include <limits.h>
#include <errno.h>
#include <fcntl.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>

#include <openssl/md5.h>

#include "Database.h"

int max(int x, int y)
{
    if (x > y)
        return x;
    else
        return y;
}

void *service_tcp(void *arg);
void *service_udp(void *arg);
long extract_type(char *message, unsigned int strLen);
char *str2md5(const char *str, int length);

struct remote_endpoint {
    int fd;
    struct sockaddr_storage endpoint;
};

int main(int argc, char *argv[])
{
    if(argc != 2) {
        fprintf(stderr, "Usage: %s <port>\n", argv[0]);
        return EX_USAGE;
    }
    // Create storage folder
    //https://stackoverflow.com/a/7430262
    struct stat st = {0};
    if (stat("Storage", &st) == -1) {
    mkdir("Storage", 0777);
}

    struct addrinfo hints = {0};
    hints.ai_family = PF_UNSPEC;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;
    struct addrinfo *results;
    int err = getaddrinfo(NULL, argv[1], &hints, &results);
    if(err != 0) {
        fprintf(stderr, "Cannot get address: %s\n", gai_strerror(err));
        return EX_NOHOST;
    }
    // Create TCP listener
    int TCPsd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(TCPsd < 0) {
        perror("Could not create socket");
        freeaddrinfo(results);
        return EX_OSERR;
    }

    // Turn on reusing of the local endpoint's address,
    // in case this server is just restarted
    int on = 1;
    err = setsockopt(TCPsd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(err < 0) {
        perror("Could not reappropriate socket");
        close(TCPsd);
        freeaddrinfo(results);
        return EX_OSERR;
    }

    err = bind(TCPsd, results->ai_addr, results->ai_addrlen);
    if(err < 0) {
        perror("Could not bind socket");
        close(TCPsd);
        freeaddrinfo(results);
        return EX_OSERR;
    }
    freeaddrinfo(results);

    // Setup UDP listener
    int udpfd = socket(AF_INET, SOCK_DGRAM, 0);
    // Binding server addr structure to udp sockfd
    bind(udpfd, results->ai_addr, results->ai_addrlen);
    // Backlog of 5 is typical
    err = listen(TCPsd, 5);
    if(err < 0) {
        perror("Could not listen on socket");
        close(TCPsd);
        return EX_OSERR;
    }

    struct sigaction ignorer = {0};
    ignorer.sa_handler = SIG_IGN;
    err = sigaction(SIGCHLD, &ignorer, NULL);
    if(err < 0) {
        perror("Could not ignore children completion");
        close(TCPsd);
        return EX_OSERR;
    }
    //Taken from https://www.geeksforgeeks.org/tcp-and-udp-server-using-select/
    fd_set rset;
    int nready, maxfdp1;
    // clear the descriptor set
    FD_ZERO(&rset);

    // Get maxfd
    maxfdp1 = max(TCPsd, udpfd) + 1;

    for(;;) {
        // Set listenfd and udpfd in readset
        FD_SET(TCPsd, &rset);
        FD_SET(udpfd, &rset);

        // Select the ready descriptor
        nready = select(maxfdp1, &rset, NULL, NULL, NULL);
        if (nready < 0)
        {
            perror("Could not set ready descriptors");
            continue;
        }
        if (FD_ISSET(TCPsd, &rset)) {
            struct remote_endpoint *endpoint = malloc(sizeof(*endpoint));
            struct sockaddr_storage client;
            socklen_t client_sz = sizeof(client);

            endpoint->fd = accept(TCPsd, (struct sockaddr *)&client, &client_sz);
            if(endpoint->fd < 0) {
                perror("Could not accept remote");
                continue;
            }



            endpoint->endpoint = client;

            pthread_t tid;
            pthread_create(&tid, NULL, service_tcp, endpoint);
        }
        if (FD_ISSET(udpfd, &rset)) {
            struct remote_endpoint *endpoint = malloc(sizeof(*endpoint));
            endpoint->fd = udpfd;
            struct sockaddr_storage client;
            endpoint->endpoint = client;

            pthread_t tid;
            pthread_create(&tid, NULL, service_udp, endpoint);
        }
    }
}

void *service_tcp(void *arg)
{
    struct remote_endpoint *remote = (struct remote_endpoint *)arg;
    char buffer[75535];
    //TODO ABC
    char *message = malloc(75535);
    unsigned int message_size = 75535;
    ssize_t received = recv(remote->fd, buffer, sizeof(buffer)-1, 0);
    while(received > 0) {
        if (strlen(message) + received > message_size)
        {
            message_size *= 2;
            //TODO ABC
            realloc(message, message_size);
        }
        buffer[received] = '\0';
        message = strncat(message, buffer, received);
        // Check for end of file signature
        if (message[strlen(message) - 1] == 3 && message[strlen(message) - 2] == 3)
        {
            // Format Message
            message[strlen(message) - 2] = '\0';
            message[strlen(message) - 1] = '\0';
            long type = extract_type(message, strlen(message));
            // Send hash to Client
            char *output = str2md5(message, strlen(message));
            send(remote->fd, output, strlen(output), 0);
            // Process Message
            if (type == 0)
            {
                //Create file with hash
                char* path = realpath("Storage", NULL);
                char* filename = malloc(strlen(path) + strlen(output) + 1);
                sprintf (filename, "%s/%s", path, output);
                FILE *fd = fopen(filename, "w");
                if (!fd)
                {
                    free(filename);
                    perror("Unable to write file out");
                    break;
                }
                fprintf(fd, "%s", message);
                fclose(fd);
                free(filename);
            }
            else if (type == 1)
            {
                //Add to data base
                message_rec rec;
                int db;
                rec.key = *output * (unsigned long)time(NULL);
                rec.message_hash = output;
                rec.message = message;
                if ((db = record_open((char *)"Database")) < 0)
                {
                    perror("Failed to open database");
                }
                if (insert_record(db, &rec) < 0)
                {
                    perror("Failed to log message");
                }
                close(db);
            }
            message[0] = '\0';
            free(output);
        }
        // Blocks here until connection is terminated
        received = recv(remote->fd, buffer, sizeof(buffer)-1, 0);
    }
    if(received < 0) {
        perror("Unable to receive");
        close(remote->fd);
        puts("");
        free(remote);
    }

    close(remote->fd);
    free(remote);

    return NULL;
}

void *service_udp(void *arg)
{
    struct remote_endpoint *remote = (struct remote_endpoint *)arg;
    struct sockaddr_in cliaddr;
    socklen_t client_sz = sizeof(cliaddr);
    char buffer[75535];
    ssize_t received = recvfrom(remote->fd, buffer, sizeof(buffer)-1, 0,
                 (struct sockaddr*)&cliaddr, &client_sz);
    long type = extract_type(buffer, received);
    buffer[received] = '\0';

    // Return hash to client
    if (buffer[strlen(buffer) - 1] == 3 && buffer[strlen(buffer) - 2] == 3)
    {
        buffer[strlen(buffer) - 2] = '\0';
        buffer[strlen(buffer) - 1] = '\0';
        // Send hash to Client
        char *output = str2md5(buffer, strlen(buffer));
        sendto(remote->fd, (const char*)output, strlen(output), 0,
                   (struct sockaddr*)&cliaddr, sizeof(cliaddr));

        if (type == 0)
        {
            //Create file with hash
            char* path = realpath("Storage", NULL);
            char* filename = malloc(strlen(path) + strlen(output) + 1);
            sprintf (filename, "%s/%s", path, output);
            FILE *fd = fopen(filename, "w");
            if (!fd)
            {
                free(filename);
                perror("Unable to write file out");
            }
            else
            {
                fprintf(fd, "%s", buffer);
                fclose(fd);
                free(filename);
            }
        }
        else if (type == 1)
        {
            message_rec rec;
            int db;
            rec.key = *output * (unsigned long)time(NULL);
            rec.message_hash = output;
            rec.message = buffer;
            if ((db = record_open((char *)"Database")) < 0)
            {
                perror("Failed to open database");
            }
            if (insert_record(db, &rec) < 0)
            {
                perror("Failed to log message");
            }
            close(db);
        }
        free(output);
    }
    if(received < 0) {
        perror("Unable to receive\n");
        puts("");
        free(remote);
    }
    free(remote);
    return NULL;
}

long extract_type(char *message, unsigned int strLen)
{
    long type = strtol(&message[0], (char **)NULL, 10);
    if (type == LONG_MAX && errno == ERANGE)
        return -1;
    memmove(message, message + 1, strLen - 1);
    return type;
}
//https://stackoverflow.com/questions/7627723/how-to-create-a-md5-hash-of-a-string-in-c
char *str2md5(const char *str, int length) {
    MD5_CTX c;
    unsigned char digest[16];
    char *out = (char*)malloc(33);

    MD5_Init(&c);

    while (length > 0) {
        if (length > 512) {
            MD5_Update(&c, str, 512);
        } else {
            MD5_Update(&c, str, length);
        }
        length -= 512;
        str += 512;
    }

    MD5_Final(digest, &c);

    for (int n = 0; n < 16; ++n) {
        snprintf(&(out[n*2]), 16*2, "%02x", (unsigned int)digest[n]);
    }

    return out;
}
