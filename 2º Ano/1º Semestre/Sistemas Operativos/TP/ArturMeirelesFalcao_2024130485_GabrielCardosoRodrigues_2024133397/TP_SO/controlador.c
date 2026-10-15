#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <signal.h>
#include <errno.h>
#include <time.h>
#include "util.h"

static service_t services[MAX_SERVICES];
static int nservices = 0;
static int next_id = 1;
static int total_km = 0;
static int sim_time = 1;
static int NVEICULOS = 3;
static char active_users[MAX_USERS][MAX_NAME];
static int user_ativos = 0;
static int running = 1;

void cleanup() { unlink(SERVER_FIFO); }

int is_user_ativos(const char *u) {
    for (int i=0;i<user_ativos;i++) if (strcmp(active_users[i],u)==0) return 1;
    return 0;
}
void add_user_ativos(const char *u) {
    if (user_ativos < MAX_USERS && !is_user_ativos(u)) {
        strncpy(active_users[user_ativos++], u, MAX_NAME-1);
    }
}
void remove_user_ativos(const char *u) {
    for (int i=0;i<user_ativos;i++) if (strcmp(active_users[i],u)==0) {
        for (int j=i;j+1<user_ativos;j++) strcpy(active_users[j], active_users[j+1]);
        user_ativos--; return;
    }
}

int count_veiculos_em_acao() {
    int c=0;
    for (int i=0;i<nservices;i++) if (services[i].estado==S_EM_TRANSITO || services[i].estado==S_ESPERA) c++;
    return c;
}

void listar() {
    printf("SERVICOS (id usuario hora local dist estado pid)\n");
    for (int i=0;i<nservices;i++) {
        printf("%d %s %d %s %d %d %d\n", services[i].id, services[i].user, services[i].hora,
               services[i].local, services[i].distancia, services[i].estado, services[i].pid_veiculo);
    }
}

void utiliz() {
    printf("UTILIZADORES ATIVOS (Nome - Estado):\n");
    for (int i = 0; i < user_ativos; i++) {
        char status[64] = "Livre / Aguardando";
        int found_active_service = 0;
        for (int j = 0; j < nservices; j++) {
            if (strcmp(services[j].user, active_users[i]) == 0) {
                if (services[j].estado == S_ESPERA) {
                    snprintf(status, sizeof(status), "À ESPERA (Serv %d)", services[j].id);
                    found_active_service = 1;
                    break;
                } else if (services[j].estado == S_EM_TRANSITO) {
                    snprintf(status, sizeof(status), "EM VIAGEM (Serv %d, %d%%)", services[j].id, services[j].progresso_pct);
                    found_active_service = 1;
                    break;
                }
            }
        }
        printf("%s - %s\n", active_users[i], status);
    }
}

void frota() {
    printf("FROTA EM ACAO (PID SERV ESTADO PROGRESSO):\n");
    for (int i=0;i<nservices;i++) {
        if (services[i].estado==S_EM_TRANSITO || services[i].estado==S_ESPERA) {
            int pct = (services[i].estado == S_ESPERA) ? 0 : services[i].progresso_pct;
            
            printf("VEIC PID %d SERV %d ESTADO %d PROGRESSO %d%%\n", 
                   services[i].pid_veiculo, 
                   services[i].id, 
                   services[i].estado,
                   pct);
        }
    }
}
void show_km() { printf("TOTAL KM: %d\n", total_km); }
void show_hora() { printf("HORA SIMULADA: %d\n", sim_time); }

void cancelar_servico_total(int id, const char *user) {
    int feito_pelo_cliente = (user != NULL);
    int encontrado = 0;

    for (int i = 0; i < nservices; i++) {
        if (feito_pelo_cliente && strcmp(services[i].user, user) != 0)
            continue;

        if (id != 0 && services[i].id != id)
            continue;

        encontrado = 1;
        char cf[128];
        snprintf(cf, sizeof(cf), CLIENT_FIFO_FORMAT, services[i].user);
        int c = open(cf, O_WRONLY | O_NONBLOCK);
        char msg[256];

        if (services[i].estado == S_AGENDADO || 
            services[i].estado == S_ESPERA || 
            services[i].estado == S_EM_TRANSITO) {

            if ((services[i].estado == S_ESPERA || services[i].estado == S_EM_TRANSITO) &&
                services[i].pid_veiculo > 0)
                kill(services[i].pid_veiculo, SIGUSR1);

            services[i].estado = S_CANCELADO;

            if (c != -1) {
                if (feito_pelo_cliente)
                    snprintf(msg, sizeof(msg), "CANCELAMENTO FEITO COM SUCESSO. (SERVICO %d)\n", services[i].id);
                else
                    snprintf(msg, sizeof(msg), "SERVICO %d CANCELADO PELO CONTROLADOR.\n", services[i].id);
                write(c, msg, strlen(msg));
                close(c);
            }

            if (feito_pelo_cliente)
                printf("CONTROLADOR: CLIENTE %s CANCELOU SERVICO %d\n", services[i].user, services[i].id);
            else
                printf("CONTROLADOR: CANCELAMENTO COM SUCESSO SERVICO %d\n", services[i].id);
            fflush(stdout);

            if (id != 0) return;

        } else { 
            if (feito_pelo_cliente && c != -1) {
                snprintf(msg, sizeof(msg), "SERVICO %d JA CANCELADO OU CONCLUIDO, NAO PODE SER CANCELADO\n", services[i].id);
                write(c, msg, strlen(msg));
                close(c);
            }

            if (!feito_pelo_cliente)
                printf("CONTROLADOR: SERVICO %d JA CANCELADO OU CONCLUIDO, NAO PODE SER CANCELADO \n", services[i].id);
            fflush(stdout);

            if (id != 0) return;
        }
    }

    if (!encontrado && feito_pelo_cliente && id != 0) {
        printf("CONTROLADOR: SERVICO %d NAO ENCONTRADO PARA CANCELAMENTO\n", id);
        fflush(stdout);

        char cf[128];
        snprintf(cf, sizeof(cf), CLIENT_FIFO_FORMAT, user);
        int c = open(cf, O_WRONLY | O_NONBLOCK);
        if (c != -1) {
            char msg[128];
            snprintf(msg, sizeof(msg), "SERVICO %d NAO ENCONTRADO\n", id);
            write(c, msg, strlen(msg));
            close(c);
        }
    }
}


void start_vehicle_for(service_t *s) {
    int p[2];
    if (pipe(p)==-1) { perror("pipe"); return; }
    pid_t pid = fork();
    if (pid==-1) { perror("fork"); close(p[0]); close(p[1]); return; }
    if (pid==0) {
        close(p[0]);
        dup2(p[1], STDOUT_FILENO);
        close(p[1]);
        char dist[16], id[16], clientfifo[128];
        snprintf(dist,sizeof(dist),"%d", s->distancia);
        snprintf(id,sizeof(id),"%d", s->id);
        snprintf(clientfifo,sizeof(clientfifo),CLIENT_FIFO_FORMAT, s->user);
        execlp("./veiculo", "./veiculo", clientfifo, dist, s->local, s->user, id, (char*)NULL);
        perror("execlp veiculo");
        _exit(1);
    } else {
        close(p[1]);
        s->pid_veiculo = pid;
        s->pipe_fd = p[0];
        s->estado = S_ESPERA;
        s->progresso_pct = 0;
        int f = fcntl(s->pipe_fd, F_GETFL, 0);
        fcntl(s->pipe_fd, F_SETFL, f | O_NONBLOCK);
        printf("LANCADO VEICULO PID %d PARA SERV %d\n", pid, s->id);
    }
}

void read_vehicle_outputs() {
    char buf[256];
        int pct;
    for (int i=0;i<nservices;i++) {
        if ((services[i].estado==S_ESPERA || services[i].estado==S_EM_TRANSITO) && services[i].pipe_fd>0) {
            ssize_t r = read(services[i].pipe_fd, buf, sizeof(buf)-1);
            if (r>0) {
                buf[r]=0;
                printf("%s", buf);

                if (strstr(buf,VEH_MSG_COMECOU))
                    services[i].estado = S_EM_TRANSITO;

                if (strstr(buf,VEH_MSG_ACABOU)) {
                    services[i].estado = S_CONCLUIDO;
                    total_km += services[i].distancia;
                    close(services[i].pipe_fd);
                    services[i].pipe_fd = 0;
                }
                
                char *pct_str = strstr(buf, VEH_MSG_PERCETAGEM);
                if (pct_str != NULL) {
                    if (sscanf(pct_str, VEH_MSG_PERCETAGEM " %d", &pct) == 1) {
                        services[i].progresso_pct = pct;
                    }
                }
                if (strstr(buf,VEH_MSG_CANCELADO)) {
                    services[i].estado = S_CANCELADO;
                    close(services[i].pipe_fd);
                    services[i].pipe_fd = 0;
                }
            }
            else if (r == 0) {
                close(services[i].pipe_fd);
                services[i].pipe_fd = 0;

                if (services[i].estado == S_ESPERA || services[i].estado == S_EM_TRANSITO) {
                    if (services[i].estado != S_CONCLUIDO) { 
                        total_km += services[i].distancia;
                    }
                    services[i].estado = S_CONCLUIDO;
                }
            }
        }
    }
}


int main(void) {
    char buffer[512];

    if (getenv("NVEICULOS")) NVEICULOS = atoi(getenv("NVEICULOS"));

    atexit(cleanup);
    unlink(SERVER_FIFO);
    if (mkfifo(SERVER_FIFO, 0666)==-1 && errno!=EEXIST) { perror("mkfifo"); return 1; }

    int fd_rd = open(SERVER_FIFO, O_RDONLY | O_NONBLOCK);
    if (fd_rd==-1) { perror("open rd"); return 1; }
    int fd_wr = open(SERVER_FIFO, O_WRONLY);
    printf("CONTROLADOR INICIADO... \n");
    fflush(stdout);

    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);

    time_t last = time(NULL);

    while (running) {
        ssize_t r = read(STDIN_FILENO, buffer, sizeof(buffer)-1);
        if (r>0) {
            buffer[r]=0;
            char cmd[32];
            if (sscanf(buffer,"%31s", cmd)>=1) {
                if (strcmp(cmd,"listar")==0) listar();
                else if (strcmp(cmd,"utiliz")==0) utiliz();
                else if (strcmp(cmd,"frota")==0) frota();
                else if (strcmp(cmd,"km")==0) show_km();
                else if (strcmp(cmd,"hora")==0) show_hora();
                else if (strcmp(cmd,"terminar")==0) { 
                    cancelar_servico_total(0,NULL);
                    for (int i=0;i<user_ativos;i++) {
                        char cf[128]; snprintf(cf,sizeof(cf),CLIENT_FIFO_FORMAT, active_users[i]);
                        int c = open(cf, O_WRONLY|O_NONBLOCK);
                        if (c!=-1) { char m[]="PLATAFORMA TERMINOU\n"; write(c,m,strlen(m)); close(c); }
                    }
                    running=0;
                    } else if (strcmp(cmd,"cancelar")==0) {
                        int id; 
                        if (sscanf(buffer+9, "%d", &id) == 1) {
                            cancelar_servico_total(id, NULL);
                        } else {
                            printf("USO: cancelar <id>\n");
                        }
                    } else printf("COMANDO DESCONHECIDO\n");
            }
        }

        ssize_t rf = read(fd_rd, buffer, sizeof(buffer)-1);
        if (rf>0) {
            buffer[rf]=0;
            char cmd[32];
            if (sscanf(buffer, "%31s", cmd)>=1) {
                if (strcmp(cmd,CMD_REGISTER)==0) {
                    char user[MAX_NAME]; if (sscanf(buffer+strlen(CMD_REGISTER), "%63s", user)==1) {
                        char clientfifo[128]; snprintf(clientfifo,sizeof(clientfifo),CLIENT_FIFO_FORMAT, user);
                        int cfd = open(clientfifo, O_WRONLY|O_NONBLOCK);
                        if (is_user_ativos(user)) { if (cfd!=-1) { write(cfd,MSG_ERROR"\n",6); close(cfd); } }
                        else if (user_ativos>=MAX_USERS) { if (cfd!=-1) { write(cfd,MSG_ERROR"\n",6); close(cfd); } }
                        else if (cfd!=-1) {
                            add_user_ativos(user);
                            write(cfd,MSG_REGISTERED"\n",11);
                            close(cfd);
                        }
                    }
                } else if (strcmp(cmd,CMD_AGENDAR)==0) {
                    char user[MAX_NAME], local[MAX_NAME]; int hora, dist;
                    if (sscanf(buffer+strlen(CMD_AGENDAR), "%63s %d %63s %d", user, &hora, local, &dist)==4) {
                        if (nservices<MAX_SERVICES && is_user_ativos(user)) {
                            service_t s = { .id = next_id++, .hora = hora, .distancia = dist, .estado = S_AGENDADO, .pid_veiculo = 0, .pipe_fd = 0, .progresso_pct = 0 };
                            strncpy(s.user, user, MAX_NAME-1); strncpy(s.local, local, MAX_NAME-1);
                            services[nservices++] = s;
                            char cf[128]; snprintf(cf,sizeof(cf),CLIENT_FIFO_FORMAT, user);
                            int cfd = open(cf, O_WRONLY|O_NONBLOCK);
                            if (cfd!=-1) { char resp[64]; snprintf(resp,sizeof(resp),MSG_AGENDADO_FORMAT, s.id); write(cfd,resp,strlen(resp)); close(cfd); }
                        }
                    }
                } else if (strcmp(cmd,CMD_CONSULTAR)==0) {
                    char user[MAX_NAME]; if (sscanf(buffer+strlen(CMD_CONSULTAR),"%63s", user)==1) {
                        char cf[128]; snprintf(cf,sizeof(cf),CLIENT_FIFO_FORMAT, user);
                        int cfd = open(cf, O_WRONLY|O_NONBLOCK);
                        if (cfd!=-1) {
                            char out[1024]="";
                            for (int i=0;i<nservices;i++) if (strcmp(services[i].user,user)==0) {
                                char line[128];
                                snprintf(line,sizeof(line),"SERV %d HORA %d LOCAL %s DIST %d EST %d\n",
                                         services[i].id, services[i].hora, services[i].local, services[i].distancia, services[i].estado);
                                strncat(out,line,sizeof(out)-strlen(out)-1);
                            }
                            write(cfd,out,strlen(out)); close(cfd);
                        }
                    }
                } else if (strcmp(cmd,CMD_CANCELAR)==0) {
                    char user[MAX_NAME]; int id;
                    if (sscanf(buffer+strlen(CMD_CANCELAR),"%63s %d", user, &id)==2) cancelar_servico_total(id, user);
                } else if (strcmp(cmd,CMD_UNREGISTER)==0) {
                    char user[MAX_NAME]; if (sscanf(buffer+strlen(CMD_UNREGISTER),"%63s", user)==1) {
                        cancelar_servico_total(0, user); remove_user_ativos(user);
                    }
                }
            }
        }

        time_t now = time(NULL);
        if (now - last >= 1) {
            last = now; sim_time++;
            for (int i=0;i<nservices;i++) {
                if (services[i].estado==S_AGENDADO && services[i].hora==sim_time) {
                    if (count_veiculos_em_acao() < NVEICULOS) start_vehicle_for(&services[i]);
                }
            }
        }

        read_vehicle_outputs();

        usleep(50000);
    }

    close(fd_rd);
    close(fd_wr);
    printf("CONTROLADOR TERMINADO\n");
    return 0;
}