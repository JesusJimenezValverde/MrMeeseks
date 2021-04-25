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


#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>

int N =1;

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

        if(type >= 0 && type <= 40){
            difficult = randGenerate(0, 45);
        }else if(type > 40 && type <= 80){
            difficult = randGenerate(45, 85);
        }else{ // 80 <= type <= 100
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
        return randGenerate(3, 10); 
        
    }else if(difficult > 45 & difficult <= 85){
        return randGenerate(1, 5);

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

key_t clave;	//Clave de acceso a la zona de memoria
long int id;	//Identificador de la zona de memoria
int *pmem=NULL;	//Puntero a la zona de memoria

char * textualRequest(int *stateDone, int amountSegToChaos){
    char * req = readRequest();
    int difficult = readDifficult();

    pid_t pid;
    int firstMeeseek;
    int temp_instancia;
    int temp_N = 1;

    int* pipe_a_padre;
    int* pipes_a_hijos;

    clave=ftok("/bin/ls",33); //Cualquier fichero existente y cualquier int
	id=shmget(clave,sizeof(int)*100,0777|IPC_CREAT);
	pmem=(int *)shmat(id,(char *)0,0);

    pmem[5] = 1;

    // Variables para medir tiempos
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    // Piepe para notificar el estado de retorno
    int fdComplete[2];
    pipe(fdComplete);
    char buf[100];

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
        printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d)\n",getpid(),getppid(),temp_N, 1);

        if(difficult <= 0){ // Muy dificil! Mr Meeseeks no puede hacer esa tarea

            pid_t newPid;

            for (int i = 0; i < 4; i++){
                if(fork() == 0){
                    printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d)\n",getpid(),getppid(),2, i+1);
                    sleep(randGenerate(1,5));
                    printf("Mr Meeseeks(pid:%d) can't do it\n",getpid());
                    exit(0);
                }else{
                    wait(NULL);
                }
            }
            //something similar but 
            //printf("Mr Meeseeks can't do it\n");
            close(fdComplete[0]);
            write(fdComplete[1],"-1",5);
            close(fdComplete[1]);
            exit(0);
        }

        while(1==1){
            printf("PID:%d tratando...\n",getpid());

            clock_gettime(CLOCK_REALTIME, &end);
            long seconds = end.tv_sec - begin.tv_sec;
            long nanoseconds = end.tv_nsec - begin.tv_nsec;
            double elapsed = seconds + nanoseconds*1e-9;

            if(elapsed > amountSegToChaos){
                close(fdComplete[0]);
                write(fdComplete[1],"0",5);
                close(fdComplete[1]);
                exit(0);
                break;
            }


            if(tryRequest(difficult)==1){
                
                printf("Lo logre!! Mr Meeseeks(pid: %d, ppid: %d, N: %d, dif: %d) Bye!!!\n",getpid(),getppid(),temp_N,difficult);
                
                close(fdComplete[0]);
                write(fdComplete[1],"1",sizeof(char)*100);
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

                        printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d, dif: %d)\n",getpid(),getppid(),temp_N, i+1,difficult);

                        // Area de memoria compartidad del numero de meeseeks
                        pmem[5] ++;

                        // Se recibe el req por un pipe
                        close(pipe_temp[1]);
                        read(pipe_temp[0], mensaje, sizeof(char)*1000);
                        close(pipe_temp[0]);
                        //printf("mensaje:%s\n",mensaje);

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
        
        // Medicion de tiempo
        clock_gettime(CLOCK_REALTIME, &end);
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed = seconds + nanoseconds*1e-9;

        char* log = malloc(sizeof(char)*1000);
        
        if(!strcmp(buf,"-1")){
            sprintf(log, "- Mr Meeseeks: %d NO logro hacer la tarea '%s'(dificultad:%d)\n",pid,req,difficult);
            *stateDone = 0;
        }else if(!strcmp(buf,"2")){
            printf("\n\n☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢\n\t\tCAOS PLANETARIO\n☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢ ☢\n\n");
            sprintf(log, "- Se declaro CAOS PLANETARIO para la tarea '%s'(dificultad:%d)\n",req,difficult);
            *stateDone = 0;
        }else{
            sprintf(log, "- Mr Meeseeks: %d, hizo tarea '%s'(dificultad:%d), tardo : %f seg, uso: %d\n",pid,req,difficult,elapsed,pmem[5]);
            *stateDone = 1;
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

char* aritmeticLogicRequest(int *stateDone){

    char* operation = malloc(sizeof(char)*10000);
    int num1 = 0;
    int num2 = 0;
    int result = 0;

    char ope;

    // Solicitar info
    operation = readOperation();

    // Medicion de tiempo
    struct timespec begin, end; 
    clock_gettime(CLOCK_REALTIME, &begin);

    // Piepe para notificar el estado de retorno
    int fdComplete[2];
    pipe(fdComplete);

    pid_t pid = fork();

    if(pid < 0){
        printf("Existence is Pain");
    }else if(pid == 0){ // Hijo
        printf("\nHi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());
        
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

        // Medir el tiempo
        clock_gettime(CLOCK_REALTIME, &end);
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed = seconds + nanoseconds*1e-9;

        char* log = malloc(sizeof(char)*1000);

        if(!strcmp(buf,"div0")){
            sprintf(log,"- Mr Meeseeks: %d, No soluciono la operacion por una division entre 0\n",pid);
            *stateDone = 0;
        }else if(!strcmp(buf,"nan")){
            sprintf(log,"- Mr Meeseeks: %d, No soluciono la operacion por un operador desconocido\n",pid);
            *stateDone = 0;
        }else{
            sprintf(log,"- Mr Meeseeks: %d, soluciono la operacion '%s', tardo : %f seg\n",pid,buf, elapsed);
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

    //printf("Mr. Meeseeks ejecutara: '%s'\n",program);
    return program;

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
    int fdComplete[2];
    pipe(fdComplete);

    pid_t pid = fork();

    if(pid < 0){
        printf("Existence is Pain");
    }else if(pid == 0){ // Hijo
        printf("\nHi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d)\n",getpid(),getppid());

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

        // Medicion de tiempo
        clock_gettime(CLOCK_REALTIME, &end);
        long seconds = end.tv_sec - begin.tv_sec;
        long nanoseconds = end.tv_nsec - begin.tv_nsec;
        double elapsed = seconds + nanoseconds*1e-9;

        char* log = malloc(sizeof(char)*1000);

        if(!strcmp(buf,"0")){
            sprintf(log,"- Mr Meeseeks: %d, No logro ejecutar el programa\n",pid);
            *stateDone = 0;
        }else{
            sprintf(log,"- Mr Meeseeks: %d, ejectuto el programa '%s', tardo : %f seg\n",pid,buf, elapsed);
            *stateDone = 1;
        }

        //printf("Mr Meeseeks %d tardo %f\n",pid,tiempoTotal);
        //printf("%s\n",log);
        return log;

    }

}
