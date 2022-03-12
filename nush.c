#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include "tokens.h"
#include "hashmap.h"

void execute(svec* tokens);

hashmap* vars;


// execute the "cd" operation
void
cd(svec* tokens) {
    if (tokens->size > 1) {
        char* dir = svec_get(tokens, 1);
        if (chdir(dir) == -1) {
            printf("cd: no such file or directory: ", dir);
        }
    } else {
        chdir(getenv("HOME"));
    }
}


// check the pipe return value (attribution: pipe0.c in Nat's scratch repo)
void
check_rv(int rv) {
    if (rv == -1) {
        perror("fail");
        exit(1);
    }
}


// execute the "<" operation
void
exe_rin(svec* tokens, int op_idx) {
    int cpid;
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
    } else {
        char* filepath = svec_get(tokens, op_idx + 1);
        int fd = open(filepath, O_RDONLY, S_IRUSR | S_IRGRP);
        dup2(fd, 0);
        close(fd);
        char* args[op_idx + 1];
        for (int ii = 0; ii < op_idx; ++ii) {
            args[ii] = svec_get(tokens, ii);
        }
        args[op_idx] = 0;
        execvp(svec_get(tokens, 0), args);
    }
}


// execute the ">" operation
void
exe_rout(svec* tokens, int op_idx) {
    int cpid;
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
    } else {
        char* filepath = svec_get(tokens, op_idx + 1);
        int fd = open(filepath, O_CREAT | O_TRUNC | O_WRONLY, S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP);
        dup2(fd, 1);
        close(fd);
        char* args[op_idx + 1];  
        for (int ii = 0; ii < op_idx; ++ii) {
            args[ii] = svec_get(tokens, ii);
        }
        args[op_idx] = 0; 
        execvp(svec_get(tokens, 0), args);
    }
}


// execute the "|" operation
void
exe_pipe(svec* tokens, int op_idx) {
    svec* left = make_svec();
    for (int ii = 0; ii < op_idx; ++ii) {
        svec_push_back(left, svec_get(tokens, ii));
    }

    svec* right = make_svec();
    for (int ii = op_idx + 1; ii < tokens->size; ++ii) {
        svec_push_back(right, svec_get(tokens, ii));
    }

    int cpid;
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
    } else {
        int pfd[2], rv;
        rv = pipe(pfd);
        check_rv(rv);

        int cpid2;
        if ((cpid2 = fork())) {

            int cpid3;
            if ((cpid3 = fork())) {
                close(pfd[0]);
                close(pfd[1]);
                int status;
                waitpid(cpid2, &status, 0);
                waitpid(cpid3, &status, 0);
            } else {
                close(pfd[1]);
                rv = dup2(pfd[0], 0);
                check_rv(rv);
                close(pfd[0]);
                execute(right);
                free_svec(right);
                exit(0);
            }
        } else {
            close(pfd[0]);
            rv = dup2(pfd[1], 1);
            check_rv(rv);
            close(pfd[1]);
            execute(left);
            free_svec(left);
            exit(0);
        }
    }
}


// execute the "&" operation
void
exe_background(svec* tokens) {
    int cpid;
    if ((cpid = fork())) {
        // leave in background
    } else {
        tokens->data[tokens->size - 1] = 0;
        execvp(svec_get(tokens, 0), tokens->data);
    }
}


// execute the "&&" and "||" operations
void
exe_logic(svec* tokens, int and, int op_idx) { 
    char* left[op_idx + 1];   
    for (int ii = 0; ii < op_idx; ++ii) {
        left[ii] = svec_get(tokens, ii);
    }
    left[op_idx] = 0;

    svec* right = make_svec();
    for (int ii = op_idx + 1; ii < tokens->size; ++ii) {
        svec_push_back(right, svec_get(tokens, ii));
    }

    int cpid;
    if ((cpid = fork())) {
        int status;
        waitpid(cpid, &status, 0);
        if ((and && !WEXITSTATUS(status)) || (!and && WEXITSTATUS(status))) {
            execute(right);
        }
        free_svec(right);
    } else {
        execvp(svec_get(tokens, 0), left);                      
    }
}


// execute the ";" operation
void
exe_semi(svec* tokens, int op_idx) {
    svec* left = make_svec();
    for (int ii = 0; ii < op_idx; ++ii) {
        svec_push_back(left, svec_get(tokens, ii));
    }

    svec* right = make_svec();
    for (int ii = op_idx + 1; ii < tokens->size; ++ii) {
        svec_push_back(right, svec_get(tokens, ii));
    }

    execute(left);
    free_svec(left);
    execute(right);
    free_svec(right);
}


// assign values to variables in a hashmap
void
assign_vars(svec* tokens, int op_idx) {
    char* var = svec_get(tokens, op_idx - 1);
    char* val = svec_get(tokens, op_idx + 1);
    hashmap_put(vars, var, val);
}


// execute a command with no operators
void
exe_nop(svec* tokens) {
    if (!strcmp(svec_get(tokens, 0), "cd")) {
        cd(tokens);
        return;
    }
    if (!strcmp(svec_get(tokens, 0), "exit")) {
        exit(0);
    }

    int cpid;
    if ((cpid = fork())) {
        int status;
		waitpid(cpid, &status, 0);
    } else {
        char* args[tokens->size];
        int ii = 0;
        int jj = 0;
        while (ii < tokens->size) {
            if (!strcmp(svec_get(tokens, ii), "$")) {
                args[jj] = hashmap_get(vars, svec_get(tokens, ii + 1));
                ii = ii + 2;
                jj++;
            } else {
                args[jj] = svec_get(tokens, ii);
                ii++;
                jj++;
            }
        }
        args[jj] = 0;
        execvp(svec_get(tokens, 0), args);
    }
}


// delegate execution based on the presence of operators
void
execute(svec* tokens) {
    if (svec_has(tokens, "=") != -1) {
            assign_vars(tokens, svec_has(tokens, "="));
    }
    else if (svec_has(tokens, ";") != -1) {
        exe_semi(tokens, svec_has(tokens, ";"));
    }
    else if (svec_has(tokens, "|") != -1) {
        exe_pipe(tokens, svec_has(tokens, "|"));
    }
    else if (svec_has(tokens, "&") != -1) {
        exe_background(tokens);
    }
    else if (svec_has(tokens, "&&") != -1) {
        exe_logic(tokens, 1, svec_has(tokens, "&&"));
    }
    else if (svec_has(tokens, "||") != -1) {
        exe_logic(tokens, 0, svec_has(tokens, "||"));
    }
    else if (svec_has(tokens, "<") != -1) {
        exe_rin(tokens, svec_has(tokens, "<"));
    }
    else if (svec_has(tokens, ">") != -1) {
        exe_rout(tokens, svec_has(tokens, ">"));
    }
    else {
        exe_nop(tokens);
    }
}


int
main(int argc, char* argv[])
{
    char cmd[256];
    vars = make_hashmap();
    
    if (argc == 1) {
        while (1) {
            printf("nush$ ");
            fflush(stdout);
            char* line = fgets(cmd, 256, stdin);
            if (!line) {
                puts("");
                break;
            }
            if (cmd[0] != '\n') {
                svec* tokens = tokenize(cmd);
                execute(tokens);
                free_svec(tokens);
            }
        }
    } else {
        FILE* file;
        file = fopen(argv[1], "r");
        while (1) {
            if (!file) {
                printf("Could not open file: %s\n", argv[1]);
                exit(1);
            }
            fflush(stdout);
            char* line = fgets(cmd, 256, file);
            if (!line) {
                fclose(file);
                break;
            }
            if (cmd[0] != '\n') {
                svec* tokens = tokenize(cmd);
                execute(tokens);
                free_svec(tokens);
            }  
        }
    }
    free_hashmap(vars);
    return 0;
}
