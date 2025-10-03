//Manejo de memoria para resolver problema de no compartir memoria entre procesos creados por fork()
//Obtener las raíces de una ecuación cuadrática
#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h> 
#include <math.h>

void ElevateNumb(int *base, int exp);
int CharToInt(char *str);

int main(int argc, char *argv[])
{
    if(argc != 4)
    {
        printf("Error: Argumentos invalidos\nUso: %s <valor1> <valor2> <valor3>\n", argv[0]);
        exit(1);
    }
    // Crear un segmento de memoria compartida 
    int shmid; 
    int *shared_data; 
    key_t key = 1234; // Llave única para la memoria compartida 

    // Crear un segmento de memoria compartida de tamaño para 2 enteros 
    if ((shmid = shmget(key, 3 * sizeof(int), IPC_CREAT | 0666)) < 0) 
    { 
        perror("Error al crear la memoria compartida");
        exit(1); 
    }
    // Adjuntar la memoria compartida
    if ((shared_data = (int *)shmat(shmid, NULL, 0)) == (int *) -1) 
    { 
        perror("Error al adjuntar la memoria compartida"); 
        exit(1); 
    } 
    // Inicializar los valores compartidos 
    shared_data[0] = CharToInt(argv[1]); 
    // Contador del primer hijo 
    shared_data[1] = CharToInt(argv[2]);

    shared_data[2] = CharToInt(argv[3]);

    printf("Valores iniciales en memoria compartida:\nPadre = %d\nAbuelo = %d\nHijo = %d\n", shared_data[0], shared_data[1], shared_data[2]);
    pid_t pid1 = fork();
    if (pid1 < 0)
    {
        perror("Error al crear al Padre");
        exit(1);
    }
    else if (pid1 == 0)
    {
        // Proceso Padre
        printf("Proceso Padre (PID: %d) en ejecución...\n", getpid());
        // Tarea...
        // Forma: Raíz(b*b - 4*a*c)
        double aux_a = shared_data[0], aux_b = shared_data[1], aux_c = shared_data[2], result = 0;
        double discriminante = (aux_b * aux_b) - (4 * aux_a * aux_c);
        if (discriminante < 0)
        {
            printf("Error: La raíz cuadrada de un número negativo no está definida en los números reales.\n");
            shared_data[0] = 0; // O manejar el error de otra manera
        }
        else
            result = sqrt((aux_b * aux_b) - (4 * aux_a * aux_c));
        printf("Proceso Padre (PID: %d) ha terminado. Valor compartido: %d\n", getpid(), shared_data[0]);
        exit(0);
    }
    else
    {
        // Esperar a que el padre termine antes de crear el hijo
        pid_t pid2 = fork();
        if (pid2 < 0)
        {
            perror("Error al crear al hijo");
            exit(1);
        }
        else if (pid2 == 0)
        {
            // Proceso hijo
            printf("Proceso hijo (PID: %d) en ejecución...\n", getpid());
            // Tarea...
            printf("Proceso hijo (PID: %d) ha terminado. Valor compartido: %d\n", getpid(), shared_data[2]);
            exit(0);
        }
        else
        {
            // Proceso Abuelo
            printf("Proceso Abuelo (PID: %d) ejecutando su operación...\n", getpid());
            wait(NULL);
            wait(NULL);
            // Imprimir los valores finales en la memoria compartida
            printf("Proceso Abuelo (PID: %d): Los valores finales son:\nPadre = %d\nAbuelo = %d\nHijo = %d\n", getpid(), shared_data[0], shared_data[1], shared_data[2]);
            // Desasociar la memoria compartida
            shmdt(shared_data);
            // Liberar el segmento de memoria compartida
            shmctl(shmid, IPC_RMID, NULL);
        }
    }
    return 0;
}
void ElevateNumb(int *base, int exp)
{
    int aux = base[0];
    for(int i = 1; i < exp; ++i)
        base[0] *= aux;
}

int CharToInt(char *str)
{ 
    int result = 0; 
    while (*str) 
    { 
        result = result * 10 + (*str - '0');
        str++; 
    } 
    return result; 
}
