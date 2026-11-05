/*
    Prof. Janniele Aparecida Soares Araújo
    Arcabouço para desenvolvimento do trabalho da disciplina de AEDSII 2020-1, utilizando os exemplos dos livros
    de Schildt, H. C Completo e Total. Ed. McGraw-Hill e de Ferraz, I. N. Programação com Arquivos. Editora Manole Ltda
      sobre o problema de funcionários e algoritmos.
*/

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "ordenacao_busca.h"
#include "lista.h"
#include "particoes.h"
#include "intercalacao.h"
#include "arvore_binaria.h"

/* Quantidade efetiva de registros em particao de tamanho variavel. */
static int conta_registros(FILE *arquivo) {
    if (!arquivo) return 0;
    int quantidade = 0;
    while (1) {
        if (fseek(arquivo, (long)quantidade * tamanho_registro(), SEEK_SET) != 0)
            break;
        TFunc *f = le_funcionario(arquivo);
        if (!f) break;
        quantidade++;
        free(f);
    }
    return quantidade;
}

/* Cada particao pode conter de 1 a nFunc registros. Alocamos ate nFunc
 * nomes para aceitar tambem entradas desfavoraveis para a selecao natural. */
static Lista *cria_nomes_particoes(int quantidade) {
    Lista *lista = NULL;
    for (int i = quantidade; i >= 1; i--) {
        char nome[50];
        snprintf(nome, sizeof(nome), "p%d.dat", i);
        char *copia = cria_str(nome);
        if (!copia) { libera(lista); return NULL; }
        Lista *novo = cria(copia, lista);
        if (!novo) { free(copia); libera(lista); return NULL; }
        lista = novo;
    }
    return lista;
}

static void libera_pilhas_particoes(TPilha **pilhas, int *topos, int quantidade) {
    if (pilhas) {
        for (int p = 0; p < quantidade; p++) {
            if (pilhas[p]) {
                if (topos) {
                    for (int i = 0; i <= topos[p]; i++) free(pilhas[p][i].info);
                }
                free(pilhas[p]);
            }
        }
        free(pilhas);
    }
    free(topos);
}

int menu(){
    int op;

    printf("\n\n**************MENU************** \n\nBASE DE DADOS\n");

    printf("\nDigite 1 para utilizar a base de dados funcionario de teste.");
    printf("\nDigite 2 se desejar imprimir a base de dados funcionario.");
    printf("\nDigite 3 se desejar imprimir o índice primário da base de funcionarios.");


    printf("\n\nINTERPOLAÇÃO\n");
    printf("\nDigite 4 para utilizar a estrutura de interpolação teste.");
    printf("\nDigite 5, 6  para gerar particões ordenadas utilizando, respectivamente, os métodos: classificação interna ou seleção natural.");
    printf("\nDigite 7 para criar uma pilha com o conteúdo das partições geradas.");
    printf("\nDigite 8, 9 para gerar intercalar as partições ordenadas utilizando, respectivamente, os métodos: intercalação basico, intercalacao arvore de vencedores.");

    printf("\n\nINSERTION SORT E ÁRVORE BINÁRIA, ARVORE B\n");
    printf("\nDigite 10 para criar árvores binárias em memória interna e externa.");
    printf("\nDigite 11 para ordenar a base de dados utilizando o método insertion sort em disco. ");
    printf("\nDigite 12 para criar Árvore B+ de prefixo simples. ");


    printf("\nDigite -1 para sair: ");
    scanf("%d", &op);

    return op;

}

int main(){

    /*criando um arquivo lógico para abir para leitura e escrita o arquivo binário*/
    FILE* arq = fopen("base_funcionarios.dat", "wb+");
    if(arq == NULL) {
        printf("Arquivo base_funcionarios.dat não encontrado.\n");
        return 1;
    }

    /*criando um arquivo lógico para abir para leitura e escrita do arquivo de indice primario ordenado*/
    FILE* arqIndicePrimario = fopen("IndicePrimarioFunc.dat", "wb+");
    if(arqIndicePrimario == NULL) {
        printf("Arquivo IndicePrimarioFunc.dat não encontrado.\n");
        return 1;
    }

    /*criando um arquivo lógico para abir para leitura e escrita do arquivo de indice primario não ordenado*/
    FILE* arqIndicePrimarioNaoOrdenado = fopen("IndicePrimarioFuncNaoOrdenado.dat", "wb+");
    if(arqIndicePrimarioNaoOrdenado == NULL) {
        printf("Arquivo IndicePrimarioFuncNaoOrdenado.dat não encontrado.\n");
        return 1;
    }

    /*criando um arquivo lógico para abir para leitura e escrita da árvore binária externa*/
    FILE* arqArvoreBinariaExterna = fopen("ArvoreBinariaExternaFunc.dat", "wb+");
    if(arqArvoreBinariaExterna == NULL) {
        printf("Arquivo ArvoreBinariaExternaFunc.dat não encontrado.\n");
        return 1;
    }


    int op = menu();
    int nFunc = 0;

    int nParticoes = 0, nElementos = 0, nPilhas = 0;
    Lista  * nomes = NULL;

    TPilha **vetPilhas = NULL;
    int crioubase = 0, crioulistadenomes = 0, criouparticoes = 0, crioupilhas = 0;

    int *vetTopo = NULL;

    while(op!=-1){
        switch (op){
            case (1):
                //criando a base de dados de funcionários teste e o arquivo de índice primário
                nFunc = 20;
                cria_base_dados_funcionarios_teste(arq, arqIndicePrimario, arqIndicePrimarioNaoOrdenado, nFunc);
                crioubase = 1;
                break;
            case (2):
                //imprime a base de dados dos funcionários
                if(crioubase==1)
                    imprime_base_dados_funcionarios(arq, nFunc);
                else  printf("Base de dados inexistente para ser impressa, crie utilizando a opção 1 do menu!!!\n");
                break;
            case (3):
                //imprime o arquivo de índices primários
                if(crioubase==1){
                    printf("\nArquivo ordenado\n ch:rrn\n");
                    imprime_indice_primario_funcionarios(arqIndicePrimario,nFunc);
                    printf("\nArquivo não ordenado\n ch:rrn\n");
                    imprime_indice_primario_funcionarios(arqIndicePrimarioNaoOrdenado,nFunc);
                }else  printf("Indice primario inexistente para ser impresso, será criado após a definição da base de dados utilizando a opção 1 do menu!!!\n");
                break;
            case (4):
                libera_pilhas_particoes(vetPilhas, vetTopo, nPilhas);
                vetPilhas = NULL; vetTopo = NULL; nPilhas = 0; crioupilhas = 0;
                if (nomes) libera(nomes);
                nElementos = 2;
                nParticoes = 10; /* numero estimado para classificacao interna */
                nomes = cria_nomes_particoes(nFunc > 0 ? nFunc : 20);
                crioulistadenomes = (nomes != NULL);
                criouparticoes = 0;
                if (crioulistadenomes)
                    printf("\nEstrutura criada (memoria = %d; reservatorio = 1).\n", nElementos);
                else
                    printf("Falha ao criar lista de nomes das particoes.\n");
                break;
            case (5):
                if (crioulistadenomes && crioubase) {
                    libera_pilhas_particoes(vetPilhas, vetTopo, nPilhas);
                    vetPilhas = NULL; vetTopo = NULL; nPilhas = 0; crioupilhas = 0;
                    printf("\nGerando particoes por classificacao interna.\n");
                    classificacao_interna(arq, nomes, nElementos, nFunc);
                    nParticoes = (nFunc + nElementos - 1) / nElementos;
                    criouparticoes = 1;
                } else printf("Crie primeiro a base (1) e a estrutura (4).\n");
                break;
            case (6):
                if (crioulistadenomes && crioubase) {
                    libera_pilhas_particoes(vetPilhas, vetTopo, nPilhas);
                    vetPilhas = NULL; vetTopo = NULL; nPilhas = 0; crioupilhas = 0;
                    printf("\nGerando particoes por selecao natural.\n");
                    int geradas = selecao_natural(arq, nomes, nElementos, nFunc, 1);
                    nParticoes = geradas > 0 ? geradas : 0;
                    criouparticoes = (geradas > 0);
                } else printf("Crie primeiro a base (1) e a estrutura (4).\n");
                break;
            case (7):
                if (criouparticoes && nParticoes > 0) {
                    libera_pilhas_particoes(vetPilhas, vetTopo, nPilhas);
                    vetPilhas = NULL; vetTopo = NULL; nPilhas = 0; crioupilhas = 0;
                    vetPilhas = calloc((size_t)nParticoes, sizeof(*vetPilhas));
                    vetTopo = malloc((size_t)nParticoes * sizeof(*vetTopo));
                    if (!vetPilhas || !vetTopo) {
                        printf("Memoria insuficiente para as pilhas.\n");
                        libera_pilhas_particoes(vetPilhas, vetTopo, 0);
                        vetPilhas = NULL; vetTopo = NULL;
                        break;
                    }
                    for (int i = 0; i < nParticoes; i++) vetTopo[i] = -1;
                    nPilhas = nParticoes;
                    Lista *atual = nomes;
                    int ok = 1;
                    for (int p = 0; p < nParticoes; p++) {
                        if (!atual) { ok = 0; break; }
                        FILE *part = fopen(atual->nome, "rb");
                        if (!part) { ok = 0; break; }
                        int qtd = conta_registros(part);
                        if (qtd <= 0) { fclose(part); ok = 0; break; }
                        vetPilhas[p] = calloc((size_t)qtd, sizeof(TPilha));
                        if (!vetPilhas[p]) { fclose(part); ok = 0; break; }
                        printf("Particao %s: %d registros\n", atual->nome, qtd);
                        cria_pilha_particao(vetPilhas[p], part, qtd, &vetTopo[p], p);
                        fclose(part);
                        atual = atual->prox;
                    }
                    if (!ok) {
                        printf("Falha ao criar as pilhas das particoes.\n");
                        libera_pilhas_particoes(vetPilhas, vetTopo, nPilhas);
                        vetPilhas = NULL; vetTopo = NULL; nPilhas = 0;
                    } else crioupilhas = 1;
                } else printf("Gere primeiro as particoes (5 ou 6).\n");
                break;
            case (8):
                if(criouparticoes){
                    printf("\n\nIntercalando as partições, utilizando o método de intercalação basico.\n");
                    intercalacao_basico("arquivo_intercalado.dat", nParticoes, nomes);
                    //imprimindo o arquivo intercalado
                    FILE *fi = fopen("arquivo_intercalado.dat","rb+");
                    imprime_base_dados_funcionarios(fi,nFunc);
                    fclose(fi);
                }else printf("Partições não foram criadas, crie as partições utilizando as opções 5 ou 6 do menu!!!\n");
                break;
            case (9):
                if (criouparticoes && crioupilhas) {
                    printf("\nIntercalando por arvore binaria de vencedores.\n");
                    intercalacao_arvore_de_vencedores(vetPilhas, vetTopo,
                                                     "arquivo_intercalado.dat", nParticoes);
                    FILE *fi = fopen("arquivo_intercalado.dat", "rb");
                    if (fi) {
                        imprime_base_dados_funcionarios(fi, nFunc);
                        fclose(fi);
                    }
                    crioupilhas = 0; /* pilhas consumidas, reexecutar opcao 7 */
                } else printf("Gere as particoes (5 ou 6) e crie as pilhas (7).\n");
                break;
            case (10):
                //cria árvore binária em memória utilizando o arquivo de índice primário ordenado
                if(crioubase==1){
                    printf("\nArvore binária em memória pelo arquivo de  índice primário ordenado\n <nopai <noesq><nodireita> >\n");
                    cria_arvore_binaria_indice_primario(arqIndicePrimario,nFunc);
                    printf("\nArvore binária em memória pelo arquivo de índice primário não ordenado\n <nopai <noesq><nodireita> >\n");
                    cria_arvore_binaria_indice_primario(arqIndicePrimarioNaoOrdenado,nFunc);
                    printf("\nArvore binária externa pelo arquivo de base de dados do funcionário\n <nopai <noesq><nodireita> >\n");

                    arqArvoreBinariaExterna = freopen(
                        "ArvoreBinariaExternaFunc.dat",
                            "wb+",
                        arqArvoreBinariaExterna
                    );

                    if (arqArvoreBinariaExterna == NULL) {
                        printf("Erro ao recriar o arquivo da arvore externa.\n");
                        break;
                    }

                    criar_arvore_binaria_externa(arq,arqArvoreBinariaExterna);
                }else  printf("Arquivo de índice primario inexistente para a criação da arvore binaria, crie utilizando a opção 1 do menu!!!\n");
                break;
            case (11):
                 //ordenando a base de dados utilizando ordenação direta no arquivo com o método inseriton sort
                if(crioubase==1){
                    insertion_sort_disco(arq, nFunc);
                    imprime_base_dados_funcionarios(arq, nFunc);
                    //procurando por um funcionário na base de dados usando o método de busca binária
                    int busca_codigo;
                    printf("\n\nInforme o codigo do funcionário procurado (digite -1 caso não queira procurar um funcionário): ");
                    scanf("%d", &busca_codigo);
                    if(busca_codigo!= -1){
                        TFunc* func = busca_binaria(busca_codigo, arq, nFunc);

                        if (func != NULL) {
                            imprime_funcionario(func);
                            free(func);
                        } else {
                            printf("Funcionario com codigo %d nao encontrado.\n",busca_codigo);
                        }

                    }
                }else printf("Base de dados inexistente para ser ordenada, crie utilizando a opção 1 do menu!!!\n");
                break;
            case (12):
                printf("Método a ser implementado na parte II do trabalho.\n");
                if(crioubase==1){
                    ;//criar_arvore_B+();
                }else printf("Base de dados inexistente para ser ordenada, crie utilizando a opção 1 do menu!!!\n");
                break;
            default:
                printf("\nOpção inválida!");
        }
        op = menu();
    }

    libera(nomes);
    libera_pilhas_particoes(vetPilhas, vetTopo, nPilhas);

    fclose(arqIndicePrimario);
    fclose(arqIndicePrimarioNaoOrdenado);
    fclose(arqArvoreBinariaExterna);
    fclose(arq);

    return 0;
}
