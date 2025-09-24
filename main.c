#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <sys/types.h>
int main()
{
    printf("El ID del proceso Abuelo es: %d, su padre es: %d\n", getpid(), getppid());
    pid_t abuelo = fork();
    if(abuelo == 0)
    {
        printf("El ID del proceso Padre es: %d, su padre es: %d\n", getpid(), getppid());
    }
    return 0;
}