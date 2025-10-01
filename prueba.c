    #include <stdio.h>
    #include <unistd.h>
    #include <sys/types.h>
    #include <stdlib.h>
    #define MAX_HIJOS 2
    int main()
    {
        pid_t PIDS[MAX_HIJOS];
        pid_t grandpa = getpid();
        printf("El ID del proceso Abuelo es: %d, su padre es: %d\n", getpid(), getppid());
        for(int i = 0; i < MAX_HIJOS; i++)
        {
            pid_t son_pid = getpid();
            if(son_pid == grandpa)
                PIDS[i] = fork();
            if(getpid() == grandpa)
                printf("El abuelo creo su hijo %d con PID: %d\n", i+1, PIDS[i]);
        }

        return 0;
    }