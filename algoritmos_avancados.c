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
#define HASH_SIZE 101

// Estrutura das salas (árvore binária)
typedef struct Sala {
    char nome[MAX_NAME];
    struct Sala *esq;
    struct Sala *dir;
    char pista[MAX_NAME];        // nome da pista, "" se não houver
    char suspeito[MAX_NAME];     // suspeito associado à pista, "" se não houver
    int temPista;                // 1 se ainda não coletada, 0 caso contrário
} Sala;

Sala* criarSala(const char *nome, Sala *esq, Sala *dir, const char *pista, const char *suspeito) {
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
        if (suspeito && suspeito[0] != '\0') {
            strncpy(s->suspeito, suspeito, MAX_NAME-1);
            s->suspeito[MAX_NAME-1] = '\0';
        } else {
            s->suspeito[0] = '\0';
        }
        s->temPista = 1;
    } else {
        s->pista[0] = '\0';
        s->suspeito[0] = '\0';
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

// Árvore de Busca Binária para pistas 
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

// Tabela Hash: Suspeito -> lista de pistas
typedef struct ClueNode {
    char pista[MAX_NAME];
    struct ClueNode *prox;
} ClueNode;

typedef struct SuspeitoEntry {
    char nome[MAX_NAME];
    ClueNode *pistas;                // lista encadeada de pistas associadas
    struct SuspeitoEntry *prox;      // próximo na bucket (encadeamento)
} SuspeitoEntry;

typedef struct HashTable {
    SuspeitoEntry *buckets[HASH_SIZE];
} HashTable;

unsigned int hashString(const char *s) {
    unsigned int h = 0;
    for (const unsigned char *p = (const unsigned char*)s; *p; ++p) {
        h = (h * 31u) + *p;
    }
    return h % HASH_SIZE;
}

HashTable* criarHash() {
    HashTable *ht = (HashTable*) malloc(sizeof(HashTable));
    if (!ht) {
        fprintf(stderr, "Erro: memória insuficiente\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; i < HASH_SIZE; ++i) ht->buckets[i] = NULL;
    return ht;
}

ClueNode* criarClueNode(const char *pista) {
    ClueNode *n = (ClueNode*) malloc(sizeof(ClueNode));
    if (!n) { fprintf(stderr, "Erro: memória insuficiente\n"); exit(EXIT_FAILURE); }
    strncpy(n->pista, pista, MAX_NAME-1);
    n->pista[MAX_NAME-1] = '\0';
    n->prox = NULL;
    return n;
}

// Retorna 1 se a pista já está na lista do suspeito, 0 caso contrário
int pistaExisteNaLista(ClueNode *head, const char *pista) {
    ClueNode *cur = head;
    while (cur) {
        if (strcmp(cur->pista, pista) == 0) return 1;
        cur = cur->prox;
    }
    return 0;
}

// Insere a associação pista -> suspeito (internamente guarda suspeito -> lista de pistas)
void inserirNaHash(HashTable *ht, const char *pista, const char *suspeito) {
    if (!ht || !suspeito || suspeito[0] == '\0' || !pista || pista[0] == '\0') return;
    unsigned int idx = hashString(suspeito);
    SuspeitoEntry *cur = ht->buckets[idx];
    SuspeitoEntry *prev = NULL;
    // procura suspeito no bucket
    while (cur) {
        if (strcmp(cur->nome, suspeito) == 0) break;
        prev = cur;
        cur = cur->prox;
    }
    if (!cur) {
        // criar nova entry para o suspeito
        cur = (SuspeitoEntry*) malloc(sizeof(SuspeitoEntry));
        if (!cur) { fprintf(stderr, "Erro: memória insuficiente\n"); exit(EXIT_FAILURE); }
        strncpy(cur->nome, suspeito, MAX_NAME-1);
        cur->nome[MAX_NAME-1] = '\0';
        cur->pistas = NULL;
        cur->prox = ht->buckets[idx];
        ht->buckets[idx] = cur;
    }
    // inserir pista na lista do suspeito se ainda não existir
    if (!pistaExisteNaLista(cur->pistas, pista)) {
        ClueNode *n = criarClueNode(pista);
        n->prox = cur->pistas;
        cur->pistas = n;
    }
}

void listarAssociacoes(HashTable *ht) {
    if (!ht) return;
    int found = 0;
    for (int i = 0; i < HASH_SIZE; ++i) {
        SuspeitoEntry *e = ht->buckets[i];
        while (e) {
            found = 1;
            printf("Suspeito: %s\n", e->nome);
            ClueNode *c = e->pistas;
            if (!c) {
                printf("  (nenhuma pista associada)\n");
            } else {
                while (c) {
                    printf("  - %s\n", c->pista);
                    c = c->prox;
                }
            }
            e = e->prox;
        }
    }
    if (!found) {
        printf("Nenhuma associação suspeito->pista registrada.\n");
    }
}

// Retorna o(s) suspeito(s) mais citado(s) (com mais pistas associadas).
// Preenche vetor 'result' (pré-alocado com tamanho max_results) e retorna quantos encontrados.
// Se houver empate, retorna todos os empatados.
int suspeitoMaisCitado(HashTable *ht, char result[][MAX_NAME], int max_results) {
    if (!ht) return 0;
    int maxCount = 0;
    // primeiro passe: determinar maxCount
    for (int i = 0; i < HASH_SIZE; ++i) {
        SuspeitoEntry *e = ht->buckets[i];
        while (e) {
            int cnt = 0;
            ClueNode *c = e->pistas;
            while (c) { cnt++; c = c->prox; }
            if (cnt > maxCount) maxCount = cnt;
            e = e->prox;
        }
    }
    if (maxCount == 0) return 0; // nenhum suspeito com pistas
    // segundo passe: coletar todos com cnt == maxCount
    int found = 0;
    for (int i = 0; i < HASH_SIZE && found < max_results; ++i) {
        SuspeitoEntry *e = ht->buckets[i];
        while (e && found < max_results) {
            int cnt = 0;
            ClueNode *c = e->pistas;
            while (c) { cnt++; c = c->prox; }
            if (cnt == maxCount) {
                strncpy(result[found], e->nome, MAX_NAME-1);
                result[found][MAX_NAME-1] = '\0';
                found++;
            }
            e = e->prox;
        }
    }
    return found;
}

void liberarHash(HashTable *ht) {
    if (!ht) return;
    for (int i = 0; i < HASH_SIZE; ++i) {
        SuspeitoEntry *e = ht->buckets[i];
        while (e) {
            SuspeitoEntry *nextE = e->prox;
            ClueNode *c = e->pistas;
            while (c) {
                ClueNode *nextC = c->prox;
                free(c);
                c = nextC;
            }
            free(e);
            e = nextE;
        }
        ht->buckets[i] = NULL;
    }
    free(ht);
}

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

void explorarSalas(Sala *raiz, Pista **raizPistas, HashTable *ht) {
    if (!raiz) return;
    Sala *atual = raiz;
    Sala *caminho[MAX_PATH];
    int profundidade = 0;

    while (1) {
        if (profundidade < MAX_PATH) caminho[profundidade++] = atual;

        printf("\nVocê está na sala: %s\n", atual->nome);

        // Se a sala possui pista e ainda não coletada, adiciona na BST e na hash
        if (atual->temPista) {
            if (!buscarPista(*raizPistas, atual->pista)) {
                *raizPistas = inserirPista(*raizPistas, atual->pista);
                printf("Você encontrou uma pista: \"%s\" (adicionada ao seu caderno)\n", atual->pista);
            } else {
                printf("Você vê a pista \"%s\", mas já a havia anotado.\n", atual->pista);
            }
            if (atual->suspeito[0] != '\0') {
                inserirNaHash(ht, atual->pista, atual->suspeito);
                printf("A pista \"%s\" está associada ao suspeito: %s\n", atual->pista, atual->suspeito);
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
        printf("  (h) listar associações pista -> suspeito (por suspeito)\n");
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
            if (profundidade > 0 && caminho[profundidade-1] == atual) profundidade--;
            continue;
        } else if (opt == 'h') {
            printf("Associações suspeito -> pistas:\n");
            listarAssociacoes(ht);
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
            printf("Opção inválida. Use 'e', 'd', 'l', 'h' ou 's'.\n");
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

int main(void) {

    // Agora cada sala pode ter (opcionalmente) uma pista e um suspeito associado
    Sala *biblioteca = criarSala("Biblioteca", NULL, NULL, "Manuscrito Antigo", "Professor X");
    Sala *salaEstudo = criarSala("Sala de Estudo", NULL, NULL, "", "");
    Sala *corredor = criarSala("Corredor", biblioteca, salaEstudo, "", "");

    Sala *despensa = criarSala("Despensa", NULL, NULL, "Chave Enferrujada", "Jardineiro");
    Sala *copa = criarSala("Copa", NULL, NULL, "", "");
    Sala *cozinha = criarSala("Cozinha", despensa, copa, "Pegada de Lama", "Jardineiro");

    Sala *jardim = criarSala("Jardim", NULL, NULL, "Pó de Folhas", "Empregada");
    Sala *salaEstar = criarSala("Sala de Estar", cozinha, jardim, "", "");

    Sala *hall = criarSala("Hall de Entrada", corredor, salaEstar, "", "");

    // raiz da BST de pistas (inicialmente vazia)
    Pista *raizPistas = NULL;

    // tabela hash para ligar suspeitos às pistas
    HashTable *ht = criarHash();

    printf("Bem-vindo(a) à Mansão\n");
    printf("Você começa no: %s\n", hall->nome);
    printf("Comandos: 'e' esquerda, 'd' direita, 'l' listar pistas, 'h' listar associações, 's' sair\n");

    explorarSalas(hall, &raizPistas, ht);

    // Ao finalizar, mostra todas as pistas encontradas
    printf("\nResumo das pistas coletadas:\n");
    if (!raizPistas) {
        printf("  Nenhuma pista coletada.\n");
    } else {
        emOrdemPistas(raizPistas);
    }

    // Mostra associações suspeito -> pistas
    printf("\nAssociações suspeito -> pistas registradas:\n");
    listarAssociacoes(ht);

    // Determina suspeito(s) mais citado(s)
    char top[10][MAX_NAME];
    int nTop = suspeitoMaisCitado(ht, top, 10);
    if (nTop == 0) {
        printf("\nNenhum suspeito possui pistas associadas.\n");
    } else if (nTop == 1) {
        printf("\nSuspeito mais citado: %s\n", top[0]);
    } else {
        printf("\nHouve empate entre os suspeitos mais citados:\n");
        for (int i = 0; i < nTop; ++i) printf("  - %s\n", top[i]);
    }

    // Permite ao jogador deduzir o culpado (opcional)
    printf("\nQuem você acha que é o culpado? Digite o nome do suspeito ou apenas ENTER para pular: ");
    char buf[128];
    if (fgets(buf, sizeof(buf), stdin)) {
        // remove newline/trailing spaces
        for (int i = 0; buf[i]; ++i) {
            if (buf[i] == '\n') { buf[i] = '\0'; break; }
        }
        // trim leading/trailing
        char guess[MAX_NAME] = "";
        int start = 0;
        while (buf[start] && isspace((unsigned char)buf[start])) start++;
        strncpy(guess, buf + start, MAX_NAME-1);
        // trim trailing
        for (int i = strlen(guess) - 1; i >= 0 && isspace((unsigned char)guess[i]); --i) guess[i] = '\0';
        if (guess[0] != '\0') {
            printf("Você declarou: %s\n", guess);
            // verificar se está entre os top(s)
            int correct = 0;
            for (int i = 0; i < nTop; ++i) {
                if (strcmp(guess, top[i]) == 0) { correct = 1; break; }
            }
            if (nTop == 0) {
                printf("Não há evidências suficientes para confirmar culpado.\n");
            } else if (correct) {
                printf("Sua dedução coincide com o(s) suspeito(s) mais citado(s).\n");
            } else {
                printf("Sua dedução não coincide com os suspeitos mais citados.\n");
                if (nTop == 1) printf("Suspeito mais citado pelas pistas: %s\n", top[0]);
                else {
                    printf("Suspeitos mais citados (empate):\n");
                    for (int i = 0; i < nTop; ++i) printf("  - %s\n", top[i]);
                }
            }
        } else {
            printf("Nenhuma dedução fornecida.\n");
        }
    }

    // libera toda a memória antes de sair
    liberarPistas(raizPistas);
    liberarHash(ht);
    liberarArvore(hall);

    return 0;
}