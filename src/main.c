#include "game.h"

int main(int argc, char *argv[]) {
    srand(time(NULL));

    // Inicialização do SDL
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("Erro ao inicializar o SDL: %s\n", SDL_GetError());
        return 1;
    } else {
        printf("SDL Inicializado com sucesso.\n");
    }

    if (TTF_Init() != 0) {
        printf("Erro ao inicializar o TTF: %s\n", TTF_GetError());
        SDL_Quit();
        return 1;
    } else {
        printf("SDL_ttf Inicializado com sucesso.\n");
    }

    SDL_Window *window = SDL_CreateWindow("Jogo da Cobrinha", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, WIDTH, HEIGHT, 0);
    if (!window) {
        printf("Erro ao criar a janela SDL: %s\n", SDL_GetError());
        TTF_Quit();
        SDL_Quit();
        return 1;
    } else {
        printf("Janela SDL criada com sucesso.\n");
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar o renderizador SDL: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    } else {
        printf("Renderizador SDL criado com sucesso.\n");
    }

    TTF_Font *font = TTF_OpenFont("/System/Library/Fonts/Supplemental/Arial.ttf", 24);
    if (!font) {
        printf("Erro ao abrir a fonte: %s\n", TTF_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    } else {
        printf("Fonte carregada com sucesso.\n");
    }

    // Tela inicial
    bool startGame = false;
    drawStartScreen(renderer, font, &startGame);

    // Inicialização do jogo
    Snake snake;
    Food food;
    init(&snake, &food);

    // Configuração inicial das fases
    int currentPhase = 1;
    char phaseFile[500];  // Garantir que o buffer tenha tamanho suficiente
    sprintf(phaseFile, "%s/phase%d.txt", "/Users/lucassantos/Desktop/A/DFSC/C/Work/Salles/CodeLive/Faculdade/AP/jogo-da-cobrinha/assets", currentPhase);

    // Debug: Verificando o caminho do arquivo
    printf("Tentando abrir o arquivo: %s\n", phaseFile);

    Phase phase;
    if (!loadPhase(phaseFile, &phase)) {
        printf("Erro: Falha ao carregar a fase inicial!\n");
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    } else {
        printf("Fase inicial carregada: velocidade=%d, pontos=%d, obstáculos=%d\n", 
               phase.snake_speed, phase.food_points, phase.obstacle_count);
    }

    int score = 0;  // Pontuação inicial
    bool running = true;
    SDL_Event event;
    Uint32 lastTick = SDL_GetTicks();

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
            if (event.type == SDL_KEYDOWN) {
                switch (event.key.keysym.sym) {
                    case SDLK_UP:
                        if (snake.dy == 0) {
                            snake.dx = 0;
                            snake.dy = -BLOCK_SIZE;
                        }
                        break;
                    case SDLK_DOWN:
                        if (snake.dy == 0) {
                            snake.dx = 0;
                            snake.dy = BLOCK_SIZE;
                        }
                        break;
                    case SDLK_LEFT:
                        if (snake.dx == 0) {
                            snake.dx = -BLOCK_SIZE;
                            snake.dy = 0;
                        }
                        break;
                    case SDLK_RIGHT:
                        if (snake.dx == 0) {
                            snake.dx = BLOCK_SIZE;
                            snake.dy = 0;
                        }
                        break;
                }
            }
        }

        Uint32 currentTick = SDL_GetTicks();
        if ((int)(currentTick - lastTick) > phase.snake_speed) {
            moveSnake(&snake);

            if (checkCollision(&snake)) {
                printf("Fim de jogo! Sua pontuação: %d\n", score);
                running = false;
                break;
            }

            if (checkFoodCollision(&snake, &food)) {
                score++;
                printf("Pontuação: %d\n", score);

                if (score >= phase.food_points) {
                    currentPhase++;
                    sprintf(phaseFile, "%s/phase%d.txt", "/Users/lucassantos/Desktop/A/DFSC/C/Work/Salles/CodeLive/Faculdade/AP/jogo-da-cobrinha/assets", currentPhase);

                        if (!loadPhase(phaseFile, &phase)) {
                            printf("Parabéns! Você completou todas as fases!\n");
                            running = false;
                        } else {
                            printf("Fase %d carregada!\n", currentPhase);
                            init(&snake, &food);  // Reinicializa para nova fase
                        }
                    }
                }

                draw(renderer, &snake, &food, &phase); 
                lastTick = currentTick;
            }
        }
    }

    // Limpeza de recursos
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
