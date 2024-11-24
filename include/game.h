#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include "snake.h"
#include "food.h"
#include "types.h"

#define WIDTH 640
#define HEIGHT 480
#define BLOCK_SIZE 20

// Estrutura para representar uma fase
typedef struct {
    int snake_speed;        // Velocidade da cobra (em ms entre movimentos)
    int food_points;        // Pontos necessários para completar a fase
    int obstacle_count;     // Número de obstáculos
    Point *obstacles;       // Posições dos obstáculos (agora um ponteiro dinâmico)
} Phase;

// Função para carregar as fases de um arquivo
bool loadPhase(const char *filename, Phase *phase);

// Funções principais do jogo
void init(Snake *snake, Food *food);
void moveSnake(Snake *snake);
bool checkCollision(Snake *snake);
bool checkFoodCollision(Snake *snake, Food *food);
void draw(SDL_Renderer *renderer, Snake *snake, Food *food, Phase *phase);
void drawStartScreen(SDL_Renderer *renderer, TTF_Font *font, bool *startGame);

#endif
