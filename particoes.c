
#include "funcionarios.h"
#include <stdlib.h>
#include <limits.h>

#include "particoes.h"

void classificacao_interna(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc) {
    rewind(arq); //posiciona cursor no inicio do arquivo


    int reg = 0;

    while (reg != nFunc) {
        //le o arquivo e coloca no vetor
        TFunc *v[M];
        int i = 0;
        while (!feof(arq)) {
            fseek(arq, (reg) * tamanho_registro(), SEEK_SET);
            v[i] = le_funcionario(arq);
       //     imprime_funcionario(v[i]);
            i++;
            reg++;
            if(i>=M) break;
        }

        //ajusta tamanho M caso arquivo de entrada tenha terminado antes do vetor
        if (i != M) {
            M = i;
        }

        //faz ordenacao
        for (int j = 1; j < M; j++) {
            TFunc *f = v[j];
            i = j - 1;
            while ((i >= 0) && (v[i]->cod > f->cod)) {
                v[i + 1] = v[i];
                i = i - 1;
            }
            v[i + 1] = f;
        }

        //cria arquivo de particao e faz gravacao
        char *nome_particao = nome_arquivos_saida->nome;
        nome_arquivos_saida = nome_arquivos_saida->prox;
        printf("\n%s\n", nome_particao);
        FILE *p;
        if ((p = fopen(nome_particao, "wb+")) == NULL) {
            printf("Erro criar arquivo de saida\n");
        } else {
            for (int i = 0; i < M; i++) {
                fseek(p, (i) * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[i], p);
                imprime_funcionario(v[i]);
            }
            fclose(p);
        }
        for(int jj = 0; jj<M; jj++)
            free(v[jj]);
    }
}

/* Retorna o indice do funcionario de menor codigo na memoria. */
static int indice_menor(TFunc **memoria, int capacidade) {
    int menor = -1;
    for (int i = 0; i < capacidade; i++) {
        if (memoria[i] != NULL &&
            (menor == -1 || memoria[i]->cod < memoria[menor]->cod)) {
            menor = i;
        }
    }
    return menor;
}

/* Le um registro no formato binario do projeto (com passo tamanho_registro()). */
static TFunc *le_proximo(FILE *arq, int *lidos, int limite) {
    if (*lidos >= limite) return NULL;
    if (fseek(arq, (long)(*lidos) * tamanho_registro(), SEEK_SET) != 0)
        return NULL;
    TFunc *f = le_funcionario(arq);
    if (f) (*lidos)++;
    else *lidos = limite;
    return f;
}

/* Escreve o registro na posicao correta e libera a memoria utilizada. */
static int escreve_e_libera(FILE *saida, TFunc **registro, int *gravados) {
    if (fseek(saida, (long)(*gravados) * tamanho_registro(), SEEK_SET) != 0) {
        return 0;
    }
    salva_funcionario(*registro, saida);
    if (ferror(saida)) return 0;
    (*gravados)++;
    free(*registro);
    *registro = NULL;
    return 1;
}

/*
 * Selecao natural com M registros ativos em memoria e reservatorio de n
 * registros. Quando a chave lida e inferior a ultima chave emitida, o
 * registro fica reservado para a proxima particao. Ao encher o reservatorio,
 * termina-se a particao atual emitindo os elementos ativos ainda ordenados.
 * Retorna o numero efetivo de particoes, ou -1 em caso de erro.
 */
int selecao_natural(FILE *arq, Lista *nome_arquivos_saida, int M, int nFunc, int n) {
    if (!arq || !nome_arquivos_saida || M <= 0 || n <= 0 || nFunc < 0) {
        fprintf(stderr, "Parametros invalidos para selecao natural.\n");
        return -1;
    }

    TFunc **memoria = calloc((size_t)M, sizeof(*memoria));
    TFunc **reservatorio = calloc((size_t)n, sizeof(*reservatorio));
    if (!memoria || !reservatorio) {
        free(memoria); free(reservatorio);
        fprintf(stderr, "Memoria insuficiente para selecao natural.\n");
        return -1;
    }

    int lidos = 0, ativos = 0, reservados = 0, particoes = 0;
    int erro = 0;
    rewind(arq);

    while (!erro && (ativos > 0 || reservados > 0 || lidos < nFunc)) {
        /* Abastece a memoria primeiro com os registros do reservatorio. */
        int retirados = (reservados < M) ? reservados : M;
        for (int i = 0; i < retirados; i++) {
            memoria[i] = reservatorio[i];
            reservatorio[i] = NULL;
            ativos++;
        }
        for (int i = retirados; i < reservados; i++) {
            reservatorio[i - retirados] = reservatorio[i];
            reservatorio[i] = NULL;
        }
        reservados -= retirados;
        for (int i = 0; i < M && ativos < M; i++) {
            if (!memoria[i]) {
                memoria[i] = le_proximo(arq, &lidos, nFunc);
                if (memoria[i]) ativos++;
                else break;
            }
        }
        if (!ativos) break;

        if (!nome_arquivos_saida) {
            fprintf(stderr, "Lista de nomes insuficiente para as particoes.\n");
            erro = 1;
            break;
        }
        FILE *saida = fopen(nome_arquivos_saida->nome, "wb");
        if (!saida) {
            perror("Erro ao criar arquivo de particao");
            erro = 1;
            break;
        }
        int gravados = 0, ultimo_codigo = INT_MIN;

        while (ativos > 0 && !erro) {
            int pos = indice_menor(memoria, M);
            if (pos < 0 || memoria[pos]->cod < ultimo_codigo) {
                erro = 1;
                break;
            }
            ultimo_codigo = memoria[pos]->cod;
            if (!escreve_e_libera(saida, &memoria[pos], &gravados)) {
                erro = 1;
                break;
            }
            ativos--;

            /* Procura reposicao valida; as menores vao ao reservatorio. */
            while (lidos < nFunc && reservados < n) {
                TFunc *proximo = le_proximo(arq, &lidos, nFunc);
                if (!proximo) break;
                if (proximo->cod >= ultimo_codigo) {
                    memoria[pos] = proximo;
                    ativos++;
                    break;
                }
                reservatorio[reservados++] = proximo;
            }

            if (reservados == n) {
                /* Reservatorio cheio: finaliza a particao sem consumir mais
                 * registros do arquivo ate comecar a proxima. */
                while (ativos > 0 && !erro) {
                    pos = indice_menor(memoria, M);
                    if (pos < 0 || memoria[pos]->cod < ultimo_codigo) {
                        erro = 1;
                        break;
                    }
                    ultimo_codigo = memoria[pos]->cod;
                    if (!escreve_e_libera(saida, &memoria[pos], &gravados)) {
                        erro = 1;
                        break;
                    }
                    ativos--;
                }
                break;
            }
        }

        if (fclose(saida) != 0) erro = 1;
        if (!erro) {
            particoes++;
            printf("Particao %d (%s): %d registros.\n",
                   particoes, nome_arquivos_saida->nome, gravados);
            nome_arquivos_saida = nome_arquivos_saida->prox;
        }
    }

    for (int i = 0; i < M; i++) free(memoria[i]);
    for (int i = 0; i < n; i++) free(reservatorio[i]);
    free(memoria);
    free(reservatorio);
    if (erro) return -1;
    printf("Selecao natural: %d particao(oes) gerada(s).\n", particoes);
    return particoes;
}
