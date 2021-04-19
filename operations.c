#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <math.h>

int randGenerate(int inicio, int fin){
    return (rand() % (fin - inicio + 1)) + inicio;
}

char* readRequest(){
    char* tarea = malloc(sizeof(char)*1000);

    printf("Que necesita que haga Mr. Meeseeks? ");
    scanf("%[^\n]", tarea);
    //getchar();

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

char * textualRequest(){
    char * req = readRequest();
    int difficult = readDifficult();

    printf(req);
    printf("%d",difficult);

    return "hola";
}