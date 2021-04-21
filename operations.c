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

char* readRequest(){
    char* tarea = malloc(sizeof(char)*1000);

    printf("Que necesita que haga Mr. Meeseeks? ");
    scanf(" %[^\n]", tarea);
    getchar();

    printf("Mr. Meeseeks hara: %s\n",tarea);
    return tarea;
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

char * textualRequest(){
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
            printf("Mr Meeseeks! can't do it\n");
            // close(fdComplete[0]);
            // write(fdComplete[1],"-1",5);
            // close(fdComplete[1]);
            exit(0);
        }

        while(1==1){
            printf("PID:%d tratando...\n",getpid());
            if(tryRequest(difficult)==1){
                
                printf("Solucionado Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d, dif: %d, mens: %s)\n",getpid(),getppid(),temp_N, numInstance,difficult,mensaje);
                
                // close(fdComplete[0]);
                // write(fdComplete[1],"1",5);
                // close(fdComplete[1]);
                exit(0);
                //kill(getpid(),0);
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
                        printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d, dif: %d, mens: %s)\n",getpid(),getppid(),temp_N, numInstance,difficult,mensaje);

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
        // close(fdComplete[1]);
        // read(fdComplete[0],buf,sizeof(buf));
        // close(fdComplete[0]);
        // printf("Padre lee: %s\n",buf );
        wait(NULL);
    }

}