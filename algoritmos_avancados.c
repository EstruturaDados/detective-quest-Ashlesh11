// Desafio Detective Quest
// Tema 4 - Árvores e Tabela Hash
// Este código inicial serve como base para o desenvolvimento das estruturas de navegação, pistas e suspeitos.
// Use as instruções de cada região para desenvolver o sistema completo com árvore binária, árvore de busca e tabela hash.


    // 🌱 Nível Novato: Mapa da Mansão com Árvore Binária
    //
    // - Crie uma struct Sala com nome, e dois ponteiros: esquerda e direita.
    // - Use funções como criarSala(), conectarSalas() e explorarSalas().
    // - A árvore pode ser fixa: Hall de Entrada, Biblioteca, Cozinha, Sótão etc.
    // - O jogador deve poder explorar indo à esquerda (e) ou à direita (d).
    // - Finalize a exploração com uma opção de saída (s).
    // - Exiba o nome da sala a cada movimento.
    // - Use recursão ou laços para caminhar pela árvore.
    // - Nenhuma inserção dinâmica é necessária neste nível.

    // 🔍 Nível Aventureiro: Armazenamento de Pistas com Árvore de Busca
    //
    // - Crie uma struct Pista com campo texto (string).
    // - Crie uma árvore binária de busca (BST) para inserir as pistas coletadas.
    // - Ao visitar salas específicas, adicione pistas automaticamente com inserirBST().
    // - Implemente uma função para exibir as pistas em ordem alfabética (emOrdem()).
    // - Utilize alocação dinâmica e comparação de strings (strcmp) para organizar.
    // - Não precisa remover ou balancear a árvore.
    // - Use funções para modularizar: inserirPista(), listarPistas().
    // - A árvore de pistas deve ser exibida quando o jogador quiser revisar evidências.

    // 🧠 Nível Mestre: Relacionamento de Pistas com Suspeitos via Hash
    //
    // - Crie uma struct Suspeito contendo nome e lista de pistas associadas.
    // - Crie uma tabela hash (ex: array de ponteiros para listas encadeadas).
    // - A chave pode ser o nome do suspeito ou derivada das pistas.
    // - Implemente uma função inserirHash(pista, suspeito) para registrar relações.
    // - Crie uma função para mostrar todos os suspeitos e suas respectivas pistas.
    // - Adicione um contador para saber qual suspeito foi mais citado.
    // - Exiba ao final o “suspeito mais provável” baseado nas pistas coletadas.
    // - Para hashing simples, pode usar soma dos valores ASCII do nome ou primeira letra.
    // - Em caso de colisão, use lista encadeada para tratar.
    // - Modularize com funções como inicializarHash(), buscarSuspeito(), listarAssociacoes().

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define MAX_NAME 64
#define MAX_PATH 128

// ---------------------- Estrutura das salas (árvore binária) ----------------------
typedef struct Sala {
    char nome[MAX_NAME];
    struct Sala *esq;
    struct Sala *dir;
    char pista[MAX_NAME]; // string da pista, "" se não houver
    int temPista;         // 1 se ainda não coletada, 0 caso contrário
} Sala;

Sala* criarSala(const char *nome, Sala *esq, Sala *dir, const char *pista) {
    Sala *s = (Sala*) malloc(sizeof(Sala));
    if (!s) {
        fprintf(stderr, "Erro: memória insuficiente\n");
        exit(EXIT_FAILURE);
    }
    strncpy(s->nome, nome, MAX_NAME-1);
    s->nome[MAX_NAME-1] = '\0';
    s->esq = esq;
    s->dir = dir;
    if (pista && pista[0] != '\0') {
        strncpy(s->pista, pista, MAX_NAME-1);
        s->pista[MAX_NAME-1] = '\0';
        s->temPista = 1;
    } else {
        s->pista[0] = '\0';
        s->temPista = 0;
    }
    return s;
}

void liberarArvore(Sala *r) {
    if (!r) return;
    liberarArvore(r->esq);
    liberarArvore(r->dir);
    free(r);
}

// ---------------------- Árvore de Busca Binária para pistas ----------------------
typedef struct Pista {
    char nome[MAX_NAME];
    struct Pista *esq;
    struct Pista *dir;
} Pista;

Pista* criarPista(const char *nome) {
    Pista *p = (Pista*) malloc(sizeof(Pista));
    if (!p) {
        fprintf(stderr, "Erro: memória insuficiente\n");
        exit(EXIT_FAILURE);
    }
    strncpy(p->nome, nome, MAX_NAME-1);
    p->nome[MAX_NAME-1] = '\0';
    p->esq = p->dir = NULL;
    return p;
}

int buscarPista(Pista *r, const char *nome) {
    if (!r) return 0;
    int cmp = strcmp(nome, r->nome);
    if (cmp == 0) return 1;
    if (cmp < 0) return buscarPista(r->esq, nome);
    return buscarPista(r->dir, nome);
}

Pista* inserirPista(Pista *r, const char *nome) {
    if (!r) return criarPista(nome);
    int cmp = strcmp(nome, r->nome);
    if (cmp == 0) {
        // já existe, não inserir duplicata
        return r;
    } else if (cmp < 0) {
        r->esq = inserirPista(r->esq, nome);
    } else {
        r->dir = inserirPista(r->dir, nome);
    }
    return r;
}

void emOrdemPistas(Pista *r) {
    if (!r) return;
    emOrdemPistas(r->esq);
    printf("  - %s\n", r->nome);
    emOrdemPistas(r->dir);
}

void liberarPistas(Pista *r) {
    if (!r) return;
    liberarPistas(r->esq);
    liberarPistas(r->dir);
    free(r);
}

// ---------------------- Leitura de opção ----------------------
char lerOpcao() {
    char buf[32];
    if (!fgets(buf, sizeof(buf), stdin)) return 's';
    for (int i = 0; buf[i] != '\0'; ++i) {
        if (!isspace((unsigned char)buf[i])) {
            return (char)tolower((unsigned char)buf[i]);
        }
    }
    return 's';
}

// ---------------------- Exploração interativa (recebe ponteiro para raiz das pistas) ----------------------
void explorarSalas(Sala *raiz, Pista **raizPistas) {
    if (!raiz) return;
    Sala *atual = raiz;
    Sala *caminho[MAX_PATH];
    int profundidade = 0;

    while (1) {
        if (profundidade < MAX_PATH) caminho[profundidade++] = atual;

        printf("\nVocê está na sala: %s\n", atual->nome);

        // Se a sala possui pista e ainda não coletada, adiciona na BST
        if (atual->temPista) {
            if (!buscarPista(*raizPistas, atual->pista)) {
                *raizPistas = inserirPista(*raizPistas, atual->pista);
                printf("Você encontrou uma pista: \"%s\" (adicionada ao seu caderno)\n", atual->pista);
            } else {
                printf("Você vê a pista \"%s\", mas já a havia anotado.\n", atual->pista);
            }
            // marca como coletada para não inserir novamente ao revisitar
            atual->temPista = 0;
        }

        // Se for nó-folha, fim do caminho
        if (!atual->esq && !atual->dir) {
            printf("Você alcançou uma sala-folha. Fim do caminho.\n");
            break;
        }

        // Mostra opções disponíveis
        printf("Escolha uma direção ou ação:\n");
        if (atual->esq) printf("  (e) esquerda -> %s\n", atual->esq->nome);
        else printf("  (e) esquerda -> (não disponível)\n");
        if (atual->dir) printf("  (d) direita  -> %s\n", atual->dir->nome);
        else printf("  (d) direita  -> (não disponível)\n");
        printf("  (l) listar pistas encontradas (ordem alfabética)\n");
        printf("  (s) sair da exploração\n");
        printf("Opção: ");

        char opt = lerOpcao();

        if (opt == 's') {
            printf("Exploração encerrada pelo jogador.\n");
            break;
        } else if (opt == 'l') {
            if (!*raizPistas) {
                printf("Você ainda não encontrou nenhuma pista.\n");
            } else {
                printf("Pistas encontradas (ordem alfabética):\n");
                emOrdemPistas(*raizPistas);
            }
            // não avança; permanece na mesma sala
            // remove a entrada repetida caso tenha preenchido caminho (mantém sem duplicar)
            if (profundidade > 0 && caminho[profundidade-1] == atual) profundidade--;
            continue;
        } else if (opt == 'e') {
            if (atual->esq) {
                atual = atual->esq;
                continue;
            } else {
                printf("Não há sala à esquerda. Tente outra opção.\n");
                if (profundidade > 0 && caminho[profundidade-1] == atual) profundidade--;
                continue;
            }
        } else if (opt == 'd') {
            if (atual->dir) {
                atual = atual->dir;
                continue;
            } else {
                printf("Não há sala à direita. Tente outra opção.\n");
                if (profundidade > 0 && caminho[profundidade-1] == atual) profundidade--;
                continue;
            }
        } else {
            printf("Opção inválida. Use 'e', 'd', 'l' ou 's'.\n");
            if (profundidade > 0 && caminho[profundidade-1] == atual) profundidade--;
            continue;
        }
    }

    // Exibe o caminho percorrido
    printf("\nCaminho percorrido (%d salas):\n", profundidade);
    for (int i = 0; i < profundidade; ++i) {
        printf("  %d) %s\n", i+1, caminho[i]->nome);
    }
    printf("Fim.\n");
}

// ---------------------- main: monta a mansão e inicia exploração ----------------------
int main(void) {
    // Monta a árvore da mansão (estática no código, alocada dinamicamente)
    //                 Hall de Entrada
    //                /               \
    //           Corredor           Sala de Estar
    //           /     \            /          \
    //     Biblioteca Sala Estudo Cozinha     Jardim
    //                              /   \
    //                         Despensa  Copa

    // Algumas salas têm pistas (strings curtas)
    Sala *biblioteca = criarSala("Biblioteca", NULL, NULL, "Manuscrito Antigo");
    Sala *salaEstudo = criarSala("Sala de Estudo", NULL, NULL, "");
    Sala *corredor = criarSala("Corredor", biblioteca, salaEstudo, "");

    Sala *despensa = criarSala("Despensa", NULL, NULL, "Chave Enferrujada");
    Sala *copa = criarSala("Copa", NULL, NULL, "");
    Sala *cozinha = criarSala("Cozinha", despensa, copa, "Pegada de Lama");

    Sala *jardim = criarSala("Jardim", NULL, NULL, "Pó de Folhas");
    Sala *salaEstar = criarSala("Sala de Estar", cozinha, jardim, "");

    Sala *hall = criarSala("Hall de Entrada", corredor, salaEstar, "");

    // raiz da BST de pistas (inicialmente vazia)
    Pista *raizPistas = NULL;

    printf("Bem-vindo(a) à Mansão\n");
    printf("Você começa no: %s\n", hall->nome);
    printf("Comandos: 'e' esquerda, 'd' direita, 'l' listar pistas, 's' sair\n");

    explorarSalas(hall, &raizPistas);

    // Ao finalizar, mostra todas as pistas encontradas
    printf("\nResumo das pistas coletadas:\n");
    if (!raizPistas) {
        printf("  Nenhuma pista coletada.\n");
    } else {
        emOrdemPistas(raizPistas);
    }

    // libera toda a memória antes de sair
    liberarPistas(raizPistas);
    liberarArvore(hall);

    return 0;
}