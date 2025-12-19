// src/utils/random.c
#include <stdlib.h>
#include <time.h>

void initRandom() {
    srand(time(NULL));
}

int getRandomInt(int min, int max) {
    return min + rand() % (max - min + 1);
}

void shuffleArray(void* array, size_t n, size_t size) {
    char* arr = (char*)array;
    
    if (n > 1) {
        for (size_t i = 0; i < n - 1; i++) {
            size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
            
            // Échanger les éléments
            for (size_t k = 0; k < size; k++) {
                char temp = arr[j * size + k];
                arr[j * size + k] = arr[i * size + k];
                arr[i * size + k] = temp;
            }
        }
    }
}