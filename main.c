#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <string.h>
#include <pthread.h>
#include <semaphore.h>
#include <time.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <errno.h>

#include "operations.c"

void mrMeeseeks(){
    int option = 1;

    while(option != 4){

        printf("Seleccione(%d) una de las opciones:\n\t1. Accion Textual\n\t2. Operaciones Aritmeticas\n\t3. Ejecutar Programa\n\t4. Salir\n\nIngrese el numero de la acción que desar realizar: ",getpid());
        
        scanf("%d", &option);

        switch (option)
        {
        case 1: // Textuales: "quiero ser feliz"
            textualRequest();
            break;

        case 2: // Aritmetica: "2 + 2"
            break;
        
        case 3: // Correr Programa
            break;

        case 4:
            break;
            
        default:
            printf("Intente de nuevo");
            break;
        }

    }    
}


int main(){

    //printf("This is a meeseks box ...");
    mrMeeseeks();

}