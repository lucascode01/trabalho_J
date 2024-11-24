#ifndef SNAKE_H
#define SNAKE_H

#include "types.h"

typedef struct {
    Point body[100];
    int length;
    int dx;
    int dy;
} Snake;

#endif
