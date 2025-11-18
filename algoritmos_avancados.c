#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#define TAMANHO_HASH 10
#define TAMANHO_PISTA 50
#define TAMANHO_SUSPEITO 20

char suspeitos[][TAMANHO_SUSPEITO] = {"Mordomo", "Jardineiro", "Dama de Companhia", "Coelho da Páscoa"};
int num_pistas_coletadas = 0;


typedef struct NoPista {
    char pista[TAMANHO_PISTA];
    struct NoPista *esquerda;
    struct NoPista *direita;
} NoPista;

typedef struct HashEntry {
    char pista[TAMANHO_PISTA];
    char suspeito[TAMANHO_SUSPEITO];
    struct HashEntry *proximo;
} HashEntry;

HashEntry *tabelaHash[TAMANHO_HASH];

typedef struct NoSala {
    char nome[TAMANHO_SUSPEITO];
    struct NoSala *esquerda;
    struct NoSala *direita;
} NoSala;


NoSala* criarSala(const char *nome) {
    NoSala *novaSala = (NoSala*) malloc(sizeof(NoSala));
    if (novaSala == NULL) {
        perror("Erro de alocação de memória para sala");
        exit(EXIT_FAILURE);
    }
    strcpy(novaSala->nome, nome);
    novaSala->esquerda = NULL;
    novaSala->direita = NULL;
    return novaSala;
}

int definirPista(const char *nomeSala, char *pista_out, char *suspeito_out) {
    if (strcmp(nomeSala, "Hall Principal") == 0) {
        strcpy(pista_out, "Pegadas de Bota Suja");
        strcpy(suspeito_out, "Jardineiro");
        return 1;
    } else if (strcmp(nomeSala, "Cozinha") == 0) {
        strcpy(pista_out, "Uma Casca de Cenoura");
        strcpy(suspeito_out, "Coelho da Páscoa");
        return 1;
    } else if (strcmp(nomeSala, "Biblioteca") == 0) {
        strcpy(pista_out, "Livro de Etiqueta Rasgado");
        strcpy(suspeito_out, "Mordomo");
        return 1;
    } else if (strcmp(nomeSala, "Quarto Principal") == 0) {
        strcpy(pista_out, "Lenço de Seda Bordado");
        strcpy(suspeito_out, "Dama de Companhia");
        return 1;
    } else if (strcmp(nomeSala, "Sala de Estar") == 0) {
        strcpy(pista_out, "Tábua de Xadrez Derrubada");
        strcpy(suspeito_out, "Mordomo");
        return 1;
    } else if (strcmp(nomeSala, "Porão") == 0) {
        strcpy(pista_out, "Pá de Jardim Escondida");
        strcpy(suspeito_out, "Jardineiro");
        return 1;
    }
    return 0;
}


NoPista* inserirPista(NoPista *raiz, const char *pista) {
    if (raiz == NULL) {
        NoPista *novo = (NoPista*) malloc(sizeof(NoPista));
        if (novo == NULL) {
            perror("Erro de alocação de memória para pista");
            return NULL;
        }
        strcpy(novo->pista, pista);
        novo->esquerda = novo->direita = NULL;
        return novo;
    }

    int comparacao = strcmp(pista, raiz->pista);

    if (comparacao < 0) {
        raiz->esquerda = inserirPista(raiz->esquerda, pista);
    } else if (comparacao > 0) {
        raiz->direita = inserirPista(raiz->direita, pista);
    } 
    

    return raiz;
}

void listarPistas(NoPista *raiz) {
    if (raiz != NULL) {
        listarPistas(raiz->esquerda);
        printf("  - %s\n", raiz->pista);
        listarPistas(raiz->direita);
    }
}


int hash(const char *chave) {
    int soma = 0;
    for (int i = 0; chave[i] != '\0'; i++) {
        soma += chave[i];
    }
    return soma % TAMANHO_HASH;
}

void inserirNaHash(const char *pista, const char *suspeito) {
    int indice = hash(pista);

    HashEntry *novaEntry = (HashEntry*) malloc(sizeof(HashEntry));
    if (novaEntry == NULL) {
        perror("Erro de alocação de memória para hash entry");
        return;
    }

    strcpy(novaEntry->pista, pista);
    strcpy(novaEntry->suspeito, suspeito);
    novaEntry->proximo = tabelaHash[indice];
    tabelaHash[indice] = novaEntry;
}

const char* encontrarSuspeito(const char *pista) {
    int indice = hash(pista);
    HashEntry *atual = tabelaHash[indice];

    while (atual != NULL) {
        if (strcmp(atual->pista, pista) == 0) {
            return atual->suspeito;
        }
        atual = atual->proximo;
    }
    return NULL; 
}

int contarPistasParaSuspeito(NoPista *raiz, const char *suspeito_acusado) {
    if (raiz == NULL) {
        return 0;
    }

    int contagem = 0;
    const char *suspeito_associado;

    suspeito_associado = encontrarSuspeito(raiz->pista);
    if (suspeito_associado != NULL && strcmp(suspeito_associado, suspeito_acusado) == 0) {
        contagem = 1;
    }

    contagem += contarPistasParaSuspeito(raiz->esquerda, suspeito_acusado);
    contagem += contarPistasParaSuspeito(raiz->direita, suspeito_acusado);

    return contagem;
}


char explorarSalas(NoSala *raizMansao, NoPista **raizPistas) {
    NoSala *salaAtual = raizMansao;
    char escolha;
    char pista_encontrada[TAMANHO_PISTA];
    char suspeito_pista[TAMANHO_SUSPEITO];

    if (salaAtual == NULL) {
        printf("A mansão está vazia!\n");
        return 's';
    }

    printf("\n--- 🧭 Começando a Exploração! ---\n");

    while (salaAtual != NULL) {
        printf("\nVocê está em: 🚪 %s\n", salaAtual->nome);

        if (definirPista(salaAtual->nome, pista_encontrada, suspeito_pista)) {
            printf("🔍 **Pista Encontrada!**\n");
            printf("  Pista: %s\n", pista_encontrada);
            printf("  Suspeito Associado: %s\n", suspeito_pista);
            
            *raizPistas = inserirPista(*raizPistas, pista_encontrada);
            inserirNaHash(pista_encontrada, suspeito_pista);
            num_pistas_coletadas++;
        } else {
            printf("Não há pistas relevantes nesta sala.\n");
        }

        printf("\nPara onde deseja ir?\n");
        printf("  [E] Esquerda (para %s)\n", salaAtual->esquerda ? salaAtual->esquerda->nome : "nenhuma sala");
        printf("  [D] Direita (para %s)\n", salaAtual->direita ? salaAtual->direita->nome : "nenhuma sala");
        printf("  [S] Sair e Acusar\n");
        printf("Escolha: ");
        
        while ((escolha = tolower(getchar())) == '\n' || escolha == ' ');
        while (getchar() != '\n'); 

        NoSala *proximaSala = NULL;
        switch (escolha) {
            case 'e':
                proximaSala = salaAtual->esquerda;
                if (proximaSala == NULL) {
                    printf("Parede! Não há cômodo para a esquerda.\n");
                }
                break;
            case 'd':
                proximaSala = salaAtual->direita;
                if (proximaSala == NULL) {
                    printf("Parede! Não há cômodo para a direita.\n");
                }
                break;
            case 's':
                printf("\nVocê decide que é hora de encerrar a investigação.\n");
                return 's';
            default:
                printf("Comando inválido. Fique onde está.\n");
                proximaSala = salaAtual;
                break;
        }

        if (proximaSala != NULL) {
            salaAtual = proximaSala;
        }
    }
    return 's';
}

void verificarSuspeitoFinal(NoPista *raizPistas) {
    if (num_pistas_coletadas == 0) {
        printf("\n\n--- ⚖️ JULGAMENTO FINAL ---\n");
        printf("\nVocê não coletou NENHUMA pista! A acusação é insustentável.\n");
        printf("Resultado: 💔 O CULPADO ESCAPA! Você falhou em coletar evidências.\n");
        return;
    }

    printf("\n\n--- ⚖️ JULGAMENTO FINAL ---\n");
    printf("Pistas coletadas (%d no total):\n", num_pistas_coletadas);
    listarPistas(raizPistas);
    printf("\n");

    printf("Suspeitos disponíveis:\n");
    for(int i = 0; i < 4; i++) {
        printf("[%d] %s\n", i + 1, suspeitos[i]);
    }

    int escolha_suspeito;
    char suspeito_acusado[TAMANHO_SUSPEITO] = "";
    
    printf("\nAcusação! Digite o número do suspeito culpado: ");
    if (scanf("%d", &escolha_suspeito) != 1 || escolha_suspeito < 1 || escolha_suspeito > 4) {
        printf("\n❌ Escolha inválida. A polícia não sabe quem prender. O culpado escapa.\n");
        return;
    }
    strcpy(suspeito_acusado, suspeitos[escolha_suspeito - 1]);

    printf("\n🚨 Você acusa: **%s**\n", suspeito_acusado);

    int count = contarPistasParaSuspeito(raizPistas, suspeito_acusado);

    printf("\nResultado da Investigação:\n");
    printf("Foram encontradas %d pistas ligando %s ao crime.\n", count, suspeito_acusado);

    if (count >= 2) {
        printf("\n🎉 **SUCESSO!**\n");
        printf("A acusação é sustentada por **%d pistas**! O criminoso é preso.\n", count);
    } else {
        printf("\n💔 **FALHA!**\n");
        printf("São necessárias pelo menos duas pistas. A evidência é fraca. O culpado escapa.\n");
    }
}


int main() {
    
    NoPista *raizPistas = NULL;

    for (int i = 0; i < TAMANHO_HASH; i++) {
        tabelaHash[i] = NULL;
    }

    
    NoSala *hall = criarSala("Hall Principal");
    NoSala *cozinha = criarSala("Cozinha");
    NoSala *biblioteca = criarSala("Biblioteca");
    NoSala *quarto = criarSala("Quarto Principal");
    NoSala *salaEstar = criarSala("Sala de Estar");
    NoSala *porao = criarSala("Porão");
    NoSala *jardim = criarSala("Jardim de Inverno");
    NoSala *atelie = criarSala("Ateliê");

    hall->esquerda = cozinha;
    hall->direita = biblioteca;
    
    cozinha->esquerda = quarto;
    cozinha->direita = salaEstar;

    biblioteca->direita = porao;
    
    quarto->esquerda = atelie;
    
    salaEstar->direita = jardim;

    
    explorarSalas(hall, &raizPistas);

    
    verificarSuspeitoFinal(raizPistas);

    
    
    
    
    return 0;
}
