#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "util.h"

#define MAX_PLACARES 20

// 1. Declarar a estrutura principal antecipadamente para poder usÃ¡-la dentro de PLACAR
typedef struct _ESTADO_CENTRAL ESTADO_CENTRAL;

// 2. Estrutura de cada placar, agora guarda um ponteiro para o estado geral
typedef struct {
    HANDLE hPipe;
    DWORD id;
    BOOL ativo;
    BOOL temAlerta;
    MSG_ALERTA alertaAtual;
    DWORD index;                // O seu Ã­ndice no array
    ESTADO_CENTRAL* estado;     // Ponteiro para a estrutura principal
} PLACAR;

// 3. Estrutura principal que guarda TUDO (o equivalente ao "DATA" das tuas aulas)
struct _ESTADO_CENTRAL {
    PLACAR placares[MAX_PLACARES];
    DWORD nextId;
    HANDLE hMutex;
    BOOL continua;

    // VariÃ¡veis da MemÃ³ria Partilhada
    HANDLE hMapFile;
    SHM_ALERTA* pShm;
    HANDLE hMutexSHM;
    HANDLE hEventoSHM;
};

// Estrutura de apoio para passar argumentos Ã  thread de ligaÃ§Ãµes
typedef struct {
    TCHAR nomePipeCompleto[256];
    ESTADO_CENTRAL* estado;
} ACEITA_DATA;


// FunÃ§Ã£o que copia o estado interno para a SHM
// Recebe o ponteiro do estado como argumento em vez de usar globais
void AtualizaSHM(ESTADO_CENTRAL* estado) {
    WaitForSingleObject(estado->hMutexSHM, INFINITE);

    // Limpar o espaÃ§o de memÃ³ria anterior
    ZeroMemory(estado->pShm, sizeof(SHM_ALERTA));
    estado->pShm->desligar = !estado->continua;

    int idx = 0;
    for (int i = 0; i < MAX_PLACARES; i++) {
        if (estado->placares[i].ativo) {
            estado->pShm->placar[idx].identificador = estado->placares[i].id;
            if (estado->placares[i].temAlerta) {
                estado->pShm->placar[idx].duracao = estado->placares[i].alertaAtual.duracao;
                _tcscpy_s(estado->pShm->placar[idx].msg, 140, estado->placares[i].alertaAtual.msg);
            }
            else {
                estado->pShm->placar[idx].duracao = 0;
                _tcscpy_s(estado->pShm->placar[idx].msg, 140, _T(""));
            }
            idx++;
        }
    }

    ReleaseMutex(estado->hMutexSHM);

    SetEvent(estado->hEventoSHM);
    Sleep(50); // Pausa breve para as threads dos monitores acordarem
    ResetEvent(estado->hEventoSHM);
}


// FunÃ§Ã£o executada para cada placar ligado
DWORD WINAPI atendePlacar(LPVOID dados) {
    // Recuperar a estrutura atravÃ©s do ponteiro
    PLACAR* ptd = (PLACAR*)dados;
    ESTADO_CENTRAL* estado = ptd->estado;

    MSG_GENERICA msgRx;
    DWORD nBytes;
    BOOL res;

    OVERLAPPED ov;
    HANDLE hEv = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (estado->continua) {
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = hEv;

        res = ReadFile(ptd->hPipe, &msgRx, sizeof(MSG_GENERICA), &nBytes, &ov);
        if (!res) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(hEv, INFINITE);
                res = GetOverlappedResult(ptd->hPipe, &ov, &nBytes, FALSE);
            }
            else {
                break; // Erro ou pipe desligado
            }
        }
        if (!res || nBytes == 0) break;

        WaitForSingleObject(estado->hMutex, INFINITE);

        switch (msgRx.tipo) {
        case 1: { // ligar
            ptd->id = estado->nextId++;
            ptd->ativo = TRUE;
            ptd->temAlerta = FALSE;

            MSG_ID respId = { 7, ptd->id };
            WriteFile(ptd->hPipe, &respId, sizeof(MSG_ID), &nBytes, NULL);
            _tprintf_s(_T("\n[SISTEMA] Placar %d registado.\nCMD> "), respId.identificador);
            AtualizaSHM(estado);
            break;
        }
        case 2: { // desligar
            MSG_CMD respDesliga = { 2 };
            WriteFile(ptd->hPipe, &respDesliga, sizeof(MSG_CMD), &nBytes, NULL);
            ptd->ativo = FALSE;
            _tprintf_s(_T("\n[SISTEMA] Placar %d desligou-se.\nCMD> "), ptd->id);
            AtualizaSHM(estado);
            break;
        }
        case 3: { // fim alerta
            ptd->temAlerta = FALSE;
            _tprintf_s(_T("\n[SISTEMA] Placar %d terminou o alerta.\nCMD> "), ptd->id);
            AtualizaSHM(estado);
            break;
        }
        case 4: { // confirmacao novo alerta
            _tprintf_s(_T("\n[SISTEMA] Placar %d confirmou rececao do alerta.\nCMD> "), ptd->id);
            break;
        }
        case 5: { // confirmacao cancelar
            _tprintf_s(_T("\n[SISTEMA] Placar %d confirmou cancelamento do alerta.\nCMD> "), ptd->id);
            break;
        }
        }
        ReleaseMutex(estado->hMutex);
    }

    CloseHandle(hEv);

    WaitForSingleObject(estado->hMutex, INFINITE);
    ptd->ativo = FALSE;
    ptd->hPipe = INVALID_HANDLE_VALUE;
    AtualizaSHM(estado);
    ReleaseMutex(estado->hMutex);

    DisconnectNamedPipe(ptd->hPipe);
    CloseHandle(ptd->hPipe);
    ExitThread(0);
}


// Thread para aceitar novas ligações no background
DWORD WINAPI aceitaLigacoes(LPVOID arg) {
    // Recuperar os dados
    ACEITA_DATA* pDados = (ACEITA_DATA*)arg;
    ESTADO_CENTRAL* estado = pDados->estado;

    HANDLE hPipe;
    OVERLAPPED ovConnect;
    HANDLE hEvConnect = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (estado->continua) {
        hPipe = CreateNamedPipe(pDados->nomePipeCompleto, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
            PIPE_WAIT | PIPE_TYPE_MESSAGE | PIPE_READMODE_MESSAGE,
            MAX_PLACARES, sizeof(MSG_GENERICA), sizeof(MSG_GENERICA), 1000, NULL);

        if (hPipe == INVALID_HANDLE_VALUE) {
            _tprintf_s(_T("[ERRO] Falha ao criar o Named Pipe!\n"));
            break;
        }

        ZeroMemory(&ovConnect, sizeof(ovConnect));
        ovConnect.hEvent = hEvConnect;

        if (!ConnectNamedPipe(hPipe, &ovConnect)) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(hEvConnect, INFINITE);
                DWORD dummy;
                GetOverlappedResult(hPipe, &ovConnect, &dummy, FALSE);
            }
            else {
                CloseHandle(hPipe);
                continue;
            }
        }

        if (!estado->continua) {
            CloseHandle(hPipe);
            break;
        }

        WaitForSingleObject(estado->hMutex, INFINITE);
        DWORD i;
        for (i = 0; i < MAX_PLACARES; i++) {
            if (estado->placares[i].hPipe == INVALID_HANDLE_VALUE) {
                estado->placares[i].hPipe = hPipe;
                // Passamos o endereço do PLACAR específico para a thread
                CreateThread(NULL, 0, atendePlacar, (LPVOID)&estado->placares[i], 0, NULL);
                break;
            }
        }
        if (i == MAX_PLACARES) {
            _tprintf_s(_T("\n[SISTEMA] Sem vagas para novos placares!\nCMD> "));
            DisconnectNamedPipe(hPipe);
            CloseHandle(hPipe);
        }
        ReleaseMutex(estado->hMutex);
    }
    CloseHandle(hEvConnect);
    ExitThread(0);
}


int _tmain(int argc, TCHAR* argv[]) {
    ESTADO_CENTRAL estado;
    ACEITA_DATA aceitaDados;

    TCHAR input[256], cmd[50];

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

    HANDLE hUnico = CreateMutex(NULL, FALSE, _T("Mutex_Central_Unica_SO2"));
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        _tprintf_s(_T("[ERRO] Ja existe uma instancia da central em execucao!\n"));
        return 1;
    }

    if (argc < 2) {
        _tprintf_s(_T("[ERRO] Uso: central.exe <nome_do_pipe>\n"));
        return 1;
    }

    // Inicializar os dados
    estado.continua = TRUE;
    estado.nextId = 1;
    estado.hMutex = CreateMutex(NULL, FALSE, NULL);
    for (int i = 0; i < MAX_PLACARES; i++) {
        estado.placares[i].hPipe = INVALID_HANDLE_VALUE;
        estado.placares[i].ativo = FALSE;
        estado.placares[i].index = i;             // Guarda o seu índice
        estado.placares[i].estado = &estado;      // Guarda o ponteiro para a estrutura principal
    }

    // Inicializar SHM
    estado.hMapFile = CreateFileMapping(INVALID_HANDLE_VALUE, NULL, PAGE_READWRITE, 0, sizeof(SHM_ALERTA), NOME_SHM);
    if (estado.hMapFile == NULL) {
        _tprintf_s(_T("[ERRO] Falha ao criar a Memoria Partilhada.\n"));
        return 1;
    }
    estado.pShm = (SHM_ALERTA*)MapViewOfFile(estado.hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    estado.hMutexSHM = CreateMutex(NULL, FALSE, NOME_MUTEX_SHM);
    estado.hEventoSHM = CreateEvent(NULL, TRUE, FALSE, NOME_EVENTO_SHM);

    WaitForSingleObject(estado.hMutex, INFINITE);
    AtualizaSHM(&estado);
    ReleaseMutex(estado.hMutex);

    // Preparar dados para a thread de aceitação
    _stprintf_s(aceitaDados.nomePipeCompleto, _countof(aceitaDados.nomePipeCompleto), _T("\\\\.\\pipe\\%s"), argv[1]);
    aceitaDados.estado = &estado;
    HANDLE hThreadAceita = CreateThread(NULL, 0, aceitaLigacoes, (LPVOID)&aceitaDados, 0, NULL);

    _tprintf_s(_T("[CENTRAL] A escutar no pipe: %s\n"), aceitaDados.nomePipeCompleto);

    while (estado.continua) {
        _tprintf_s(_T("CMD> "));
        _fgetts(input, 256, stdin);

        size_t len = _tcslen(input);
        if (len > 0 && input[len - 1] == _T('\n')) input[len - 1] = _T('\0');

        _stscanf_s(input, _T("%s"), cmd, (unsigned)_countof(cmd));

        if (_tcsicmp(cmd, _T("alerta")) == 0) {
            TCHAR msg[140];
            DWORD duracao, targetId;
            if (_stscanf_s(input, _T("%*s \"%139[^\"]\" %d %d"), msg, (unsigned)_countof(msg), &duracao, &targetId) == 3 ||
                _stscanf_s(input, _T("%*s '%139[^']' %d %d"), msg, (unsigned)_countof(msg), &duracao, &targetId) == 3 ||
                _stscanf_s(input, _T("%*s %139s %d %d"), msg, (unsigned)_countof(msg), &duracao, &targetId) == 3) {

                MSG_ALERTA alerta = { 4, _T(""), duracao };
                _tcscpy_s(alerta.msg, _countof(alerta.msg), msg);
                DWORD written;

                WaitForSingleObject(estado.hMutex, INFINITE);
                for (int i = 0; i < MAX_PLACARES; i++) {
                    if (estado.placares[i].ativo && (targetId == 0 || estado.placares[i].id == targetId)) {
                        estado.placares[i].temAlerta = TRUE;
                        estado.placares[i].alertaAtual = alerta;
                        WriteFile(estado.placares[i].hPipe, &alerta, sizeof(MSG_ALERTA), &written, NULL);
                    }
                }
                AtualizaSHM(&estado);
                ReleaseMutex(estado.hMutex);
            }
            else {
                _tprintf_s(_T("Formato invalido.\n"));
            }
        }
        else if (_tcsicmp(cmd, _T("cancelar")) == 0) {
            DWORD targetId;
            if (_stscanf_s(input, _T("%*s %d"), &targetId) == 1) {
                MSG_CMD cancelar = { 5 };
                DWORD written;

                WaitForSingleObject(estado.hMutex, INFINITE);
                for (int i = 0; i < MAX_PLACARES; i++) {
                    if (estado.placares[i].ativo && estado.placares[i].id == targetId && estado.placares[i].temAlerta) {
                        estado.placares[i].temAlerta = FALSE;
                        WriteFile(estado.placares[i].hPipe, &cancelar, sizeof(MSG_CMD), &written, NULL);
                        break;
                    }
                }
                AtualizaSHM(&estado);
                ReleaseMutex(estado.hMutex);
            }
        }
        else if (_tcsicmp(cmd, _T("listar")) == 0) {
            WaitForSingleObject(estado.hMutex, INFINITE);
            _tprintf_s(_T("\n--- PLACARES ATIVOS ---\n"));
            for (int i = 0; i < MAX_PLACARES; i++) {
                if (estado.placares[i].ativo) {
                    _tprintf_s(_T("ID: %d"), estado.placares[i].id);
                    if (estado.placares[i].temAlerta) {
                        _tprintf_s(_T(" | Alerta Ativo: '%s' (%ds)"), estado.placares[i].alertaAtual.msg, estado.placares[i].alertaAtual.duracao);
                    }
                    _tprintf_s(_T("\n"));
                }
            }
            _tprintf_s(_T("-----------------------\n"));
            ReleaseMutex(estado.hMutex);
        }
        else if (_tcsicmp(cmd, _T("encerrar")) == 0) {
            MSG_CMD encerrar = { 6 };
            DWORD written;
            WaitForSingleObject(estado.hMutex, INFINITE);
            estado.continua = FALSE;
            for (int i = 0; i < MAX_PLACARES; i++) {
                if (estado.placares[i].ativo) {
                    WriteFile(estado.placares[i].hPipe, &encerrar, sizeof(MSG_CMD), &written, NULL);
                }
            }
            AtualizaSHM(&estado);
            ReleaseMutex(estado.hMutex);
        }
    }

    // Libertar todos os recursos do sistema
    CloseHandle(estado.hMutex);
    UnmapViewOfFile(estado.pShm);
    CloseHandle(estado.hMapFile);
    CloseHandle(estado.hMutexSHM);
    CloseHandle(estado.hEventoSHM);

    return 0;
}