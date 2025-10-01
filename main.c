//Manejo de memoria para resolver problema de no compartir memoria entre procesos creados por fork()
// Ejemplo de Creación de un segmento de memoria compartida 
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
    
    if (argc < 4)
    {
        perror("Error, no se ingresaron argumentos necesarios");
        exit(1);
    }
    if(argc > 4)
    {
        perror("Error, se ingresaron demasiados argumentos");
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

    printf("Valores iniciales en memoria compartida:\nhijo 1 = %d\nhijo 2 = %d\nhijo 3 = %d\n", shared_data[0], shared_data[1], shared_data[2]);
    pid_t pid1, pid2; 
    pid1 = fork();
    if (pid1 < 0) 
    { 
        perror("Error al crear al Padre"); 
        exit(1);
    } 
    else 
        if (pid1 == 0)
        {
            // Proceso Padre 
            printf("Proceso Padre (PID: %d) en ejcución...\n", getpid()); 
            shared_data[0] *= 2; // Duplica *2 el primer valor
            /*
            for (int i = 0; i < 5; i++) 
            {
                // Incrementar el contador del hijo 1 
                //shared_data[0] += 1; 
                // sleep(1); 
                // Simular trabajo
            }*/
            printf("Proceso Padre (PID: %d) ha terminado. Valor compartido: %d\n", getpid(), shared_data[0]);
            exit(0); 
        } 
    else 
    {
        // Crear el segundo proceso hijo 
        pid2 = fork();
        if (pid2 < 0) 
        { 
            perror("Error al crear al hijo"); 
            exit(1); 
        } 
        else if (pid2 == 0) 
        {
            // Proceso hijo 2 
            printf("Proceso hijo (PID: %d) en ejecución...\n", getpid());
            sqrt(shared_data[2]); // Raíz cuadrada del tercer valor
            /*
            for (int i = 0; i < 5; i++) 
            {
                shared_data[1] += 2; 
                // Incrementar el contador del hijo 2 
                //sleep(1);
                // Simular trabajo 
            }*/
            printf("Proceso hijo (PID: %d) ha terminado. Valor compartido: %d\n", getpid(), shared_data[2]);
            exit(0); 
        } 
        else 
        { 
            // Proceso Abuelo
            printf("Proceso Abuelo (PID: %d) esperando a los hijos...\n", getpid()); 
            ElevateNumb(&shared_data[2], 3); // Eleva a la potencia 3 el segundo valor
            // Esperar a que ambos hijos terminen 
            wait(NULL);
            wait(NULL);
            // Imprimir los valores finales en la memoria compartida
            printf("Proceso Abuelo (PID: %d): Los valores finales son:\nAbuelo = %d\nPadre = %d\nHijo = %d\n", getpid(), shared_data[0], shared_data[1], shared_data[2]); 
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
    for(int i = 1; i < exp; i++)
        base[0] *= base[0];
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
/*
Explicación:
1.	Memoria Compartida:
	Usamos shmget para crear un segmento de memoria compartida.
	shmat lo adjunta al espacio de direcciones del proceso para su uso.
	shmdt desasocia la memoria compartida y shmctl libera el segmento cuando ya no es necesario.
2.	Proceso Padre:
	Inicializa dos valores en la memoria compartida: uno para cada hijo.
	Crea dos procesos hijos.
3.	Proceso Hijo 1:
	Incrementa su valor compartido en la memoria en cada iteración.
4.	Proceso Hijo 2:
	Incrementa su valor compartido de manera diferente (en este caso, suma de 2 en 2).
5.	Sincronización:
	El proceso padre usa wait() para esperar a que ambos hijos terminen.
	Luego, imprime los valores finales de la memoria compartida.*/
