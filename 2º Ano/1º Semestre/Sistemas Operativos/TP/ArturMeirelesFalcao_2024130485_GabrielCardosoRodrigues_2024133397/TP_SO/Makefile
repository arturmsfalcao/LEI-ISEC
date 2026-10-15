CC := gcc

WARNING_FLAGS := -Wall -Wextra -pedantic
DEBUG_FLAGS := -g -fsanitize=address

# Nomes dos executáveis do seu projeto
CONTROLADOR_OUTPUT_NAME := controlador
CLIENTE_OUTPUT_NAME := cliente
VEICULO_OUTPUT_NAME := veiculo

# Objetos do seu projeto
OBJECTS := controlador.o cliente.o veiculo.o

# General rules

default: all

all: $(CONTROLADOR_OUTPUT_NAME) $(CLIENTE_OUTPUT_NAME) $(VEICULO_OUTPUT_NAME)

controlador: controlador.o
	$(CC) controlador.o -o $(CONTROLADOR_OUTPUT_NAME) $(WARNING_FLAGS)

cliente: cliente.o
	$(CC) cliente.o -o $(CLIENTE_OUTPUT_NAME) $(WARNING_FLAGS)

veiculo: veiculo.o
	$(CC) veiculo.o -o $(VEICULO_OUTPUT_NAME) $(WARNING_FLAGS)

controlador.o: controlador.c util.h
	$(CC) controlador.c -c $(WARNING_FLAGS)

cliente.o: cliente.c util.h
	$(CC) cliente.c -c $(WARNING_FLAGS)

veiculo.o: veiculo.c util.h
	$(CC) veiculo.c -c $(WARNING_FLAGS)


# Debug rules (Opcional - para desenvolvimento)

all-debug: controlador-debug cliente-debug veiculo-debug

controlador-debug: controlador.c util.h
	$(CC) controlador.c -o $(CONTROLADOR_OUTPUT_NAME) $(WARNING_FLAGS) $(DEBUG_FLAGS)

cliente-debug: cliente.c util.h
	$(CC) cliente.c -o $(CLIENTE_OUTPUT_NAME) $(WARNING_FLAGS) $(DEBUG_FLAGS)

veiculo-debug: veiculo.c util.h
	$(CC) veiculo.c -o $(VEICULO_OUTPUT_NAME) $(WARNING_FLAGS) $(DEBUG_FLAGS)

# Clean rules

clean: clean-obj clean-exe

clean-obj:
	rm -f $(OBJECTS)

clean-exe:
	rm -f $(CONTROLADOR_OUTPUT_NAME) $(CLIENTE_OUTPUT_NAME) $(VEICULO_OUTPUT_NAME)

clean-pipes:
    # Limpa os FIFOs usados no projeto
	rm -f empresa_taxis cli_*
