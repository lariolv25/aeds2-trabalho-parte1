/* Testes automatizados para selecao natural e arvore de vencedores.
 * Compilar com o Makefile na raiz do projeto.
 */
#include "funcionarios.h"
#include "particoes.h"
#include "intercalacao.h"
#include "pilha.h"
#include "lista.h"
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static int quantidade(FILE *arquivo) {
    int i = 0;
    while (1) {
        if (fseek(arquivo, (long)i * tamanho_registro(), SEEK_SET) != 0) break;
        TFunc *f = le_funcionario(arquivo);
        if (!f) break;
        free(f);
        i++;
    }
    return i;
}

static Lista *nomes(int n) {
    Lista *lista = NULL;
    for (int i = n; i >= 1; i--) {
        char nome[60];
        snprintf(nome, sizeof(nome), "teste_p%03d.dat", i);
        lista = cria(cria_str(nome), lista);
    }
    return lista;
}

static void testar(const char *rotulo, const int *dados, int total, int m, int reserva) {
    printf("\n=== Teste: %s (N=%d, M=%d, R=%d) ===\n", rotulo, total, m, reserva);
    FILE *base = fopen("teste_entrada.dat", "wb+");
    assert(base);
    for (int i = 0; i < total; i++) {
        char nome[60];
        snprintf(nome, sizeof(nome), "Registro-%d", i);
        TFunc *f = funcionario(dados[i], nome, "111.111.111-11", "01/01/2000", (double)i);
        assert(f);
        assert(fseek(base, (long)i * tamanho_registro(), SEEK_SET) == 0);
        salva_funcionario(f, base);
        free(f);
    }
    fflush(base);
    Lista *lista = nomes(total ? total : 1);
    int partes = selecao_natural(base, lista, m, total, reserva);
    assert(partes >= 0 && partes <= total);
    if (total == 0) {
        assert(partes == 0);
        fclose(base);
        libera(lista);
        return;
    }
    assert(partes > 0);
    int *vistos = calloc((size_t)total, sizeof(*vistos));
    TPilha **pilhas = calloc((size_t)partes, sizeof(*pilhas));
    int *topos = calloc((size_t)partes, sizeof(*topos));
    assert(vistos && pilhas && topos);
    int lidos = 0;
    Lista *cursor = lista;
    for (int p = 0; p < partes; p++) {
        FILE *arquivo = fopen(cursor->nome, "rb");
        assert(arquivo);
        int tamanho = quantidade(arquivo);
        assert(tamanho > 0);
        int anterior = -2147483647-1;
        for (int i = 0; i < tamanho; i++) {
            assert(fseek(arquivo, (long)i * tamanho_registro(), SEEK_SET) == 0);
            TFunc *f = le_funcionario(arquivo);
            assert(f && f->cod >= anterior);
            anterior = f->cod;
            int origem = -1;
            assert(sscanf(f->nome, "Registro-%d", &origem) == 1);
            assert(origem >= 0 && origem < total);
            assert(f->cod == dados[origem]);
            vistos[origem]++;
            lidos++;
            free(f);
        }
        pilhas[p] = calloc((size_t)tamanho, sizeof(TPilha));
        assert(pilhas[p]);
        topos[p] = -1;
        cria_pilha_particao(pilhas[p], arquivo, tamanho, &topos[p], p);
        assert(topos[p] == tamanho - 1);
        fclose(arquivo);
        cursor = cursor->prox;
    }
    assert(lidos == total);
    for (int i = 0; i < total; i++) assert(vistos[i] == 1);
    memset(vistos, 0, (size_t)total * sizeof(*vistos));

    intercalacao_arvore_de_vencedores(pilhas, topos, "teste_saida.dat", partes);
    FILE *saida = fopen("teste_saida.dat", "rb");
    assert(saida && quantidade(saida) == total);
    int anterior = -2147483647-1;
    for (int i = 0; i < total; i++) {
        assert(fseek(saida, (long)i * tamanho_registro(), SEEK_SET) == 0);
        TFunc *f = le_funcionario(saida);
        assert(f && f->cod >= anterior);
        anterior = f->cod;
        int origem = -1;
        assert(sscanf(f->nome, "Registro-%d", &origem) == 1);
        assert(origem >= 0 && origem < total);
        assert(f->cod == dados[origem]);
        assert(f->salario == (double)origem);
        vistos[origem]++;
        free(f);
    }
    fclose(saida);
    for (int i = 0; i < total; i++) assert(vistos[i] == 1);
    for (int p = 0; p < partes; p++) {
        assert(topos[p] == -1);
        free(pilhas[p]);
    }
    free(vistos); free(pilhas); free(topos);
    for (Lista *atual = lista; atual; atual = atual->prox) remove(atual->nome);
    libera(lista);
    fclose(base);
    remove("teste_entrada.dat");
    remove("teste_saida.dat");
    printf("RESULTADO: OK (%d registros, %d particoes)\n", total, partes);
}

int main(void) {
    const int um[] = {4};
    const int crescente[] = {1,2,3,4,5,6,7,8,9,10};
    const int decrescente[] = {10,9,8,7,6,5,4,3,2,1};
    const int repetidos[] = {4,2,4,1,1,4,2,4,3,3};
    const int negativos[] = {-15,0,-1,-15,8,3,0,7,-2};
    const int alternados[] = {10,1,9,2,8,3,7,4,6,5};
    const int iguais[] = {5,5,5,5,5,5,5};
    int aleatorios[250];
    srand(20261105);
    for (int i = 0; i < 250; i++) aleatorios[i] = (rand() % 100) - 50;
    testar("arquivo vazio", NULL, 0, 2, 1);
    testar("um registro", um, 1, 2, 1);
    testar("ja ordenado", crescente, 10, 2, 1);
    testar("ordem inversa", decrescente, 10, 2, 1);
    testar("chaves repetidas", repetidos, 10, 2, 1);
    testar("numeros negativos", negativos, 9, 2, 1);
    testar("alternados", alternados, 10, 2, 1);
    testar("chaves iguais", iguais, 7, 3, 2);
    testar("reservatorio maior que memoria", negativos, 9, 2, 4);
    testar("memoria maior que arquivo", negativos, 9, 20, 3);
    testar("memoria minima", negativos, 9, 1, 1);
    testar("250 registros aleatorios", aleatorios, 250, 5, 3);
    puts("\nTODOS OS TESTES PASSARAM.\n");
    return 0;
}
