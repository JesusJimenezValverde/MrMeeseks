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

    printf("Mr. Meeseeks hara: %s\n",req);
    return req;
}

int readDifficult(){
    int difficult;
    printf("Ingrese la dificultad de su solicitud (100:trivial, 0:imposible, -1:No sabe): ");
    scanf("%d", &difficult);
    //getchar();

    if(difficult == -1){
        difficult = randGenerate(0, 100);
        printf("Mr. Meeseeks dice que la dificultad es: %d\n", difficult);
    }else
    {
        printf("Mr. Meeseeks hara la tarea con dificultad: %d\n", difficult);
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

char * textualRequest(int *stateDone){
    char * req = readRequest();
    int difficult = readDifficult();

    pid_t pid;
    int firstMeeseek;
    int temp_instancia;
    int temp_N = 1;

    int* pipe_a_padre;
    int* pipes_a_hijos;

    numInstance = 0;

    // Variables para medir tiempos
    clock_t timeInit = clock();
    double totalTime = 0.0;

    // Piepe para notificar el estado de retorno
    int fdComplete[2];
    pipe(fdComplete);
    char buf[10];

    // Pipe para paso de solicitud entre procesos
    char* mensaje = malloc(sizeof(char)*1000);
    int* pipe_temp = malloc(sizeof(int)*2);
    pipe(pipe_temp);

    // Semaforo
    sem_t instance_sem;
    sem_init(&instance_sem, 1, 1);

    sem_t level_sem;
    sem_init(&level_sem, 1, 1);

    pid = fork();

    if(pid == 0){ // Hijo
        firstMeeseek = getpid();
        printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d)\n",getpid(),getppid(),temp_N, numInstance);

        if(difficult <= 0){ // Muy dificil! Mr Meeseeks no puede hacer esa tarea

            //something similar but 
            printf("Mr Meeseeks! can't do it\n");
            close(fdComplete[0]);
            write(fdComplete[1],"-1",5);
            close(fdComplete[1]);
            exit(0);
        }

        while(1==1){
            printf("PID:%d tratando...\n",getpid());
            if(tryRequest(difficult)==1){
                
                printf("Solucionado Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, dif: %d, mens: %s)\n",getpid(),getppid(),temp_N,difficult,mensaje);
                
                close(fdComplete[0]);
                write(fdComplete[1],"1",5);
                close(fdComplete[1]);
                exit(0);
                
                break;
            }else{
                int numChild = amountChild(difficult);
                printf("El pid:%d creara %d hijos\n",getpid(),numChild);
                
                temp_N++;

                for(int i = 0; i < numChild; i++){
                    pid = fork();

                    if(pid == 0){ //si el fork si se realizó y un hijo se creó

                        // Manejo de semarofos para aumentar el numero de instancias
                        sem_wait(&instance_sem);

                        numInstance++;
                        printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d, dif: %d, mens: %s)\n",getpid(),getppid(),temp_N, i+1,difficult,mensaje);

                        sem_post(&instance_sem);

                        
                        // Se recibe el req por un pipe
                        close(pipe_temp[1]);
                        read(pipe_temp[0], mensaje, sizeof(char)*1000);
                        close(pipe_temp[0]);
                        printf("mensaje:%s\n",mensaje);

                        difficult = diluirDificultad(difficult,numChild); //difficult*2;
                        break;
                    }else{ // Padre

                        close(pipe_temp[0]);
                        write(pipe_temp[1], req, sizeof(char)*1000);
                        close(pipe_temp[1]);
                        
                        //wait(NULL); // si pongo esto cada hijo solo crea 1, no crea los n que diga num child
                    }

                }
                if(pid != 0){
                    while (wait(NULL) > 0){}
                    break;
                }
            }
        }

    }else{ // Padre
        wait(NULL);

        close(fdComplete[1]);
        read(fdComplete[0],buf,sizeof(buf));
        close(fdComplete[0]);
        printf("Padre lee: %s, %d\n",buf,numInstance );
        
        totalTime = (double)(clock() - timeInit) / CLOCKS_PER_SEC; //printf("Mr Meeseeks %d tardo %f\n",pid,totalTime);

        char* log = malloc(sizeof(char)*1000);
        
        if(!strcmp(buf,"1")){
            sprintf(log, "- Mr Meeseeks: %d, hizo tarea '%s'(dificultad:%d), tardo : %f\n",pid,req,difficult,totalTime);
            *stateDone = 1;
        }else if(!strcmp(buf,"-1")){
            sprintf(log, "- Mr Meeseeks: %d NO logro hacer la tarea '%s'(dificultad:%d)\n",pid,req,difficult);
            *stateDone = 0;
        }

        //printf("%s",log);
        return log;

    }

}


/*FUNCIONES PARA LAS PETICIONES ARITMETICAS*/

char* readOperation(){
    char* operation = malloc(sizeof(char)*10000);

    printf("Los Mr Meeseeks solucionan operaciones de tipo '50 + 5' (+, -, *, /, &, |, ~)\n");
    printf("Ingrese la operacion a resolver: ");

    scanf(" %[^\n]", operation);
    getchar();

    printf("Mr. Meeseeks hara: %s\n",operation);
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

char* aritmeticLogicRequest(int *stateDone){

    char* operation = malloc(sizeof(char)*10000);
    int num1 = 0;
    int num2 = 0;
    int result = 0;

    char ope;

    // Medicion de tiempo
    clock_t inicio = clock();
    double tiempoTotal = 0.0;

    // Piepe para notificar el estado de retorno
    int fdComplete[2];
    pipe(fdComplete);

    pid_t pid = fork();

    if(pid < 0){
        printf("Existence is Pain");
    }else if(pid == 0){ // Hijo
        printf("\nHi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());
        operation = readOperation();
        
        sscanf(operation,"%d %c %d",&num1,&ope,&num2);

        int state  = operate(num1,ope,num2, &result);
        char* resOper = malloc(sizeof(char)*100);

        if(state == 1){ // Todo bien
            printf("Mr Meeseeks llego al resultado de la operacion %s = %d\n",operation,result);
            
            sprintf(resOper,"%s = %d",operation,result);
            
            close(fdComplete[0]);
            write(fdComplete[1],resOper,sizeof(char)*100);
            close(fdComplete[1]);
        
        }else if(state == 0){ // Division entre 0
            printf("Mr Meeseeks no puede resolver divisiones entre O\n");
            close(fdComplete[0]);
            write(fdComplete[1],"div0",5);
            close(fdComplete[1]);
        }else{ // operacion invalida
            printf("Mr Meeseeks no puede realizar la operacion: '%c'\n",ope);
            close(fdComplete[0]);
            write(fdComplete[1],"nan",5);
            close(fdComplete[1]);
        }

        exit(0);

    }else{ // Padre
        wait(NULL);
    
        char buf[100];

        close(fdComplete[1]);
        read(fdComplete[0],buf,sizeof(buf));
        close(fdComplete[0]);

        tiempoTotal = (double)(clock() - inicio) / CLOCKS_PER_SEC;
        char* log = malloc(sizeof(char)*1000);

        if(!strcmp(buf,"div0")){
            sprintf(log,"- Mr Meeseeks: %d, No soluciono la operacion por una division entre 0\n",pid);
            *stateDone = 0;
        }else if(!strcmp(buf,"nan")){
            sprintf(log,"- Mr Meeseeks: %d, No soluciono la operacion por un operador desconocido\n",pid);
            *stateDone = 0;
        }else{
            sprintf(log,"- Mr Meeseeks: %d, soluciono la operacion '%s', tardo : %f\n",pid,buf, tiempoTotal);
            *stateDone = 1;
        }

        //printf("Mr Meeseeks %d tardo %f\n",pid,tiempoTotal);
        //printf("%s\n",log);
        return log;
    }

}

/*FUNCIONES PARA LAS PETICIONES DE CORRER PROGRAMAS*/

char* readProgram(){
    char* program = malloc(sizeof(char)*10000);

    printf("Ingrese el nombre del programa a ejecutar: ");

    scanf(" %[^\n]", program);
    getchar();

    printf("Mr. Meeseeks ejecutara: '%s'\n",program);
    return program;

}


char* runProgram(int *stateDone){

    char* program = malloc(sizeof(char)*10000);
    
    int state;

    // Medicion de tiempo
    clock_t inicio = clock();
    double tiempoTotal = 0.0;

    // Piepe para notificar el estado de retorno
    int fdComplete[2];
    pipe(fdComplete);

    pid_t pid = fork();

    if(pid < 0){
        printf("Existence is Pain");
    }else if(pid == 0){ // Hijo
        printf("\nHi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());
        program = readProgram();

        state = system(program);

        if(state == 0){
            printf("Mr Meeseeks pudo ejecutar '%s'\n",program);
            
            close(fdComplete[0]);
            write(fdComplete[1],program,sizeof(char)*10000);
            close(fdComplete[1]);
        }else{
            printf("Mr Meeseeks NO pudo ejecutar '%s'\n",program);

            close(fdComplete[0]);
            write(fdComplete[1],"0",5);
            close(fdComplete[1]);
        }

        exit(0);

    }else{ // Padre
        wait(NULL);

        char buf[10000];

        close(fdComplete[1]);
        read(fdComplete[0],buf,sizeof(buf));
        close(fdComplete[0]);

        tiempoTotal = (double)(clock() - inicio) / CLOCKS_PER_SEC;
        char* log = malloc(sizeof(char)*1000);

        if(!strcmp(buf,"0")){
            sprintf(log,"- Mr Meeseeks: %d, No logro ejecutar el programa\n",pid);
            *stateDone = 0;
        }else{
            sprintf(log,"- Mr Meeseeks: %d, ejectuto el programa '%s', tardo : %f\n",pid,buf, tiempoTotal);
            *stateDone = 1;
        }

        //printf("Mr Meeseeks %d tardo %f\n",pid,tiempoTotal);
        //printf("%s\n",log);
        return log;

    }

}


