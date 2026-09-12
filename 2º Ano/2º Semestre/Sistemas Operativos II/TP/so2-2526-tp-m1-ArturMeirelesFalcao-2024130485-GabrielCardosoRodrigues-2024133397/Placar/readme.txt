Trabalho Laboratorial - Sistemas Operativos 2 (2025/26)
Entrega: M1 - Programa Placar

Grupo
- Artur Meireles de Sousa Falcão (2024130485)
- Gabriel Cardoso Rodrigues (2024133397)


Requisitos Implementados
1. Comandos de Instalação: A aplicação processa o comando 'liga', gerando e apresentando um identificador aleatório, e o comando 'desliga' que termina a aplicação de forma totalmente limpa e controlada, garantindo o fecho de todas as handles (Threads, Eventos, Timers e Registry).

2. Gestão do Named Pipe via Registry: O programa aceita o nome do named pipe por argumento da linha de comandos e guarda-o no Registo do Windows (HKEY_CURRENT_USER\Software\TrabSO2 no valor NPIPE). Caso não seja passado argumento, lê corretamente o valor armazenado no Registo. Se a chave não existir, apresenta o respetivo erro.

3. Arquitetura Multithread: O programa utiliza concorrência com duas threads distintas: a thread principal (_tmain) dedicada à leitura de comandos do teclado, e uma thread secundária (threadAlertas) dedicada exclusivamente à escuta de eventos, leitura de dados e temporização.

4. Receção e Apresentação de Alertas: A thread de alertas fica bloqueada à espera do evento de manual-reset com o nome 'notificar'. Assim que assinalado, lê a estrutura binária MSG_ALERTA do Registo e imprime no ecrã a mensagem e a hora exata do sistema. O evento é reposto imediatamente (ResetEvent).

5. Temporização (Waitable Timers): A duração do alerta é gerida através de um Waitable Timer. A thread não fica bloqueada passivamente à espera do fim do temporizador. Em vez disso, utiliza um WaitForMultipleObjects para aguardar simultaneamente pelo fim do timer, por uma ordem de encerramento, ou por novos alertas. Se um novo alerta chegar antes de o tempo do alerta anterior terminar, o programa cancela o timer em curso, assume o novo alerta e reinicia a temporização sem encravar (passando com sucesso no teste de sobreposição da validação funcional).