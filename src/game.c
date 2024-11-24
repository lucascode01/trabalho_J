#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>

// Variáveis globais para estatísticas
static int total_apples = 0;
static int total_movements = 0;

// Inicializa as estatísticas (caso necessário no início do programa)
void init_statistics() {
    total_apples = 0;
    total_movements = 0;
}

// Atualiza as estatísticas após cada partida
void update_statistics(int apples_eaten, int movements) {
    total_apples += apples_eaten;
    total_movements += movements;
}

// Exibe as estatísticas no terminal
void show_statistics() {
    printf("\n=== Estatísticas ===\n");
    printf("Maçãs coletadas: %d\n", total_apples);
    printf("Movimentos realizados: %d\n", total_movements);
    printf("====================\n\n");
}

bool loadPhase(const char *phaseFile, Phase *phase) {
    FILE *file = fopen(phaseFile, "r");

    if (!file) {
        printf("Erro ao abrir o arquivo da fase: %s\n", phaseFile);
        return false;
    } else {
        printf("Arquivo da fase %s aberto com sucesso.\n", phaseFile);
    }

    // Lê a velocidade, os pontos e o número de obstáculos da fase
    if (fscanf(file, "velocidade=%d\n", &phase->snake_speed) != 1 || 
        fscanf(file, "pontos=%d\n", &phase->food_points) != 1 || 
        fscanf(file, "obstáculos=%d\n", &phase->obstacle_count) != 1) {
        printf("Erro ao ler os dados da fase.\n");
        fclose(file);
        return false;
    }

    printf("Dados da fase lidos: velocidade=%d, pontos=%d, obstáculos=%d\n", 
            phase->snake_speed, phase->food_points, phase->obstacle_count);

    // Aloca memória para os obstáculos
    phase->obstacles = malloc(sizeof(Point) * phase->obstacle_count);
    if (!phase->obstacles) {
        printf("Erro ao alocar memória para obstáculos.\n");
        fclose(file);
        return false;
    }

    // Leitura das coordenadas dos obstáculos
    for (int i = 0; i < phase->obstacle_count; i++) {
        printf("Lendo obstáculo %d...\n", i + 1);  // Debug para saber qual obstáculo está sendo lido
        if (fscanf(file, "%d %d\n", &phase->obstacles[i].x, &phase->obstacles[i].y) != 2) {
            printf("Erro ao ler coordenadas dos obstáculos na linha %d.\n", i + 1);
            free(phase->obstacles);  // Libera memória em caso de erro
            fclose(file);
            return false;
        }
        // Impressão para depuração
        printf("Obstáculo %d: (%d, %d)\n", i + 1, phase->obstacles[i].x, phase->obstacles[i].y);
    }

    fclose(file);
    return true;
}

void init(Snake *snake, Food *food) {
    snake->length = 1;
    snake->body[0].x = WIDTH / 2;
    snake->body[0].y = HEIGHT / 2;
    snake->dx = BLOCK_SIZE;
    snake->dy = 0;

    food->position.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
    food->position.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
}

void moveSnake(Snake *snake) {
    for (int i = snake->length - 1; i > 0; i--) {
        snake->body[i] = snake->body[i - 1];
    }
    snake->body[0].x += snake->dx;
    snake->body[0].y += snake->dy;
}

bool checkCollision(Snake *snake, Phase *phase) {
    // Colisão com as bordas
    if (snake->body[0].x < 0 || snake->body[0].x >= WIDTH ||
        snake->body[0].y < 0 || snake->body[0].y >= HEIGHT) {
        return true;
    }
    
    // Colisão com o próprio corpo
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
            return true;
        }
    }

    // Colisão com obstáculos
    for (int i = 0; i < phase->obstacle_count; i++) {
        if (snake->body[0].x == phase->obstacles[i].x && snake->body[0].y == phase->obstacles[i].y) {
            return true;
        }
    }

    return false;
}

bool checkFoodCollision(Snake *snake, Food *food) {
    if (snake->body[0].x == food->position.x && snake->body[0].y == food->position.y) {
        snake->length++;
        food->position.x = (rand() % (WIDTH / BLOCK_SIZE)) * BLOCK_SIZE;
        food->position.y = (rand() % (HEIGHT / BLOCK_SIZE)) * BLOCK_SIZE;
        return true;
    }
    return false;
}

void draw(SDL_Renderer *renderer, Snake *snake, Food *food, Phase *phase) {
    // Limpa a tela com uma cor
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255); // Cor preta
    SDL_RenderClear(renderer);

    // Desenha a cobra
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255); // Cor verde
    for (int i = 0; i < snake->length; i++) {
        SDL_Rect rect = {snake->body[i].x, snake->body[i].y, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &rect);
    }

    // Desenha o alimento
    SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255); // Cor vermelha
    SDL_Rect foodRect = {food->position.x, food->position.y, BLOCK_SIZE, BLOCK_SIZE};
    SDL_RenderFillRect(renderer, &foodRect);

    // Desenha os obstáculos (se houver)
    SDL_SetRenderDrawColor(renderer, 255, 255, 0, 255); // Cor amarela
    for (int i = 0; i < phase->obstacle_count; i++) {
        SDL_Rect obstacleRect = {phase->obstacles[i].x, phase->obstacles[i].y, BLOCK_SIZE, BLOCK_SIZE};
        SDL_RenderFillRect(renderer, &obstacleRect);
    }

    // Exibe as mudanças na tela
    SDL_RenderPresent(renderer);
}

void drawStartScreen(SDL_Renderer *renderer, TTF_Font *font, bool *startGame) {
    // Limpa a tela e define a cor de fundo
    SDL_SetRenderDrawColor(renderer, 78, 1, 0, 255);
    SDL_RenderClear(renderer);

    // Desenha o botão
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect buttonRect = {WIDTH / 2 - 100, HEIGHT / 2 - 25, 200, 50};
    SDL_RenderFillRect(renderer, &buttonRect);

    // Renderiza o texto no botão
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Iniciar Jogo", white);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {WIDTH / 2 - textSurface->w / 2, HEIGHT / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(renderer);  // Atualiza a tela

    // Espera o usuário clicar ou sair
    SDL_Event event;
    while (!*startGame) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                *startGame = true;
                return;
            }
            if (event.type == SDL_MOUSEBUTTONDOWN) {
                int x = event.button.x;
                int y = event.button.y;
                if (x >= buttonRect.x && x <= buttonRect.x + buttonRect.w &&
                    y >= buttonRect.y && y <= buttonRect.y + buttonRect.h) {
                    *startGame = true;
                }
            }
        }
    }
}

int main() {
    // Inicialização do SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0 || TTF_Init() == -1) {
        printf("Erro ao inicializar o SDL: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Window *window = SDL_CreateWindow("Jogo da Cobra",
                                          SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                          WIDTH, HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Erro ao criar janela: %s\n", SDL_GetError());
        return 1;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        printf("Erro ao criar renderizador: %s\n", SDL_GetError());
        return 1;
    }

    // Carregar a fonte
    TTF_Font *font = TTF_OpenFont("assets/font.ttf", 24);
    if (!font) {
        printf("Erro ao carregar a fonte: %s\n", TTF_GetError());
        return 1;
    }

    // Variáveis do jogo
    Snake snake;
    Food food;
    Phase phase;

    // Inicializa o jogo
    init(&snake, &food);

    bool startGame = false;
    drawStartScreen(renderer, font, &startGame);

    // Carrega a fase
    char phaseFile[100];
    sprintf(phaseFile, "assets/phase1.txt");
    if (!loadPhase(phaseFile, &phase)) {
        printf("Falha ao carregar a fase.\n");
        return 1;
    }

    // Jogo em execução
    while (!startGame) {
        // Evento de quit
        SDL_Event e;
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) {
                startGame = true;
            }
        }

        moveSnake(&snake);
        if (checkCollision(&snake, &phase)) {
            printf("Colisão! Fim de jogo.\n");
            break;
        }

        if (checkFoodCollision(&snake, &food)) {
            printf("Comida coletada!\n");
        }

        // Atualiza o estado do jogo
        draw(renderer, &snake, &food, &phase);
        SDL_Delay(100); // Ajuste de velocidade do jogo
    }

    // Exibe as estatísticas finais
    show_statistics();

    // Finaliza recursos do SDL
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_CloseFont(font);
    SDL_Quit();
    TTF_Quit();

    return 0;
}
