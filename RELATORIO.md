# RELATÓRIO DE DESENVOLVIMENTO – TRABALHO PARTE I

**Disciplina:** CSI104 – Algoritmos e Estruturas de Dados II  
**Docente:** Profª. Janniele Aparecida Soares Araujo  
**Tema:** Seleção Natural e Intercalação com Árvores Binárias de Vencedores  
**Integrantes:** [PREENCHER COM NOMES DOS INTEGRANTES]  
**Data:** Novembro de 2026

## 1. Objetivo

O trabalho consiste na implementação, sobre o esqueleto disponibilizado na disciplina, de dois algoritmos de ordenação externa: geração de partições ordenadas por Seleção Natural e intercalação das partições por Árvore Binária de Vencedores. O projeto utiliza registros de funcionários armazenados em arquivos binários, cuja chave de ordenação é o campo `cod`.

## 2. Desenvolvimento

### 2.1 Geração de Partições – Seleção Natural

A função `selecao_natural()` foi implementada em `particoes.c`. O procedimento mantém `M` registros ativos na memória e um reservatório com capacidade de `n` registros. O menor registro ativo é escrito na partição atual. Caso um novo registro possua código menor que a última chave emitida, ele é direcionado ao reservatório para ser utilizado em uma partição posterior. Quando o reservatório fica cheio, os registros restantes da memória são emitidos em ordem, a partição é encerrada e uma nova é iniciada. O algoritmo retorna a quantidade real de partições geradas.

Funções auxiliares: `indice_menor()` seleciona o menor elemento ativo; `le_proximo()` lê o registro respeitando o posicionamento do arquivo; `escreve_e_libera()` persiste o registro e libera a memória utilizada.

### 2.2 Intercalação – Árvore de Vencedores

A função `intercalacao_arvore_de_vencedores()` foi implementada em `intercalacao.c`. Cada folha representa uma partição e cada nó interno registra o índice da partição cujo funcionário possui a menor chave. A raiz aponta o próximo registro a ser emitido. Depois da gravação, o topo da pilha vencedora é removido e apenas os nós no caminho da folha à raiz são recalculados, evitando buscar o menor elemento em todas as partições a cada iteração. A função auxiliar `vencedor()` efetua a comparação dos concorrentes.

### 2.3 Integração com o projeto original

O `main.c` foi ajustado para registrar a quantidade real de partições, permitir o carregamento de pilhas com tamanhos variáveis e executar os dois algoritmos pelas opções 6 e 9 do menu. O código de criação de pilhas em `pilha.c` também foi ajustado para ler exatamente a quantidade de elementos de cada arquivo, do fim para o início. Foram acrescentadas funções de apoio para contagem de registros, geração de nomes de arquivos e liberação de memória.

## 3. Funções implementadas ou alteradas

- `selecao_natural()`: produz partições ordenadas e devolve sua quantidade.
- `indice_menor()`: encontra a menor chave presente na memória principal.
- `le_proximo()`: realiza leitura posicional de registros binários da base.
- `escreve_e_libera()`: escreve o funcionário na partição e libera seu espaço.
- `intercalacao_arvore_de_vencedores()`: faz a fusão das partições em ordem crescente.
- `vencedor()`: seleciona o vencedor de cada confronto da árvore.
- `cria_pilha_particao()`: carrega os registros de uma partição para a pilha em ordem inversa de leitura.
- `conta_registros()`, `cria_nomes_particoes()` e `libera_pilhas_particoes()`: funções de integração e gerenciamento de arquivos e memória.

## 4. Resultados e testes

Com a base fornecida no esqueleto, contendo 20 funcionários, a Seleção Natural foi executada com `M = 2` e `n = 1`. O resultado foi a criação de três partições ordenadas, com respectivamente 9, 10 e 1 registro. A Árvore de Vencedores intercalou os 20 funcionários em um único arquivo, cujos códigos ficaram ordenados de 1 a 20. Uma segunda execução com as dez partições geradas pela Classificação Interna original também produziu a sequência correta.

Foram executados ainda 12 cenários automatizados com entradas vazias, unitárias, crescentes, decrescentes, repetidas, negativas, alternadas e aleatórias, além de diferentes tamanhos da memória e do reservatório. Todos os testes passaram. A entrada de 250 registros aleatórios, utilizando `M = 5` e `n = 3`, produziu 24 partições, que foram corretamente intercaladas. A verificação conferiu a ordenação e a presença de todos os registros, sem perdas nem duplicações.

## 5. Participação dos integrantes

**[PREENCHER – integrante 1]:** [descrever as tarefas efetivamente realizadas].  
**[PREENCHER – integrante 2]:** [descrever as tarefas efetivamente realizadas].  
**[ADICIONAR OS DEMAIS INTEGRANTES, SE HOUVER].**

## 6. Conclusão

A implementação permitiu integrar os dois métodos de ordenação externa ao código-base em C, preservando o modelo de funcionários e os arquivos binários originais. Os testes indicaram que a Seleção Natural gera partições internamente ordenadas e que a Árvore de Vencedores combina essas partições em um resultado global ordenado, inclusive com diferentes distribuições de chaves. Para submeter o trabalho, é necessário identificar os participantes, registrar suas contribuições reais e adicionar o endereço do repositório GitHub.

## 7. Repositório GitHub

**URL:** [PREENCHER APÓS PUBLICAÇÃO NO GITHUB]  
**Versão/data:** [PREENCHER COM O COMMIT OU DATA DA VERSÃO ENTREGUE]
