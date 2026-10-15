#include <windows.h>
#include <tchar.h>
#include <windowsx.h>
#include <stdio.h>
#include "..\central\util.h"
#include "resource.h"

// Estrutura principal da aplicacao
typedef struct {
    // Configuracoes da DialogBox
    DWORD maxAlertasPorPagina;
    TCHAR nomeShm[256];
    TCHAR nomeMutex[256];
    TCHAR nomeEvento[256];
    // Recursos do Sistema Operativo
    HANDLE hMapFile;
    SHM_ALERTA* pShm;
    HANDLE hMutexSHM;
    HANDLE hEventoSHM;
    HANDLE hThreadMonitor;
    // Controlo de estado local
    HWND hWnd;
    BOOL continua;
    DWORD paginaAtual;
    // Cópia local protegida para o WM_PAINT ler sem prender a SHM global
    SHM_ALERTA copiaLocalShm;
} DATA_MONITOR;

LRESULT CALLBACK trataEventos(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK trataDialog(HWND, UINT, WPARAM, LPARAM);

// Funcao de apoio para ligar/religar aos recursos criados pela Central
BOOL LigarRecursos(DATA_MONITOR* ptd) {
    // Se ja existirem recursos abertos anteriormente, fechamos primeiro
    if (ptd->pShm) UnmapViewOfFile(ptd->pShm);
    if (ptd->hMapFile) CloseHandle(ptd->hMapFile);
    if (ptd->hMutexSHM) CloseHandle(ptd->hMutexSHM);
    if (ptd->hEventoSHM) CloseHandle(ptd->hEventoSHM);

    ptd->hMapFile = OpenFileMapping(FILE_MAP_ALL_ACCESS, FALSE, ptd->nomeShm);
    if (ptd->hMapFile == NULL) return FALSE;

    ptd->pShm = (SHM_ALERTA*)MapViewOfFile(ptd->hMapFile, FILE_MAP_ALL_ACCESS, 0, 0, 0);
    if (ptd->pShm == NULL) {
        CloseHandle(ptd->hMapFile);
        ptd->hMapFile = NULL;
        return FALSE;
    }

    ptd->hMutexSHM = OpenMutex(MUTEX_ALL_ACCESS, FALSE, ptd->nomeMutex);
    ptd->hEventoSHM = OpenEvent(EVENT_ALL_ACCESS, FALSE, ptd->nomeEvento);

    if (!ptd->hMutexSHM || !ptd->hEventoSHM) {
        if (ptd->pShm) { UnmapViewOfFile(ptd->pShm); ptd->pShm = NULL; }
        if (ptd->hMapFile) { CloseHandle(ptd->hMapFile); ptd->hMapFile = NULL; }
        if (ptd->hMutexSHM) { CloseHandle(ptd->hMutexSHM); ptd->hMutexSHM = NULL; }
        if (ptd->hEventoSHM) { CloseHandle(ptd->hEventoSHM); ptd->hEventoSHM = NULL; }
        return FALSE;
    }
    // Fazer uma copia de seguranca inicial para exibicao estatica imediata
    WaitForSingleObject(ptd->hMutexSHM, INFINITE);
    CopyMemory(&ptd->copiaLocalShm, ptd->pShm, sizeof(SHM_ALERTA));
    ReleaseMutex(ptd->hMutexSHM);

    return TRUE;
}

// ============================================================================
// FUNCAO DE INICIO DO PROGRAMA - _tWinMain()
// ============================================================================
int WINAPI _tWinMain(HINSTANCE hInst, HINSTANCE hPrevInst, LPTSTR lpCmdLine, int nCmdShow) {
    HWND hWnd;
    MSG lpMsg;
    WNDCLASSEX wcApp;
    DATA_MONITOR d;

    // Inicializar os parâmetros por defeito da aplicação
    d.maxAlertasPorPagina = 5;
    _tcscpy_s(d.nomeShm, _countof(d.nomeShm), NOME_SHM);
    _tcscpy_s(d.nomeMutex, _countof(d.nomeMutex), NOME_MUTEX_SHM);
    _tcscpy_s(d.nomeEvento, _countof(d.nomeEvento), NOME_EVENTO_SHM);
    d.hMapFile = NULL;
    d.pShm = NULL;
    d.hMutexSHM = NULL;
    d.hEventoSHM = NULL;
    d.hThreadMonitor = NULL;
    d.paginaAtual = 0;
    d.continua = TRUE;
    ZeroMemory(&d.copiaLocalShm, sizeof(SHM_ALERTA));

    wcApp.cbSize = sizeof(WNDCLASSEX);
    wcApp.hInstance = hInst;
    wcApp.lpszClassName = _T("ClasseMonitorSO2");
    wcApp.lpfnWndProc = trataEventos;
    wcApp.style = CS_HREDRAW | CS_VREDRAW;
    wcApp.hIcon = LoadIcon(NULL, IDI_APPLICATION);
    wcApp.hIconSm = LoadIcon(NULL, IDI_INFORMATION);
    wcApp.hCursor = LoadCursor(NULL, IDC_ARROW);
    wcApp.lpszMenuName = MAKEINTRESOURCE(IDR_MENU1); // Associa a barra de menu
    wcApp.cbClsExtra = 0;
    wcApp.cbWndExtra = sizeof(DATA_MONITOR*); // Reserva espaço para o ponteiro de d
    wcApp.hbrBackground = (HBRUSH)CreateSolidBrush(RGB(30, 30, 30)); // Fundo escuro

    if (!RegisterClassEx(&wcApp))
        return(0);

    hWnd = CreateWindow(
        _T("ClasseMonitorSO2"),
        TEXT("Painel Monitor de Alertas Ativos - SO2"),
        WS_OVERLAPPEDWINDOW,
        300, 150, 650, 420,
        (HWND)HWND_DESKTOP,
        (HMENU)NULL,
        (HINSTANCE)hInst,
        (LPVOID)&d); // Passa o endereço de d para o WM_CREATE

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    while (GetMessage(&lpMsg, NULL, 0, 0) > 0) {
        TranslateMessage(&lpMsg);
        DispatchMessage(&lpMsg);
    }

    return (int)lpMsg.wParam;
}

// ============================================================================
// THREAD SECUNDÁRIA: Escuta as atualizações da SHM
// ============================================================================
DWORD WINAPI threadMonitorSHM(LPVOID dados) {
    DATA_MONITOR* ptd = (DATA_MONITOR*)dados;

    while (ptd->continua) {
        // Se a central não foi detetada (Handle é NULL), a thread dorme 1 segundo e volta a perguntar, sem bloquear o CPU.
        if (ptd->hEventoSHM == NULL) {
            Sleep(1000);
            continue;
        }
        // Fica à espera que a Central dispare o Evento.
        DWORD resWait = WaitForSingleObject(ptd->hEventoSHM, 1000);
        // Se o evento foi ativado pela Central (WAIT_OBJECT_0)
        if (resWait == WAIT_OBJECT_0) {
            if (!ptd->continua) break;
            // Copiar os dados da SHM protegendo o acesso com o Mutex global
            if (ptd->hMutexSHM != NULL) {
                WaitForSingleObject(ptd->hMutexSHM, INFINITE);
                CopyMemory(&ptd->copiaLocalShm, ptd->pShm, sizeof(SHM_ALERTA));
                ReleaseMutex(ptd->hMutexSHM);
            }
            // Se a plataforma foi encerrada pela Central, o monitor fecha-se automaticamente
            if (ptd->copiaLocalShm.desligar) {
                PostMessage(ptd->hWnd, WM_CLOSE, 0, 0);
                break;
            }
            // Forçar o redesenho imediato do ecrã
            InvalidateRect(ptd->hWnd, NULL, TRUE);
        }
    }

    ExitThread(0);
}

// ============================================================================
// FUNÇÃO DE PROCESSAMENTO DA DIALOGBOX DE CONFIGURAÇÃO
// ============================================================================
INT_PTR CALLBACK trataDialog(HWND hDlg, UINT messg, WPARAM wParam, LPARAM lParam) {
    static DATA_MONITOR* ptd = NULL;

    switch (messg) {
    case WM_INITDIALOG:
        ptd = (DATA_MONITOR*)lParam; // Recupera o pd enviado via DialogBoxParam
        // Preencher os campos editáveis com a configuração atual
        SetDlgItemInt(hDlg, IDC_EDIT_MAX, ptd->maxAlertasPorPagina, FALSE);
        SetDlgItemText(hDlg, IDC_EDIT_SHM, ptd->nomeShm);
        SetDlgItemText(hDlg, IDC_EDIT_MUTEX, ptd->nomeMutex);
        SetDlgItemText(hDlg, IDC_EDIT_EVENTO, ptd->nomeEvento);
        return TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK) {
            BOOL valido;
            UINT maxVal = GetDlgItemInt(hDlg, IDC_EDIT_MAX, &valido, FALSE);
            if (valido && maxVal > 0) {
                ptd->maxAlertasPorPagina = maxVal;
            }
            GetDlgItemText(hDlg, IDC_EDIT_SHM, ptd->nomeShm, 256);
            GetDlgItemText(hDlg, IDC_EDIT_MUTEX, ptd->nomeMutex, 256);
            GetDlgItemText(hDlg, IDC_EDIT_EVENTO, ptd->nomeEvento, 256);
            // Tenta efetuar a ligação aos novos recursos introduzidos
            if (!LigarRecursos(ptd)) {
                MessageBox(hDlg, _T("[ERRO] Nao foi possivel mapear os recursos especificados!"), _T("Erro de Conexao"), MB_OK | MB_ICONERROR);
            }

            EndDialog(hDlg, IDOK);
            return TRUE;
        }
        if (LOWORD(wParam) == IDCANCEL) {
            EndDialog(hDlg, IDCANCEL);
            return TRUE;
        }
        break;
    }
    return FALSE;
}

// ============================================================================
// FUNÇÃO DE PROCESSAMENTO DOS EVENTOS DA JANELA PRINCIPAL
// ============================================================================
LRESULT CALLBACK trataEventos(HWND hWnd, UINT messg, WPARAM wParam, LPARAM lParam) {
    DATA_MONITOR* ptd = NULL;
    HDC hdc;
    PAINTSTRUCT ps;
    TCHAR strAux[512];

    switch (messg) {
    case WM_CREATE:
        // Recuperar o endereço de d passado no CreateWindow
        ptd = (DATA_MONITOR*)((LPCREATESTRUCT)lParam)->lpCreateParams;
        ptd->hWnd = hWnd;
        SetWindowLongPtr(hWnd, 0, (LONG_PTR)ptd);
        // Tentar estabelecer a ligação inicial
        if (!LigarRecursos(ptd)) {
            MessageBox(hWnd, _T("[AVISO] Central nao detetada."), _T("Aviso"), MB_OK | MB_ICONWARNING);
        }
        // Lançar a thread consumidora
        ptd->hThreadMonitor = CreateThread(NULL, 0, threadMonitorSHM, (LPVOID)ptd, 0, NULL);
        break;

    case WM_PAINT:
        ptd = (DATA_MONITOR*)GetWindowLongPtr(hWnd, 0);
        hdc = BeginPaint(hWnd, &ps);
        // Configuração estática do texto
        SetTextColor(hdc, RGB(240, 240, 240));
        SetBkMode(hdc, TRANSPARENT);
        // Cabeçalho estruturado
        _stprintf_s(strAux, _countof(strAux), _T("%-15s   %-35s   %-15s"), _T("ID PLACAR"), _T("MENSAGEM DE ALERTA"), _T("DURACAO"));
        TextOut(hdc, 30, 20, strAux, (int)_tcslen(strAux));

        MoveToEx(hdc, 30, 42, NULL);
        LineTo(hdc, 580, 42);
        // Contabilizar alertas ativos na cópia local
        int totalAtivos = 0;
        for (int i = 0; i < 20; i++) {
            if (ptd->copiaLocalShm.placar[i].identificador != 0) totalAtivos++;
        }
        // Cálculo das páginas (Requisito Teclas PageUp/PageDown)
        DWORD inicio = ptd->paginaAtual * ptd->maxAlertasPorPagina;
        DWORD fim = inicio + ptd->maxAlertasPorPagina;
        int linhaY = 60;
        int indexAtivo = 0;

        for (int i = 0; i < 20; i++) {
            if (ptd->copiaLocalShm.placar[i].identificador != 0) {
                if (indexAtivo >= (int)inicio && indexAtivo < (int)fim) {
                    // Formatar texto com alinhamento tabular fixo
                    _stprintf_s(strAux, _countof(strAux), _T("Placar %-8d   '%-33s'   %-12d segundos"),
                        ptd->copiaLocalShm.placar[i].identificador,
                        ptd->copiaLocalShm.placar[i].msg,
                        ptd->copiaLocalShm.placar[i].duracao);

                    TextOut(hdc, 30, linhaY, strAux, (int)_tcslen(strAux));
                    linhaY += 30;
                }
                indexAtivo++;
            }
        }
        // Barra de rodapé com a paginação
        int totalPaginas = (totalAtivos + ptd->maxAlertasPorPagina - 1) / ptd->maxAlertasPorPagina;
        if (totalPaginas == 0) totalPaginas = 1;
        // Corrigir estouro de página caso os placares reduzam dinamicamente
        if (ptd->paginaAtual >= (DWORD)totalPaginas) ptd->paginaAtual = totalPaginas - 1;

        _stprintf_s(strAux, _countof(strAux), _T("Pagina %d de %d  |  Total de Alertas Ativos: %d"), ptd->paginaAtual + 1, totalPaginas, totalAtivos);
        SetTextColor(hdc, RGB(255, 165, 0)); // Texto a Laranja
        TextOut(hdc, 30, 320, strAux, (int)_tcslen(strAux));

        EndPaint(hWnd, &ps);
        break;

    case WM_KEYDOWN:
        ptd = (DATA_MONITOR*)GetWindowLongPtr(hWnd, 0);

        int ativos = 0;
        for (int i = 0; i < 20; i++) {
            if (ptd->copiaLocalShm.placar[i].identificador != 0) ativos++;
        }
        int paginas = (ativos + ptd->maxAlertasPorPagina - 1) / ptd->maxAlertasPorPagina;
        if (paginas == 0) paginas = 1;

        if (wParam == VK_NEXT) { // Tecla Page Down
            if (ptd->paginaAtual + 1 < (DWORD)paginas) {
                ptd->paginaAtual++;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        else if (wParam == VK_PRIOR) { // Tecla Page Up
            if (ptd->paginaAtual > 0) {
                ptd->paginaAtual--;
                InvalidateRect(hWnd, NULL, TRUE);
            }
        }
        break;

    case WM_COMMAND:
        ptd = (DATA_MONITOR*)GetWindowLongPtr(hWnd, 0);

        if (LOWORD(wParam) == ID_FICHEIRO_CONFIG) {
            // Passagem estruturada de ptd via DialogBoxParam
            DialogBoxParam(GetModuleHandle(NULL), MAKEINTRESOURCE(IDD_DIALOG1), hWnd, trataDialog, (LPARAM)ptd);
            InvalidateRect(hWnd, NULL, TRUE);
        }
        else if (LOWORD(wParam) == ID_FICHEIRO_ACERCA) {
            MessageBox(hWnd, _T("Plataforma desenvolvida por:\nArtur Meireles de Sousa Falcao - 2024130485\nGabriel Cardoso Rodrigues - 2024133397"), _T("Acerca dos Autores"), MB_OK | MB_ICONINFORMATION);
        }
        else if (LOWORD(wParam) == ID_FICHEIRO_SAIR) {
            DestroyWindow(hWnd);
        }
        break;

    case WM_CLOSE:
        if (MessageBox(hWnd, _T("Deseja fechar o Monitor?"), _T("Sair"), MB_OKCANCEL | MB_ICONQUESTION) == IDOK) {
            DestroyWindow(hWnd);
        }
        break;

    case WM_DESTROY:
        ptd = (DATA_MONITOR*)GetWindowLongPtr(hWnd, 0);
        if (ptd != NULL) {
            ptd->continua = FALSE;
            // Forçar o despertar da thread caso esteja presa a dormir no WaitForSingleObject
            if (ptd->hEventoSHM) SetEvent(ptd->hEventoSHM);

            if (ptd->hThreadMonitor) {
                WaitForSingleObject(ptd->hThreadMonitor, INFINITE);
                CloseHandle(ptd->hThreadMonitor);
            }
            // Libertar e encerrar os mapeamentos locais
            if (ptd->pShm) UnmapViewOfFile(ptd->pShm);
            if (ptd->hMapFile) CloseHandle(ptd->hMapFile);
            if (ptd->hMutexSHM) CloseHandle(ptd->hMutexSHM);
            if (ptd->hEventoSHM) CloseHandle(ptd->hEventoSHM);
        }
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, messg, wParam, lParam);
    }
    return 0;
}