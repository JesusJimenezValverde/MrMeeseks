/*
SO - Tarea #2 - Mr.Meeseeks
Tecnológico de Costa Rica, Sede Central Cartago, Primer Semestre 2021
Escuela de Ingeieria en Computación, Principios de Sistemas Operativos
Prof.: Esteban Árias Mendez
Hecho por: 
Fabián Ceciliano Ramírez - 2018142999
Jesus Esteban Jiménez Valverde - 2017112650
Fecha de entrega: 24-4-2021
*/
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

#include "extraOperations.c"

void mrMeeseeks(){
    int option = 1;

    int originalBox = getpid();

    int totalReq = 0;
    int amountFinished = 0;
    int amountNotFinishef = 0;

    int amountSegToChaos = 5*60;

    char* request  = malloc(sizeof(char)*1000);
    int state = 0;

    char* log  = malloc(sizeof(char)*10000); 

    while(option != 5){

        printf("\n                    █████                         \n");
        printf("--------------------------------------------------\n");
        printf("Seleccione(%d) una de las opciones:\n\t1. Accion Textual\n\t2. Operacion Logica-Aritmeticas\n\t3. Ejecutar Programa\n\t4. Configurar tiempo antes del caos planetario\n\t5. Salir\n\nIngrese el numero de la acción que desar realizar: ",getpid());
        
        scanf("%d", &option);

        char* request  = malloc(sizeof(char)*1000);

        switch (option)
        {
        case 1: // Textuales: "quiero ser feliz"
            strcat(request,textualRequest2(&state,amountSegToChaos));
            
            printf("%s",request);
            
            if(state == 1){
                amountFinished++;
            }else{
                amountNotFinishef++;
            }
            //textualRequest();
            break;

        case 2: // Aritmetica: "2 + 2"
            strcat(request,aritmeticLogicRequest(&state));
            
            printf("%s",request);
            
            if(state == 1){
                amountFinished++;
            }else{
                amountNotFinishef++;
            }
            //aritmeticLogicRequest();
            break;
        
        case 3: // Correr Programa
            strcat(request,runProgram(&state));
            
            printf("%s",request);
            
            if(state == 1){
                amountFinished++;
            }else{
                amountNotFinishef++;
            }
            //runProgram();
            break;

        case 4:
            printf("Ingrese la cantidad de segundos antes del caos planetario: ");
            scanf("%d", &amountSegToChaos);
            break;

        case 5:
            printf("------------------------------------------BITACORA\n");
            printf("Cantidad total: %d\nCantidad terminadas: %d\nCantidad NO terminadas: %d\n",totalReq,amountFinished,amountNotFinishef);
            printf("Tareas:\n%s",log);
            printf("------------------------------------------Gracias por usar la Meeseeks box\n");
            break;
            
        default:
            printf("Intente de nuevo");
            break;
        }

        if(getpid()==originalBox){
            // hecer algo con "request"
            strcat(log,request);
            totalReq++;
        }else{
            break;
        }

    }    
}


int main(){

    //printf("This is a meeseks box ...");
    mrMeeseeks();

}