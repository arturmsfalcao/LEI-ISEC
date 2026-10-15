#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include "util.h"

int distancia = 0;
char client_fifo[128];
int service_id = 0;
volatile sig_atomic_t cancelled = 0;

void notify_client(const char *m) {
    int fd = open(client_fifo, O_WRONLY | O_NONBLOCK);
    if (fd != -1) {
        write(fd, m, strlen(m));
        close(fd);
    }
}

void handler(int sig) {
    (void)sig;
    cancelled = 1;
    notify_client("CANCELAMENTO: CONTROLADOR CANCELOU.\n");
    printf("SERVICO ID %d %s\n", service_id, VEH_MSG_CANCELADO);
    fflush(stdout);
    _exit(0);
}

int main(int argc, char *argv[]) {
    if (argc < 6) {
        fprintf(stderr, "USO: %s <client_fifo> <distancia> <local> <user> <id>\n", argv[0]);
        return 1;
    }

    strncpy(client_fifo, argv[1], sizeof(client_fifo)-1);
    distancia = atoi(argv[2]);
    service_id = atoi(argv[5]);

    signal(SIGUSR1, handler);

    int test = open(client_fifo, O_WRONLY | O_NONBLOCK);
    if (test == -1) {
        notify_client("A VIAGEM FOI CANCELADA.\n");
        printf("SERVICO ID %d %s\n", service_id, VEH_MSG_CANCELADO);
        fflush(stdout);
        return 0;
    }
    close(test);

    notify_client("VEICULO CHEGOU. A VIAGEM VAI COMECAR.\n");
    printf("SERVICO ID %d %s\n", service_id, VEH_MSG_COMECOU);
    fflush(stdout);

    int steps = 10;
    int seg = distancia > 0 ? distancia / steps : 1; 

    for (int p = 1; p <= steps && !cancelled; p++) {

        sleep(seg);

        int pct = p * 10;
        printf("SERVICO ID %d %s %d\n", service_id, VEH_MSG_PERCETAGEM, pct);
        fflush(stdout);
    }

    if (cancelled) return 0;

    notify_client("SERVICO CONCLUIDO: CHEGOU AO DESTINO.\n");
    printf("SERVICO ID %d %s, KMS: %d\n", service_id, VEH_MSG_ACABOU, distancia);
    fflush(stdout);

    return 0;
}