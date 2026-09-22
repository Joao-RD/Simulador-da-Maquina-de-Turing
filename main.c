#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define MAX 1001


//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////--------STRUCTS--------///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Aresta{
    char simb_anterior, simb_posterior, direcao;
    int prox_estado;
    struct Aresta *prox;
}aresta;

typedef struct Nodo{
    int estado_final;
    aresta *ligacoes;
}nodo;

typedef struct Machine{
    nodo **estados;
}machine;

typedef struct Comandos{
    int num_comando, pos_string, quant_recursoes;
    char **operacoes;
    struct Comandos *prox, *anterior;
}comandos;


//////////////////////////////////////////////////////////////////////////////////////////////
/////////////////////////////--------CHAMADA DE FUNCOES--------//////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
nodo *cria_estado();

machine *cria_maquina(int num_estados);

aresta *cria_ligacao();

comandos *cria_lista_comandos();

nodo *insere_transicao(nodo *estado, char X, char Y, char C, int estado_j);

char *tranfere_caracteres(char *palavra, char *fita);

comandos *insere_comandos(comandos *bloco_operacoes, char *fita, int pos_fita, int num_estado);

void libera_comandos(comandos *bloco_operacoes);

comandos *rearranjo_comandos(comandos *bloco_operacoes);

int verifica_palavra(machine *maquina, int num_estado, char *fita, int pos_fita, comandos *bloco_operacoes);

void libera_ligacoes(aresta *ligacao);


//////////////////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////--------MAIN--------/////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
int main(){

///Variaveis
    int quant_estados, quant_transicoes, quant_finais;
    int estado_i, estado_j, valor_verificacao;
    char X, Y, C;
    char fita[MAX], aux_fita[MAX], palavra_entrada[MAX], *fita_modificada;
    machine *maquina_Turing;
    comandos *operacoes_execucao, *aux_execusoes;
    int i;

///Lendo a quantidade de estados e transicoes
    scanf("%d %d", &quant_estados, &quant_transicoes);
    maquina_Turing = cria_maquina(quant_estados);

///Criando a Maquina de Turing
    for(i = 0; i < quant_transicoes; i++){
        scanf("%d %c/%c,%c %d", &estado_i, &X, &Y, &C, &estado_j);
        maquina_Turing->estados[estado_i] = insere_transicao(maquina_Turing->estados[estado_i], X, Y, C, estado_j);
    }

///Determinando os estados finais
    scanf("%d", &quant_finais);
    for(i = 0; i < quant_finais; i++){
        scanf("%d", &estado_i);
        maquina_Turing->estados[estado_i]->estado_final = 1;
    }

///Preenchendo a fita com o caracter branco 'b'
    for(i = 0; i < MAX; i++)
        fita[i] = 'b';
    fita[MAX] = '\0';

///Lendo e verificando se a palavra pertence ou nao a linguagem
    while(1){
        scanf("%s", palavra_entrada);
        if(strcmp(palavra_entrada, "*") == 0)                                                               //Comparacao para finalizar a leitura
            break;

        strcpy(aux_fita, fita);                                                                             //Realiza uma copia da fita
        fita_modificada = tranfere_caracteres(palavra_entrada, aux_fita);
        operacoes_execucao = cria_lista_comandos();

        valor_verificacao = verifica_palavra(maquina_Turing, 0, fita_modificada, 0, operacoes_execucao);

        i = 0;
        if(valor_verificacao == 1){                                                                         //Caso a palavra pertenca a linguagem
            printf("%s aceita: ", palavra_entrada);
            while(fita_modificada[i] != 'b'){
                printf("%c", fita_modificada[i]);
                i++;
            }
            aux_execusoes = operacoes_execucao;
            while(aux_execusoes != NULL){                                                                   //Imprime as verificacoes feitas durante o processo
                i = 0;
                while(i < aux_execusoes->num_comando){
                    printf("\n%s", aux_execusoes->operacoes[i]);
                    i++;
                }
                aux_execusoes = aux_execusoes->prox;
            }
            printf("\n\n");
        }
        else                                                                                                //Caso a palavra nao pertenca a linguagem
            printf("%s rejeitada.\n", palavra_entrada);
        libera_comandos(operacoes_execucao);
    }

///Liberando as memorias alocadas
    for(i = 0; i < quant_estados; i++){
        if(maquina_Turing->estados[i]->ligacoes != NULL)
            libera_ligacoes(maquina_Turing->estados[i]->ligacoes);
        free(maquina_Turing->estados[i]);
    }
    free(maquina_Turing->estados);
    free(maquina_Turing);

    return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////
///////////////////////////////////--------FUNCOES--------///////////////////////////////////
////////////////////////////////////////////////////////////////////////////////////////////
/*
    Cria um estado, alocando memoria para o mesmo e definindo estado_final como 0 e
    o ponteiro (do tipo aresta) para ligacoes para NULL. Por fim, retorna o estado alocado.
                                                                                          */
nodo *cria_estado(){
    nodo *aux_estado = (nodo*)malloc(sizeof(nodo));

    aux_estado->estado_final = 0;
    aux_estado->ligacoes = NULL;
    return aux_estado;
}


/*
    Aloca-se a memoria para uma Maquina de Turing, alocando tambem um vetor de ponteiro
    (tipo **nodo) com tamanho equivalente a quantidade de estados (quant_estados) fornecido
    pelo usuario. Ademais, para cada posicao i do vetor de estados (aux->estados[i]), cria-se
    um estado chamando a funcao cria_estado e por fim retorna-se o ponteiro (do tipo machine)
    para a Maquina de Turing.
                                                                                              */
machine *cria_maquina(int num_estados){
    int i;

    machine *aux = (machine*)malloc(sizeof(machine));
    aux->estados = (nodo**)malloc(num_estados*sizeof(nodo*));

    for(i = 0; i < num_estados; i++)
        aux->estados[i] = cria_estado();

    return aux;
}


/*
    Funcao responsavel por criar uma ligacao de uma estado, a qual aloca-se um ponteiro
    (tipo *aresta) e determina que a proxima aresta aponta para NULL. No fim, retorna o
    ponteiro criado.
                                                                                        */
aresta *cria_ligacao(){
    aresta *aux = (aresta*)malloc(sizeof(aresta));
    aux->prox = NULL;
    return aux;
}


/*
    Aloca-se dinamicamente um ponteiro do tipo comandos e outro do tipo **char com tamanho
    MAX. Outrossim, define o proximo do ponteiro aux para NULL e para os inteiros numero do
    comando, quantidade de recursao e posicao na string para 0. Por fim, para cada posicao
    i do vetor operacoes (aux->operacoes[i]) aloca-se um ponteiro do tipo char com tamanho
    equivalente a MAX, retornando o ponteiro para a matriz de comandos criada.
                                                                                            */
comandos *cria_lista_comandos(){
    int i;
    comandos *aux = (comandos*)malloc(sizeof(comandos));
    aux->operacoes = (char**)malloc(MAX*sizeof(char*));
    aux->prox = aux->anterior = NULL;
    aux->num_comando = aux->quant_recursoes = 0;
    aux->pos_string = 0;

    for(i = 0; i < MAX; i++)
        aux->operacoes[i] = (char*)malloc(MAX*sizeof(char));

    return aux;
}


/*
    Funcao responsavel por inserir as transicoes na Maquina de Turing, a qual, caso nao haja
    nenhuma ligacao existente no estado (estado->ligacao == NULL), chama a funcao cria_ligacao
    e determina a mesma como o ponteiro do tipo aresta para o estado (estado->ligacoes = nova_aresta).
    Caso ja exista ligacoes no estado, a funcao percorrera a sequencia de ligacoes ate encontrar que o
    proximo ponteiro equivale a NULL (aux_aresta->prox == NULL), onde a funcao cria_ligacao sera
    chamada e a nova aresta sera a ultima da sequencia de ligacoes (aux_aresta->prox = nova_aresta).
    Em ambos os casos, as informacoes obtidas atraves do usuario sao armazenadas na nova ligacao e,
    por fim,  retorna-se o ponteiro do tipo nodo (return estado).
                                                                                                    */
nodo *insere_transicao(nodo *estado, char X, char Y, char C, int estado_j){
    aresta *aux_aresta, *nova_aresta;

    if(estado->ligacoes == NULL){
        nova_aresta = cria_ligacao();
        estado->ligacoes = nova_aresta;
    }
    else{
        aux_aresta = estado->ligacoes;
        while(aux_aresta->prox != NULL){
            aux_aresta = aux_aresta->prox;
        }
        nova_aresta = cria_ligacao();
        aux_aresta->prox = nova_aresta;
    }

    nova_aresta->simb_anterior = X;
    nova_aresta->simb_posterior = Y;
    nova_aresta->direcao = C;
    nova_aresta->prox_estado = estado_j;

    return estado;
}


/*
    Funcao que realiza a copia de caracter por caracter da palavra fornecida para um outro vetor
    de caracter, mantendo o que ja existe no vetor que recebera os caracteres. Por fim, retorna-se
    o vetor que possui os caracteres copiados.
                                                                                                    */
char *tranfere_caracteres(char *palavra, char *fita){
    int i = 0;

    while(palavra[i] != '\0'){
        fita[i] = palavra[i];
        i++;
    }
    return fita;
}


/*
    Funcao a qual armazena as informacoes do processo durante a verificacao do pertencimento ou
    nao da palavra a linguagem. Primeiramente verifica-se o bloco de comandos esta sem espaco
    disponivel (bloco_operacoes->num_comando == MAX), caso esteja, chama-se a funcao cria_lista_comando
    que criara o proximo bloco de comandos (bloco_operacoes->prox = novo_bloco). O restante da
    funcao armazena, em cada linha da matriz de comandos (aux_bloco->operacoes[aux_bloco->num_comando),
    os dados coletados enquanto ocorre a execucao da Maquina de Turing
                                                                                                         */
comandos *insere_comandos(comandos *bloco_operacoes, char *fita, int pos_fita, int num_estado){
    comandos *aux_bloco = bloco_operacoes;
    int i;

    if(bloco_operacoes->num_comando == MAX){
        comandos *novo_bloco = cria_lista_comandos();
        bloco_operacoes->prox = novo_bloco;
        novo_bloco->anterior = bloco_operacoes;
        aux_bloco = novo_bloco;
    }

    i = 0;
    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = '(';
    aux_bloco->pos_string++;

    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = 'q';
    aux_bloco->pos_string++;

    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = num_estado + '0';
    aux_bloco->pos_string++;

    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = ',';
    aux_bloco->pos_string++;

    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = ' ';
    aux_bloco->pos_string++;

    while(1){
        if(i == pos_fita){
            aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = '(';
            aux_bloco->pos_string++;

            aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = fita[pos_fita];
            aux_bloco->pos_string++;

            aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = ')';
        }
        else if(i > pos_fita && fita[i] == 'b')
            break;
        else{
            aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = fita[i];
        }

        i++;
        aux_bloco->pos_string++;
    }

    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = ')';
    aux_bloco->pos_string++;
    aux_bloco->operacoes[aux_bloco->num_comando][aux_bloco->pos_string] = '\0';

    aux_bloco->num_comando++;
    aux_bloco->pos_string = 0;


    return bloco_operacoes;
}


/*
    Libera a memoria alocada de todos os blocos de comandos utilizados durante a execucao de uma
    fita na Maquina de Turing.
                                                                                                  */
void libera_comandos(comandos *bloco_operacoes){
    if(bloco_operacoes == NULL)
        return;
    libera_comandos(bloco_operacoes->prox);

    int i;
    for(i = 0; i < MAX; i++)
        free(bloco_operacoes->operacoes[i]);
    free(bloco_operacoes->operacoes);
    free(bloco_operacoes);
}


/*
    Funcao necessaria para quando o caminho percorrido em uma Maquina de Turing resulta no nao pertencimento
    da palavra fornecida do usuario a linguagem. Dessa forma, a funcao ira organizar a posicao correta no
    bloco de comandos a fim de armazenar os proximos comandos em suas devidas posicoes, eliminando os comandos
    realizados durante o processo incorreto. Caso o quantidade de recursao executada para tal processo for
    maior que o numero de comandos no bloco (bloco_operacoes->num_comando < 0), libera-se o mesmo e o conjunto
    de comandos passara a ser o anterior do que teve sua memoria desalocada, sendo necessario ainda o ajuste
    no numero de comandos para que a quantidade faltante resulte em zero. No fim, retorna-se o ponteiro para
    o conjunto de comandos
                                                                                                               */
comandos *rearranjo_comandos(comandos *bloco_operacoes){
    int aux_quant_num = 0;
    comandos *aux_bloco;

    while(bloco_operacoes->num_comando != bloco_operacoes->quant_recursoes)
        bloco_operacoes->num_comando--;

    if(bloco_operacoes->num_comando < 0){
        aux_quant_num = bloco_operacoes->num_comando;
        aux_bloco = bloco_operacoes->anterior;

        libera_comandos(bloco_operacoes);

        bloco_operacoes = aux_bloco;
        while(aux_quant_num < 0){
            bloco_operacoes->num_comando--;
            aux_quant_num++;
        }
    }

    return bloco_operacoes;
}


/*
    Funcao recursiva com a finalidade de verificar se a palavra fornecida pelo usuario pertence ou nao
    a linguagem. A recursao acaba quando nao existe ligacoes no estado em que encontra a analise
    (maquina->estados[num_estado]->ligacoes == NULL) e quando o mesmo estado e final
    (maquina->estado[num_estado]->estado_final == 1), retornando o valor 1. No restante da funcao, ha
    um loop que procura os possiveis caminhos na maquina de acordo com a posicao do cabecote na fita,
    que, caso encontre um caminho possivel, chama a propria funcao vertifica_palavra alterando seus
    argumentos para que tanto o cabecote da fita quanto o estado da Maquina de Turing sigam as
    as condicoes da transicao realizada. Caso o caminho percorrido resulte em 0 (o que significa que
    a Maquina de Turing nao aceita a palavra seguindo aquele percurso), as alteracoes realizadas na fita
    sao desfeitas com o apoio de uma copia de fita realizada antes de ter ocorrido a recursao e a funcao
    rearranjo_bloco_operacoes e chamada para organizar os comandos armazenados. No fim, retorna o inteiro
    resultado.
                                                                                                          */
int verifica_palavra(machine *maquina, int num_estado, char *fita, int pos_fita, comandos *bloco_operacoes){
    if(maquina->estados[num_estado]->ligacoes == NULL && maquina->estados[num_estado]->estado_final == 1){
        bloco_operacoes = insere_comandos(bloco_operacoes, fita, pos_fita, num_estado);
        return 1;
    }

    int resultado = 0;
    comandos *aux_bloco;
    char aux_fita[MAX];
    aresta *aux_aresta = maquina->estados[num_estado]->ligacoes;

    while(resultado == 0){
        if(fita[pos_fita] == aux_aresta->simb_anterior){
            bloco_operacoes = insere_comandos(bloco_operacoes, fita, pos_fita, num_estado);
            bloco_operacoes->quant_recursoes++;
            strcpy(aux_fita, fita);
            aux_fita[pos_fita] = aux_aresta->simb_posterior;
            if(aux_aresta->direcao == 'R')
                resultado = verifica_palavra(maquina, aux_aresta->prox_estado, aux_fita, pos_fita + 1, bloco_operacoes);
            else
                resultado = verifica_palavra(maquina, aux_aresta->prox_estado, aux_fita, pos_fita - 1, bloco_operacoes);
        }
        if(resultado == 1)
            strcpy(fita, aux_fita);
        else{
            if(bloco_operacoes->num_comando != bloco_operacoes->quant_recursoes)
                bloco_operacoes = rearranjo_comandos(bloco_operacoes);
        }
        if(aux_aresta->prox == NULL)
            break;
        else
            aux_aresta = aux_aresta->prox;
   }

    if(resultado == 0)
        bloco_operacoes->quant_recursoes--;
    return resultado;
}


/*
    Funcao com a finalidade de liberar a memoria alocada das ligacoes utilizadas durante o
    processamento da Maquina de Turing.
                                                                                            */
void libera_ligacoes(aresta *ligacao){
    if(ligacao == NULL)
        return;
    libera_ligacoes(ligacao->prox);
    free(ligacao);
}

