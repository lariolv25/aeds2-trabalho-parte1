CC = gcc
CFLAGS = -std=c11 -Wall -Wextra -O2
SRCS = arvore_binaria.c funcionario.c intercalacao.c lista.c main.c manipulacao_arquivos.c ordenacao_busca.c particoes.c pilha.c
LIBSRCS = arvore_binaria.c funcionario.c intercalacao.c lista.c manipulacao_arquivos.c ordenacao_busca.c particoes.c pilha.c

.PHONY: all test clean
all: aedsii

aedsii: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $@

testar_algoritmos: testes/testar_algoritmos.c $(LIBSRCS)
	$(CC) $(CFLAGS) -I. testes/testar_algoritmos.c $(LIBSRCS) -o $@

test: testar_algoritmos
	@mkdir -p build_testes
	@cd build_testes && ../testar_algoritmos

clean:
	rm -f aedsii testar_algoritmos
	rm -rf build_testes
