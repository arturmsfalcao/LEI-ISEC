#include <windows.h>
#include <tchar.h>
#include <io.h>
#include <stdio.h>
#include <fcntl.h>
#include <math.h>
#include <stdlib.h>
#include <time.h>

typedef struct {
	BYTE tipo;
	TCHAR msg[140];
	DWORD duracao;
} MSG_ALERTA;

typedef struct {
	TCHAR nomePipe[256];
	HANDLE hNotificar;
	HANDLE hTerminar;
	BOOL continua;
} TDados;


DWORD WINAPI threadAlertas(LPVOID dados) {
	TDados* ptd = (TDados*)dados;
	HKEY chave;
	LSTATUS res;
	DWORD tipo, tam;
	MSG_ALERTA alerta;
	SYSTEMTIME st;

	//criar o Waitable Timer (inicia desligado)
	HANDLE hTimer = CreateWaitableTimer(NULL, FALSE, NULL);

	//esperamos pelos 3 eventos em simultâneo
	HANDLE eventos[3] = { ptd->hTerminar, ptd->hNotificar, hTimer };

	while (ptd->continua) {
		//thread bloqueia até que qualquer um dos 3 eventos aconteça
		DWORD resWait = WaitForMultipleObjects(3, eventos, FALSE, INFINITE);

		if (resWait == WAIT_OBJECT_0) {
			//evento hTerminar foi ativado -> sair do loop
			break;
		}
		else if (resWait == WAIT_OBJECT_0 + 1) {
			//evento hNotificar foi ativado -> novo alerta
			res = RegOpenKeyEx(HKEY_CURRENT_USER, _T("Software\\TrabSO2"), 0, KEY_READ, &chave);
			if (res == ERROR_SUCCESS) {
				tam = sizeof(MSG_ALERTA);
				res = RegQueryValueEx(chave, ptd->nomePipe, NULL, &tipo, (LPBYTE)&alerta, &tam);

				if (res == ERROR_SUCCESS && tipo == REG_BINARY) {
					GetLocalTime(&st);
					_tprintf_s(_T("\n%02d/%02d/%04d (%02d:%02d:%02d): '%s'\nCMD: "),
						st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond, alerta.msg);

					//configurar o temporizador, mas se já houvesse um ativo, ele sobrepoe com este novo tempo
					LARGE_INTEGER liDueTime;
					liDueTime.QuadPart = -(LONGLONG)alerta.duracao * 10000000LL;
					SetWaitableTimer(hTimer, &liDueTime, 0, NULL, NULL, FALSE);
				}
				RegCloseKey(chave);
			}

			//repor o evento manual a FALSE para não ficar em loop infinito
			ResetEvent(ptd->hNotificar);
		}
		else if (resWait == WAIT_OBJECT_0 + 2) {
			//hTimer foi ativado -> o tempo do alerta atual acabou!
			GetLocalTime(&st);
			_tprintf_s(_T("\n%02d/%02d/%04d (%02d:%02d:%02d): '---'\nCMD: "),
				st.wDay, st.wMonth, st.wYear, st.wHour, st.wMinute, st.wSecond);
		}
	}

	CloseHandle(hTimer);
	ExitThread(0);
}


int _tmain(int argc, TCHAR * argv[]) {
	HKEY chave = NULL;
	LSTATUS res;
	DWORD estado, tam, tipo;
	TCHAR nomePipe[256] = _T("");
	TCHAR str[100];
	TDados tdados;

#ifdef UNICODE 
	_setmode(_fileno(stdin), _O_WTEXT);
	_setmode(_fileno(stdout), _O_WTEXT);
	_setmode(_fileno(stderr), _O_WTEXT);
#endif

	//id aleatorio
	srand((unsigned int)time(NULL));

	//abrir ou criar chave
	res = RegCreateKeyEx(HKEY_CURRENT_USER, _T("Software\\TrabSO2"), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_ALL_ACCESS, NULL, &chave, &estado);
	if (res != ERROR_SUCCESS) {
		_tprintf_s(_T("[ERRO] Nao foi possivel criar/abrir a chave no Registry.\n"));
		return 1;
	}
	//nome veio por argumento de linha de comandos -> guardar no Registry
	if (argc > 1) {
		_tcscpy_s(nomePipe, _countof(nomePipe), argv[1]);
		RegSetValueEx(chave, _T("NPIPE"), 0, REG_SZ, (LPBYTE)nomePipe, (DWORD)(_tcslen(nomePipe) + 1) * sizeof(TCHAR));
	}
	//nome nao veio por argumento -> tentar ler do Registry
	else {
		tam = sizeof(nomePipe);
		res = RegQueryValueEx(chave, _T("NPIPE"), NULL, &tipo, (LPBYTE)nomePipe, &tam);
		if (res != ERROR_SUCCESS) {
			_tprintf_s(_T("[ERRO] Named pipe nao encontrado no Registry.\n"));
			RegCloseKey(chave);
			return 1;
		}
	}
	//apresenta na consola o nome do named pipe a utilizar
	_tprintf(_T("NamedPipe = '%s'\n"), nomePipe);

	//fechar chave
	if (chave != NULL) {
		RegCloseKey(chave);
	}

	//preparar dados para a thread
	_tcscpy_s(tdados.nomePipe, _countof(tdados.nomePipe), nomePipe);
	tdados.continua = TRUE;
	tdados.hNotificar = CreateEvent(NULL, TRUE, FALSE, _T("notificar"));
	tdados.hTerminar = CreateEvent(NULL, TRUE, FALSE, NULL);
	HANDLE hThread = CreateThread(NULL, 0, threadAlertas, (LPVOID)&tdados, 0, NULL);

	//interacao com o utilizador
	do {
		_tprintf_s(_T("CMD: "));
		_tscanf_s(_T("%s"), str, _countof(str));
		if (_tcscmp(str, _T("liga")) == 0) {
			int idAleatorio = (rand() % 100) + 1;
			_tprintf_s(_T("Identificador = %d\n"), idAleatorio);
		}
	} while (_tcscmp(str, _T("desliga")) != 0);

	//terminar
	tdados.continua = FALSE;
	SetEvent(tdados.hTerminar);
	WaitForSingleObject(hThread, INFINITE);
	CloseHandle(hThread);
	CloseHandle(tdados.hNotificar);
	CloseHandle(tdados.hTerminar);

	return 0;
}