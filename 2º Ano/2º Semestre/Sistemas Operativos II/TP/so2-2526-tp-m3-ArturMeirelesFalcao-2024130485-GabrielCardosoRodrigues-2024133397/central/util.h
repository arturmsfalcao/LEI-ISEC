#pragma once
#include <windows.h>
#include <tchar.h>

// Estruturas definidas no enunciado
typedef struct {
    BYTE tipo; // 1 (ligar), 2 (desligar), 3 (fim alerta), 5 (cancelar), 6 (encerrar)
} MSG_CMD;

typedef struct {
    BYTE tipo; // 4 (novo alerta)
    TCHAR msg[140];
    DWORD duracao;
} MSG_ALERTA;

typedef struct {
    BYTE tipo; // 7 (atribuir ID)
    DWORD identificador;
} MSG_ID;

// União genérica para simplificar a leitura de qualquer pacote no Named Pipe
typedef union {
    BYTE tipo;
    MSG_CMD cmd;
    MSG_ALERTA alerta;
    MSG_ID id;
} MSG_GENERICA;

// Estrutura do Alerta para a SHM
typedef struct {
    struct {
        DWORD identificador;
        DWORD duracao;
        TCHAR msg[140];
        BOOL ativo; // Adicionado para facilitar saber se o slot está em uso
    } placar[20];
    BOOL desligar;
} SHM_ALERTA;

// Nomes globais (hardcoded para já, o Monitor permite configurar depois)
#define NOME_SHM _T("SHM_Alertas_SO2")
#define NOME_MUTEX_SHM _T("Mutex_SHM_SO2")
#define NOME_EVENTO_SHM _T("Evento_Update_SO2")