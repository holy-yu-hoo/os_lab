#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <netinet/in.h>
#include <arpa/inet.h>

static volatile sig_atomic_t sighup = 0;
static volatile sig_atomic_t run = 1;

static void sig_handler(int signo) {
    switch (signo) {
    case (SIGHUP): {
        sighup = 1;
        break;
    }
    case (SIGINT): {
        run = 0;
        break;
    }
    }
}

const int port = 9999;

int main() {

    int server = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (server == -1) { perror("socket"); return EXIT_FAILURE; }

    int opt = 1;
    setsockopt(server, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)); // это для быстрого перезапуска

    struct sockaddr_in address;
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port);
    if (bind(server, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("bind"); close(server); return EXIT_FAILURE;
    }

    if (listen(server, 16) == -1) {
        perror("listen");
        close(server);
        return EXIT_FAILURE;
    }

    struct sigaction action;
    memset(&action, 0, sizeof(action));
    action.sa_handler = sig_handler;
    sigemptyset(&action.sa_mask);

    if (sigaction(SIGHUP, &action, nullptr) == -1) {
        perror("sigaction"); return EXIT_FAILURE;
    }

    memset(&action, 0, sizeof(action));
    action.sa_handler = sig_handler;
    sigemptyset(&action.sa_mask);
    if (sigaction(SIGINT, &action, nullptr) == -1) {
        perror("sigaction"); return EXIT_FAILURE;
    }


    sigset_t blocked_mask, original_mask;
    sigemptyset(&blocked_mask);
    sigaddset(&blocked_mask, SIGHUP);
    if (sigprocmask(SIG_BLOCK, &blocked_mask, &original_mask) == -1) {
        perror("sigprocmask"); return EXIT_FAILURE;
    }

    printf("Server listening on port %d (PID %d)\n", port, getpid());

    int client = -1;


    while (run) {
        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(server, &read_fds);
        int max_fd = server;

        if (client != -1) {
            FD_SET(client, &read_fds);
            if (client > max_fd) max_fd = client;
        }

        int ready = pselect(max_fd + 1, &read_fds, nullptr, nullptr, nullptr, &original_mask);

        if (ready == -1 && errno != EINTR) {
            perror("pselect");
            break;
        }

        if (sighup) {
            sighup = 0;
            printf("SIGHUP received\n");
        }

        if (ready > 0) {
            if (FD_ISSET(server, &read_fds)) {
                int new_fd = accept(server, nullptr, nullptr);
                if (new_fd != -1) {
                    printf("New connection: fd=%d\n", new_fd);
                    if (client == -1) client = new_fd;
                    else close(new_fd);
                }
            }

            if (client != -1 && FD_ISSET(client, &read_fds)) {
                char buffer[4096];
                ssize_t n = recv(client, buffer, sizeof(buffer), 0);
                if (n > 0) printf("Received %zd bytes\n", n);
                else if (n == 0) {
                    printf("Close connection\n");
                    close(client);
                    client = -1;
                } else {
                    perror("recv");
                    close(client);
                    client = -1;
                }
            }
        }
    }

    if (client != -1) close(client);
    close(server);
    sigprocmask(SIG_SETMASK, &original_mask, nullptr);
    printf("Server stopped\n");
    return EXIT_SUCCESS;
}