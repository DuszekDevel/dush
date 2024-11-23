#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

#define MAX_CMD_LEN 1024
#define MAX_ARG_COUNT 100

// Funkcja do obsługi komendy `cd`
void change_directory(char *path) {
    if (chdir(path) != 0) {
        perror("cd failed");
    }
}

// Funkcja do wczytywania i przetwarzania komendy
void execute_command(char *cmd) {
    char *args[MAX_ARG_COUNT];
    char *token;
    int arg_count = 0;

    // Podziel komendę na argumenty
    token = strtok(cmd, " \t\n");
    while (token != NULL) {
        args[arg_count++] = token;
        token = strtok(NULL, " \t\n");
    }
    args[arg_count] = NULL; // Terminator tablicy argumentów

    if (args[0] == NULL) {
        return; // Jeśli komenda jest pusta
    }

    // Komenda `exit`
    if (strcmp(args[0], "exit") == 0) {
        printf("Exiting shell...\n");
        exit(0);
    }

    // Komenda `cd`
    if (strcmp(args[0], "cd") == 0) {
        if (arg_count > 1) {
            change_directory(args[1]);
        } else {
            fprintf(stderr, "cd: missing argument\n");
        }
        return;
    }

    // Uruchom polecenie systemowe
    pid_t pid = fork();
    if (pid == 0) {  // Dziecko
        if (execvp(args[0], args) == -1) {
            perror("Execution failed");
            exit(1);
        }
    } else if (pid < 0) {
        perror("Fork failed");
    } else {  // Rodzic
        wait(NULL);  // Czekaj na zakończenie procesu dziecka
    }
}

// Funkcja główna terminala
int main() {
    char cmd[MAX_CMD_LEN];

    // Pętla główna terminala
    while (1) {
        printf("my_shell> ");
        if (fgets(cmd, sizeof(cmd), stdin) == NULL) {
            if (feof(stdin)) {
                break;  // Jeśli EOF, zakończ program
            } else {
                perror("Error reading command");
                continue;
            }
        }

        execute_command(cmd);
    }

    return 0;
}
