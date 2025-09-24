#include <stdio.h>
#include <unistd.h>
int main()
{
    int x = 5;
    int times = 2;
    for(int i = 0; i < x; i++)
    {
        times *= times;
    }
    printf("El valor de times es: %d\n", times);
    for(int i = 0; i < x; i++)
    {
        fork();
    }
    printf("Hola mundo\n");
    return 0;
}