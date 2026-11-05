#include "intercalacao.h"
#include "funcionarios.h"
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

void intercalacao_basico(char *nome_arquivo_saida, int num_p, Lista *nome_particoes) {

    int fim = 0; //variavel que controla fim do procedimento
    FILE *out; //declara ponteiro para arquivo

    //abre arquivo de saida para escrita
    if ((out = fopen(nome_arquivo_saida, "wb")) == NULL) {
        printf("Erro ao abrir arquivo de sa?da\n");
    } else {
        //cria vetor de particoes
        TVet v[num_p];

        //abre arquivos das particoes, colocando variavel de arquivo no campo f do vetor
        //e primeiro funcionario do arquivo no campo func do vetor
        for (int i=0; i < num_p; i++) {
            v[i].f = fopen(nome_particoes->nome, "rb");
            v[i].aux_p = 0;
            if (v[i].f != NULL) {
                fseek(v[i].f, v[i].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le_funcionario(v[i].f);
                if (f == NULL) {
                    //arquivo estava vazio
                    //coloca HIGH VALUE nessa posi??o do vetor
                    v[i].func = funcionario(INT_MAX, "","","",0);
                }
                else {
                    //conseguiu ler funcionario, coloca na posi??o atual do vetor
                    v[i].func = f;
                }
            }
            else {
                fim = 1;
            }
            nome_particoes = nome_particoes->prox;
        }

        int aux = 0;
        while (!(fim)) { //conseguiu abrir todos os arquivos
            int menor = INT_MAX;
            int pos_menor;
            //encontra o funcionario com menor chave no vetor
            for(int i = 0; i < num_p; i++){
                if(v[i].func->cod < menor){
                    menor = v[i].func->cod;
                    pos_menor = i;
                }
            }
            if (menor == INT_MAX) {
                fim = 1; //terminou processamento
            }
            else {
                //salva funcionario no arquivo de saída
                fseek(out, aux * tamanho_registro(), SEEK_SET);
                salva_funcionario(v[pos_menor].func, out);
                //atualiza posição pos_menor do vetor com pr?ximo funcionario do arquivo
                v[pos_menor].aux_p++;
                fseek(v[pos_menor].f, v[pos_menor].aux_p * tamanho_registro(), SEEK_SET);
                TFunc *f = le_funcionario(v[pos_menor].f);
                aux++;
                if (f == NULL) {
                    //arquivo estava vazio
                    //coloca HIGH VALUE nessa posiçao do vetor
                    v[pos_menor].func = funcionario(INT_MAX, "", "", "",0.0);
                }
                else {
                    v[pos_menor].func = f;
                }

            }
        }

        //fecha arquivos das partiÇões de entrada
        for(int i = 0; i < num_p; i++){
            fclose(v[i].f);
        //    free(v[i].func);
        }
        //fecha arquivo de saída
        fclose(out);
    }
}

/* Escolhe o vencedor entre duas particoes da arvore (menor codigo). */
static int vencedor(int a, int b, TPilha **pilhas, int *topos) {
    if (a < 0) return b;
    if (b < 0) return a;
    TFunc *fa = peek_func(pilhas[a], 0, &topos[a]);
    TFunc *fb = peek_func(pilhas[b], 0, &topos[b]);
    if (!fa) return b;
    if (!fb) return a;
    if (fa->cod < fb->cod) return a;
    if (fb->cod < fa->cod) return b;
    return (a < b) ? a : b; /* desempate deterministico */
}

/*
 * Arvore binaria de vencedores: folhas representam as particoes e cada
 * no interno guarda o indice da particao vencedora (menor chave).
 * Apos emitir um registro, somente o caminho da folha ate a raiz e refeito.
 * Consome as pilhas; recrie-as (opcao 7) para executar a fusao novamente.
 */
void intercalacao_arvore_de_vencedores(TPilha **pilhas, int *topos,
                                      char *nome_arquivo_saida, int num_p) {
    if (!pilhas || !topos || !nome_arquivo_saida || num_p <= 0) {
        fprintf(stderr, "Particoes/pilhas invalidas para intercalacao.\n");
        return;
    }

    int base = 1;
    while (base < num_p) base *= 2;
    int *arvore = malloc((size_t)(2 * base) * sizeof(*arvore));
    if (!arvore) {
        fprintf(stderr, "Falha ao alocar arvore de vencedores.\n");
        return;
    }

    for (int i = 0; i < base; i++) {
        arvore[base + i] = (i < num_p && topos[i] >= 0) ? i : -1;
    }
    for (int i = base - 1; i >= 1; i--) {
        arvore[i] = vencedor(arvore[2*i], arvore[2*i+1], pilhas, topos);
    }

    FILE *saida = fopen(nome_arquivo_saida, "wb");
    if (!saida) {
        perror("Nao foi possivel criar arquivo de intercalacao");
        free(arvore);
        return;
    }
    int gravados = 0, erro = 0;
    while (arvore[1] >= 0) {
        int id = arvore[1];
        TFunc *f = pop(pilhas[id], 0, &topos[id]);
        if (!f) { erro = 1; break; }

        if (fseek(saida, (long)gravados * tamanho_registro(), SEEK_SET) != 0) {
            free(f); erro = 1; break;
        }
        salva_funcionario(f, saida);
        free(f);
        if (ferror(saida)) { erro = 1; break; }
        gravados++;

        int pos = base + id;
        arvore[pos] = (topos[id] >= 0) ? id : -1;
        for (pos /= 2; pos >= 1; pos /= 2) {
            arvore[pos] = vencedor(arvore[2*pos], arvore[2*pos+1], pilhas, topos);
        }
    }
    if (fclose(saida) != 0) erro = 1;
    free(arvore);
    if (erro) fprintf(stderr, "Falha durante intercalacao de particoes.\n");
    else printf("Arvore de vencedores: %d registros intercalados em %s.\n",
                gravados, nome_arquivo_saida);
}

