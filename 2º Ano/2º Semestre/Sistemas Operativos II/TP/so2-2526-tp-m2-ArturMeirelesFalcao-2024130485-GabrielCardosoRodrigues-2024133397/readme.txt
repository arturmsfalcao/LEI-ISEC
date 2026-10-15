SISTEMAS OPERATIVOS 2 (2025/26)

META 2

ELEMENTOS DO GRUPO:
- Artur Falcão (2024130485)
- Gabriel Rodrigues (2024133397) 

REQUISITOS IMPLEMENTADOS:

PROGRAMA CENTRAL:
1. Receção do nome do named pipe através dos argumentos da linha de comandos.
2. Interface de comandos do administrador operacional:
   - "alerta <msg> <duracao> <id>": Envia novo alerta e recebe confirmação. Suporta mensagens simples e compostas.
   - "cancelar <id>": Cancela o alerta ativo num placar específico e recebe confirmação.
   - "listar": Apresenta de forma coerente a lista de placares ligados, os seus identificadores e alertas ativos.
   - "encerrar": Envia a notificação de término a todos os placares e encerra o sistema em segurança.
3. Comunicação concorrente com múltiplos processos 'placar' através de named pipes (comunicação bidirecional assíncrona em modo message via Overlapped I/O).
4. Processamento automático de pedidos dos placares (comando "ligar", comando "desligar" e aviso de "fim do alerta").
5. Gestão de estado global coerente e thread-safe (utilização de Mutex) para garantir a consistência dos dados do sistema.

PROGRAMA PLACAR:
1. Leitura do nome do named pipe via linha de comandos, com fallback para leitura/escrita no Registry (par nome-valor 'NPIPE' em HKEY_CURRENT_USER\Software\TrabSO2).
2. Interface bidirecional assíncrona com utilização de múltiplas threads para gerir comandos do instalador e comunicações da central simultaneamente.
3. Interação com a central através de comandos do instalador:
   - "ligar": Envia pedido e recebe da central o Identificador único gerado.
   - "desligar": Informa a central da desconexão, aguarda confirmação e encerra a aplicação de forma limpa.
4. Gestão e apresentação de Alertas:
   - Recebe alertas da central, envia confirmação de receção e apresenta a mensagem com data e hora.
   - Controla a duração do alerta através de um Waitable Timer. 
   - Ao terminar o tempo do timer, limpa o ecrã ("---") e avisa a central.
5. Reação a Comandos Externos: Cancela imediatamente alertas em curso quando recebe o comando "cancelar" da central, e termina a sua execução ao receber o comando "encerrar".

ESTRUTURAS DE DADOS:
- Implementação num ficheiro de cabeçalho comum (util.h) da estrutura de dados exata especificada no enunciado para otimizar as comunicações (MSG_CMD, MSG_ALERTA, MSG_ID).