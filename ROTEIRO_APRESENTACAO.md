# Roteiro sugerido para apresentação do Trabalho – Parte I

**Importante:** adaptar a divisão conforme o grupo real. Todos os membros devem participar do vídeo, conforme orientação do enunciado. O PDF não estabelece duração específica.

## Abertura – integrante [PREENCHER]

“Este trabalho da disciplina Algoritmos e Estruturas de Dados II implementa dois métodos de ordenação externa sobre o código-base de funcionários: Seleção Natural para gerar partições ordenadas e Árvore Binária de Vencedores para intercalar essas partições. A ordenação é feita pelo código do funcionário.”

## Seleção Natural – integrante [PREENCHER]

“A função `selecao_natural`, em `particoes.c`, recebe uma base e mantém até M registros ativos na memória. Ela escolhe o menor código, escreve na partição e tenta preencher a posição com outro registro da entrada. Se o registro lido tiver código menor que o último já gravado, é armazenado no reservatório. Quando o reservatório enche, a partição é concluída e outra começa com os registros reservados. Assim, cada partição fica ordenada, mas não necessariamente tem o mesmo tamanho das outras.”

**Mostrar no programa:** opções `1`, `4` e `6`, destacando a quantidade de partições e seus tamanhos.

## Árvore Binária de Vencedores – integrante [PREENCHER]

“Para intercalar as partições, criamos uma pilha para cada uma. Cada folha da árvore representa uma partição, e os nós internos indicam qual das duas opções possui a menor chave. A raiz aponta o próximo registro da saída. Depois de gravá-lo, removemos o topo da pilha vencedora e recalculamos somente o caminho daquela folha até a raiz. A saída final fica globalmente ordenada.”

**Mostrar no programa:** opções `7` e `9`; conferir a impressão dos códigos de `1` a `20`.

## Testes e conclusão – integrante [PREENCHER]

“Na base original, com 20 funcionários e memória para dois registros, a Seleção Natural gerou três partições, com 9, 10 e 1 registro. A Árvore de Vencedores devolveu os 20 códigos em ordem crescente. Também executamos testes automatizados com entradas em ordem inversa, repetidas, negativas e aleatórias. Um teste com 250 registros gerou 24 partições e todas foram corretamente intercaladas. O projeto mantém as estruturas e as funções de arquivo do código-base da professora.”

**Para encerrar:** mencionar o repositório GitHub e reforçar que o relatório descreve funções, resultados e participação real dos integrantes.

## Perguntas prováveis

**Por que as partições não têm sempre dois elementos?** Porque na Seleção Natural `M = 2` é a capacidade da memória ativa, não o limite do tamanho de uma partição. Ela pode emitir outros registros enquanto as chaves permitirem.

**Por que usar um reservatório?** Para separar registros que violariam a ordem crescente da partição atual, usando-os em uma partição posterior.

**Por que a Árvore de Vencedores é melhor do que procurar linearmente o menor elemento?** Depois de criada, a árvore pode ser atualizada em `O(log P)` por registro, com `P` partições, em vez de examinar as `P` partições a cada escolha.

**O algoritmo preserva registros com código repetido?** Sim. Os testes também verificam um identificador individual para garantir que os funcionários não sejam perdidos ou duplicados.

**Por que usar `fseek` no arquivo binário?** Para manter o mesmo deslocamento entre registros que o esqueleto usa com `tamanho_registro()`; isso evita leituras desalinhadas.
