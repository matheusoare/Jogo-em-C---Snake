#include "raylib.h"
#include "snake.h"
#include <time.h> 

int main(void) {
    SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    InitWindow(LARGURA_PADRAO, ALTURA_PADRAO, "Snake Game - Trabalho Final");
    InitAudioDevice(); 
    SetTargetFPS(60); 
    srand(time(NULL)); 

    Game game = {0};   
    inicia_jogo(&game); 
    
    CarregarAssets(&game);

    game.cor_comida = RED;
    Posicionar_Comida(&game);

    while (!WindowShouldClose()) {
        
        AtualizarMusica(&game);

        switch (game.estado) {
            case TELA_MENU:
                atualiza_menu(&game); 
                break;
            case TELA_CONFIG: 
                Atualiza_config(&game);
                break;
            case TELA_JOGO:
                if (GetTime() - game.tempo_movimento > 0.15) {
                    mover_cobra(&game);
                    game.tempo_movimento = GetTime();
                }
                
                int novaDir = -1;
                if (IsKeyPressed(KEY_UP))    novaDir = 0;
                if (IsKeyPressed(KEY_RIGHT)) novaDir = 1;
                if (IsKeyPressed(KEY_DOWN))  novaDir = 2;
                if (IsKeyPressed(KEY_LEFT))  novaDir = 3;

                if (novaDir != -1 && game.buffer_count < 2) {
                    int ultimaDir = (game.buffer_count == 0) ? game.cobra.direcao : game.input_buffer[game.buffer_count-1];
                    bool oposto = (novaDir == 0 && ultimaDir == 2) || 
                                  (novaDir == 2 && ultimaDir == 0) || 
                                  (novaDir == 1 && ultimaDir == 3) || 
                                  (novaDir == 3 && ultimaDir == 1);
                                  
                    if (!oposto && novaDir != ultimaDir) {
                        game.input_buffer[game.buffer_count] = novaDir;
                        game.buffer_count++;
                    }
                }

                if (game.game_over) {
                    StopMusicStream(game.musica_fundo); 
                    
                    if (Verifica_Novo_Recorde(&game)) {
                        game.qtd_letras = 0;
                        game.nome_buffer[0] = '\0';
                        game.estado = TELA_NOME; 
                    } else {
                        game.estado = TELA_ENDING;
                    }
                }
                if (IsKeyPressed(KEY_ESCAPE)) {
                    liberar_cobra(&game);
                    game.estado = TELA_MENU;
                }
                break;
            case TELA_NOME:
                Atualiza_Input_Nome(&game);
                break;
            case TELA_ENDING:
                if (IsKeyPressed(KEY_ENTER)) {
                    liberar_cobra(&game); 
                    int largAtual = game.largura_tela;
                    int altAtual = game.altura_tela;
                    inicia_jogo(&game); 
                    game.largura_tela = largAtual;
                    game.altura_tela = altAtual;
                    
                    Posicionar_Comida(&game);
                    
                    game.estado = TELA_MENU;
                }
                break;
            case TELA_RANKING:
                if (IsKeyPressed(KEY_ENTER) || IsKeyPressed(KEY_ESCAPE)) {
                    if (game.game_over) {
                        liberar_cobra(&game);
                        int largAtual = game.largura_tela;
                        int altAtual = game.altura_tela;
                        inicia_jogo(&game);
                        game.largura_tela = largAtual;
                        game.altura_tela = altAtual;
                        Posicionar_Comida(&game);
                    }
                    game.estado = TELA_MENU;
                }
                break;
            default: break;
        }

        BeginDrawing();
        ClearBackground(RAYWHITE); 
        switch (game.estado) {
            case TELA_MENU:
                desenha_menu(&game);
                break;
            case TELA_CONFIG: 
                DesenhaConfig(&game);
                break;
            case TELA_JOGO:
                DesenhaJogo(&game); 
                break;
            case TELA_NOME:
                Desenha_Input_Nome(&game);
                break;
            case TELA_ENDING: { 
                DrawRectangle(0, 0, game.largura_tela, game.altura_tela, Fade(BLACK, 0.8f)); 
                int cx = game.largura_tela / 2;
                DrawText("GAME OVER", cx - MeasureText("GAME OVER", 60)/2, 300, 60, RED);
                const char* textoPontos = TextFormat("Pontuação Final: %d", game.pontuacao_atual);
                DrawText(textoPontos, cx - MeasureText(textoPontos, 30)/2, 400, 30, WHITE);
                DrawText("Pressione [ENTER] para Voltar ao Menu", cx - MeasureText("Pressione [ENTER] para Voltar ao Menu", 20)/2, 500, 20, GRAY);
                break;
            }
            case TELA_RANKING:
                DesenhaRanking(&game);
                break;
            default: break;
        }
        EndDrawing();
    }

    DescarregarAssets(&game);
    liberar_cobra(&game); 
    CloseAudioDevice();
    CloseWindow();
    return 0;
}