#ifndef SNAKE_H
#define SNAKE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "raylib.h"

#define LARGURA_PADRAO 820
#define ALTURA_PADRAO 820
#define TAM_GRID 40      
#define MAX_BARREIRAS 50 
#define MAX_SCORE 5      
#define ARQUIVO_RANKING "ranking.dat"

typedef enum { 
    TELA_MENU, 
    TELA_JOGO, 
    TELA_RANKING, 
    TELA_CONFIG,
    TELA_NOME,    
    TELA_ENDING
} EstadoTela;

typedef enum{
    Opcao_iniciar = 0,
    Opcao_ranking,
    Opcao_config,
    Opcao_sair
} OpcoesMenu;

typedef enum{
    CONF_MAPA = 0,
    CONF_RESOLUCAO,
    CONF_VOLTAR
} OpcoesConfig;

typedef struct menu{
    int Opcao_select;
    char Nome_player[20];
} MENU;

typedef struct corpo{
    Rectangle pos; 
    struct corpo *prox;
    struct corpo *ant;
} Body;

typedef struct cobra{
    Body* cabeca;
    Body* cauda;
    int tamanho;
    int direcao; 
    Color cor;
} Snake;

typedef struct ranking{
    char Nome_ranker[20];
    int pontuacao;
} Score;

typedef struct jogo{
    EstadoTela estado; 
    MENU menu;
    Snake cobra;
    Score HighScores[MAX_SCORE]; 

    char nome_buffer[20]; 
    int qtd_letras;       

    Rectangle Comida;
    Color cor_comida;
    
    Texture2D tex_comida; 
    Texture2D tex_background;

    Sound snd_comer;      
    Sound snd_morrer;     
    Music musica_fundo;

    float timer_efeito_sonoro;

    Rectangle barreiras[MAX_BARREIRAS]; 
    int mapa_selec;
    int qtd_barreiras;
    int id_mapa;

    int largura_tela;      
    int altura_tela;       
    int conf_selecionada;  

    int pontuacao_atual;
    int game_over; 
    double tempo_movimento; 
    
    int input_buffer[2];
    int buffer_count;

} Game;

// Funções de Inicialização e Menu
void inicia_jogo(Game *g);
void atualiza_menu(Game *g);
void desenha_menu(Game *g);

// Funções de Configuração, Ranking e Input
void Atualiza_config(Game *g);
void DesenhaConfig(Game *g);
void Carregar_ranking(Game *g);
int Verifica_Novo_Recorde(Game *g); 
void Salvar_pontuacao(Game *g);
void DesenhaRanking(Game *g);
void Atualiza_Input_Nome(Game *g);
void Desenha_Input_Nome(Game *g);

// Funções de Mapa e Jogo
void Carregar_mapa(Game *g, int id_mapa);
void DesenhaJogo(Game *g);
void Posicionar_Comida(Game *g);

// Funções da Cobra
void iniciar_cobra(Game *g);
void mover_cobra(Game *g);
void liberar_cobra(Game *g);

// Funções de Assets
void CarregarAssets(Game *g);
void AtualizarMusica(Game *g);
void DescarregarAssets(Game *g);

#endif