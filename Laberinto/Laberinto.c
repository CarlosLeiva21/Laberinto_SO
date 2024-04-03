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

//Arreglo que contendra los hilos activos
struct Hilo hilosActivos[100];
int contadorHilos = 0;

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

void *imprimir_laberinto(void *args){

    //Tiempo que estara dormido
    struct timespec tiempo = { 2,5 };

    //Convertir argumento en la estructura
    struct PrintArgs *print_args = (struct PrintArgs *)args;

    char (*laberinto)[MAX_COLUMNAS] = print_args->laberinto;
    int filas = *(print_args->filas);

    while(1){
        printf("Laberinto:\n");
        for (int i = 0; i < filas + 1; i++) {
            for (int j = 0; j < MAX_COLUMNAS; j++) {
                // Verificar si hay un hilo en esta posición
                int hilo_encontrado = 0;
                for (int k = 0; k < contadorHilos; k++) {
                    if (hilosActivos[k].fila == i && hilosActivos[k].columna == j) {
                        hilo_encontrado = 1;
                        break;
                    }
                }
                // Imprimir '1' si hay un hilo en esta posición, de lo contrario, imprimir el laberinto normal
                if (hilo_encontrado) {
                    printf("1");
                } else {
                    printf("%c", laberinto[i][j]);
                }
            }
            printf("\n");
        }
        pthread_delay_np(&tiempo);
    }
        


    return NULL;
}

void *HiloLogic(void *args) {

    //Tiempo que estara dormido
    struct timespec tiempo = { 3,0 };

    // Convertir el argumento a la estructura ThreadArgs
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;

    // Acceder a los datos de la estructura y asignarlos a variables
    struct Hilo *hilo = thread_args->hilo;
    char (*laberinto)[MAX_COLUMNAS] = thread_args->laberinto;

    //Agregar el hilo al arreglo de hilos activos
    hilosActivos[contadorHilos].fila = hilo->fila;
    hilosActivos[contadorHilos].columna = hilo->columna;
    hilosActivos[contadorHilos].dir = hilo->dir;
    contadorHilos++;
    

    //Revisa todas las posibilidades de movimiento A: abajo R: Arriba D: derecha I: izquierda
    //En el ciclo while, movera el hilo en la direccion dada, hasta que ya no se pueda
    //Despues elimina el hilo del arreglo de hilos activos
    //Despues revisa si puede crear un nuevo hilo en las direcciones que se le permiten
    if(hilo->dir == 'A'){
        //Abajo
        while(laberinto[hilo->fila+1][hilo->columna] == '0'){
            hilo->fila = hilo->fila + 1;
            hilosActivos[contadorHilos - 1].fila = hilo->fila;
            pthread_delay_np(&tiempo);
        }

        for (int i = contadorHilos-1; i < contadorHilos - 1; i++) {
            hilosActivos[i] = hilosActivos[i + 1];
        }
        contadorHilos--;

        //Revisa Izquierda y Derecha
        if(laberinto[hilo->fila][hilo->columna+1] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'D'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }
        printf("HOLA");

        if(laberinto[hilo->fila][hilo->columna-1] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'I'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }

    }else if(hilo->dir == 'D'){
        //Abajo
        while(laberinto[hilo->fila][hilo->columna + 1] == '0'){
            hilo->columna = hilo->columna + 1;
            hilosActivos[contadorHilos - 1].columna = hilo->columna;
            pthread_delay_np(&tiempo);
        }

        for (int i = contadorHilos-1; i < contadorHilos - 1; i++) {
            hilosActivos[i] = hilosActivos[i + 1];
        }
        contadorHilos--;

        //Revisa Arriba y Abajo
        if(laberinto[hilo->fila-1][hilo->columna] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'R'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }

        if(laberinto[hilo->fila+1][hilo->columna] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'A'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }

    }else if(hilo->dir == 'R'){
        //Abajo
        while(laberinto[hilo->fila-1][hilo->columna] == '0'){
            hilo->fila = hilo->fila - 1;
            hilosActivos[contadorHilos - 1].fila = hilo->fila;
            pthread_delay_np(&tiempo);
        }

        for (int i = contadorHilos-1; i < contadorHilos - 1; i++) {
            hilosActivos[i] = hilosActivos[i + 1];
        }
        contadorHilos--;

        //Revisa Izquierda y Derecha
        if(laberinto[hilo->fila][hilo->columna+1] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'D'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }

        if(laberinto[hilo->fila][hilo->columna-1] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'I'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }

    }else if(hilo->dir == 'I'){
        //Abajo
        while(laberinto[hilo->fila][hilo->columna - 1] == '0'){
            hilo->columna = hilo->columna - 1;
            hilosActivos[contadorHilos - 1].columna = hilo->columna;
            pthread_delay_np(&tiempo);
        }

        for (int i = contadorHilos-1; i < contadorHilos - 1; i++) {
            hilosActivos[i] = hilosActivos[i + 1];
        }
        contadorHilos--;

        //Revisa Arriba y Abajo
        if(laberinto[hilo->fila-1][hilo->columna] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'R'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }

        if(laberinto[hilo->fila+1][hilo->columna] == '0'){
            //Crear Hilo
            struct Hilo hilo1 = {hilo->fila, hilo->columna, 'A'};
            struct ThreadArgs thread_args;
            thread_args.hilo = &hilo1;
            thread_args.laberinto = laberinto;

            pthread_t HiloInicial;
            pthread_create(&HiloInicial, NULL, &HiloLogic, &thread_args);
            pthread_join(HiloInicial, NULL);
        }
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
