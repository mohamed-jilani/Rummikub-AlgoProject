// src/utils/random.h
#ifndef RANDOM_H
#define RANDOM_H

#include <stdlib.h>

void initRandom();
int getRandomInt(int min, int max);
void shuffleArray(void* array, size_t n, size_t size);

#endif