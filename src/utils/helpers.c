// src/utils/helpers.c
#include <stdio.h>
#include <string.h>
#include <ctype.h>

void clearInputBuffer() {
    int c;
    while ((c = getchar()) != '\n' && c != EOF);
}

int readInt(const char* prompt, int min, int max) {
    int value;
    printf("%s", prompt);
    
    while(1) {
        if(scanf("%d", &value) != 1) {
            printf("Veuillez entrer un nombre valide: ");
            clearInputBuffer();
        } else if(value < min || value > max) {
            printf("Veuillez entrer un nombre entre %d et %d: ", min, max);
            clearInputBuffer();
        } else {
            clearInputBuffer();
            return value;
        }
    }
}

void toLowerCase(char* str) {
    for(int i = 0; str[i]; i++) {
        str[i] = tolower(str[i]);
    }
}

void trimWhitespace(char* str) {
    int i = 0, j = 0;
    
    // Supprimer les espaces au début
    while(str[i] == ' ' || str[i] == '\t' || str[i] == '\n') {
        i++;
    }
    
    // Copier le reste
    while(str[i] != '\0') {
        str[j++] = str[i++];
    }
    
    // Supprimer les espaces à la fin
    while(j > 0 && (str[j-1] == ' ' || str[j-1] == '\t' || str[j-1] == '\n')) {
        j--;
    }
    
    str[j] = '\0';
}