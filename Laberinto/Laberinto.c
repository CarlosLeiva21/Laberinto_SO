#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define MAX_FILAS 100
#define MAX_COLUMNAS 100

// Definición de la estructura del Hilo
struct Hilo {
    int fila;
    int columna;
    char dir;
};

// Definición de la estructura para los argumentos que se usan en la funcion del hilo
struct ThreadArgs {
    struct Hilo *hilo;
    char (*laberinto)[MAX_COLUMNAS];
};

// Definición de la estructura para los argumentos que se usan en la funcion de imprimir
struct PrintArgs {
    int *filas;
    char (*laberinto)[MAX_COLUMNAS];
};

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
            if(c == ' '){
                laberinto[*filas][(*columnas)++] = '0';
            }else{
                laberinto[*filas][(*columnas)++] = c;
            }
        }
    }

    // Cerrar el archivo
    fclose(archivo);
}

//Funcion para imprimir el laberinto
void imprimirLaberinto(char laberinto[][MAX_COLUMNAS], int filas) {
    printf("Laberinto:\n");
    for (int i = 0; i < filas + 1; i++) {
        printf("%s\n", laberinto[i]);
    }
}

void *imprimir_laberinto(void *args){
    //Convertir argumento en la estructura
    struct PrintArgs *print_args = (struct PrintArgs *)args;

    char (*laberinto)[MAX_COLUMNAS] = print_args->laberinto;
    int filas = *(print_args->filas);

    printf("Laberinto:\n");
    for (int i = 0; i < filas + 1; i++) {
        printf("%s\n", laberinto[i]);
    };

    return NULL;
}

void *HiloLogic(void *args) {
    // Convertir el argumento genérico a la estructura ThreadArgs
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;

    // Acceder a los datos de la estructura
    struct Hilo *hilo = thread_args->hilo;
    char (*laberinto)[MAX_COLUMNAS] = thread_args->laberinto;
    
    if(hilo->dir == 'A'){
        while(laberinto[hilo->fila+1][hilo->columna] == '0'){
            printf("Todo Bien");
            hilo->fila = hilo->fila + 1;
        }
        printf("Choco con pared");
    }
    
   
    return NULL;
}

int main() {

    char laberinto[MAX_FILAS][MAX_COLUMNAS];
    int filas, columnas;
    struct Hilo hilo1 = {0, 0, 'A'};

    leer_archivo(laberinto, &filas, &columnas, "laberinto.txt");

    struct ThreadArgs thread_args;
    thread_args.hilo = &hilo1;
    thread_args.laberinto = laberinto;

    struct PrintArgs print_args;
    print_args.laberinto = laberinto;
    print_args.filas = &filas;

    pthread_t HiloInicial;
    pthread_t HiloImprimir;
    pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
    pthread_create(&HiloImprimir, NULL, &imprimir_laberinto, &print_args);
    pthread_join(HiloInicial, NULL);
    pthread_join(HiloImprimir, NULL);
    

    return 0;
}
