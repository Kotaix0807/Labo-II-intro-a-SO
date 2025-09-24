//Manejo de memoria para resolver problema de no compartir memoria entre procesos creados por fork()

// Ejemplo de Creación de un segmento de memoria compartida 

#include <stdio.h> 
#include <stdlib.h> 
#include <unistd.h> 
#include <sys/types.h> 
#include <sys/ipc.h> 
#include <sys/shm.h> 
#include <sys/wait.h> 
int main() {
    
    // Crear un segmento de memoria compartida 
    int shmid; 
    int *shared_data; 
    key_t key = 1234; // Llave única para la memoria compartida 

    // Crear un segmento de memoria compartida de tamaño para 2 enteros 
    if ((shmid = shmget(key, 2 * sizeof(int), IPC_CREAT | 0666)) < 0) 
    { 
        perror("Error al crear la memoria compartida"); 
        exit(1); 
    }
    // Adjuntar la memoria compartida
    if ((shared_data = (int *)shmat(shmid, NULL, 0)) == (int *)-1) 
    { 
        perror("Error al adjuntar la memoria compartida"); 
        exit(1); 
    } 
    // Inicializar los valores compartidos 
    shared_data[0] = 0; 
    // Contador del primer hijo 
    shared_data[1] = 0; 
    // Contador del segundo hijo 
    pid_t pid1, pid2; 
    // Crear el primer proceso hijo 
    pid1 = fork();
    if (pid1 < 0) 
    { 
        perror("Error al crear el primer hijo"); 
        exit(1);
    } 
    else 
        if (pid1 == 0)
        {
        // Proceso hijo 1 
        printf("Proceso hijo 1 (PID: %d) en ejecución...\n", getpid()); 
        for (int i = 0; i < 5; i++) 
        {
            shared_data[0] += 1; 
        // Incrementar el contador del hijo 1 
            sleep(1); 
        // Simular trabajo 
        } 
        printf("Proceso hijo 1 (PID: %d) ha terminado. Valor compartido: %d\n", getpid(), shared_data[0]); exit(0); 
        } 
    else 
    {
        // Crear el segundo proceso hijo 
        pid2 = fork();
        if (pid2 < 0) 
        { 
            perror("Error al crear el segundo hijo"); 
            exit(1); 
        } 
        else if (pid2 == 0) 
        {
            // Proceso hijo 2 
            printf("Proceso hijo 2 (PID: %d) en ejecución...\n", getpid()); 
            for (int i = 0; i < 5; i++) 
            {
                shared_data[1] += 2; 
                // Incrementar el contador del hijo 2 
                sleep(1); 
                // Simular trabajo 
            }
            printf("Proceso hijo 2 (PID: %d) ha terminado. Valor compartido: %d\n", getpid(), shared_data[1]); exit(0); 
        } 
        else 
        { 
            // Proceso padre 
            printf("Proceso padre (PID: %d) esperando a los hijos...\n", getpid()); 
            // Esperar a que ambos hijos terminen 
            wait(NULL);
            wait(NULL);
            // Imprimir los valores finales en la memoria compartida
            printf("Proceso padre (PID: %d): Los valores finales son: hijo 1 = %d, hijo 2 = %d\n", getpid(), shared_data[0], shared_data[1]); 
            // Desasociar la memoria compartida
            shmdt(shared_data); 
            // Liberar el segmento de memoria compartida 
            shmctl(shmid, IPC_RMID, NULL); 
        } 
    } 
    return 0; 
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
