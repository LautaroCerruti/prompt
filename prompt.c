#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <wait.h>

#define MAXIMO 10

char *leer_entrada() {
    char *entrada = malloc(MAXIMO * sizeof(char));
    int c = 1;
    char caracter;
    printf("$ ");
    for (; (caracter = getchar()) != '\n'; ++c) {
        if ((c % MAXIMO) == 0)
            entrada = realloc(entrada, (c + MAXIMO) * sizeof(char));
        *(entrada + (c - 1)) = caracter;
    }
    entrada = realloc(entrada, c * sizeof(char));
    *(entrada + c - 1) = '\0';
    return entrada;
}

void agregar_argumento(char ***argumentos, int *cantidadArgumentos, char *argumento) {
    (*cantidadArgumentos)++;
    *argumentos = realloc(*argumentos, *cantidadArgumentos * sizeof(char*));

    *(*argumentos + *cantidadArgumentos - 1) = malloc(strlen(argumento) * sizeof(char));
    strcpy(*(*argumentos + *cantidadArgumentos - 1), argumento);
}

int main() {
    char *entrada, *cadena, **argumentos;
    int cantidadArgumentos, aprsn, c, finalizado = 0, error;
    pid_t p;

    while (!finalizado) {
        aprsn = 0, cantidadArgumentos = 1;

        entrada = leer_entrada();

        cadena = strtok(entrada, " ");

        if (cadena) {
            if (!strcmp(cadena, "exit"))
                finalizado = 1;
            else {
                argumentos = malloc(sizeof(char*));
                *(argumentos) = malloc(strlen(cadena) * sizeof(char));
                strcpy(*(argumentos), cadena);

                while (cadena = strtok(NULL, " ")) {
                    if (!strcmp(cadena, "&")) {
                        if (!(cadena = strtok(NULL, " ")))
                            aprsn = 1;
                        else {
                            agregar_argumento(&argumentos, &cantidadArgumentos, "&");
                            agregar_argumento(&argumentos, &cantidadArgumentos, cadena);
                        }
                    } else
                        agregar_argumento(&argumentos, &cantidadArgumentos, cadena);
                }

                p = fork();

                if (p < 0)
                    perror("Error\n");
                
                if (p == 0) {
                    error = execv(*argumentos, argumentos);
                    
                    if (error < 0)
                        perror("Error en ejecucion\n");

                    _exit(0);
                } else {
                    if (!aprsn) {
                        int status;
                        waitpid(p, &status, 0);
                        if (!WIFEXITED(status))
                            perror("Error en el programa");
                    } 
                }

                for (c = 0; c < cantidadArgumentos; ++c)
                    free(*(argumentos + c));
                free(argumentos);
            }
            free(entrada);
        }
    }

    return 0;
}