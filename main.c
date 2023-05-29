#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#define M 5
#define m 2
#define codigoVendas 10001
#define user "admin"
#define keyword "admin"


typedef struct key{
    int codigo;
    char nome[100];
    int mc; //1 para caso seja um medicamento controlado e 0 para caso não seja
    int estoque; // quaantidade disponivel
    double valor;
    struct elem* filho;
}Chave;

typedef struct elem{
    Chave* remedios[M];
    struct elem* filho;
}Arv;

typedef struct pil{
    Arv* tree;
    struct pil* prox;
}Pilha;

typedef struct e{
    char pais[40];
    char estado[30];
    char cidade[30];
    char bairro[50];
    char cep[15];
    char rua[30];
    char numero[5];
}Endereco;

typedef struct car{
    Chave* ch;
    double valor;
    int qnt;
    struct car* prox;
}Carrinho;//pilha de carrinhos

typedef struct pag{
    char usuario[15];
    int codigo;
    int op;
    int qntp;
    int status;
}Pagamento;

typedef struct ven{
    char usuario[15];
    int codigo;
    Pagamento* p;
    Carrinho* c;
    Endereco* add;
    struct ven* prox;
}Vendas;

Chave* novaChave(int codigo, int mc, int estoque, double valor, char nome[]){
    Chave* ch = malloc(sizeof(Chave));
    ch->codigo = codigo;
    strcpy(ch->nome, nome);
    ch->estoque = estoque;
    ch->mc = mc;
    ch->valor = valor;
    ch->filho = NULL;
    return ch;
}

Pagamento* novoPagamento(char u[], int codigo, int op, int qntp, int status){
    Pagamento* p = malloc(sizeof(Pagamento));
    p->codigo = codigo;
    strcpy(p->usuario, u);
    p->op = op;
    p->qntp = qntp;
    p->status = status;
    return p;
}

Carrinho* novoCarrinho(Chave* ch, int qnt){
    Carrinho* c = malloc(sizeof(Carrinho));
    c->ch = ch;
    c->valor = ch->valor*qnt;
    c->qnt = qnt;
    c->prox = NULL;
    return c;
}

Vendas* novaVenda(int codigo, Carrinho* c, Endereco* add, char u[], Pagamento* p){
    Vendas* venda = malloc(sizeof(Vendas));
    venda->codigo = codigo;
    venda->c = c;
    strcpy(venda->usuario, u);
    venda->add = add;
    venda->p = p;
    venda->prox = NULL;
    return venda;
}

Endereco* novoEndereco(char pais[], char estado[], char cidade[], char bairro[], char rua[], char numero[], char cep[]){
    Endereco* add = malloc(sizeof(Endereco));
    strcpy(add->pais, pais);
    strcpy(add->estado, estado);
    strcpy(add->cidade, cidade);
    strcpy(add->bairro, bairro);
    strcpy(add->rua, rua);
    strcpy(add->numero, numero);
    strcpy(add->cep, cep);
    return add;
}

Pilha* novaPilha(Arv* tree){
    Pilha* p = malloc(sizeof(Pilha*));
    p->tree = tree;
    p->prox = NULL;
    return p;
}

Vendas* inserirVenda(Vendas* v, char u[], int cv, Pagamento* p, Carrinho* c, Endereco* add){
    if(v==NULL){
        v = novaVenda(cv, c, add, u, p);
        return v;
    }else{
        v->prox = inserirVenda(v->prox, u, cv, p, c, add);
        return v;
    }
}

int codigoVendaAtual(Vendas* v){
    if(v==NULL){
        return codigoVendas - 1;
    }else{
        if(v->prox==NULL){
            return v->codigo;
        }else{
            return codigoVendaAtual(v->prox);
        }
    }
}

Pilha* empilhar(Pilha* p, Arv* tree){
    if(p==NULL){
        p = novaPilha(tree);
        return p;
    }else{
        Pilha* novo = novaPilha(tree);
        novo->prox = p;
        return novo;
    }
}

Arv* retNulo(Arv* tree, int i){
    if(i==M){
        return tree;
    }else{
        tree->remedios[i] = NULL;
        return retNulo(tree, i+1);
    }
}

Arv* novaArvore(){
    Arv* tree = malloc(sizeof(Arv));
    tree->filho = NULL;
    tree = retNulo(tree, 0);
    return tree;
}

void exibir(Arv* tree, int i){
    if(tree==NULL){
        return;
    }
    if(tree->remedios[i]!=NULL){
        Chave* ch = tree->remedios[i];
        if(ch->filho!=NULL){
            exibir(ch->filho, 0);
        }
            printf("CODIGO: %d\n", ch->codigo);
            printf("NOME: %s\n", ch->nome);
            if(ch->mc==1){
                printf("REMEDIO CONTROLADO\n");
            }else{
                printf("REMEDIO NAO CONTROLADO\n");
            }
            printf("ESTOQUE: %d\n", ch->estoque);
            printf("VALOR: %.2lf\n\n", ch->valor);
            exibir(tree, i+1);
    }else{
        exibir(tree->filho, 0);
    }
}

void atualizarFILE(Arv* tree, FILE *remedios, int i){
    if(tree==NULL){
        return;
    }
    if(tree->remedios[i]!=NULL){
        Chave* ch = tree->remedios[i];
        if(ch->filho!=NULL){
            atualizarFILE(ch->filho, remedios, 0);
        }
            fprintf(remedios, "%d %s %d %d %.2lf\n", ch->codigo, ch->nome, ch->mc, ch->estoque, ch->valor);
            atualizarFILE(tree, remedios, i+1);
    }else{
        atualizarFILE(tree->filho, remedios, 0);
    }
}


Arv* removePrimeiro(Arv* tree, int i, int t){
    if(i==t){
        tree->remedios[t] = NULL;
        tree->filho = NULL;
        return tree;
    }else{
        tree->remedios[i] = tree->remedios[i+1];
        return removePrimeiro(tree, i+1, t);
    }
}

int verificaEspaco(Arv* tree, int i){
    if(i==M){
        return -1;
    }else if(tree->remedios[i]==NULL){
        return i;
    }else{
        return verificaEspaco(tree, i+1);
    }

}

Arv* vetDir(Arv* tree, int i, int t){
    if(t>=i){
        tree->remedios[t+1]=tree->remedios[t];
        t--;
        return vetDir(tree, i, t);
    }
    return tree;
}

Carrinho* inserirCar(Chave* ch, Carrinho* c, int qnt){
    if(c==NULL){
        c = novoCarrinho(ch, qnt);
        return c;
    }else{
        Carrinho* novo = novoCarrinho(ch, qnt);
        novo->valor = novo->valor + c->valor;
        novo->prox = c;
        return novo;
    }
}

Arv* inserirOrdenado(Arv* tree, Chave* ch, int i, int t){
    if(i<0){
        return tree;
    }
    if(ch->codigo>tree->remedios[i]->codigo){
        tree->remedios[i+1]=ch;
        return tree;
    }else{
        if(ch->codigo<tree->remedios[i]->codigo  && ch->codigo>tree->remedios[i-1]->codigo){
            tree = vetDir(tree, i, t);
            tree->remedios[i]=ch;
            return tree;
        }else{
            return inserirOrdenado(tree, ch, i-1, t);
        }
    }
}

//rotaciona para a esquerda
Arv* rotEsq(Arv* tree, int i){
    int k = verificaEspaco(tree->remedios[i]->filho, 0);
    if(k!=(-1)){
        Arv* aux = tree->remedios[i]->filho;
        tree->remedios[i]->filho = NULL;
        aux = inserirOrdenado(aux, tree->remedios[i], k-1, k-1);
        tree->remedios[i] = tree->filho->remedios[0];
        k = verificaEspaco(tree->filho, 0);
        tree->filho = removePrimeiro(tree->filho, 0, k);
        if(tree->filho == NULL) tree->filho = novaArvore();
        tree->remedios[i]->filho = aux;
        return rotEsq(tree, i);
    }else{
        return tree;
    }
}

Arv* copiaChaves(Arv* tree, Arv* newTree, int cont, int inicio, int fim){
    if(inicio>fim){
        return newTree;
    }else{
        newTree->remedios[cont] = tree->remedios[inicio];
        return copiaChaves(tree, newTree, cont+1, inicio+1, fim);
    }
}

Arv* splitTree(Arv* tree){
    Arv* newTree = novaArvore();
    Arv* esq = novaArvore();
    Arv* dir = novaArvore();
    newTree->remedios[0] = tree->remedios[m];
    esq = copiaChaves(tree, esq, 0, 0, m-1);
    dir = copiaChaves(tree, dir, 0, m+1, M-1);
    //free(tree);
    newTree->remedios[0]->filho = esq;
    newTree->filho = dir;
    return newTree;
}

Chave* buscarRemedio(Arv* tree, int codigo, int i){

    if(i==5 || (i<5 && tree->remedios[i]==NULL)){
        return buscarRemedio(tree->filho, codigo, 0);
    }
    if(tree==NULL || tree->remedios[i]==NULL){
        return NULL;
    }
    if(codigo==tree->remedios[i]->codigo){
        return tree->remedios[i];
    }else{
        if(codigo<tree->remedios[i]->codigo){
            return buscarRemedio(tree->remedios[i]->filho, codigo, 0);
        }else{
            if(codigo>tree->remedios[i]->codigo){
                return buscarRemedio(tree, codigo, i+1);
            }
        }
    }
}

int verificaFolha(Arv* tree){
    if(tree->remedios[0]->filho==NULL){
        return 1;
    }else{
        return 0;
    }
}

Arv* acharFolha(Arv* tree, Chave* ch, int i){
    if(verificaFolha(tree)==1){
        return tree;
    }else{
        if(ch->codigo<tree->remedios[i]->codigo){
            return acharFolha(tree->remedios[i]->filho, ch, 0);
        }else{
            //|| tree->filho->remedios[0]->codigo<ch->codigo
            if(tree->remedios[i+1]==NULL){
                return tree->filho;
            }else{
                return acharFolha(tree, ch, i+1);
            }
        }
    }
}

int buscaPosicao(Arv* tree, Chave* ch, int i){
    if(tree->remedios[i]==NULL){
        return i;
    }else{
        return buscaPosicao(tree, ch, i+1);
    }
}

int buscaPosPai(Arv* pai, Arv* tree, Arv* newTree, int i){
    if(pai->filho==tree || pai->remedios[i]->filho==NULL){
        return M;
    }
    if(pai->remedios[i]->filho==tree){
        return i;
    }else{
        return buscaPosPai(pai, tree, newTree, i+1);
    }
}

Arv* inserir(Arv* tree, int codigo, int mc, int estoque, double valor, char nome[], Arv* pai, Pilha* p){
    Arv* aux;
    p = empilhar(p, tree);
    if(tree==NULL){//se a arvore for nula
        tree = novaArvore();
        tree->filho = novaArvore();
        tree->remedios[0] = novaChave(codigo, mc, estoque, valor, nome);
        return tree;
    }else{
        if(verificaFolha(tree)==1){
            int i = verificaEspaco(tree, 0);
            if(i!=(-1)){
                tree = inserirOrdenado(tree, novaChave(codigo, mc, estoque, valor, nome), i-1, i-1);
                return tree;
            }else{
                aux = splitTree(tree);
                return inserir(aux, codigo, mc, estoque, valor, nome, aux, p);
            }
        }else{
            aux = acharFolha(tree, novaChave(codigo, mc, estoque, valor, nome), 0);
            if(aux==NULL) aux = novaArvore();
            int i = verificaEspaco(aux, 0);
            if(i!=(-1)){
                if(i==0){
                        aux->remedios[i] = novaChave(codigo, mc, estoque, valor, nome);
                        tree->filho = aux;
                        return tree;
                }else{
                    tree = inserirOrdenado(aux, novaChave(codigo, mc, estoque, valor, nome), i-1, i-1);
                    int pos = buscaPosPai(p->tree, aux, tree, 0);
                    if(i<M){
                        p->tree->remedios[pos]->filho = tree;
                    }else{
                        p->tree->filho = tree;
                    }
                    i = verificaEspaco(p->tree, 0);
                    p->tree = rotEsq(p->tree, i-1);
                    return p->tree;
                }
            }else{
                int pos = buscaPosicao(p->tree, aux->remedios[m], 0);
                p->tree->remedios[pos] = aux->remedios[m];
                Arv* esq = novaArvore();
                esq = copiaChaves(aux, esq, 0, 0, m-1);
                Arv* dir = novaArvore();
                dir = copiaChaves(aux, dir, 0, m+1, M-1);

                tree = p->tree;
                tree->remedios[pos]->filho = esq;
                tree->filho = dir;
                return inserir(tree, codigo, mc, estoque, valor, nome, tree, p);
            }
        }
    }
}

Arv* readFileRem(Arv* tree, FILE *remedios){
    int codigo, mc, estoque;
    char nome[100];
    double valor;
    Pilha* p = NULL;
    if(fscanf(remedios, "%d %s %d %d %lf", &codigo, &nome, &mc, &estoque, &valor)!=EOF){
        tree = inserir(tree, codigo, mc, estoque, valor, nome, tree, p);
        tree = readFileRem(tree, remedios);
    }
        return tree;
}

Arv* openFileRem(Arv* tree){
    FILE *remedios;
    remedios = fopen("remedios.txt", "r");

    tree = readFileRem(tree, remedios);
    fclose(remedios);
    return tree;
}

void layout_cadastro(){
    printf("-------------- DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("CADASTRO\n\n");
    printf("               ");
    printf("1 - Pessoa fisica\n\n");
    printf("               ");
    printf("2 - Instituicao\n\n");
    printf("               ");
    printf("3 - Voltar\n\n");
    printf("Selecione 1 se for pessoa fisica, selecione 2 se for uma instituicao ou selecione 3 para voltar: ");
}

//busca se o usuario ja ta cadastrado
int buscaCad(FILE *usuarios, char aux[]){
    char str[100];
    if(fscanf(usuarios, "%s", &str)!=EOF){
        if(strcmp(aux, str)==0){
            return 1;
        }else{
            return buscaCad(usuarios, aux);
        }
    }else{
        return 0;
    }
}

//faz login do cliente
int buscaUser(FILE *usuarios, char aux[], int i){
    char str[100];
    if(fscanf(usuarios, "%s", &str)!=EOF){
        if(strcmp(aux, str)==0){
            return i;
        }else{
            return buscaUser(usuarios, aux, i+1);
        }
    }else{
        return -1;
    }
}

void cadEndereco(FILE *usuarios, int cv, char u[]){
    fflush(stdin);
    char aux[100];
    fprintf(usuarios, "%s\n", u);
    fprintf(usuarios, "%d\n", cv);
    printf("Informe seu endereco:\n\n");
    printf("Nome do pais: ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s", aux);
    printf("Nome do estado: ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s", aux);
    printf("Nome da cidade: ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s", aux);
    printf("Nome do bairro: ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s", aux);
    printf("Nome da rua: ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s", aux);
    printf("Numero: ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s", aux);
    printf("CEP (sem pontuacao): ");
    fgets(aux, 100, stdin);
    fflush(stdin);
    fprintf(usuarios, "%s\n", aux);
}

void cadPagamento(FILE *usuarios, int cv, char u[]){
    system("cls");
    fflush(stdin);
    char aux[100];


    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("COMPRA: FORMA DE PAGAMENTO\n\n");
    printf("               ");
    printf("1 - Cartao de credito\n");
    printf("               ");
    printf("2 - Cartao de debito\n");
    printf("               ");
    printf("3 - Boleto\n");
    printf("\nEscolha uma opcao de pagamento: ");
    int op, s;
    scanf("%d", &op);

    if(op==1){
        system("cls");
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("COMPRA: CARTAO DE DEBITO\n\n");
        printf("Nao se preocupe, seus dados bancarios nao serao salvos!\n\n");
        fflush(stdin);
        printf("Informe o numero do cartao (somente numeros): ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        printf("\nInforme o odigo de seguranca (cvc): ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        printf("\nInforme a quantidade de parcaelas (1 para pagamento  vista): ");
        scanf("%d", &s);
        fflush(stdin);
        fprintf(usuarios, "%s %d %d %d %d\n", u, cv, op, s, 0);
    }else if(op==2){
        system("cls");
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("COMPRA: CARTAO DE DEBITO\n\n");
        printf("Nao se preocupe, seus dados bancarios nao serao salvos!\n\n");
        fflush(stdin);
        printf("Informe o numero do cartao (somente numeros): ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        printf("\nInforme o odigo de seguranca (cvc): ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        fprintf(usuarios, "%s %d %d %d %d\n", u, cv, op, 1, 0);

    }else if(op==3){
        printf("\nBoleto gerado! Uma copia do PDF foi enviada para o seu e-mail.\n");
        printf("Voce tem ate 3 dias uteis para efetuar o pagamento!\n");
        fprintf(usuarios, "%s %d %d %d %d\n", u, cv, op, 1, 0);
    }else{
        printf("\nInforme uma forma de pagamento valida!\n");
        system("pause");
        cadPagamento(usuarios, cv, u);
    }
}

void cadDados(int cd){
    char aux[100];
    FILE *usuarios;
    usuarios = fopen("usuarios.txt", "a");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("CADASTRO\n\n");
    if(cd==1){
        printf("Informe seu CPF (sem pontuacao): ");

    }else if(cd==2){
        printf("Informe o CNPJ da instituicao (sem pontuacao): ");
    }
    fgets(aux, 100, stdin);
    fflush(stdin);

    FILE *buscas;
    buscas = fopen("usuarios.txt", "r");
    int i = buscaCad(buscas, aux);
    fclose(buscas);

    if(i==0){
        fprintf(usuarios, "%d\n", cd);
        fprintf(usuarios, "%s", aux);

        printf("\nCrie uma senha (no minimo 6 caracteres): ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        fprintf(usuarios, "%s", aux);

        printf("\nInforme o nome: ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        fprintf(usuarios, "%s", aux);

        printf("\nInforme o e-mail: ");
        fgets(aux, 100, stdin);
        fflush(stdin);
        fprintf(usuarios, "%s", aux);

        system("pause");
        system("cls");

        fputc('\n', usuarios);
        printf("Cadastro realizado com sucesso!\n\n");
        system("pause");
    }else{
        printf("Ja existe um usuario cadastrado com esse documento!\n\n");
        system("pause");
        system("cls");
    }
    fclose(usuarios);
}

void cadastro(Arv* tree, Vendas* venda){
    layout_cadastro();

    int cd;
    scanf("%d", &cd);
    fflush(stdin);

    if(cd==3){
        system("cls");
        return;
    }else if(cd!=1 && cd!=2){
        printf("Informe uma opcao valida!\n\n");
        system("pause");
        system("cls");
    }else{
        system("cls");
        cadDados(cd);
    }
    system("cls");
    cadastro(tree, venda);
}

void layout_pagInicial(){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("1 - Cadastre-se\n\n");
    printf("               ");
    printf("2 - Login\n\n");
    printf("               ");
    printf("3 - SAC\n\n");
    printf("Selecione 1 para fazer cadastro, selecione 2 para fazer login ou selecione 3 para acessar o SAC: ");
}

void layout_login(){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("LOGIN\n\n");
    printf("               ");
    printf("1 - Realizar Login como cliente\n\n");
    printf("               ");
    printf("2 - Realizar Login como gerente\n\n");
    printf("               ");
    printf("3 - Voltar\n\n");
    printf("Selecione 1 para fazer login como cliente, 2 para fazer login como gerente ou 3 para voltar: ");
}

void layoutMenuGerente(){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("MENU: GERENTE\n\n");
    printf("               ");
    printf("1 - Atualizar estoque\n\n");
    printf("               ");
    printf("2 - Relatorio de vendas\n\n");
    printf("               ");
    printf("3 - Relatorio de troca/devolucao\n\n");
    printf("               ");
    printf("4 - Relatorio de cancelamento\n\n");
    printf("               ");
    printf("5 - Sair\n\n");
    printf("Selecione uma opcao: ");
}

void exibeChave(Chave* ch, int j){
    printf("CODIGO: %d\n", ch->codigo);
    printf("NOME: %s\n", ch->nome);
    if(ch->mc==1){
        printf("REMEDIO CONTROLADO\n");
    }else{
        printf("REMEDIO NAO CONTROLADO\n");
    }
    if(j!=1){
        printf("ESTOQUE: %d\n", ch->estoque);
    }
    printf("VALOR: %.2lf\n", ch->valor);
}

void exibirCarrinho(Carrinho* c, int i, int j){
    if(c==NULL){
        if(i==0){
            printf("               ");
            printf("\nCarrinho vazio!\n");
        }
        return;
    }
    if(i==0){
        printf("               ");
        printf("VALOR TOTAL: %.2lf\n\n", c->valor);
        printf("PRODUTOS:\n\n");
        i=1;
    }
    exibeChave(c->ch, j);
    printf("QUANTIDADE: %d\n", c->qnt);
    printf("SUB-TOTAL: %.2lf\n\n", c->ch->valor*c->qnt);
    exibirCarrinho(c->prox, i, j);
}

void exibeEndereco(Endereco* add){
    char aux[50];
    strcpy(aux, add->numero);
    aux[strlen(aux)-1] = '\0';
    printf("Endereco: %s, ", aux);
    strcpy(aux, add->rua);
    aux[strlen(aux)-1] = '\0';
    printf("%s, ", aux);
    strcpy(aux, add->bairro);
    aux[strlen(aux)-1] = '\0';
    printf("%s, ", aux);
    strcpy(aux, add->cidade);
    aux[strlen(aux)-1] = '\0';
    printf("%s, ", aux);
    strcpy(aux, add->estado);
    aux[strlen(aux)-1] = '\0';
    printf("%s, ", aux);
    strcpy(aux, add->pais);
    aux[strlen(aux)-1] = '\0';
    printf("%s\n", aux);
    printf("CEP: %s\n", add->cep);
}

void exibePagamento(Pagamento* p){
    if(p->op==1){
        printf("Forma de pagamento: Cartao de Credito\n");
        printf("Quantidade de parcelas: %d\n", p->qntp);
    }else if(p->op==2){
        printf("Forma de pagamento: Cartao de Debito\n");
    }else{
        printf("Forma de pagamento: Boleto\n\n");
    }
}

void evendas(Vendas* venda){
            printf("               ");
            printf("Codigo da venda: %d\n", venda->codigo);
            printf("               ");
            printf("CPF/CNPJ do comprador: %s\n", venda->usuario);
            exibirCarrinho(venda->c, 0, 1);
            exibeEndereco(venda->add);
            exibePagamento(venda->p);
}

void exibeVendas(Vendas* venda, int i){
    if(venda!=NULL){
        if(i==0){
            if(venda->p->status==0){
                evendas(venda);
                printf("Status: A CAMINHO\n");
                printf("----------------------------\n");
            }else if(venda->p->status==1){
                evendas(venda);
                printf("Status: ENTREGUE\n");
                printf("----------------------------\n");
            }
        }else if(i==1){
            if(venda->p->status==2){
                evendas(venda);
                printf("Status: DEVOLUCAO\n");
                printf("----------------------------\n");
            }else if(venda->p->status==3){
                evendas(venda);
                printf("Status: TROCA\n");
                printf("----------------------------\n");
            }else if(venda->p->status==4){
                evendas(venda);
                printf("Status: TROCA/DEVOLUCAO REMANEJADA PARA O FABRICANTE\n");
                printf("----------------------------\n");
            }
        }else if(i==2){
            if(venda->p->status==5){
                evendas(venda);
                printf("Status: COMPRA CANCELADA\n");
                printf("----------------------------\n");
            }
        }
        exibeVendas(venda->prox, i);
    }
}

Arv* atualizaEstoqueGerente(Arv* tree, Chave* ch, int i){
    if(i==5 || (i<5 && tree->remedios[i]==NULL)){
        tree->filho = atualizaEstoqueGerente(tree->filho, ch, 0);
        return tree;
    }
    if(tree==NULL || tree->remedios[i]==NULL){
        return NULL;
    }
    if(ch->codigo==tree->remedios[i]->codigo){
        tree->remedios[i]->estoque = ch->estoque;
        return tree;
    }else{
        if(ch->codigo<tree->remedios[i]->codigo){
            tree->remedios[i]->filho = atualizaEstoqueGerente(tree->remedios[i]->filho, ch, 0);
            return tree;
        }else{
            if(ch->codigo>tree->remedios[i]->codigo){
                tree = atualizaEstoqueGerente(tree, ch, i+1);
                return tree;
            }
        }
    }
}

Arv* buscarP(Arv* tree){
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("BUSCAR PRODUTOS\n\n");
    printf("Informe o codigo do produto: ");
    int cd;
    scanf("%d", &cd);
    Chave* ch = buscarRemedio(tree, cd, 0);
    if(ch==NULL){
        printf("Condigo Invalido!\n\n");
    }else{
        exibeChave(ch, 0);
        printf("\n\n");
        printf("               ");
        printf("1 - Aicionar estoque ao produto\n");
        printf("               ");
        printf("2 - Voltar\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &cd);
        if(cd==1){
            printf("\nInfrme a quantidade que dejsa adicionar: ");
            scanf("%d", &cd);
            if(cd<=0){
                printf("\nErro! Tente realizar a operacao novamente!\n");
            }else{
                ch->estoque = ch->estoque + cd;
                tree = atualizaEstoqueGerente(tree, ch, 0);
                printf("\nProduto adicionado ao carrinho!\n\n");
            }
        }
    }
    system("pause");
    return tree;
}

void layout_adicionarEstoque(){
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("ADICIONAR ESTOQUE\n\n");
    printf("               ");
    printf("1 - Lista de produtos\n\n");
    printf("               ");
    printf("2 - Buscar produto\n\n");
    printf("               ");
    printf("3 - Voltar\n\n");
    printf("Escolha uma opcao: ");
}

Arv* adicionarEstoque(Arv* tree){
    layout_adicionarEstoque();
    int op;
    scanf("%d", &op);
    if(op==1){
            system("cls");
            printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
            printf("               ");
            printf("PRODUTOS\n\n");
            exibir(tree, 0);
    }else if(op==2){
            tree = buscarP(tree);
    }else if(op==3){
        return tree;
    }else{
        printf("\nInforme uma opcao valida!\n");
    }
    system("pause");
    return adicionarEstoque(tree);
}

int buscarCodRem(Arv* tree, int i){
    if(tree==NULL || tree->remedios[i]==NULL){
        return codigoVendas-1;
    }
        if(tree->remedios[i+1]==NULL){
            if(tree->filho==NULL || tree->filho->remedios[0]==NULL){
                return tree->remedios[i]->codigo;
            }else{
                return buscarCodRem(tree->filho, 0);
            }
        }else{
            return buscarCodRem(tree, i+1);
        }
}

Arv* adicionarNovo(Arv* tree){
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("ADICIONAR NOVO PRODUTO\n\n");
    int cd = buscarCodRem(tree, 0);
    cd++;
    char nome[100];
    int estoque, mc;
    double valor;
    printf("Informe o nome: ");
    fflush(stdin);
    scanf("%s", &nome);
    fflush(stdin);
    printf("Medicamento controlado? (1 para sim ou 0 para nao): ");
    scanf("%d", &mc);
    fflush(stdin);
    printf("Quantidade disponivel no estoque: ");
    scanf("%d", &estoque);
    fflush(stdin);
    printf("Valor: ");
    scanf("%lf", &valor);
    fflush(stdin);
    Pilha* p = NULL;
    tree = inserir(tree, cd, mc, estoque, valor, nome, tree, p);
    return tree;
}

void layout_editarEstoque(){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("EDITAR ESTOQUE\n\n");
        printf("               ");
        printf("1 - Adicionar estoque de um produto existente\n\n");
        printf("               ");
        printf("2 - Adicionar um novo produto\n\n");
        printf("               ");
        printf("3 - Voltar\n\n");
        printf("Escolha uma opcao: ");
}

Arv* editarEstoque(Arv* tree){
        layout_editarEstoque();
        int op;
        scanf("%d", &op);
        if(op==1){
            tree = adicionarEstoque(tree);
            FILE *remedios;
            remedios = fopen("remedios.txt", "w");
            atualizarFILE(tree, remedios, 0); //atualiza arquivo de remedios
            fclose(remedios);
        }else if(op==2){
            tree = adicionarNovo(tree);
            FILE *remedios;
            remedios = fopen("remedios.txt", "w");
            atualizarFILE(tree, remedios, 0); //atualiza arquivo de remedios
            fclose(remedios);
            printf("\nProduto adicionado!\n");
        }else if(op==3){
            return tree;
        }else{
            printf("\nInforme uma opcao valida!\n");
        }
        system("pause");
        system("cls");
        return editarEstoque(tree);
}

void layoutMenuCliente(Carrinho* c){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("MENU: CLIENTE\n\n");
    printf("               ");
    if(c==NULL){
        printf("VALOR DO CARRINHO: 0.00$\n\n");
    }else{
        printf("VALOR DO CARRINHO: %.2lf$\n\n", c->valor);
    }
    printf("               ");
    printf("1 - Buscar produto\n\n");
    printf("               ");
    printf("2 - Lista de produtos\n\n");
    printf("               ");
    printf("3 - Ver carrinho\n\n");
    printf("               ");
    printf("4 - Finalizar compra\n\n");
    printf("               ");
    printf("5 - Sair\n\n");
    printf("Selecione uma opcao: ");
}

Carrinho* desempilhar(Carrinho* c, Chave* ch, int qnt){
    if(c==NULL){
        return c;
    }else{
        if(c->ch==ch){
            if(qnt == c->qnt){
                c = c->prox;
            }else{
                c->qnt = c->qnt - qnt;
                c->valor = c->ch->valor*c->qnt;
            }
        }else{
            c->prox = desempilhar(c->prox, ch, qnt);
        }
    }
    return c;
}

Carrinho* buscarCarrinho(Carrinho* c, int cd){
    if(c==NULL){
        printf("\n\nEste produto nao esta no carrinho! Tente Novamente!\n");
        return NULL;
    }else{
        if(c->ch->codigo == cd){
            return c;
        }else{
            return buscarCarrinho(c->prox, cd);
        }
    }
}

Carrinho* atualizaValor(Carrinho* c){
    if(c==NULL){
        return c;
    }else{
        c->valor = c->ch->valor*c->qnt;
        c->prox = atualizaValor(c->prox);
    }
    return c;
}

Carrinho* somaValor(Carrinho* c){
    if(c->prox==NULL){
        return c;
    }else{
        c->prox = somaValor(c->prox);
        c->valor = c->valor + c->prox->valor;
        return c;
    }
}

Carrinho* buscarProduto(Arv* tree, Carrinho* c){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("BUSCAR PRODUTOS\n\n");
    printf("Informe o codigo do produto: ");
    int cd;
    scanf("%d", &cd);
    Chave* ch = buscarRemedio(tree, cd, 0);
    if(ch==NULL){
        printf("Condigo Invalido!\n\n");
    }else{
        exibeChave(ch, 0);
        printf("\n\n");
        printf("               ");
        printf("1 - Aicionar produto ao carrinho\n");
        printf("               ");
        printf("2 - Voltar\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &cd);
        if(cd==1){
            printf("\nInfrme a quantidade que dejsa desse produto: ");
            scanf("%d", &cd);
            if(cd<=0 || cd>ch->estoque){
                printf("\nEstoque indisponivel! Tente realizar a operacao novamente!\n");
            }else{
                c = inserirCar(ch, c, cd);
                printf("\nProduto adicionado ao carrinho!\n\n");
            }
        }
    }
    return c;
}

Carrinho* atualizaCarrinho(Carrinho* c, Arv* tree){
    int cd, q;
    printf("\n\n");
    printf("               ");
    printf("1 - Remover produto ao carrinho\n");
    printf("               ");
    printf("2 - Voltar\n");
    printf("\nEscolha uma opcao: ");
    scanf("%d", &cd);
    if(cd==1){
        printf("\nInforme o codigo do produto que deseja edidar: ");
        scanf("%d", &cd);
        Carrinho* aux = buscarCarrinho(c, cd);
        if(aux==NULL){
            return c;
        }else{
            printf("\nInforme a quantidade que deseja remover: ");
            scanf("%d", &q);
            if(q<=0 || q>aux->qnt){
                printf("\nQuantidade invalida! Tente novamente!\n");
            }else{
                Chave* ch = buscarRemedio(tree, cd, 0);
                 c = desempilhar(c, ch, q);
                 c = atualizaValor(c);
                 c = somaValor(c);
            }
            return c;
        }
    }else{
        return c;
    }
}

int verificaRemControlado(Carrinho* c){
    if(c==NULL){
        return 0;
    }else{
        if(c->ch->mc==1){
            return 1;
        }else{
            return verificaRemControlado(c->prox);
        }
    }
}

void escreverVenda(FILE *v, Carrinho* c, int cv, char u[]){
    if(c!=NULL){
        if(cv!=0){
            fprintf(v, "%s %d %.2lf", u, cv, c->valor);
            cv = 0;
        }
        fprintf(v, " %d %d", c->ch->codigo, c->qnt);
        escreverVenda(v, c->prox, cv, u);
    }
}

void ctVendas(FILE *ve, Vendas* venda){
    if(venda!=NULL){
        escreverVenda(ve, venda->c, venda->codigo, venda->usuario);
        fputc('\n', ve);
        ctVendas(ve, venda->prox);
    }
}

Endereco* openFileEndereco(FILE *enderecos, int i, int cv){
    char aux[50];
    if(fgets(aux, 50, enderecos)!=NULL){
        i++;
        if(i==10){
            i=0;
        }
        if(i==2){
            int k = strtol(aux, NULL, 10);
            if(k==cv){
                char pais[40], estado[30], bairro[50], cep[15], rua[30], numero[5], cidade[30];
                fgets(pais, 40, enderecos);
                fgets(estado, 30, enderecos);
                fgets(cidade, 30, enderecos);
                fgets(bairro, 50, enderecos);
                fgets(rua, 30, enderecos);
                fgets(numero, 50, enderecos);
                fgets(cep, 15, enderecos);
                Endereco* novo = novoEndereco(pais, estado, cidade, bairro, rua, numero, cep);
                return novo;
            }
        }
        return openFileEndereco(enderecos, i, cv);
    }
}

Pagamento* openFilePagamentos(FILE *pagamentos, int cv){
    char u[15];
    int cd, st, qnt, op;
    if(fscanf(pagamentos, "%s %d %d %d %d", &u, &cd, &op, &qnt, &st)!=EOF){
        if(cd==cv){
            Pagamento* novo = novoPagamento(u, cd, op, qnt, st);
            return novo;
        }
        return openFilePagamentos(pagamentos, cv);
    }

}

Carrinho* readFileVendas(FILE *v, Carrinho* c, Arv* tree){
    char var1[30], var2[30];
    if(fscanf(v, "%s %s", var1, var2)!=EOF){
        int k = strtol(var1, NULL, 10);
        int q = strtol(var2, NULL, 10);
        Chave* ch = NULL;
        ch = buscarRemedio(tree, k, 0);
        c = inserirCar(ch, c, q);
        if(fgetc(v)=='\n'){
            return c;
        }else{
            return readFileVendas(v, c, tree);
        }
    }
    return c;
}

Vendas* openFileVendas(Vendas* venda, Arv* tree, FILE *v, Carrinho* c, Pagamento* p, Endereco* add){

    char aux[30], aux2[30], aux3[30];

    if(fscanf(v, "%s %s", aux, aux2)!=EOF){
        c = NULL;
        add = NULL;
        p = NULL;
            fscanf(v, "%s", aux3);
            int k = strtol(aux2, NULL, 10);
            c = readFileVendas(v, c, tree);

            FILE *enderecos;
            enderecos = fopen("enderecos.txt", "r");
            add = openFileEndereco(enderecos, 0, k);
            fclose(enderecos);

            FILE *pagamentos;
            pagamentos = fopen("fpagamento.txt", "r");
            p = openFilePagamentos(pagamentos, k);

            venda = inserirVenda(venda, aux, k, p, c, add);


        venda = openFileVendas(venda, tree, v, c, p, add);
    }
    return venda;
}

Vendas* openFILEs(Arv* tree, Vendas* venda){
    FILE *v;
    v = fopen("vendas.txt", "r");
    Carrinho* c = NULL;
    Pagamento* p = NULL;
    Endereco* add = NULL;
    venda = openFileVendas(venda, tree, v, c, p, add);
    fclose(v);
    return venda;
}

Vendas* readFileSolicitacao(Vendas* venda, Arv* tree, FILE *v, Carrinho* c, Pagamento* p, Endereco* add){

    char aux[30], aux2[30], aux3[30];

    if(fscanf(v, "%s %s", aux, aux2)!=EOF){
        c = NULL;
        add = NULL;
        p = NULL;
            fscanf(v, "%s", aux3);
            int k = strtol(aux2, NULL, 10);
            c = readFileVendas(v, c, tree);

            FILE *enderecos;
            enderecos = fopen("enderecos.txt", "r");
            add = openFileEndereco(enderecos, 0, k);
            fclose(enderecos);

            FILE *pagamentos;
            pagamentos = fopen("fp.txt", "r");
            p = openFilePagamentos(pagamentos, k);

            venda = inserirVenda(venda, aux, k, p, c, add);


        venda = readFileSolicitacao(venda, tree, v, c, p, add);
    }
    return venda;
}

Vendas* openFileSolicitacao(Arv* tree, Vendas* venda){
    FILE *v;
    v = fopen("solicitacoes.txt", "r");
    Carrinho* c = NULL;
    Pagamento* p = NULL;
    Endereco* add = NULL;
    venda = readFileSolicitacao(venda, tree, v, c, p, add);
    fclose(v);
    return venda;
}
Arv* atualizaEstoque(Arv* tree, Carrinho* c, int i){
    if(i==5 || (i<5 && tree->remedios[i]==NULL)){
        tree->filho = atualizaEstoque(tree->filho, c, 0);
        return tree;
    }
    if(tree==NULL || tree->remedios[i]==NULL){
        return NULL;
    }
    if(c->ch->codigo==tree->remedios[i]->codigo){
        tree->remedios[i]->estoque = tree->remedios[i]->estoque - c->qnt;
        return tree;
    }else{
        if(c->ch->codigo<tree->remedios[i]->codigo){
            tree->remedios[i]->filho = atualizaEstoque(tree->remedios[i]->filho, c, 0);
            return tree;
        }else{
            if(c->ch->codigo>tree->remedios[i]->codigo){
                tree = atualizaEstoque(tree, c, i+1);
                return tree;
            }
        }
    }
}

Arv* attEstoque(Arv* tree, Carrinho* c){
    if(c==NULL){
        return tree;
    }else{
        tree = atualizaEstoque(tree, c, 0);
        return attEstoque(tree, c->prox);
    }
}

void menuGerente(Arv* tree, Vendas* venda, Vendas* solicitacao){
    layoutMenuGerente();
    int op;
    scanf("%d", &op);
    system("cls");
    if(op==1){
        tree = editarEstoque(tree);
    }else if(op==2){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("RELATORIO DE VENDAS\n\n");
        exibeVendas(venda, 0);
    }else if(op==3){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("RELATORIO DE TROCA/DEVOLUCAO\n\n");
        exibeVendas(solicitacao, 1);
    }else if(op==4){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("RELATORIO DE CANCELAMENTOS\n\n");
        exibeVendas(solicitacao, 2);
    }else if(op==5){
        system("cls");
        printf("               ");
        printf("Deseja deslogar da conta?\n\n");
        printf("               ");
        printf("1 - sim\n\n");
        printf("               ");
        printf("2 - sim\n\n");
        printf("Escolha uma opcao: ");
        scanf("%d", &op);
        if(op==1){
            return;
        }

    }else{
        printf("\nInforme uma opcao valida!\n");
    }
    system("pause");
    system("cls");
    menuGerente(tree, venda, solicitacao);
}

void loginGerente(Arv* tree, Vendas* venda, Vendas* solicitacao){
    char aux[10];
    int i=0;
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("LOGIN\n\n");
    printf("Informe o nome de usuario: ");
    scanf("%s", &aux);
    fflush(stdin);
    if(strcmp(aux, user)!=0) i=1;
    printf("\n\nInforme a senha: ");
    scanf("%s", &aux);
    fflush(stdin);
    if(strcmp(aux, user)!=0) i=1;

    if(i==1){
        printf("Usuario ou senha incorretos!\n\n");
        system("pause");
        system("cls");
    }else{
        system("cls");
        printf("\n\nLogin realizado com sucesso!\n\n");
        system("pause");
        system("cls");
        menuGerente(tree, venda, solicitacao);
    }
}

void menuCliente(Arv* tree, Carrinho* c, int cv, char u[], Vendas* venda){
    layoutMenuCliente(c);
    int op;
    scanf("%d", &op);
    system("cls");

    if(op==1){
        c = buscarProduto(tree, c);
    }else if(op==2){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("PRODUTOS\n\n");
        exibir(tree, 0);
    }else if(op==3){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("CARRINHO:\n\n");
        exibirCarrinho(c, 0, 0);
        if(c!=NULL){
            c = atualizaCarrinho(c, tree);
        }
    }else if(op==4){
        if(c!=NULL){
            printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
            printf("               ");
            printf("COMPRA: ENDERECO DE ENTREGA\n\n");
            FILE *enderecos;
            enderecos = fopen("enderecos.txt", "a");
            cadEndereco(enderecos, cv, u);
            fclose(enderecos);
            system("pause");
            system("cls");

            FILE *v;
            v = fopen("vendas.txt", "a");
            escreverVenda(v, c, cv, u);
            fputc('\n', v);
            fclose(v);

            FILE *pagamentos;
            pagamentos = fopen("fpagamento.txt", "a");
            cadPagamento(pagamentos, cv, u);
            fclose(pagamentos);
            system("pause");
            system("cls");

            printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");

            int i = verificaRemControlado(c);
            if(i==1){
                printf("               ");
                printf("CODIGO DE CONDUTA PARA MEDICAMENTOS CONTROLADOS\n\n");
                printf("Verificamos que existe um ou mais medicamentos controlados em sua compra!\n");
                printf("Sua compra esta em processo de analise, para podermos concluir o processo\n");
                printf("envie o ALVARA SANITARIO da empresa em que o medicamento sera utilizado!\n");
                printf("Envie o documento para farmavidadistribuidora@gmail.com com o codigo %d\n", cv);
                printf("da compra para podermos terminar a analise e finalizar a compra!\n\n");

                system("pause");
                printf("\nEstamos aguardando o envio...\n");
                Sleep(1000);
                printf("\nE-mail recebido, estamos analisando o documento, por favor aguarde...\n");
                Sleep(1000);
                printf("\nDocumento em ordem!\n\n");
            }
            printf("Compra finalizada! Dentro de alguns dias o produto sera entregue!\n\n");
            tree = attEstoque(tree, c);//atualiza esqtoque
            FILE *remedios;
            remedios = fopen("remedios.txt", "w");
            atualizarFILE(tree, remedios, 0); //atualiza arquivo de remedios
            fclose(remedios);

            c = NULL;
            cv++;
        }else{
            printf("\nCarrinho vazio, para realizar uma compra adicone um produto ao carronho!\n\n");
        }
    }else if(op==5){
        printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
        printf("               ");
        printf("Voce sera deslogado e seu carrinho sera esvaziado. Deseja continuar?\n");
        printf("               ");
        printf("1 - Sim\n");
        printf("               ");
        printf("2 - Nao\n");
        printf("\nEscolha uma opcao: ");
        scanf("%d", &op);
        if(op==1){
            return;
        }
    }
    system("pause");
    system("cls");
    menuCliente(tree, c, cv, u, venda);
}

void loginCliente(Arv* tree, int cv, Vendas* venda){
    char aux[15], u[15];
    int i, j;

    FILE *usuarios;
    usuarios = fopen("usuarios.txt", "r");

    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("LOGIN\n\n");
    printf("Informe o CPF/CNPJ (somente numeros): ");
    scanf("%s", &u);
    fflush(stdin);
    i = buscaUser(usuarios, u, 0);

    printf("\n\nInforme a senha: ");
    scanf("%s", &aux);
    fflush(stdin);
    j = buscaUser(usuarios, aux, i);

    if(i==-1){
        printf("Esse usuario nao existe!\n\n");
    }else if(j==-1 || (i!=j)){
        printf("Senha incorreta!\n\n");
    }else if(i==j){
        system("cls");
        printf("\n\nLogin realizado com sucesso\n\n");
        system("pause");
        system("cls");
        Carrinho* c = NULL;
        return menuCliente(tree, c, cv, u, venda);
    }
}

void login(Arv* tree, Vendas* venda, Vendas* solicitacao){
    layout_login();
    int lg;
    int cv = codigoVendaAtual(venda) + 1;
    scanf("%d", &lg);
    fflush(stdin);
    system("cls");
    if(lg==1){
        return loginCliente(tree, cv, venda);
    }else if(lg==2){
        return loginGerente(tree, venda, solicitacao);
    }else if(lg==3){
        return;
    }else{
        printf("Informe uma opcao valida!\n\n");
    }
    system("pause");
    system("cls");
    login(tree, venda, solicitacao);
}

void layout_garantia(){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("GARANTIA\n\n");
    printf("               Ha quanto tempo voce recebeu o produto?\n");
    printf("               ");
    printf("1 - 7 dias ou menos\n\n");
    printf("               ");
    printf("2 - Mais de 7 dias\n\n");
    printf("               ");
    printf("3 - Sair\n");
    printf("Escolha uma opcao: ");
}

void layout_7d(){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("PRODUTO DENTRO DA GARANTIA DA FARMAVIDA\n\n");
    printf("               ");
    printf("1 - Solicitar troca\n\n");
    printf("               ");
    printf("2 - Solicitar devolucao\n\n");
    printf("               ");
    printf("3 - Sair\n");
    printf("Escolha uma opcao: ");
}

void layout_sac(){
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("               ");
    printf("SAC - SERVICO DE ATENDIMENTO AO CONSUMIDOR\n\n");
    printf("               ");
    printf("1 - Garantia do produto\n\n");
    printf("               ");
    printf("2 - Cancelamento ou desistencia de compra\n\n");
    printf("               ");
    printf("3 - Sair\n\n");
    printf("Selecione uma opcao: ");
}

Vendas* buscarVenda(Vendas* venda, int codigo){
    if(venda==NULL){
        return NULL;
    }else{
        if(venda->codigo == codigo){
            return venda;
        }else{
            return buscarVenda(venda->prox, codigo);
        }
    }
}

Vendas* removerVenda(Vendas* venda, int codigo){
    if(venda->codigo == codigo){
        Vendas* aux = venda->prox;
        free(venda);
        return aux;
    }else{
        venda->prox = removerVenda(venda->prox, codigo);
        return venda;
    }
}

void atualizarVendas(FILE *fp, Vendas* venda){
    if(venda!=NULL){
        fprintf(fp, "%s %d %d %d %d\n", venda->usuario, venda->codigo, venda->p->op, venda->p->qntp, venda->p->status);
        atualizarVendas(fp, venda->prox);
    }
}

Vendas* solicitarTroca(Vendas* venda, Vendas* solicitacao){
    char doc[15];
    int cod;
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("Informe o CPF/CNPJ do comprador: ");
    fflush(stdin);
    fgets(doc, 15, stdin);
    fflush(stdin);
    doc[strlen(doc)-1]='\0';
    printf("Informe o codigo da compra: ");
    scanf("%d", &cod);

    Vendas* aux = buscarVenda(venda, cod);
    if(aux == NULL){
        printf("\nEssa venda nao existe!\n");
        return solicitacao;
    }else{
        if(strcmp(aux->usuario, doc)!=0){
            printf("\nEssa venda nao pertence a esse usuario!\n");
            return solicitacao;
        }else{
            if(aux->p->status==0){
                printf("\nO produto ainda nao foi entregue, voce nao pode realizar essa operacao!\n");
                return solicitacao;
            }
            aux->p->status = 3;
             venda = removerVenda(venda, cod);

            FILE *fp;
            fp = fopen("fpagamento.txt", "w");
            atualizarVendas(fp, venda);
            fclose(fp);

            FILE *ve;
            ve = fopen("vendas.txt", "w");
            ctVendas(ve, venda);
            fclose(ve);

            solicitacao = inserirVenda(solicitacao, doc, aux->codigo, aux->p, aux->c, aux->add);
            FILE *sl;
            sl = fopen("solicitacoes.txt", "w");
            ctVendas(sl, solicitacao);
            fclose(sl);

            FILE *pf;
            pf = fopen("fp.txt", "w");
            atualizarVendas(pf, solicitacao);
            fclose(pf);

            printf("\nSolicitacao de troca foi realizada com sucesso!\n");
            printf("\nTemos 30 (trinta) dias de prazo para analisar se sua solicitacao preenche os requisitos de nossa Politica de Troca e Devolucao, a contar a partir da data que recebeu o produto. Caso preencha, enviaremos outro produto como solicitado. \n\n");
            return solicitacao;
        }
    }
}

Vendas* solicitarDevolucao(Vendas* venda, Vendas* solicitacao){
    char doc[15];
    int cod;
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("Informe o CPF/CNPJ do comprador: ");
    fflush(stdin);
    fgets(doc, 15, stdin);
    fflush(stdin);
    doc[strlen(doc)-1]='\0';
    printf("Informe o codigo da compra: ");
    scanf("%d", &cod);

    Vendas* aux = buscarVenda(venda, cod);
    if(aux == NULL){
        printf("\nEssa venda nao existe!\n");
        return solicitacao;
    }else{
        if(strcmp(aux->usuario, doc)!=0){
            printf("\nEssa venda nao pertence a esse usuario!\n");
            return solicitacao;
        }else{
            if(aux->p->status==0){
                printf("\nO produto ainda nao foi entregue, voce nao pode realizar essa operacao!\n");
                return solicitacao;
            }
            aux->p->status = 2;

            venda = removerVenda(venda, cod);

            FILE *fp;
            fp = fopen("fpagamento.txt", "w");
            atualizarVendas(fp, venda);
            fclose(fp);

            FILE *ve;
            ve = fopen("vendas.txt", "w");
            ctVendas(ve, venda);
            fclose(ve);

            solicitacao = inserirVenda(solicitacao, doc, aux->codigo, aux->p, aux->c, aux->add);
            FILE *sl;
            sl = fopen("solicitacoes.txt", "w");
            ctVendas(sl, solicitacao);
            fclose(sl);

            FILE *pf;
            pf = fopen("fp.txt", "w");
            atualizarVendas(pf, solicitacao);
            fclose(pf);

            printf("\nSolicitacao de devolucao realizada com sucesso!\n");
            printf("\nNossa equipe esta ciente do seu desejo de devolucao, no seu email foram enviadas instrucoes sobre o estado e encaminhamento do produto.\n\n");
            printf("A restituicao de pagamento sera feita pela mesma forma de pagamento escolhida na hora da compra.\n");
            printf("Enviaremos mais informacoes por email apos a confirmacao da devolucao!\n");
            return solicitacao;
        }
    }
}

Vendas* foraDaGarantia(Vendas* venda, Vendas* solicitacao){
    char doc[15];
    int cod;
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("Informe o CPF/CNPJ do comprador: ");
    fflush(stdin);
    fgets(doc, 15, stdin);
    fflush(stdin);
    doc[strlen(doc)-1]='\0';
    printf("Informe o codigo da compra: ");
    scanf("%d", &cod);
    Vendas* aux = buscarVenda(venda, cod);
    if(aux == NULL){
        printf("\nEssa venda nao existe!\n");
        return solicitacao;
    }else{
        if(strcmp(aux->usuario, doc)!=0){
            printf("\nEssa venda nao pertence a esse usuario!\n");
            return solicitacao;
        }else{
            if(aux->p->status==0){
                printf("\nO produto ainda nao foi entregue, voce nao pode realizar essa operacao!\n");
                return solicitacao;
            }
            aux->p->status = 4;
             venda = removerVenda(venda, cod);

            FILE *fp;
            fp = fopen("fpagamento.txt", "w");
            atualizarVendas(fp, venda);
            fclose(fp);

            FILE *ve;
            ve = fopen("vendas.txt", "w");
            ctVendas(ve, venda);
            fclose(ve);

            solicitacao = inserirVenda(solicitacao, doc, aux->codigo, aux->p, aux->c, aux->add);
            FILE *sl;
            sl = fopen("solicitacoes.txt", "w");
            ctVendas(sl, solicitacao);
            fclose(sl);

            FILE *pf;
            pf = fopen("fp.txt", "w");
            atualizarVendas(pf, solicitacao);
            fclose(pf);

            printf("\nInfelizmente seu produto esta fora da garantia da FARMAVIDA DISTRIBUIDORA!\n");
            printf("\nNossa equipe enviou para o seu email instrucoes para realizar o contato com o fabricante!\n\n");
            return solicitacao;
        }
    }
}

Vendas* cancelamento(Vendas* venda, Vendas* solicitacao){
    char doc[15];
    int cod;
    system("cls");
    printf("-------------- FARMAVIDA DISTRIBUIDORA --------------\n\n");
    printf("Informe o CPF/CNPJ do comprador: ");
    fflush(stdin);
    fgets(doc, 15, stdin);
    fflush(stdin);
    doc[strlen(doc)-1]='\0';
    printf("Informe o codigo da compra: ");
    scanf("%d", &cod);

    Vendas* aux = buscarVenda(venda, cod);
    if(aux == NULL){
        printf("\nEssa venda nao existe!\n");
        return solicitacao;
    }else{
        if(strcmp(aux->usuario, doc)!=0){
            printf("\nEssa venda nao pertence a esse usuario!\n");
            return solicitacao;
        }else{
            if(aux->p->status!=0){
                printf("\nO produto ja foi entregue, voce nao pode realizar essa operacao!\n");
                return solicitacao;
            }
            aux->p->status = 5;
             venda = removerVenda(venda, cod);

            FILE *fp;
            fp = fopen("fpagamento.txt", "w");
            atualizarVendas(fp, venda);
            fclose(fp);

            FILE *ve;
            ve = fopen("vendas.txt", "w");
            ctVendas(ve, venda);
            fclose(ve);

            solicitacao = inserirVenda(solicitacao, doc, aux->codigo, aux->p, aux->c, aux->add);
            FILE *sl;
            sl = fopen("solicitacoes.txt", "w");
            ctVendas(sl, solicitacao);
            fclose(sl);

            FILE *pf;
            pf = fopen("fp.txt", "w");
            atualizarVendas(pf, solicitacao);
            fclose(pf);

            printf("\nRecebemos sua solicitacao de cancelamento!\n");
            printf("\nQuando o produto for entregue recuse-o!\n\n");
            printf("A restituicao de pagamento sera feita pela mesma forma de pagamento escolhida na hora da compra.\n");
            printf("Enviaremos mais informacoes por email apos a confirmacao do cancelamento!\n");
            return solicitacao;
        }
    }
}

Vendas* solicitacoes(Vendas* venda, Vendas* solicitacao){
    system("cls");
    layout_7d();
    int op;
    scanf("%d", &op);
    if(op==1){
        solicitacao = solicitarTroca(venda, solicitacao);
    }else if(op==2){
        solicitacao = solicitarDevolucao(venda, solicitacao);
    }else if(op==3){
        return solicitacao;
    }else{
        printf("Escolha uma opcao valida!\n\n");
    }
    system("pause");
    system("cls");
    return solicitacoes(venda, solicitacao);
}

Vendas* garantia(Vendas* venda, Vendas* solicitacao){
    system("cls");
    layout_garantia();
    int op;
    scanf("%d", &op);
    if(op==1){
        solicitacao = solicitacoes(venda, solicitacao);
    }else if(op==2){
        solicitacao = foraDaGarantia(venda, solicitacao);
    }else if(op==3){
        return solicitacao;
    }else{
        printf("Escolha uma opcao valida!\n\n");
    }
    system("pause");
    system("cls");
    garantia(venda, solicitacao);
}

void sac(Arv* tree, Vendas* venda, Vendas* solicitacao){
    system("cls");
    layout_sac();
    int op;
    scanf("%d", &op);
    if(op==1){
        solicitacao = garantia(venda, solicitacao);
    }else if(op==2){
        solicitacao = cancelamento(venda, solicitacao);
    }else if(op==3){
        return;
    }else{
        printf("Escolha uma opcao valida!\n\n");
    }
    system("pause");
    system("cls");
    sac(tree, venda, solicitacao);
}

//10013 Venlafaxina 1 24 53.00
//10014 ferripolimaltose 0 20 36.70

void pagInicial(Arv* tree, Vendas* venda, Vendas* solicitacao){
    tree = NULL;
    tree = openFileRem(tree);
    venda = NULL;
    venda = openFILEs(tree, venda);
    solicitacao = NULL;
    solicitacao = openFileSolicitacao(tree, solicitacao);

    layout_pagInicial();
    char op[2];
    scanf("%s", &op);
    if(strlen(op)==1){
        if(op[0]=='1'){
            system("cls");
            cadastro(tree, venda);
        }else if(op[0]=='2'){
            system("cls");
            login(tree, venda, solicitacao);
        }else if(op[0]=='3'){
            sac(tree, venda, solicitacao);
        }else{
            printf("Escolha uma opcao valida!\n\n");
            system("pause");
        }
    }else{
        printf("Escolha uma opcao valida!\n\n");
        system("pause");
    }
    system("cls");
    pagInicial(tree, venda, solicitacao);
}

int main()
{
    Arv* remedios = NULL;
    Vendas* venda = NULL;
    Vendas* solicitacao = NULL;
    pagInicial(remedios, venda, solicitacao);
    return 0;
}
