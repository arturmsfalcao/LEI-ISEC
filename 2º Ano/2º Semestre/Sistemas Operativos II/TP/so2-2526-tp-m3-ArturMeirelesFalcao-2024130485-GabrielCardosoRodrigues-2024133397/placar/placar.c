#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include "..\central\util.h"

typedef struct {
    HANDLE hPipe;
    BOOL continua;
} TDados;

// Thread secundária que escuta o Pipe e o temporizador simultaneamente
DWORD WINAPI threadComunicacao(LPVOID dados) {
    TDados* ptd = (TDados*)dados;
    MSG_GENERICA msgRx;
    DWORD bytesRead;
    BOOL res;
    SYSTEMTIME st;

    HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);
    HANDLE hEventPipe = CreateEvent(NULL, TRUE, FALSE, NULL);
    OVERLAPPED ov;

    HANDLE eventos[2] = { hEventPipe, hTimer };

    // Iniciar primeira leitura Overlapped
    ZeroMemory(&ov, sizeof(ov));
    ov.hEvent = hEventPipe;
    ReadFile(ptd->hPipe, &msgRx, sizeof(MSG_GENERICA), &bytesRead, &ov);

    while (ptd->continua) {
        DWORD resWait = WaitForMultipleObjects(2, eventos, FALSE, INFINITE);

        if (resWait == WAIT_OBJECT_0) {
            // Evento 0: Recebemos dados no Named Pipe
            GetOverlappedResult(ptd->hPipe, &ov, &bytesRead, FALSE);

            if (bytesRead > 0) {
                switch (msgRx.tipo) {
                case 7: { // MSG_ID (Recebe ID)
                    _tprintf_s(_T("\nIdentificador = %d\nCMD> "), msgRx.id.identificador);
                    break;
                }
                case 2: { // MSG_CMD desligar (Confirmacao do Central)
                    _tprintf_s(_T("\n[AVISO] Desligado com sucesso. A terminar...\n"));
                    ExitProcess(0); // MATA O PROGRAMA IMEDIATAMENTE
                    break;
                }
                case 4: { // MSG_ALERTA (Novo Alerta)
                    GetLocalTime(&st);
                    _tprintf_s(_T("\n%02d/%02d/%04d (%02d:%02d:%02d): '%s'\nCMD> "),
                        st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, msgRx.alerta.msg);

                    // Configurar o Timer
                    LARGE_INTEGER liDueTime;
                    liDueTime.QuadPart = -(LONGLONG)msgRx.alerta.duracao * 10000000LL;
                    SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, FALSE);

                    // Enviar confirmacao
                    DWORD bytesWritten;
                    WriteFile(ptd->hPipe, &msgRx, sizeof(MSG_ALERTA), &bytesWritten, NULL);
                    break;
                }
                case 5: { // MSG_CMD cancelar
                    CancelWaitableTimer(hTimer);
                    GetLocalTime(&st);
                    _tprintf_s(_T("\n%02d/%02d/%04d (%02d:%02d:%02d): '---'\nCMD> "),
                        st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);

                    // Enviar confirmacao
                    DWORD bytesWritten;
                    WriteFile(ptd->hPipe, &msgRx, sizeof(MSG_CMD), &bytesWritten, NULL);
                    break;
                }
                case 6: { // MSG_CMD encerrar
                    _tprintf_s(_T("\n[AVISO] A central encerrou a plataforma. A terminar...\n"));
                    ExitProcess(0); // MATA O PROGRAMA IMEDIATAMENTE
                    break;
                }
                }
            }

            // Repor leitura assincrona
            if (ptd->continua) {
                ResetEvent(hEventPipe);
                ZeroMemory(&ov, sizeof(ov));
                ov.hEvent = hEventPipe;
                ReadFile(ptd->hPipe, &msgRx, sizeof(MSG_GENERICA), &bytesRead, &ov);
            }
        }
        else if (resWait == WAIT_OBJECT_0 + 1) {
            // Evento 1: Timer esgotou (Fim do alerta)
            GetLocalTime(&st);
            _tprintf_s(_T("\n%02d/%02d/%04d (%02d:%02d:%02d): '---'\nCMD> "),
                st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);

            // Avisar Central
            MSG_CMD msgFim = { 3 };
            DWORD bytesWritten;
            WriteFile(ptd->hPipe, &msgFim, sizeof(MSG_CMD), &bytesWritten, NULL);
        }
    }

    CloseHandle(hTimer);
    CloseHandle(hEventPipe);
    ExitThread(0);
}

int _tmain(int argc, TCHAR* argv[]) {
    HKEY chave = NULL;
    LSTATUS res;
    DWORD estado, tam, tipo;
    TCHAR nomePipeReg[256] = _T("");
    TCHAR nomePipeCompleto[256] = _T("");
    TCHAR str[100];
    HANDLE hPipe;

#ifdef UNICODE 
    _setmode(_fileno(stdin), _O_WTEXT);
    _setmode(_fileno(stdout), _O_WTEXT);
#endif

    // Abrir/Criar Registry
    res = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\TrabSO2"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &estado);

    if (argc > 1) {
        _tcscpy_s(nomePipeReg, _countof(nomePipeReg), argv[1]);
        RegSetValueEx(chave, _T("NPIPE"), 0, REG_SZ, (LPBYTE)nomePipeReg, (DWORD)(_tcslen(nomePipeReg) + 1) * sizeof(TCHAR));
    }
    else {
        tam = sizeof(nomePipeReg);
        res = RegQueryValueEx(chave, _T("NPIPE"), NULL, &tipo, (LPBYTE)nomePipeReg, &tam);
        if (res != ERROR_SUCCESS) {
            _tprintf_s(_T("[ERRO] Named pipe nao fornecido e ausente no Registry.\n"));
            RegCloseKey(chave);
            return 1;
        }
    }
    RegCloseKey(chave);

    _tprintf(_T("A utilizar NamedPipe = '%s'\n"), nomePipeReg);
    _stprintf_s(nomePipeCompleto, _countof(nomePipeCompleto), _T("\\\\.\\pipe\\%s"), nomePipeReg);

    // Conectar ao pipe
    if (!WaitNamedPipe(nomePipeCompleto, NMPWAIT_WAIT_FOREVER)) {
        _tprintf(_T("[ERRO] Central nao encontrada (WaitNamedPipe).\n"));
        return 1;
    }

    hPipe = CreateFile(nomePipeCompleto, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, FILE_FLAG_OVERLAPPED, NULL);
    if (hPipe == INVALID_HANDLE_VALUE) {
        _tprintf_s(_T("[ERRO] Falha ao conectar ao Central (CreateFile).\n"));
        return 1;
    }

    TDados tdados;
    tdados.hPipe = hPipe;
    tdados.continua = TRUE;
    HANDLE hThread = CreateThread(NULL, 0, threadComunicacao, (LPVOID)&tdados, 0, NULL);

    // Input Consola do Placar
    do {
        _tprintf_s(_T("CMD> "));
        _tscanf_s(_T("%s"), str, _countof(str));

        if (_tcscmp(str, _T("ligar")) == 0) {
            MSG_CMD msgLigar = { 1 };
            DWORD bw;
            WriteFile(hPipe, &msgLigar, sizeof(MSG_CMD), &bw, NULL);
        }
        else if (_tcscmp(str, _T("desligar")) == 0) {
            MSG_CMD msgDesligar = { 2 };
            DWORD bw;
            WriteFile(hPipe, &msgDesligar, sizeof(MSG_CMD), &bw, NULL);
            // O loop vai terminar porque a thread irá receber a confirmação e colocar tdados.continua a FALSE
        }
    } while (tdados.continua && _tcscmp(str, _T("desligar")) != 0);

    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hPipe);

    return 0;
}