#ifndef UTIL_H
#define UTIL_H

#include <sys/types.h>

#define SERVER_FIFO "empresa_taxis"
#define MAX_USERS 30
#define MAX_SERVICES 100
#define MAX_NAME 64
#define CLIENT_FIFO_FORMAT "cli_%s"

// Estados de Serviço
typedef enum {
    S_AGENDADO,
    S_ESPERA,
    S_EM_TRANSITO,
    S_CANCELADO,
    S_CONCLUIDO
} estado_t;

// Estrutura de Dados do Serviço
typedef struct {
    int id;
    char user[MAX_NAME];
    int hora;
    char local[MAX_NAME];
    int distancia;
    estado_t estado;
    pid_t pid_veiculo;
    int pipe_fd;
    int progresso_pct;
} service_t;

#define CMD_REGISTER "REGISTER"
#define CMD_AGENDAR "AGENDAR"
#define CMD_CONSULTAR "CONSULTAR"
#define CMD_CANCELAR "CANCELAR"
#define CMD_UNREGISTER "UNREGISTER"

#define MSG_REGISTERED "REGISTERED"
#define MSG_ERROR "ERROR"
#define MSG_AGENDADO_FORMAT "AGENDADO ID %d\n"

#define VEH_MSG_COMECOU "COMECOU"
#define VEH_MSG_ACABOU "ACABOU"
#define VEH_MSG_CANCELADO "CANCELADO"
#define VEH_MSG_PERCETAGEM "PERCETAGEM"

#endif