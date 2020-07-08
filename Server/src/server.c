#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sysexits.h>
#include <unistd.h>

#include <arpa/inet.h>

#include <sys/socket.h>
#include <sys/types.h>

void *service(void *arg);

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

    int sd = socket(results->ai_family, results->ai_socktype, results->ai_protocol);
    if(sd < 0) {
        perror("Could not create socket");
        freeaddrinfo(results);
        return EX_OSERR;
    }

    // Turn on reusing of the local endpoint's address,
    // in case this server is just restarted
    int on = 1;
    err = setsockopt(sd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
    if(err < 0) {
        perror("Could not reappropriate socket");
        close(sd);
        freeaddrinfo(results);
        return EX_OSERR;
    }

    err = bind(sd, results->ai_addr, results->ai_addrlen);
    if(err < 0) {
        perror("Could not bind socket");
        close(sd);
        freeaddrinfo(results);
        return EX_OSERR;
    }
    freeaddrinfo(results);

    // Backlog of 5 is typical
    err = listen(sd, 5);
    if(err < 0) {
        perror("Could not listen on socket");
        close(sd);
        return EX_OSERR;
    }

    struct sigaction ignorer = {0};
    ignorer.sa_handler = SIG_IGN;
    err = sigaction(SIGCHLD, &ignorer, NULL);
    if(err < 0) {
        perror("Could not ignore children completion");
        close(sd);
        return EX_OSERR;
    }

    for(;;) {
        struct remote_endpoint *endpoint = malloc(sizeof(*endpoint));
        struct sockaddr_storage client;
        socklen_t client_sz = sizeof(client);

        endpoint->fd = accept(sd, (struct sockaddr *)&client, &client_sz);
        if(endpoint->fd < 0) {
            perror("Could not accept remote");
            continue;
        }

        endpoint->endpoint = client;

        pthread_t tid;
        pthread_create(&tid, NULL, service, endpoint);
    }

    close(sd);
}

void *service(void *arg)
{
    struct remote_endpoint *remote = (struct remote_endpoint *)arg;
    struct sockaddr_storage client = remote->endpoint;

    char addr[INET6_ADDRSTRLEN];
    unsigned short port = 0;
    if(client.ss_family == AF_INET6) {
        inet_ntop(client.ss_family,
                &((struct sockaddr_in6 *)&client)->sin6_addr, addr, sizeof(addr));
        port = ntohs(((struct sockaddr_in6 *)&client)->sin6_port);
    } else {
        inet_ntop(client.ss_family, &((struct sockaddr_in *)&client)->sin_addr,
                addr, sizeof(addr));
        port = ntohs(((struct sockaddr_in *)&client)->sin_port);
    }
    printf("Received from %s:%hu\n", addr, port);

    // Use a small buffer for such a demo program
    char buffer[128];
    ssize_t received = recv(remote->fd, buffer, sizeof(buffer)-1, 0);
    while(received > 0) {
        buffer[received] = '\0';
        printf("%s", buffer);
        received = recv(remote->fd, buffer, sizeof(buffer)-1, 0);
    }
    if(received < 0) {
        perror("Unable to receive");
    }

    close(remote->fd);
    puts("");
    free(remote);

    return NULL;
}
