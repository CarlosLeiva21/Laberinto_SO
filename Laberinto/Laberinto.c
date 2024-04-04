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
    char caracter;
    int posicion_arreglo;

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

//Prototipo, para que la funcion este creada desnte antes y no me de problemas con las revisiones horizontales y verticales
void *hilo_logic(void *args);

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

    int ciclo = 1;

    //Tiempo que estara dormido
    struct timespec tiempo = { 1,5 };

    //Convertir argumento en la estructura
    struct PrintArgs *print_args = (struct PrintArgs *)args;

    char (*laberinto)[MAX_COLUMNAS] = print_args->laberinto;
    int filas = *(print_args->filas);

    while(ciclo){
        printf("Laberinto:\n");
        for (int i = 0; i < filas + 1; i++) {
            for (int j = 0; j < MAX_COLUMNAS; j++) {
                // Verificar si hay un hilo en esta posición
                int hilo_encontrado = 0;
                for (int k = 0; k < contadorHilos; k++) {
                    if (hilosActivos[k].fila == i && hilosActivos[k].columna == j) {
                        if(hilosActivos[k].caracter == '/'){
                            ciclo = 0;
                            break;
                        }
                        putchar(hilosActivos[k].caracter);
                        hilo_encontrado = 1;
                        break;
                    }
                }
                // Imprimir '1' si hay un hilo en esta posición, de lo contrario, imprimir el laberinto normal
                if (hilo_encontrado) {
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

void revisar_horizontal(char (*laberinto)[100], struct Hilo *hilo){

    //Revisa Izquierda y Derecha
    if(laberinto[hilo->fila][hilo->columna+1] == '0' && laberinto[hilo->fila][hilo->columna-1] == '0'){
        //Crear Hilo Para Derecha
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'D','1'};
        struct ThreadArgs thread_args1;
        thread_args1.hilo = &hilo1;
        thread_args1.laberinto = laberinto;

        //Crear Hilo Izquierda
        struct Hilo hilo2 = {hilo->fila, hilo->columna, 'I','1'};
        struct ThreadArgs thread_args2;
        thread_args2.hilo = &hilo2;
        thread_args2.laberinto = laberinto;

        pthread_t Hilo1;
        pthread_t Hilo2;
        pthread_create(&Hilo1, NULL, &hilo_logic, &thread_args1);
        pthread_create(&Hilo2, NULL, &hilo_logic, &thread_args2);
        pthread_join(Hilo1, NULL);
        pthread_join(Hilo2, NULL);

    } else if(laberinto[hilo->fila][hilo->columna+1] == '0'){
        //Crear Hilo
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'D','1'};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    } else if(laberinto[hilo->fila][hilo->columna-1] == '0'){
        //Crear Hilo
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'I','1'};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    }
}

void revisar_vertical(char (*laberinto)[100], struct Hilo *hilo){

    //Revisa Arriba y Abajo
    if(laberinto[hilo->fila-1][hilo->columna] == '0' && laberinto[hilo->fila+1][hilo->columna] == '0'){
        //Crear Hilo Para Derecha
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'R','1'};
        struct ThreadArgs thread_args1;
        thread_args1.hilo = &hilo1;
        thread_args1.laberinto = laberinto;

        //Crear Hilo Izquierda
        struct Hilo hilo2 = {hilo->fila, hilo->columna, 'A','1'};
        struct ThreadArgs thread_args2;
        thread_args2.hilo = &hilo2;
        thread_args2.laberinto = laberinto;

        pthread_t Hilo1;
        pthread_t Hilo2;
        pthread_create(&Hilo1, NULL, &hilo_logic, &thread_args1);
        pthread_create(&Hilo2, NULL, &hilo_logic, &thread_args2);
        pthread_join(Hilo1, NULL);
        pthread_join(Hilo2, NULL);

    }
    if(laberinto[hilo->fila-1][hilo->columna] == '0'){
        //Crear Hilo
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'R','1'};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    }

    if(laberinto[hilo->fila+1][hilo->columna] == '0'){
        //Crear Hilo
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'A','1'};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    }
}

void *hilo_logic(void *args) {

    //Tiempo que estara dormido
    struct timespec tiempo = { 5,0 };

    // Convertir el argumento a la estructura ThreadArgs
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;

    // Acceder a los datos de la estructura y asignarlos a variables
    struct Hilo *hilo = thread_args->hilo;
    char (*laberinto)[MAX_COLUMNAS] = thread_args->laberinto;

    hilo->posicion_arreglo = contadorHilos;

    //Agregar el hilo al arreglo de hilos activos
    hilosActivos[contadorHilos].fila = hilo->fila;
    hilosActivos[contadorHilos].columna = hilo->columna;
    hilosActivos[contadorHilos].dir = hilo->dir;
    hilosActivos[contadorHilos].caracter = hilo->caracter;
    hilosActivos[contadorHilos].posicion_arreglo = contadorHilos;
    contadorHilos++;
    

    //Revisa todas las posibilidades de movimiento A: abajo R: Arriba D: derecha I: izquierda
    //En el ciclo while, movera el hilo en la direccion dada, hasta que ya no se pueda
    //Despues elimina el hilo del arreglo de hilos activos
    //Despues revisa si puede crear un nuevo hilo en las direcciones que se le permiten
    if(hilo->dir == 'A'){
        //Abajo
        while(laberinto[hilo->fila+1][hilo->columna] == '0'){
            hilo->fila = hilo->fila + 1;
            hilosActivos[hilo->posicion_arreglo].fila = hilo->fila;
            pthread_delay_np(&tiempo);
        }

        if(laberinto[hilo->fila+1][hilo->columna] == '/'){
            hilosActivos[hilo->posicion_arreglo].caracter = '/';
        }else{
           hilosActivos[hilo->posicion_arreglo].caracter = '0'; 
        }

        revisar_horizontal(laberinto,hilo);

    }else if(hilo->dir == 'D'){
        //Abajo
        while(laberinto[hilo->fila][hilo->columna + 1] == '0'){
            hilo->columna = hilo->columna + 1;
            hilosActivos[hilo->posicion_arreglo].columna = hilo->columna;
            pthread_delay_np(&tiempo);
        }
        hilosActivos[hilo->posicion_arreglo].caracter = '0';

        revisar_vertical(laberinto,hilo);

    }else if(hilo->dir == 'R'){
        //Abajo
        while(laberinto[hilo->fila-1][hilo->columna] == '0'){
            hilo->fila = hilo->fila - 1;
            hilosActivos[hilo->posicion_arreglo].fila = hilo->fila;
            pthread_delay_np(&tiempo);
        }
        hilosActivos[hilo->posicion_arreglo].caracter = '0';

        revisar_horizontal(laberinto,hilo);

    }else if(hilo->dir == 'I'){
        //Abajo
        while(laberinto[hilo->fila][hilo->columna - 1] == '0'){
            hilo->columna = hilo->columna - 1;
            hilosActivos[hilo->posicion_arreglo].columna = hilo->columna;
            pthread_delay_np(&tiempo);
        }
        hilosActivos[hilo->posicion_arreglo].caracter = '0';

        revisar_vertical(laberinto,hilo);
    }

    return NULL;
}

int main() {

    char laberinto[MAX_FILAS][MAX_COLUMNAS];
    int filas, columnas;
    struct Hilo hilo1 = {0, 0, 'A','1'};

    leer_archivo(laberinto, &filas, &columnas, "laberinto.txt");

    struct ThreadArgs thread_args;
    thread_args.hilo = &hilo1;
    thread_args.laberinto = laberinto;

    struct PrintArgs print_args;
    print_args.laberinto = laberinto;
    print_args.filas = &filas;

    pthread_t HiloInicial;
    pthread_t HiloImprimir;
    pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
    pthread_create(&HiloImprimir, NULL, &imprimir_laberinto, &print_args);
    pthread_join(HiloInicial, NULL);
    pthread_join(HiloImprimir, NULL);
    

    return 0;
}
