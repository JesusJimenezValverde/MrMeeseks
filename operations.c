#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

int N =1;
int numInstance = 1;

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

    int* pipe_a_padre;
    int* pipes_a_hijos;

    clock_t timeInit = clock();
    double totalTime = 0.0;

    int fdComplete[2];
    pipe(fdComplete);
    char buf[10];

    pid = fork();

    if(pid == 0){ // Hijo
        firstMeeseek = getpid();
        printf("Hi, I'm Mr Meeseeks! Look at me. (pid: %d, ppid: %d, N: %d, i: %d)\n",getpid(),getppid(),N, numInstance);

        if(difficult <= 0){ // Muy dificil! Mr Meeseeks no puede hacer esa tarea
            printf("Mr Meeseeks! can't do it\n");
            close(fdComplete[0]);
            write(fdComplete[1],"-1",5);
            close(fdComplete[1]);
            exit(0);
        }

        /*
        do{
            if(tryRequest(difficult)==1){
                printf(
                    "Hi I'm Mr Meeseeks! Look at Meeeee. (pid: %d, ppid: %d, Difi: %d)\n",
                    getpid(),
                    getppid(),
                    difficult
                );
                close(fdComplete[0]);
                write(fdComplete[1],"1",5);
                close(fdComplete[1]);
                exit(0);
                break;
            }else{
                int numChild = amountChild(difficult);
                difficult = diluirDificultad(difficult, numChild);
                
                for(int i = 0; i < numChild; i++){
                    //temp_instancia = i +1;

                    int* pipe_temp = malloc(sizeof(int)*2);
                    pipe(pipe_temp);

                    pid = fork();

                    if(pid == 0){// Hijo

                        printf(
                            "Hi I'm Mr Meeseeks! Look at Meeeee. (pid: %d, ppid: %d, N: %d, i: %d, diff:%d)\n",
                            getpid(),
                            getppid(),
                            N,
                            numInstance,difficult
                        );

                        char* mensaje = malloc(sizeof(char)*500);
                        close(pipe_a_padre[1]);
                        read(pipe_a_padre[0], mensaje, sizeof(mensaje));
                        close(pipe_a_padre[0]);

                        difficult = diluirDificultad(difficult, numChild);
                        printf("%d\n",difficult);
                        //exit(0);
                        break;

                    }else{ // Padre

                        close(pipe_temp[0]);
                        write(pipe_temp[1], req, sizeof(req));
                        close(pipe_temp[1]);

                    }

                }

            }
            
        }while(true);*/


        /*while(1==1){
            if(tryRequest(difficult)==1){ // Mr Meeseeks puede hacerlo sin ayuda
                printf(
                    "Hi I'm Mr Meeseeks! Look at Meeeee. (pid: %d, ppid: %d, Difi: %d)\n",
                    getpid(),
                    getppid(),
                    difficult
                );
                //close(fdComplete[0]);
                //write(fdComplete[1],"1",5);
                //close(fdComplete[1]);
                exit(0);
                break;
            }else{ // Mr Meeseeks necesita ayuda de otros Mr Meeseeks
                int numChild = amountChild(difficult);
                printf("\n\nVoy a crear %d hijos\n\n",numChild);
                //N++;

                //if(numInstance > 100){ // caos
                //    close(fdComplete[0]);
                //    write(fdComplete[1],"2",5); // caos
                //    close(fdComplete[1]);
                //    exit(0);
                //    break;
                //}

                //pipes_a_hijos = malloc(sizeof(int)*numChild);

                for(int i = 0; i < numChild; i++){
                    //temp_instancia = i +1;

                    int* pipe_temp = malloc(sizeof(int)*2);
                    pipe(pipe_temp);

                    pid = fork();

                    if(pid == 0){// Hijo

                        printf(
                            "Hi I'm Mr Meeseeks! Look at Meeeee. (pid: %d, ppid: %d, N: %d, i: %d, diff:%d)\n",
                            getpid(),
                            getppid(),
                            N,
                            numInstance,difficult
                        );

                        char* mensaje = malloc(sizeof(char)*500);
                        close(pipe_a_padre[1]);
                        read(pipe_a_padre[0], mensaje, sizeof(mensaje));
                        close(pipe_a_padre[0]);
                        printf("mensaje:%s",mensaje);

                        difficult = diluirDificultad(difficult, numChild);
                        printf("New Dificult: %d\n",difficult);
                        //exit(0);
                        break;

                    }else{ // Padre

                        close(pipe_temp[0]);
                        write(pipe_temp[1], req, sizeof(req));
                        close(pipe_temp[1]);

                        wait(NULL);


                    }

                }
                
                wait(NULL);
                
                // if(pid != 0){
                //     while (wait(NULL) > 0){}
                //     break;
                // }

                //exit(0);
                //break;
            }
        }*/
        
        while(1==1){
            printf("PID:%d tratando...\n",getpid());
            if(tryRequest(difficult)==1){
                //printf("Yo pid:%d necesito ayuda, lo hago solo\n",getpid());
                printf(
                    "Solucionado Hi I'm Mr Meeseeks! Look at Meeeee. (pid: %d, ppid: %d, Difi: %d)\n",
                    getpid(),
                    getppid(),
                    difficult
                );
                exit(0);
                break;
            }else{
                int numChild = amountChild(difficult);
                printf("El pid:%d creara %d hijos\n",getpid(),numChild);
                for(int i = 0; i < numChild; i++){
                    pid = fork();

                    if(pid == 0){ //si el fork si se realizó y un hijo se creó
                        printf(
                            "Hi I'm Mr Meeseeks! Look at Meeeee. (pid: %d, ppid: %d, Difi: %d)\n",
                            getpid(),
                            getppid(),
                            difficult
                        );
                        difficult = diluirDificultad(difficult,numChild); //difficult*2;
                        break;
                    }else{ // Padre
                        wait(NULL);
                        printf("Padre de los sub meeseeks\n");
                    }

                }
                // if(pid != 0){
                //     exit(0);
                //     break;
                // }
                if(pid != 0){
                    while (wait(NULL) > 0){}
                    break;
                }
            }
        }

    }else{ // Padre
        //close(fdComplete[1]);
        //read(fdComplete[0],buf,sizeof(buf));
        //printf("Padre lee: %s\n",buf );
        wait(NULL);
    }

}