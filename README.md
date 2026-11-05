# Trabalho – Parte I | CSI104 – Algoritmos e Estruturas de Dados II

Projeto em **C** baseado no esqueleto fornecido pela Profª. Janniele Aparecida Soares Araujo. Implementa:

1. **Geração de partições por Seleção Natural**, com memória interna de `M` registros e reservatório de `n` registros (`particoes.c`).
2. **Intercalação com Árvore Binária de Vencedores**, utilizando uma pilha por partição (`intercalacao.c`).

## Requisitos

- Code::Blocks com compilador GCC, **ou** GCC com `make` (Linux, macOS, MinGW/MSYS2 no Windows).
- Não requer bibliotecas de terceiros.

## Compilação

### Code::Blocks

1. Abra o arquivo `AEDSII.cbp`.
2. Selecione **Build and Run** (F9).
3. Interaja com o menu no console do programa.

### Terminal (GCC)

```bash
make
./aedsii
```

Em Windows, se usar GCC diretamente, substitua `./aedsii` por `aedsii.exe`.

## Execução recomendada

No menu, digite as opções **nesta ordem**:

| Opção | Ação |
|---|---|
| `1` | Cria a base de testes com 20 funcionários |
| `4` | Prepara nomes das partições e parâmetros da ordenação |
| `6` | Gera partições usando Seleção Natural |
| `7` | Cria as pilhas a partir das partições geradas |
| `9` | Intercala as partições com Árvore de Vencedores |
| `-1` | Encerra o programa |

A execução produz `p1.dat`, `p2.dat`, ... e `arquivo_intercalado.dat`. O arquivo final possui 20 funcionários, organizados por código crescente (1 a 20).

**Atenção:** o `main.c` original utiliza `wb+` para abrir os arquivos da base ao iniciar; portanto, **sobrescreve os arquivos de dados existentes** no diretório atual. Execute em uma pasta de testes se desejar preservar arquivos antigos.

**Alternativa de comparação:** após as opções `1` e `4`, use `5` para Classificação Interna, `7` para criar as pilhas e `9` para intercalar com a mesma Árvore de Vencedores. O método básico de intercalação original permanece disponível na opção `8`.

## Como funcionam os algoritmos

### Seleção Natural

1. Inicializa um vetor com até `M` funcionários da base de entrada.
2. Localiza o funcionário com **menor código** na memória e o grava na partição corrente.
3. Lê o próximo registro: se sua chave for maior ou igual à última emitida, ele pode ocupar a vaga livre na memória; se for menor, fica no **reservatório**.
4. Quando o reservatório atinge `n` elementos, termina de gravar ordenadamente os elementos ativos e encerra a partição.
5. Inicia uma nova partição usando primeiro os elementos reservados e, depois, os próximos registros de entrada.
6. Repete até esgotar entrada, memória e reservatório.

A quantidade e o tamanho das partições são **dinâmicos**. O programa retorna o número efetivo de arquivos gerados.

### Árvore Binária de Vencedores

1. Cada folha representa uma partição, cuja chave candidata está no topo de sua pilha.
2. Os nós internos armazenam o **índice da partição vencedora**, aquela com a menor chave no confronto dos filhos.
3. A raiz indica qual registro será escrito no arquivo final.
4. Remove o elemento do topo da pilha vencedora e atualiza **somente o caminho** dessa folha até a raiz.
5. Repete enquanto existir alguma pilha com registros.

Para `P` partições e `N` registros, a intercalação tem custo `O(P + N log P)` em tempo e `O(P)` para os índices da árvore, além das pilhas. A implementação da Seleção Natural utiliza busca linear do menor registro, com custo de até `O(N × M)` em tempo, e `O(M+n)` em memória auxiliar.

## Testes

```bash
make test
```

O programa de testes `testes/testar_algoritmos.c` verifica **12 cenários**, incluindo:

- arquivo vazio e um único registro;
- entrada crescente, decrescente e alternada;
- chaves repetidas e negativas;
- reservatório maior do que a memória principal;
- `M` maior do que o número de registros e memória mínima;
- 250 registros pseudoaleatórios.

Os testes conferem a **ordenação de cada partição**, a **ordenação final**, a **quantidade total de registros** e se **nenhum registro foi perdido ou duplicado**, inclusive quando as chaves se repetem.

No teste com a base original de 20 registros, `M = 2` e `n = 1`, foram geradas **3 partições com 9, 10 e 1 registro**, respectivamente. A intercalação resultou nos códigos 1 a 20. No teste de 250 registros, `M = 5` e `n = 3`, foram geradas 24 partições, com resultado corretamente ordenado.

## Principais arquivos

| Arquivo | Responsabilidade |
|---|---|
| `particoes.c` / `particoes.h` | Seleção Natural, geração de partições |
| `intercalacao.c` / `intercalacao.h` | Árvore de Vencedores e intercalação básica |
| `pilha.c` / `pilha.h` | Leitura de cada partição e pilhas |
| `main.c` | Menu e fluxo integrado do trabalho |
| `funcionario.c` / `funcionarios.h` | Representação e acesso aos registros binários do esqueleto |
| `testes/testar_algoritmos.c` | Validação automatizada dos algoritmos |
| `AEDSII.cbp` | Projeto Code::Blocks |
| `Makefile` | Compilação e testes via GCC |

**Observação sobre a serialização:** por compatibilidade com o código-base, os registros continuam sendo gravados com as funções originais `salva_funcionario`, `le_funcionario` e `tamanho_registro`. As funções usam `fseek` para preservar o espaçamento esperado entre registros nos arquivos binários do projeto.

O arquivo `selecao_natural.cpp` foi preservado apenas como **referência legada** do projeto original. Ele **não faz parte** da compilação do trabalho em C.

## Entrega

1. Coloque o projeto em um repositório GitHub.
2. Preencha os nomes reais, as contribuições reais e a URL do repositório no relatório editável.
3. Gere a versão final do relatório em PDF, se essa for a preferência do Moodle.
4. Prepare a apresentação/vídeo, garantindo a participação dos membros do grupo.

O link do GitHub não pode ser preenchido automaticamente antes de o projeto ser publicado na conta do grupo.
