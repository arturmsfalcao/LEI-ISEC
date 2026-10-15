#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <errno.h>
#include <sys/select.h>
#include "util.h"

char client_fifo[128];
char username[MAX_NAME];

void usage() { fprintf(stderr,"USO: ./cliente <username>\n"); }

int main(int argc, char *argv[]) {
    if (argc<2) { usage(); return 1; }
    strncpy(username, argv[1], sizeof(username)-1);
    snprintf(client_fifo, sizeof(client_fifo), CLIENT_FIFO_FORMAT, username);
    
    unlink(client_fifo);
    if (mkfifo(client_fifo, 0666)==-1 && errno!=EEXIST) { perror("mkfifo"); return 1; }

    int sv = open(SERVER_FIFO, O_WRONLY);
    if (sv==-1) { fprintf(stderr,"ERRO: CONTROLADOR NAO DISPONIVEL\n"); unlink(client_fifo); return 1; }
    
    char reg[128]; snprintf(reg,sizeof(reg),"%s %s\n", CMD_REGISTER, username);
    write(sv, reg, strlen(reg));

    int cf = open(client_fifo, O_RDONLY | O_NONBLOCK);
    if (cf==-1) { perror("open client fifo"); close(sv); unlink(client_fifo); return 1; }

    printf("CLIENTE %s INICIADO. A AGUARDAR REGISTO...\n", username);

    fd_set rfds;
    char line[256];
    int registered = 0;

    while (!registered) {
        ssize_t r = read(cf, line, sizeof(line)-1);
        if (r>0) { 
            line[r]=0; 
            if (strstr(line,MSG_REGISTERED)) registered=1; 
            else if (strstr(line,MSG_ERROR)) { 
                printf("ERRO REGISTO: %s\n", line); 
                close(cf); close(sv); unlink(client_fifo); return 1; 
            } 
        } else if (r==-1 && errno!=EAGAIN) {
            perror("read registration");
            close(cf); close(sv); unlink(client_fifo); return 1;
        }
        sleep(1);
    }
    
    printf("REGISTO BEM SUCEDIDO.\n");
    printf("COMANDOS: agendar <h> <l> <d>, consultar, cancelar <id>, terminar\n");

    while (1) {
        FD_ZERO(&rfds);
        FD_SET(0, &rfds);
        FD_SET(cf, &rfds);
        int maxfd = (cf > 0) ? cf : 0;
        
        int sel = select(maxfd+1, &rfds, NULL, NULL, NULL);
        if (sel>0) {
            if (FD_ISSET(cf, &rfds)) {
                ssize_t r = read(cf, line, sizeof(line)-1);
                if (r>0) { 
                    line[r]=0; 
                    printf("MSG SERVIDOR: %s", line); 
                    if (strstr(line,"PLATAFORMA TERMINOU")) break; 
                } else if (r==0) {
                }
            }
            if (FD_ISSET(0, &rfds)) {
                if (!fgets(line, sizeof(line), stdin)) break;
                char cmd[32];
                if (sscanf(line,"%31s", cmd)>=1) {
                    if (strcmp(cmd,"agendar")==0) {
                        int hora, dist; char local[MAX_NAME];
                        if (sscanf(line+7, "%d %63s %d", &hora, local, &dist)==3) {
                            char out[256]; snprintf(out,sizeof(out),"%s %s %d %s %d\n", CMD_AGENDAR, username, hora, local, dist);
                            write(sv, out, strlen(out));
                        } else printf("USO: agendar <hora> <local> <distancia>\n");
                    } else if (strcmp(cmd,"consultar")==0) {
                        char out[64]; snprintf(out,sizeof(out),"%s %s\n", CMD_CONSULTAR, username); write(sv, out, strlen(out));
                    } else if (strcmp(cmd,"cancelar")==0) {
                        int id; if (sscanf(line+8,"%d",&id)==1) { char out[64]; snprintf(out,sizeof(out),"%s %s %d\n", CMD_CANCELAR, username, id); write(sv,out,strlen(out)); }
                        else printf("USO: cancelar <id>\n");
                    } else if (strcmp(cmd,"terminar")==0) {
                        char out[128]; snprintf(out,sizeof(out),"%s %s\n", CMD_UNREGISTER, username); write(sv,out,strlen(out));
                        printf("SAINDO\n"); break;
                    } else printf("COMANDO DESCONHECIDO\n");
                }
            }
        }
    }

    close(cf); close(sv); unlink(client_fifo);
    printf("CLIENTE %s TERMINADO.\n", username);
    return 0;
}