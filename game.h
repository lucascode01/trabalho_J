#ifndef GAME_H
#define GAME_H

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>

// Constantes
#define WIDTH 1280   // Largura da janela
#define HEIGHT 720   // Altura da janela
#define BLOCK_SIZE 20

// Estruturas
typedef struct {
    int x, y;
    int dx, dy;
    int length;
    SDL_Point body[100]; // Corpo da cobra com no máximo 100 segmentos
} Snake;

typedef struct {
    int x, y;
} Food;

typedef struct {
    int snake_speed;     // Velocidade da cobra (ms entre atualizações)
    int food_points;     // Pontos necessários para passar de fase
    int obstacle_count;  // Número de obstáculos na fase
} Phase;

// Funções
void drawStartScreen(SDL_Renderer *renderer, TTF_Font *font, bool *startGame, int apples, int moves);
void init(Snake *snake, Food *food);
void moveSnake(Snake *snake);
bool checkCollision(Snake *snake);
bool checkFoodCollision(Snake *snake, Food *food);
void draw(SDL_Renderer *renderer, Snake *snake, Food *food, Phase *phase);
bool loadPhase(const char *filename, Phase *phase);

#endif // GAME_H
