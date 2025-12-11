#include "snake.h"

//função auxiliar

Body* criar_segmento(float x, float y){
    Body *aux = (Body*)malloc(sizeof(Body));

    if(aux == NULL){
        printf("ERRO NO MALLOC\n");
        exit(1);
    }

    aux->pos = (Rectangle){x, y, TAM_GRID, TAM_GRID};
    aux->ant = NULL;
    aux->prox = NULL;

    return aux;
}

//gestão de arquivos

void Carregar_ranking(Game *g) {
    FILE *arquivo = fopen(ARQUIVO_RANKING, "rb");
    
    if (arquivo == NULL) {
        for(int i=0; i < MAX_SCORE; i++) {
            sprintf(g->HighScores[i].Nome_ranker, "---");
            g->HighScores[i].pontuacao = 0;
        }
        return;
    }
    
    fread(g->HighScores, sizeof(Score), MAX_SCORE, arquivo);
    fclose(arquivo);
}

int Verifica_Novo_Recorde(Game *g) {
    if (g->pontuacao_atual > g->HighScores[MAX_SCORE-1].pontuacao) {
        return 1;
    }
    return 0;
}

void Salvar_pontuacao(Game *g) {
    strcpy(g->HighScores[MAX_SCORE-1].Nome_ranker, g->nome_buffer);
    g->HighScores[MAX_SCORE-1].pontuacao = g->pontuacao_atual;

    for (int i = 0; i < MAX_SCORE - 1; i++) {
        for (int j = 0; j < MAX_SCORE - i - 1; j++) {
            if (g->HighScores[j].pontuacao < g->HighScores[j+1].pontuacao) {
                Score temp = g->HighScores[j];
                g->HighScores[j] = g->HighScores[j+1];
                g->HighScores[j+1] = temp;
            }
        }
    }

    FILE *arquivo = fopen(ARQUIVO_RANKING, "wb");
    fwrite(g->HighScores, sizeof(Score), MAX_SCORE, arquivo);
    fclose(arquivo);
}

void DesenhaRanking(Game *g) {
    ClearBackground(BLACK);
    int centroX = g->largura_tela / 2;
    
    DrawText("TOP 5 JOGADORES", centroX - MeasureText("TOP 5 JOGADORES", 40)/2, 50, 40, GOLD);

    for (int i = 0; i < MAX_SCORE; i++) {
        char buffer[60];
        sprintf(buffer, "%d. %s ........ %d", i+1, g->HighScores[i].Nome_ranker, g->HighScores[i].pontuacao);
        
        Color cor = (i == 0) ? GREEN : WHITE;
        DrawText(buffer, centroX - 150, 150 + (i*60), 30, cor);
    }
    
    DrawText("Pressione [ENTER] para Voltar", centroX - MeasureText("Pressione [ENTER] para Voltar", 20)/2, g->altura_tela - 100, 20, GRAY);
}

void Atualiza_Input_Nome(Game *g) {
    int key = GetCharPressed();
    while (key > 0) {
        if ((key >= 32) && (key <= 125) && (g->qtd_letras < 15)) {
            g->nome_buffer[g->qtd_letras] = (char)key;
            g->qtd_letras++;
            g->nome_buffer[g->qtd_letras] = '\0';
        }
        key = GetCharPressed();
    }

    if (IsKeyPressed(KEY_BACKSPACE)) {
        if (g->qtd_letras > 0) {
            g->qtd_letras--;
            g->nome_buffer[g->qtd_letras] = '\0';
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (g->qtd_letras == 0) strcpy(g->nome_buffer, "Anonimo");
        Salvar_pontuacao(g);
        g->estado = TELA_RANKING; 
    }
}

void Desenha_Input_Nome(Game *g) {
    DesenhaJogo(g);
    DrawRectangle(0, 0, g->largura_tela, g->altura_tela, Fade(BLACK, 0.85f));

    int cx = g->largura_tela / 2;
    int cy = g->altura_tela / 2;

    DrawText("NOVO RECORDE!", cx - MeasureText("NOVO RECORDE!", 40)/2, cy - 100, 40, GOLD);
    DrawText("Digite seu nome:", cx - MeasureText("Digite seu nome:", 20)/2, cy - 40, 20, WHITE);

    DrawRectangleLines(cx - 150, cy, 300, 50, GREEN);
    DrawText(g->nome_buffer, cx - MeasureText(g->nome_buffer, 30)/2, cy + 10, 30, GREEN);
    
    DrawText("Pressione [ENTER] para Salvar", cx - MeasureText("Pressione [ENTER] para Salvar", 20)/2, cy + 80, 20, GRAY);
}

//limpeza de memoria

void liberar_cobra(Game *g) {
    Body* atual = g->cobra.cabeca;
    while (atual != NULL) {
        Body* proximo = atual->prox;
        free(atual);
        atual = proximo;
    }
    g->cobra.cabeca = NULL;
    g->cobra.cauda = NULL;
    g->cobra.tamanho = 0;
}

//forma de criar mostrando o menu

void inicia_jogo(Game *g){
    //estado do game e menu
    g->estado = TELA_MENU;
    g->menu.Opcao_select = 0;

    //variaveis de resolução padrão
    g->largura_tela = LARGURA_PADRAO;
    g->altura_tela = ALTURA_PADRAO;
    g->conf_selecionada = 0;

    //variavel de "perdeu"
    g->game_over = 0;

    //variavel do mapa atual
    g->mapa_selec = 1; 
    
    g->timer_efeito_sonoro = 0.0f;
    
    g->input_buffer[0] = -1;
    g->input_buffer[1] = -1;
    g->buffer_count = 0;

    Carregar_ranking(g);
}

//atualizando o menu

void atualiza_menu(Game *g){
    //navegação

    if(IsKeyPressed(KEY_DOWN)){
        g->menu.Opcao_select ++;
        if(g->menu.Opcao_select > 3){
            g->menu.Opcao_select = 0;
        }
    }

    if(IsKeyPressed(KEY_UP)){
        g->menu.Opcao_select --;
        if(g->menu.Opcao_select < 0){
            g->menu.Opcao_select = 3;
        }
    }
    
    if(IsKeyPressed(KEY_ENTER)){
        switch(g->menu.Opcao_select){
            //escolha do usuario
            case Opcao_iniciar:
                {
                    Image img = LoadImage("assets/background.jpeg");
                    if (img.data == NULL) img = LoadImage("assets/background.jpg"); 
                    
                    if (img.data != NULL) {
                        ImageResize(&img, g->largura_tela, g->altura_tela);
                        if (g->tex_background.id > 0) UnloadTexture(g->tex_background);
                        
                        g->tex_background = LoadTextureFromImage(img);
                        UnloadImage(img);
                    }
                }
                
                Carregar_mapa(g, g->mapa_selec);
                iniciar_cobra(g);
                PlayMusicStream(g->musica_fundo);
                g->estado = TELA_JOGO;
                break;

            case Opcao_ranking:
                g->estado = TELA_RANKING;
                break;

            case Opcao_config:
                g->estado = TELA_CONFIG;
                break;

            case Opcao_sair:
                CloseWindow();
                exit(0);
                break;
        }
    }
}

void desenha_menu(Game *g){
    const char* textos[4] = { "INICIAR JOGO", "RANKING", "CONFIGURACOES", "SAIR" };
    int centroX = g->largura_tela / 2;
    int inicioY = 300;

    DrawText("SNAKE GAME", centroX - MeasureText("SNAKE GAME", 60)/2, 100, 60, DARKGREEN);

    for (int i = 0; i < 4; i++) {
        Color cor = GRAY;
        int tamanho = 20;
        
        // Se for a opção selecionada
        if (i == g->menu.Opcao_select) {
            cor = DARKGREEN;
            tamanho = 30;
            DrawText(">", centroX - MeasureText(textos[i], tamanho)/2 - 20, inicioY + (i * 50), tamanho, cor);
        }

        DrawText(textos[i], 
                 centroX - MeasureText(textos[i], tamanho)/2, 
                 inicioY + (i * 50), 
                 tamanho, 
                 cor);
    }
}

void Atualiza_config(Game *g) {
    if (IsKeyPressed(KEY_UP)) {
        g->conf_selecionada--;
        if (g->conf_selecionada < 0) g->conf_selecionada = 2;
    }
    if (IsKeyPressed(KEY_DOWN)) {
        g->conf_selecionada++;
        if (g->conf_selecionada > 2) g->conf_selecionada = 0;
    }

    if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_RIGHT)) {
        
        if (g->conf_selecionada == CONF_MAPA) {
            if (g->mapa_selec == 1) g->mapa_selec = 2;
            else g->mapa_selec = 1;
        }
        
        if (g->conf_selecionada == CONF_RESOLUCAO) {
            if (g->largura_tela == 820) {
                g->largura_tela = 660;
                g->altura_tela = 660;
            } else {
                g->largura_tela = 820;
                g->altura_tela = 820;
            }
            SetWindowSize(g->largura_tela, g->altura_tela);
            
            // Redimensiona background
            if (g->tex_background.id > 0) {
                Image imgTmp = LoadImageFromTexture(g->tex_background);
                ImageResize(&imgTmp, g->largura_tela, g->altura_tela);
                UnloadTexture(g->tex_background);
                g->tex_background = LoadTextureFromImage(imgTmp);
                UnloadImage(imgTmp);
            }

            int m = GetCurrentMonitor();
            SetWindowPosition(
                GetMonitorWidth(m)/2 - g->largura_tela/2,
                GetMonitorHeight(m)/2 - g->altura_tela/2
            );
        }
    }

    if (IsKeyPressed(KEY_ENTER)) {
        if (g->conf_selecionada == CONF_VOLTAR) {
            g->estado = TELA_MENU;
        }
    }
}

void DesenhaConfig(Game *g) {
    ClearBackground(BLACK);
    int centroX = g->largura_tela / 2;
    int y = 200;

    DrawText("CONFIGURACOES", centroX - MeasureText("CONFIGURACOES", 40)/2, 50, 40, BLUE);

    Color c1 = (g->conf_selecionada == CONF_MAPA) ? GREEN : GRAY;
    DrawText("MAPA:", centroX - 100, y, 20, c1);
    DrawText(TextFormat("< %d >", g->mapa_selec), centroX + 50, y, 20, c1);

    Color c2 = (g->conf_selecionada == CONF_RESOLUCAO) ? GREEN : GRAY;
    DrawText("RESOLUCAO:", centroX - 140, y + 60, 20, c2);
    DrawText(TextFormat("< %dx%d >", g->largura_tela, g->altura_tela), centroX + 50, y + 60, 20, c2);

    Color c3 = (g->conf_selecionada == CONF_VOLTAR) ? GREEN : GRAY;
    DrawText("VOLTAR AO MENU", centroX - MeasureText("VOLTAR AO MENU", 20)/2, y + 150, 20, c3);
    
    DrawText("Use SETAS para alterar e ENTER para voltar", centroX - 200, g->altura_tela - 50, 20, DARKGRAY);
}

void iniciar_cobra(Game *g){
    g->cobra.tamanho = 1;
    g->cobra.direcao = 1;
    g->cobra.cor = GREEN;

    float inicio_x = (g->largura_tela / 2) - ((g->largura_tela / 2) % TAM_GRID);
    float inicio_y = (g->altura_tela / 2) - ((g->altura_tela / 2) % TAM_GRID);

    Body *cabeca = criar_segmento(inicio_x, inicio_y);

    g->cobra.cabeca = cabeca;
    g->cobra.cauda = cabeca;
}

//garantir que a comida não nasça em lugar proibido

void Posicionar_Comida(Game *g) {
    bool posicao_invalida;
    
    do {
        posicao_invalida = false;
        
        float x = (rand() % (g->largura_tela/TAM_GRID)) * TAM_GRID;
        float y = (rand() % (g->altura_tela/TAM_GRID)) * TAM_GRID;
        
        g->Comida = (Rectangle){x, y, TAM_GRID, TAM_GRID};

        for (int i = 0; i < g->qtd_barreiras; i++) {
            if (CheckCollisionRecs(g->Comida, g->barreiras[i])) {
                posicao_invalida = true;
                break;
            }
        }

        if (!posicao_invalida) {
            Body* atual = g->cobra.cabeca;
            while (atual != NULL) {
                if (CheckCollisionRecs(g->Comida, atual->pos)) {
                    posicao_invalida = true;
                    break;
                }
                atual = atual->prox;
            }
        }

    } while (posicao_invalida);
}

void mover_cobra(Game *g){
    //condição de parada da função = perdeu
    if(g->game_over){
        return;
    }

    if (g->buffer_count > 0) {
        g->cobra.direcao = g->input_buffer[0];
        g->input_buffer[0] = g->input_buffer[1];
        g->input_buffer[1] = -1;
        g->buffer_count--;
    }

    //nova cabeça x e y

    float novo_x = g->cobra.cabeca->pos.x;
    float novo_y = g->cobra.cabeca->pos.y;

    //Calculo básico
    if (g->cobra.direcao == 0){
         novo_y -= TAM_GRID; // Cima
    }
    if (g->cobra.direcao == 1){
         novo_x += TAM_GRID; // Direita
    }
    if (g->cobra.direcao == 2){
         novo_y += TAM_GRID; // Baixo
    }
    if (g->cobra.direcao == 3){
        novo_x -= TAM_GRID; // Esquerda
    }

    //Conferindo se não está passando da tela em ambos os casos

    if (novo_x >= g->largura_tela){
         novo_x = 0;
    }else if (novo_x < 0){
        novo_x = g->largura_tela - TAM_GRID;
    }
    
    if (novo_y >= g->altura_tela){
         novo_y = 0;
    }else if (novo_y < 0){
        novo_y = g->altura_tela - TAM_GRID;
    }

    //conferindo se bateu em uma barreira

    Rectangle retanguloFuturo = (Rectangle){novo_x, novo_y, TAM_GRID, TAM_GRID};

    for(int i = 0; i < g->qtd_barreiras; i++) {
        if (CheckCollisionRecs(retanguloFuturo, g->barreiras[i])) { 
            g->game_over = 1;
            StopMusicStream(g->musica_fundo);
            PlaySound(g->snd_morrer);
            return;
        }
    }

    //conferindo se não bateu no proprio corpo

    Body* temp = g->cobra.cabeca;
    while (temp != NULL) {
        if (temp != g->cobra.cauda && CheckCollisionRecs(retanguloFuturo, temp->pos)) {
             g->game_over = 1;
             StopMusicStream(g->musica_fundo);
             PlaySound(g->snd_morrer);
             return;
        }
        temp = temp->prox;
    }

    // Criando nova cabeça

    Body* nova_cabeca = criar_segmento(novo_x, novo_y);
    
    nova_cabeca->prox = g->cobra.cabeca;
    g->cobra.cabeca->ant = nova_cabeca;
    g->cobra.cabeca = nova_cabeca;

    // Checando comida

    if (CheckCollisionRecs(nova_cabeca->pos, g->Comida)) {
        g->cobra.tamanho++;
        g->pontuacao_atual++;
        
        PlaySound(g->snd_comer);
        g->timer_efeito_sonoro = 0.5f; 
        
        Posicionar_Comida(g);
    } 
    else {
        // Remove a cauda
        Body* velha_cauda = g->cobra.cauda;
        g->cobra.cauda = velha_cauda->ant;
        
        if (g->cobra.cauda != NULL) {
            g->cobra.cauda->prox = NULL;
        }
        free(velha_cauda);
    }
}

void Carregar_mapa(Game *g, int id_mapa) {
    g->qtd_barreiras = 0;
    g->id_mapa = id_mapa;

    if (id_mapa == 1) {
        //mapa "comum"
    } 
    else {
        //mapa com obstaculo
        
        float tamBloco = TAM_GRID * 2;
        int L = g->largura_tela;
        int A = g->altura_tela;
        
        //barreiras
        g->barreiras[g->qtd_barreiras++] = (Rectangle){ 200, 200, tamBloco, tamBloco };
        g->barreiras[g->qtd_barreiras++] = (Rectangle){ L - 200 - tamBloco, 200, tamBloco, tamBloco };
        g->barreiras[g->qtd_barreiras++] = (Rectangle){ 200, A - 200 - tamBloco, tamBloco, tamBloco };
        g->barreiras[g->qtd_barreiras++] = (Rectangle){ L - 200 - tamBloco, A - 200 - tamBloco, tamBloco, tamBloco };

        float larguraBarra = 300;
        
        g->barreiras[g->qtd_barreiras++] = (Rectangle){
            (L / 2) - (larguraBarra / 2), 
            80,                                  
            larguraBarra, TAM_GRID
        };

        g->barreiras[g->qtd_barreiras++] = (Rectangle){
            (L / 2) - (larguraBarra / 2), 
            A - 120,                             
            larguraBarra, TAM_GRID
        };
    }
}

void DesenhaJogo(Game *g) {
    if (g->tex_background.id > 0) DrawTexture(g->tex_background, 0, 0, WHITE);

    //Desenha o Mapa + Barreiras

    for (int i = 0; i < g->qtd_barreiras; i++) {
        DrawRectangleRec(g->barreiras[i], GRAY);       
        DrawRectangleLinesEx(g->barreiras[i], 2, DARKGRAY);
    }

    //desenha objetos do jogo (comida)

    if (g->tex_comida.id > 0) {
        DrawTexture(g->tex_comida, (int)g->Comida.x, (int)g->Comida.y, WHITE);
    } else {
        DrawRectangleRec(g->Comida, RED); 
    }

    //Desenha a Cobra (Percorrendo a lista)

    Body* atual = g->cobra.cabeca;
    while (atual != NULL) {
        Color corSegmento = (atual == g->cobra.cabeca) ? DARKGREEN : LIME;
        
        DrawRectangleRec(atual->pos, corSegmento);
        DrawRectangleLinesEx(atual->pos, 1, DARKGREEN); 
        
        atual = atual->prox;
    }

    //Desenha o HUD (Interface)
    DrawText(TextFormat("SCORE: %d", g->pontuacao_atual), 20, 20, 20, WHITE);

    // Mostrando qual mapa é
    DrawText(TextFormat("MAPA %d", g->id_mapa), g->largura_tela - 100, 20, 20, LIGHTGRAY);
}

void CarregarAssets(Game *g) {
    Image imgTmp;

    if (FileExists("assets/comida.png")) {
        imgTmp = LoadImage("assets/comida.png");
        ImageResize(&imgTmp, TAM_GRID, TAM_GRID);
        g->tex_comida = LoadTextureFromImage(imgTmp);
        UnloadImage(imgTmp);
    }

    if (FileExists("assets/background.jpeg")) {
        imgTmp = LoadImage("assets/background.jpeg");
        ImageResize(&imgTmp, LARGURA_PADRAO, ALTURA_PADRAO);
        g->tex_background = LoadTextureFromImage(imgTmp);
        UnloadImage(imgTmp);
    } 
    else if (FileExists("assets/background.jpg")) {
        imgTmp = LoadImage("assets/background.jpg");
        ImageResize(&imgTmp, LARGURA_PADRAO, ALTURA_PADRAO);
        g->tex_background = LoadTextureFromImage(imgTmp);
        UnloadImage(imgTmp);
    }

    if (FileExists("assets/comer.wav")) g->snd_comer = LoadSound("assets/comer.wav");
    if (FileExists("assets/morrer.wav")) g->snd_morrer = LoadSound("assets/morrer.wav");
    
    if (FileExists("assets/music.wav")) {
        g->musica_fundo = LoadMusicStream("assets/music.wav");
        g->musica_fundo.looping = true; 
        SetMusicVolume(g->musica_fundo, 0.5f);
    }
}

void AtualizarMusica(Game *g) {
    if (!IsMusicStreamPlaying(g->musica_fundo) && g->estado == TELA_JOGO) {
        PlayMusicStream(g->musica_fundo);
    }

    if (g->estado == TELA_JOGO && IsMusicStreamPlaying(g->musica_fundo)) {
        UpdateMusicStream(g->musica_fundo);

        if (g->timer_efeito_sonoro > 0) {
            SetMusicVolume(g->musica_fundo, 0.0f);
            g->timer_efeito_sonoro -= GetFrameTime();
        } else {
            SetMusicVolume(g->musica_fundo, 0.5f);
        }
    } 
    else {
        if (IsMusicStreamPlaying(g->musica_fundo) && g->estado != TELA_JOGO) {
             StopMusicStream(g->musica_fundo);
        }
    }
}

void DescarregarAssets(Game *g) {
    UnloadTexture(g->tex_comida);
    UnloadTexture(g->tex_background);
    
    if (IsAudioDeviceReady()) {
        UnloadSound(g->snd_comer);
        UnloadSound(g->snd_morrer);
        UnloadMusicStream(g->musica_fundo);
    }
}