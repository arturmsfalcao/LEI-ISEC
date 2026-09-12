#include <windows.h>
#include <tchar.h>
#include <stdio.h>
#include <io.h>
#include <fcntl.h>
#include "util.h"

#define MAX_PLACARES 20

typedef struct {
    HANDLE hPipe;
    DWORD id;
    BOOL ativo;
    BOOL temAlerta;
    MSG_ALERTA alertaAtual;
} PLACAR;

typedef struct {
    PLACAR placares[MAX_PLACARES];
    DWORD nextId;
    HANDLE hMutex;
    BOOL continua;
} ESTADO_CENTRAL;

ESTADO_CENTRAL estado;

// Função executada para cada placar ligado
DWORD WINAPI atendePlacar(LPVOID dados) {
    DWORD index = (DWORD)(ULONG_PTR)dados;
    HANDLE hPipe = estado.placares[index].hPipe;
    MSG_GENERICA msgRx;
    DWORD nBytes;
    BOOL res;

    OVERLAPPED ov;
    HANDLE hEv = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (estado.continua) {
        ZeroMemory(&ov, sizeof(ov));
        ov.hEvent = hEv;

        res = ReadFile(hPipe, &msgRx, sizeof(MSG_GENERICA), &nBytes, &ov);
        if (!res) {
            if (GetLastError() == ERROR_IO_PENDING) {
                WaitForSingleObject(hEv, INFINITE);
                res = GetOverlappedResult(hPipe, &ov, &nBytes, FALSE);
            }
            else {
                break; // Erro ou pipe desligado
            }
        }
        if (!res || nBytes == 0) break;

        WaitForSingleObject(estado.hMutex, INFINITE);

        switch (msgRx.tipo) {
        case 1: { // ligar
            estado.placares[index].id = estado.nextId++;
            estado.placares[index].ativo = TRUE;
            estado.placares[index].temAlerta = FALSE;

            MSG_ID respId = { 7, estado.placares[index].id };
            WriteFile(hPipe, &respId, sizeof(MSG_ID), &nBytes, NULL);
            _tprintf_s(_T("\n[SISTEMA] Placar %d registado.\nCMD> "), respId.identificador);
            break;
        }
        case 2: { // desligar
            MSG_CMD respDesliga = { 2 };
            WriteFile(hPipe, &respDesliga, sizeof(MSG_CMD), &nBytes, NULL);
            estado.placares[index].ativo = FALSE;
            _tprintf_s(_T("\n[SISTEMA] Placar %d desligou-se.\nCMD> "), estado.placares[index].id);
            break;
        }
        case 3: { // fim alerta
            estado.placares[index].temAlerta = FALSE;
            _tprintf_s(_T("\n[SISTEMA] Placar %d terminou o alerta.\nCMD> "), estado.placares[index].id);
            break;
        }
        case 4: { // confirmacao novo alerta
            _tprintf_s(_T("\n[SISTEMA] Placar %d confirmou rececao do alerta.\nCMD> "), estado.placares[index].id);
            break;
        }
        case 5: { // confirmacao cancelar
            _tprintf_s(_T("\n[SISTEMA] Placar %d confirmou cancelamento do alerta.\nCMD> "), estado.placares[index].id);
            break;
        }
        }
        ReleaseMutex(estado.hMutex);
    }

    CloseHandle(hEv);

    WaitForSingleObject(estado.hMutex, INFINITE);
    estado.placares[index].ativo = FALSE;
    estado.placares[index].hPipe = INVALID_HANDLE_VALUE;
    ReleaseMutex(estado.hMutex);

    DisconnectNamedPipe(hPipe);
    CloseHandle(hPipe);
    ExitThread(0);
}

// Thread para aceitar novas ligações no background
DWORD WINAPI aceitaLigacoes(LPVOID arg) {
    TCHAR* nomePipeCompleto = (TCHAR*)arg;
    HANDLE hPipe;
    OVERLAPPED ovConnect;
    HANDLE hEvConnect = CreateEvent(NULL, TRUE, FALSE, NULL);

    while (estado.continua) {
        hPipe = CreateNamedPipe(nomePipeCompleto, PIPE_ACCESS_DUPLEX | FILE_FLAG_OVERLAPPED,
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

        if (!estado.continua) {
            CloseHandle(hPipe);
            break;
        }

        WaitForSingleObject(estado.hMutex, INFINITE);
        DWORD i;
        for (i = 0; i < MAX_PLACARES; i++) {
            if (estado.placares[i].hPipe == INVALID_HANDLE_VALUE) {
                estado.placares[i].hPipe = hPipe;
                CreateThread(NULL, 0, atendePlacar, (LPVOID)(ULONG_PTR)i, 0, NULL);
                break;
            }
        }
        if (i == MAX_PLACARES) {
            _tprintf_s(_T("\n[SISTEMA] Sem vagas para novos placares!\nCMD> "));
            DisconnectNamedPipe(hPipe);
            CloseHandle(hPipe);
        }
        ReleaseMutex(estado.hMutex);
    }
    CloseHandle(hEvConnect);
    ExitThread(0);
}

int _tmain(int argc, TCHAR* argv[]) {
    TCHAR nomePipeCompleto[256];
    TCHAR input[256], cmd[50];

#ifdef UNICODE
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

    if (argc < 2) {
        _tprintf_s(_T("[ERRO] Uso: central.exe <nome_do_pipe>\n"));
        return 1;
    }

    _stprintf_s(nomePipeCompleto, _countof(nomePipeCompleto), _T("\\\\.\\pipe\\%s"), argv[1]);

    estado.continua = TRUE;
    estado.nextId = 1;
    estado.hMutex = CreateMutex(NULL, FALSE, NULL);
    for (int i = 0; i < MAX_PLACARES; i++) {
        estado.placares[i].hPipe = INVALID_HANDLE_VALUE;
        estado.placares[i].ativo = FALSE;
    }

    HANDLE hThreadAceita = CreateThread(NULL, 0, aceitaLigacoes, (LPVOID)nomePipeCompleto, 0, NULL);

    _tprintf_s(_T("[CENTRAL] A escutar no pipe: %s\n"), nomePipeCompleto);

    while (estado.continua) {
        _tprintf_s(_T("CMD> "));
        _fgetts(input, 256, stdin);

        // Remover \n
        size_t len = _tcslen(input);
        if (len > 0 && input[len - 1] == _T('\n')) input[len - 1] = _T('\0');

        _stscanf_s(input, _T("%s"), cmd, (unsigned)_countof(cmd));

        if (_tcsicmp(cmd, _T("alerta")) == 0) {
            TCHAR msg[140];
            DWORD duracao, targetId;
            // Lê ignorando aspas simplificadas
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
                ReleaseMutex(estado.hMutex);
            }
            else {
                _tprintf_s(_T("Formato invalido. Exemplo: alerta \"Acidente\" 60 1\n"));
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
            for (int i = 0; i < MAX_PLACARES; i++) {
                if (estado.placares[i].ativo) {
                    WriteFile(estado.placares[i].hPipe, &encerrar, sizeof(MSG_CMD), &written, NULL);
                }
            }
            ReleaseMutex(estado.hMutex);
            estado.continua = FALSE;
        }
    }

    CloseHandle(estado.hMutex);
    return 0;
}