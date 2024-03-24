#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#define MAX_FILAS 100
#define MAX_COLUMNAS 100

void leer_archivo(char laberinto[MAX_FILAS][MAX_COLUMNAS], int *filas, int *columnas, const char *nombre_archivo) {
    FILE *archivo;
    char c;
    *filas = 0;
    *columnas = 0;

    // Abrir el archivo
    archivo = fopen(nombre_archivo, "r");
    if (archivo == NULL) {
        printf("Error al abrir el archivo\n");
        return;
    }

    // Inicializar el arreglo laberinto con caracteres nulos
    memset(laberinto[0], '\0', MAX_FILAS * MAX_COLUMNAS * sizeof(char));

    // Leer el archivo y almacenar el contenido en el arreglo
    while ((c = fgetc(archivo)) != EOF) {
        if (c == '\n') {
            (*filas)++;
            *columnas = 0;
        } else {
            laberinto[*filas][(*columnas)++] = c;
        }
    }

    // Cerrar el archivo
    fclose(archivo);
}

int main() {

    char laberinto[MAX_FILAS][MAX_COLUMNAS];
    int filas, columnas;

    leer_archivo(laberinto, &filas, &columnas, "laberinto.txt");

    // Imprimir el laberinto
    printf("Laberinto:\n");
    for (int i = 0; i < filas; i++) {
        printf("%s\n", laberinto[i]);
    }

    return 0;
}
