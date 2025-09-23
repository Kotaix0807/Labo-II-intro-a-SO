#include <stdio.h>
#include <unistd.h>

int main()
{
    fork();
    printf("El ID de este proceso es: %d\n", getpid());
    return 0;
}