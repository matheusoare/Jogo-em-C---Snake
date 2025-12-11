🐍 Snake Game - Raylib Implementation

Este projeto consiste em uma implementação completa e moderna do clássico jogo Snake, desenvolvido na linguagem C utilizando a biblioteca gráfica Raylib. O jogo foi estruturado com foco em boas práticas de programação, gerenciamento de memória e arquitetura de software.
👥 Autores

    Clésio Junior

    Matheus Soares

🎮 Funcionalidades

O jogo vai além da mecânica básica, implementando recursos avançados:

    Sistema de Menus: Navegação completa entre Menu Principal, Jogo, Ranking e Configurações.

    Múltiplos Mapas:

        Mapa 1: Clássico (paredes livres com teletransporte).

        Mapa 2: Com obstáculos estratégicos no centro e cantos.

    Ranking Persistente: Sistema de High Scores (Top 5) salvo em arquivo binário (ranking.dat), permitindo inserção de nome do jogador.

    Configurações Dinâmicas: Possibilidade de alterar a resolução da tela e o mapa através do menu.

    Áudio Imersivo: Trilha sonora de fundo e efeitos sonoros. A música possui um sistema inteligente de "ducking" (pausa momentânea) ao comer a fruta.

    Gráficos: Uso de texturas (sprites) para a comida e background, com fallback automático para formas geométricas caso os arquivos não sejam encontrados.

⚙️ Lógica de Execução e Arquitetura

O código foi arquitetado para ser modular e seguro. Abaixo, explicamos os pilares da implementação:
1. Máquina de Estados (State Machine)

O fluxo do jogo é controlado por uma máquina de estados finita, definida no main.c. Um switch central gerencia qual tela deve ser atualizada e desenhada a cada frame:

    TELA_MENU: Navegação e seleção.

    TELA_JOGO: Onde a lógica da cobra acontece.

    TELA_CONFIG: Alteração de variáveis globais (resolução/mapa).

    TELA_NOME: Input de texto caso o jogador bata um recorde.

    TELA_RANKING: Exibição e leitura do arquivo de pontuação.

2. Lista Encadeada (Linked List)

A estrutura da cobra não é um vetor fixo. Utilizamos uma Lista Duplamente Encadeada (struct Body).

    Movimento: A cada passo, alocamos (malloc) uma nova "cabeça" na direção desejada e liberamos (free) o último pedaço da "cauda".

    Crescimento: Ao comer uma fruta, criamos a nova cabeça mas não removemos a cauda, aumentando efetivamente o tamanho da lista.

    Memória: A função liberar_cobra garante que toda a memória alocada dinamicamente seja limpa ao reiniciar o jogo ou fechar a janela, prevenindo memory leaks.

3. Gerenciamento de Assets

Para garantir robustez, o carregamento de imagens e sons possui verificações de segurança:

    As imagens são redimensionadas em tempo de execução para se adequarem ao tamanho do grid (TAM_GRID), evitando que imagens grandes "quebrem" o visual do jogo.

    O sistema de áudio utiliza UpdateMusicStream para manter a música em looping, e altera o volume/pausa dinamicamente baseado em eventos do jogo (comer/morrer).

4. Sistema de Arquivos

O ranking utiliza a biblioteca padrão stdio.h para manipular arquivos binários.

    Ao iniciar, o jogo tenta ler ranking.dat. Se não existir, cria uma lista virgem.

    Ao bater um recorde, a lista é reordenada usando Bubble Sort e o arquivo é sobrescrito com os novos dados.

🚀 Como Compilar e Rodar
Pré-requisitos

Certifique-se de ter o compilador gcc, make e as dependências da Raylib instaladas no seu sistema Linux/WSL.
Estrutura de Pastas Necessária

Para que os assets funcionem, a pasta do projeto deve estar organizada assim:
Plaintext

/ProjetoSnake
|-- main.c
|-- snake.c
|-- snake.h
|-- Makefile
|-- assets/
    |-- comida.png
    |-- background.jpeg (ou .jpg)
    |-- music.wav
    |-- comer.wav
    |-- morrer.wav

Comandos de Compilação

Abra o terminal na pasta do projeto e execute:

    Limpar compilações anteriores (Recomendado):
    Bash

make clean

Compilar e Rodar:
Bash

    make run

🕹️ Controles

    Setas Direcionais: Movimentam a cobra e navegam nos menus.

    ENTER: Seleciona opções no menu e confirma o nome no ranking.

    ESC: Pausa/Desiste da partida (volta ao Menu) ou fecha o jogo.

Trabalho desenvolvido para a disciplina de Laboratório de Programação.