#include "game.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>




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

bool checkCollision(Snake *snake) {
    if (snake->body[0].x < 0 || snake->body[0].x >= WIDTH ||
        snake->body[0].y < 0 || snake->body[0].y >= HEIGHT) {
        return true;
    }
    for (int i = 1; i < snake->length; i++) {
        if (snake->body[0].x == snake->body[i].x && snake->body[0].y == snake->body[i].y) {
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
    // Clear screen and set background color
    SDL_SetRenderDrawColor(renderer, 78, 1, 0, 255);
    SDL_RenderClear(renderer);

    // Draw button
    SDL_SetRenderDrawColor(renderer, 0, 255, 0, 255);
    SDL_Rect buttonRect = {WIDTH / 2 - 100, HEIGHT / 2 - 25, 200, 50};
    SDL_RenderFillRect(renderer, &buttonRect);

    // Render text on button
    SDL_Color white = {255, 255, 255, 255};
    SDL_Surface *textSurface = TTF_RenderText_Blended(font, "Iniciar Jogo", white);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);

    SDL_Rect textRect = {WIDTH / 2 - textSurface->w / 2, HEIGHT / 2 - textSurface->h / 2, textSurface->w, textSurface->h};
    SDL_FreeSurface(textSurface);

    SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);

    SDL_RenderPresent(renderer);  // Update screen

    // Wait for user to click or quit
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
