#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

//Numero de Filas y Columnas Maximas
#define MAX_FILAS 100
#define MAX_COLUMNAS 100
// Definir el rango de caracteres permitidos
#define MIN_CHAR 'a'
#define MAX_CHAR 'z'

// Variable estática para mantener el registro de caracteres asignados
static int caracteres_asignados[MAX_FILAS * MAX_COLUMNAS] = {0};
// Mutex para sincronizar el acceso a la función generar_caracter_aleatorio
pthread_mutex_t mutex_generar_caracter = PTHREAD_MUTEX_INITIALIZER;

//Laberinto
char laberinto[MAX_FILAS][MAX_COLUMNAS];

// Definición de la estructura del Hilo
struct Hilo {
    int fila;
    int columna;
    char dir;
    int caracter;
    int caracter_recorrido;
    int posicion_arreglo;
    int espacios_recorridos;
};

//Caracter Hilo
int caracter_hilo = 1;

//Arreglo que contendra los hilos activos
struct Hilo hilosActivos[MAX_FILAS];
int contadorHilos = 0;

// Definición de la estructura para los argumentos que se usan en la funcion de la logica del hilo
struct ThreadArgs {
    struct Hilo *hilo;
    char (*laberinto)[MAX_COLUMNAS];
};

// Definición de la estructura para los argumentos que se usan en la funcion de imprimir
struct PrintArgs {
    int *filas;
    char (*laberinto)[MAX_COLUMNAS];
};

//Prototipo para que la funcion este creada desde antes y no de problemas con las revisiones horizontales y verticales
void *hilo_logic(void *args);

//Funcion que genera un caracter random
char generar_caracter_aleatorio() {

    // Bloquear el mutex para asegurar acceso exclusivo a la generación de caracteres aleatorios
    pthread_mutex_lock(&mutex_generar_caracter);

    // Establecer la semilla para la generación de números aleatorios
    srand(time(NULL));

    char caracter_aleatorio;
    int num_aleatorio;

    // Generar caracteres aleatorios hasta encontrar uno que no esté asignado
    do {
        // Generar un número aleatorio en el rango de valores de los caracteres permitidos
        num_aleatorio = rand() % (MAX_CHAR - MIN_CHAR + 1) + MIN_CHAR;
        caracter_aleatorio = (char)num_aleatorio;
    } while (caracteres_asignados[num_aleatorio - MIN_CHAR] != 0);

    // Marcar el caracter como asignado
    caracteres_asignados[num_aleatorio - MIN_CHAR] = 1;

    // Desbloquear el mutex después de generar el caracter
    pthread_mutex_unlock(&mutex_generar_caracter);

    return caracter_aleatorio;
}

//Funcion para leer el archivo
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
void *imprimir_laberinto(void *args){

    //Tiempo que estara dormido
    struct timespec tiempo = { 1,5 };

    //Convertir argumento en la estructura
    struct PrintArgs *print_args = (struct PrintArgs *)args;
    int filas = *(print_args->filas);

    while(1){

        printf("Laberinto:\n");
        for (int fila = 0; fila < filas + 1; fila++) {
            for (int columna = 0; columna < MAX_COLUMNAS; columna++) {
                // Verificar si hay un hilo en esta posición
                int hilo_encontrado = 0;
                for (int hilo = 0; hilo < contadorHilos; hilo++) {
                    if (hilosActivos[hilo].fila == fila && hilosActivos[hilo].columna == columna) {

                        if(hilosActivos[hilo].caracter_recorrido){
                            // laberinto[fila][columna] = hilosActivos[hilo].caracter_recorrido;
                            putchar(hilosActivos[hilo].caracter_recorrido);
                        }else{
                            printf("%d", hilosActivos[hilo].caracter); 
                        }
                        hilo_encontrado = 1; 
                        break;
                    }
                }

                if (!hilo_encontrado) {
                    putchar(laberinto[fila][columna]);
                }
            }
            printf("\n");
            
        }

        // Mostrar los caracteres recorridos de los hilos terminados
        printf("Hilos terminados:\n");
        for (int hilo = 0; hilo < contadorHilos; hilo++) {

            if(contadorHilos > 10){
                hilo = contadorHilos - 2;
            }

            if(hilosActivos[hilo].caracter == -1){
                printf("Hilo %d: Carácter recorrido: %c Espacios Recorridos: %d\n", hilo + 1, hilosActivos[hilo].caracter_recorrido, hilosActivos[hilo].espacios_recorridos);
                exit(EXIT_SUCCESS);
            }

            if(hilosActivos[hilo].caracter_recorrido){
                printf("Hilo %d: Carácter recorrido: %c Espacios Recorridos: %d\n", hilo + 1, hilosActivos[hilo].caracter_recorrido, hilosActivos[hilo].espacios_recorridos);
            }
        }

        usleep(tiempo.tv_sec * 1000000 + tiempo.tv_nsec / 1000);
    }
    return NULL;
}

void revisar_horizontal(struct Hilo *hilo){

    //Reiniciar caracter hilo en caso de llegar a ser de 2 digitos
    if(caracter_hilo + 2 > 8){
        caracter_hilo = 1;
    }

    //Revisa Izquierda y Derecha
    if(laberinto[hilo->fila][hilo->columna+1] == '0' && laberinto[hilo->fila][hilo->columna-1] == '0'){
        //Crear Hilo Para Derecha
        caracter_hilo++;
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'D',caracter_hilo};
        struct ThreadArgs thread_args1;
        thread_args1.hilo = &hilo1;
        thread_args1.laberinto = laberinto;

        //Crear Hilo Izquierda
        caracter_hilo++;
        struct Hilo hilo2 = {hilo->fila, hilo->columna, 'I',caracter_hilo};
        struct ThreadArgs thread_args2;
        thread_args2.hilo = &hilo2;
        thread_args2.laberinto = laberinto;

        pthread_t Hilo1;
        pthread_t Hilo2;
        pthread_create(&Hilo1, NULL, &hilo_logic, &thread_args1);
        pthread_create(&Hilo2, NULL, &hilo_logic, &thread_args2);
        pthread_join(Hilo1,NULL);
        pthread_join(Hilo2, NULL);

    } else if(laberinto[hilo->fila][hilo->columna+1] == '0'){
        //Crear Hilo
        caracter_hilo++;
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'D',caracter_hilo};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    } else if(laberinto[hilo->fila][hilo->columna-1] == '0'){
        //Crear Hilo
        caracter_hilo++;
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'I',caracter_hilo};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    }
}

void revisar_vertical(struct Hilo *hilo){

    //Reiniciar caracter hilo en caso de llegar a ser de 2 digitos
    if(caracter_hilo + 2 > 8){
        caracter_hilo = 1;
    }

    //Revisa Arriba y Abajo
    if(laberinto[hilo->fila-1][hilo->columna] == '0' && laberinto[hilo->fila+1][hilo->columna] == '0'){
        //Crear Hilo Para Derecha
        caracter_hilo++;
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'R',caracter_hilo};
        struct ThreadArgs thread_args1;
        thread_args1.hilo = &hilo1;
        thread_args1.laberinto = laberinto;

        //Crear Hilo Izquierda
        caracter_hilo++;
        struct Hilo hilo2 = {hilo->fila, hilo->columna, 'A',caracter_hilo};
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
        caracter_hilo++;
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'R',caracter_hilo};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    }

    if(laberinto[hilo->fila+1][hilo->columna] == '0'){
        //Crear Hilo
        caracter_hilo++;
        struct Hilo hilo1 = {hilo->fila, hilo->columna, 'A',caracter_hilo};
        struct ThreadArgs thread_args;
        thread_args.hilo = &hilo1;
        thread_args.laberinto = laberinto;

        pthread_t HiloInicial;
        pthread_create(&HiloInicial, NULL, &hilo_logic, &thread_args);
        pthread_join(HiloInicial, NULL);
    }
}

// Función para revisar horizontal de forma asíncrona
void *revisar_horizontal_asinc(void *args) {
    // Convertir el argumento a la estructura ThreadArgs
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;

    struct Hilo *hilo = thread_args-> hilo;
    
    // Ejecutar la revisión horizontal
    revisar_horizontal(hilo);

    return NULL;
}

// Función para revisar vertical de forma asíncrona
void *revisar_vertical_asinc(void *args) {
    // Convertir el argumento a la estructura ThreadArgs
    struct ThreadArgs *thread_args = (struct ThreadArgs *)args;

    struct Hilo *hilo = thread_args-> hilo;

    // Ejecutar la revisión vertical
    revisar_vertical(thread_args->hilo);

    return NULL;
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
    if(hilo->espacios_recorridos){
       hilosActivos[contadorHilos].espacios_recorridos = hilo->espacios_recorridos; 
    }else{hilosActivos[contadorHilos].espacios_recorridos = 0;}
    
    contadorHilos++;

    char caracter_recorrido = generar_caracter_aleatorio();
    
    //Revisa todas las posibilidades de movimiento A: abajo R: Arriba D: derecha I: izquierda
    //En el ciclo while, movera el hilo en la direccion dada, hasta que ya no se pueda
    //Despues elimina el hilo del arreglo de hilos activos
    //Despues revisa si puede crear un nuevo hilo en las direcciones que se le permiten
    if(hilo->dir == 'A'){
        //Abajo
        while(laberinto[hilo->fila+1][hilo->columna] == '0'){

            hilo->fila = hilo->fila + 1;
            hilosActivos[hilo->posicion_arreglo].fila = hilo->fila; 
            hilosActivos[hilo->posicion_arreglo].espacios_recorridos++;

            laberinto[hilo->fila-1][hilo->columna] = caracter_recorrido;

            pthread_t hilo_revisar_horizontal;
            pthread_create(&hilo_revisar_horizontal, NULL, &revisar_horizontal_asinc, args);

            usleep(tiempo.tv_sec * 1000000 + tiempo.tv_nsec / 1000);

        }

        //Verifica si ya llego al final, en caso que no verifica si puede moverse
        if(laberinto[hilo->fila+1][hilo->columna] == '/'){
            hilosActivos[hilo->posicion_arreglo].caracter = -1;
            hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        }else{
            hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
            // revisar_horizontal(hilo);
        }

    }else if(hilo->dir == 'D'){
        //Derecha
        while(laberinto[hilo->fila][hilo->columna + 1] == '0'){

            hilo->columna = hilo->columna + 1;
            hilosActivos[hilo->posicion_arreglo].columna = hilo->columna;
            hilosActivos[hilo->posicion_arreglo].espacios_recorridos++;

            laberinto[hilo->fila][hilo->columna-1] = caracter_recorrido;

            pthread_t hilo_revisar_vertical;
            pthread_create(&hilo_revisar_vertical, NULL, &revisar_vertical_asinc, args);

            usleep(tiempo.tv_sec * 1000000 + tiempo.tv_nsec / 1000);

        }

        if(laberinto[hilo->fila+1][hilo->columna] == '/'){
            hilosActivos[hilo->posicion_arreglo].caracter = -1;
            hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        }else{
           hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        //    revisar_vertical(hilo);
        }


    }else if(hilo->dir == 'R'){
        //Arriba
        while(laberinto[hilo->fila-1][hilo->columna] == '0'){

            hilo->fila = hilo->fila - 1;
            hilosActivos[hilo->posicion_arreglo].fila = hilo->fila;
            hilosActivos[hilo->posicion_arreglo].espacios_recorridos++;

            laberinto[hilo->fila + 1][hilo->columna] = caracter_recorrido;

            pthread_t hilo_revisar_horizontal;
            pthread_create(&hilo_revisar_horizontal, NULL, &revisar_horizontal_asinc, args);

            usleep(tiempo.tv_sec * 1000000 + tiempo.tv_nsec / 1000);

        }

        if(laberinto[hilo->fila+1][hilo->columna] == '/'){
            hilosActivos[hilo->posicion_arreglo].caracter = -1;
            hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        }else{
           hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        //    revisar_horizontal(hilo);
        }

    }else if(hilo->dir == 'I'){
        //Izquierda
        while(laberinto[hilo->fila][hilo->columna - 1] == '0'){

            hilo->columna = hilo->columna - 1;
            hilosActivos[hilo->posicion_arreglo].columna = hilo->columna;
            hilosActivos[hilo->posicion_arreglo].espacios_recorridos++;

            laberinto[hilo->fila][hilo->columna+1] = caracter_recorrido;

            pthread_t hilo_revisar_vertical;
            pthread_create(&hilo_revisar_vertical, NULL, &revisar_vertical_asinc, args);

            usleep(tiempo.tv_sec * 1000000 + tiempo.tv_nsec / 1000);            

        }

        if(laberinto[hilo->fila+1][hilo->columna] == '/'){
            hilosActivos[hilo->posicion_arreglo].caracter = -1;
            hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        }else{
           hilosActivos[hilo->posicion_arreglo].caracter_recorrido = caracter_recorrido;
        //    revisar_vertical(hilo);
        }   
    }

    return NULL;
}

int main() {
    int filas, columnas;
    struct Hilo hilo1 = {0, 0, 'A',caracter_hilo};
    hilo1.espacios_recorridos = 1;

    leer_archivo(laberinto, &filas, &columnas, "lab2.txt");

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
    pthread_join(HiloInicial,NULL);
    pthread_join(HiloImprimir, NULL);
    

    return 0;
}
