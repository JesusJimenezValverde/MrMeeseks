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
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>
#include <pthread.h>

int N =1;
int numInstance = 0;

int randGenerate(int inicio, int fin){
    return (rand() % (fin - inicio + 1)) + inicio;
}

/*FUNCIONES PARA LAS PETICIONES TEXTUALES*/

char* readRequest(){
    char* req = malloc(sizeof(char)*1000);

    printf("Que necesita que haga Mr. Meeseeks? ");
    scanf(" %[^\n]", req);
    getchar();

    //printf("Mr. Meeseeks hara: %s\n",req);
    return req;
}

int readDifficult(){
    int difficult;
    printf("Ingrese la dificultad de su solicitud (100:trivial, 0:imposible, -1:No sabe): ");
    scanf("%d", &difficult);
    //getchar();

    if(difficult == -1){
        //difficult = randGenerate(0, 100);

        int type = randGenerate(0, 100);

        if(type >= 0 && type <= 15){
            difficult = randGenerate(0, 45);
        }else if(type > 15 && type <= 85){
            difficult = randGenerate(45, 85);
        }else{ // 86 <= type <= 100
            difficult = randGenerate(85, 100);
        }

        printf("Mr. Meeseeks hara la tarea con dificultad: %d\n", difficult);

    }else
    {
        //printf("Mr. Meeseeks hara la tarea con dificultad: %d\n", difficult);
    }
    
    return difficult;
}

int tryRequest(double difficult){
    int seg = randGenerate(1, 5);
    sleep(seg); // Simulando que hace la tarea

    if(difficult <= 85.01){
        return 0; //Necesita ayuda de otros Mr. Meeseeks
    } else{
        return 1; //Logro resolver la tarea facil
    }
}

int amountChild(double difficult){
    if (difficult >= 0 & difficult <= 45){
        return randGenerate(1, 5); 
        
    }else if(difficult > 45 & difficult <= 85){
        return randGenerate(1, 2);

    }else{
        return 0;
    }
}

double diluirDificultad(double dificultad, int numHIjos){
    if(dificultad == 0){
        return dificultad;
    } else{
        double temp = randGenerate(1, (int) dificultad); //Random no mayor a la dificultad
        double reduc = temp * (dificultad / randGenerate(350,550)); //Se reduce a una milesima dictada por la dificultad
        double extra = (reduc * (numHIjos));

        return dificultad + reduc + extra;
    }
}

struct TextReqParams{
    double difficult;
    int amountChild;
    char * req;
};

int terminado;
int fdTextualComplete[2];
sem_t finish_sem;
struct timespec beginTextual, endTextual;
int amountSegToChaosTextual;

void * textualTry(void * params){
    struct TextReqParams * actualParams = params;

    clock_gettime(CLOCK_REALTIME, &endTextual);
    long seconds = endTextual.tv_sec - beginTextual.tv_sec;
    long nanoseconds = endTextual.tv_nsec - beginTextual.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;
    
    if(elapsed > amountSegToChaosTextual){
        //printf("\n\n☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢\n\t\tCAOS PLANETARIO\n☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢\n\n");
        sem_wait(&finish_sem);
        terminado = 1;
        sem_post(&finish_sem);
        write(fdTextualComplete[1],"caos",sizeof(char)*100);
        return NULL;
    }
        
    if(terminado == 0){
        if (tryRequest(actualParams->difficult) == 1){

            if(terminado == 0){
                //printf("Lo logre!! Mr Meeseeks(pid: %d, ppid: %d, dif: %f) Bye!!!\n",getpid(),getppid(),actualParams->difficult);
                write(fdTextualComplete[1],"complete",sizeof(char)*100);
            }
            
            sem_wait(&finish_sem);
            terminado = 1;
            sem_post(&finish_sem);


        }else{
            int numChild = amountChild(actualParams->difficult);
            double difficult = diluirDificultad(actualParams->difficult, numChild);

            pthread_t thread_id[numChild];

            printf("Meeseek (pid: %d) creara: %d hijos\n",getpid(),numChild);

            for (int i = 0; i < numChild; i++){
                printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, i: %d, difficult: %f)\n",getpid(),getppid(), i+1, actualParams->difficult);
    
                struct TextReqParams * params = malloc(sizeof *params);
                params->amountChild = numChild;
                params->difficult = difficult;
                strcmp(params->req, actualParams->req);

                pthread_create(&thread_id[i], NULL, *textualTry, params);
            }
            
            for (int j = 0; j < numChild; j++){
                pthread_join( thread_id[j], NULL);
            }
        
        }
    }
    
}

char* textualRequest2(int *stateDone, int amountSegToChaos){

    char * req = readRequest();
    int difficult = readDifficult();

    amountSegToChaosTextual = amountSegToChaos;

    //Pipe para notificar de terminado
    pipe(fdTextualComplete);

    //Semaforo
    sem_init(&finish_sem, 0, 1);
    terminado = 0;

    struct TextReqParams * params = malloc(sizeof *params);
    params->amountChild = 1;
    params->difficult = difficult;
    strcmp(params->req, req);

    clock_gettime(CLOCK_REALTIME, &beginTextual);

    printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());

    pthread_t firstMeeseek;
    pthread_create(&firstMeeseek, NULL, *textualTry, params);
    pthread_join(firstMeeseek,NULL);

    // Medir el tiempo
    clock_gettime(CLOCK_REALTIME, &endTextual);
    long seconds = endTextual.tv_sec - beginTextual.tv_sec;
    long nanoseconds = endTextual.tv_nsec - beginTextual.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    char buf[100];
    read(fdTextualComplete[0],buf,sizeof(buf));

    char* log = malloc(sizeof(char)*1000);

    if(!strcmp(buf,"complete")){
        sprintf(log, "- Mr Meeseeks: %d, hizo tarea '%s'(dificultad:%d), tardo : %f seg\n",getpid(),req,difficult,elapsed);
        *stateDone = 1;
    }else if(!strcmp(buf,"caos")){
        printf("\n\n☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢\n\t\tCAOS PLANETARIO\n☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢\n\n");
        sprintf(log, "- Se declaro CAOS PLANETARIO para la tarea '%s'(dificultad:%d)\n",req,difficult);
        *stateDone = 0;
    }

    return log;

}

/*FUNCIONES PARA LAS PETICIONES ARITMETICAS*/

char* readOperation(){
    char* operation = malloc(sizeof(char)*10000);

    printf("Los Mr Meeseeks solucionan operaciones de tipo '50 + 5' (+, -, *, /, &, |, ~)\n");
    printf("Ingrese la operacion a resolver: ");

    scanf(" %[^\n]", operation);
    getchar();

    //printf("Mr. Meeseeks hara: %s\n",operation);
    return operation;

}

int operate(int num1, char operator, int num2, int *result){
    
    switch (operator){
        case '+':
            *result = num1 + num2;
            return 1;
        case '-':
            *result = num1 - num2;
            return 1;
        case '*':
            *result = num1 * num2;
            return 1;
        case '/':
            if(num2 == 0){
                return 0;
            }
            *result = num1 / num2;
            return 1;
        case '&':
            *result = num1 && num2;
            return 1;
        case '|':
            *result = num1 || num2;
            return 1;
        case '~':
            if(num1){
                *result = 0;
            }else{
                *result = 1;
            }
            return 1;
        default:
            return -1;
    }
    
}

//-----------------------> CON HILOS

int fdAritmeticComplete[2];

void *aritmeticLogicTry(void * params){
    int num1 = 0;
    int num2 = 0;
    int result = 0;
    char ope;

    char * operation = params;
    
    printf("\nHi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());

    sscanf(operation,"%d %c %d",&num1,&ope,&num2);

    int state  = operate(num1,ope,num2, &result);
    char* resOper = malloc(sizeof(char)*100);

    if(state == 1){ // Todo bien
        printf("Mr Meeseeks llego al resultado de la operacion %s = %d\n",operation,result);
        
        sprintf(resOper,"%s = %d",operation,result);
        
        write(fdAritmeticComplete[1],resOper,sizeof(char)*100);
    
    }else if(state == 0){ // Division entre 0
        printf("Mr Meeseeks no puede resolver divisiones entre O\n");
        write(fdAritmeticComplete[1],"div0",5);
    }else{ // operacion invalida
        printf("Mr Meeseeks no puede realizar la operacion: '%c'\n",ope);
        write(fdAritmeticComplete[1],"nan",5);
    }

    return NULL;

}

char* aritmeticLogicRequest(int *stateDone){

    char* operation = malloc(sizeof(char)*10000);
    
    // Solicitar info
    operation = readOperation();

    // Medicion de tiempo
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    // Piepe para notificar el estado de retorno
    pipe(fdAritmeticComplete);

    // Creacion y llamada al hilo
    pthread_t firstMeeseek;
    pthread_create(&firstMeeseek, NULL, *aritmeticLogicTry, operation);
    pthread_join(firstMeeseek,NULL);

    // Lectura del pipe
    char buf[100];
    read(fdAritmeticComplete[0],buf,sizeof(buf));

    // Medir el tiempo
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    char* log = malloc(sizeof(char)*1000);

    if(!strcmp(buf,"div0")){
        sprintf(log,"- Mr Meeseeks: %d, No soluciono la operacion por una division entre 0\n",getpid());
        *stateDone = 0;
    }else if(!strcmp(buf,"nan")){
        sprintf(log,"- Mr Meeseeks: %d, No soluciono la operacion por un operador desconocido\n",getpid());
        *stateDone = 0;
    }else{
        sprintf(log,"- Mr Meeseeks: %d, soluciono la operacion '%s', tardo : %f seg\n",getpid(),buf, elapsed);
        *stateDone = 1;
    }

    //printf("Mr Meeseeks %d tardo %f\n",pid,tiempoTotal);
    //printf("%s\n",log);
    return log;

}


/*FUNCIONES PARA LAS PETICIONES DE CORRER PROGRAMAS*/

char* readProgram(){
    char* program = malloc(sizeof(char)*10000);

    printf("Ingrese el nombre del programa a ejecutar: ");

    scanf(" %[^\n]", program);
    getchar();

    //printf("Mr. Meeseeks ejecutara: '%s'\n",program);
    return program;

}

//-----------------------> CON HILOS
int fdrunProgramComplete[2];

void * runProgramTry(void *params){

    char * program = params;

    printf("\nHi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());

    int state = system(program);

    if(state == 0){
        printf("Mr Meeseeks pudo ejecutar '%s'\n",program);
        
        write(fdrunProgramComplete[1],program,sizeof(char)*10000);
        
    }else{
        printf("Mr Meeseeks NO pudo ejecutar '%s'\n",program);

        write(fdrunProgramComplete[1],"0",5);
    }

    return NULL;
    
}

char* runProgram(int *stateDone){

    char* program = malloc(sizeof(char)*10000);
    
    int state;

    // Leer info
    program = readProgram();
    
    // Medicion de tiempo
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    // Piepe para notificar el estado de retorno
    pipe(fdrunProgramComplete);
    
    // Creacion y llamada al hilo
    pthread_t firstMeeseek;
    pthread_create(&firstMeeseek, NULL, *runProgramTry, program);
    pthread_join(firstMeeseek,NULL);

    // Lectura del pipe
    char buf[100];
    read(fdrunProgramComplete[0],buf,sizeof(buf));

    // Medicion de tiempo
    clock_gettime(CLOCK_REALTIME, &end);
    long seconds = end.tv_sec - begin.tv_sec;
    long nanoseconds = end.tv_nsec - begin.tv_nsec;
    double elapsed = seconds + nanoseconds*1e-9;

    char* log = malloc(sizeof(char)*1000);
    printf("BUFER: %s\n",buf);

    if(!strcmp(buf,"0")){
        sprintf(log,"- Mr Meeseeks: %d, No logro ejecutar el programa\n",getpid());
        *stateDone = 0;
    }else{
        sprintf(log,"- Mr Meeseeks: %d, ejectuto el programa '%s', tardo : %f seg\n",getpid(),buf, elapsed);
        *stateDone = 1;
    }

    //printf("Mr Meeseeks %d tardo %f\n",pid,tiempoTotal);
    //printf("%s\n",log);
    return log;

}
