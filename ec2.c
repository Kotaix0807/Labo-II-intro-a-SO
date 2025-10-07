#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/wait.h>
#include <math.h>

// Convertir string a número (double)

double CharToDouble(char *str);

int main(int argc, char *argv[])
{
    if (argc != 4)
    {
        printf("Uso: %s <a> <b> <c>\n", argv[0]);
        exit(1);
    }

    key_t key = 1234;
    int shmid;
    double *shared_data;

    // Crear segmento de memoria compartida
    if ((shmid = shmget(key, 6 * sizeof(double), IPC_CREAT | 0666)) < 0)
    {
        perror("Error al crear la memoria compartida");
        exit(1);
    }

    // Adjuntar memoria compartida
    if ((shared_data = (double *)shmat(shmid, NULL, 0)) == (double *)-1)
    {
        perror("Error al adjuntar la memoria compartida");
        exit(1);
    }

    // Inicializar a, b, c
    shared_data[0] = CharToDouble(argv[1]); // a
    shared_data[1] = CharToDouble(argv[2]); // b
    shared_data[2] = CharToDouble(argv[3]); // c
    shared_data[6] = 0.0; // raiz del discriminante

    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        perror("Error al crear el proceso Padre");
        exit(1);
    }
    else if (pid1 == 0)
    {
        // Proceso Padre: calcular discriminante y su raiz
        double a = shared_data[0];
        double b = shared_data[1];
        double c = shared_data[2];

        double discriminante = (b * b) - (4 * a * c);
        if (discriminante < 0)
        {
            printf("Padre: discriminante negativo, no hay raices reales.\n");
            shared_data[6] = -1.0; // indicar raiz inválida
        }
        else
            shared_data[3] = sqrt(discriminante); // guardar raiz

        printf("Padre (PID: %d): raiz del discriminante = %.4f\n", getpid(), shared_data[3]);
        exit(0);
    } 
    else
    {
        wait(NULL); // Esperar al Padre

        pid_t pid2 = fork();
        if (pid2 < 0)
        {
            perror("Error al crear el proceso Hijo");
            exit(1);
        }
        else if (pid2 == 0)
        {
            // Proceso Hijo: sumar y restar los resultados
            double a = shared_data[0];
            double b = shared_data[1];
            double raiz = shared_data[3];

            if (shared_data[6] == -1.0)
            {
                printf("Hijo: no puede continuar, raiz inválida.\n");
                exit(0);
            }

            shared_data[4] = -b + raiz; // parte positiva
            shared_data[5] = -b - raiz; // parte negativa

            printf("Hijo (PID: %d): suma = %.4f, resta = %.4f\n", getpid(), shared_data[4], shared_data[5]);
            exit(0);
        }
        else
        {
            // Proceso Abuelo
            wait(NULL); // Esperar al Hijo

            double a = shared_data[0];
            double x1 = shared_data[4] / (2 * a);
            double x2 = shared_data[5] / (2 * a);

            printf("Abuelo (PID: %d): raices finales:\n", getpid());
            printf("x1 = %.4f\tx2 = %.4f\n", x1, x2);

            // Limpiar memoria
            shmdt(shared_data);
            shmctl(shmid, IPC_RMID, NULL);
        }
    }

    return 0;
}

double CharToDouble(char *str)
{
    return atof(str);
}