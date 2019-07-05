#include "myshell.h"
#include <sys/wait.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

static char *notice();
static int  replace_newline_to_zero(char *buf);
static void do_exec(char *buf);
static void sig_int(int signo);

int main(void){
    char  buf[MAX_CHAR_SIZE];
    pid_t pid;
    int   status;

    if(signal(SIGINT, sig_int) == SIG_ERR)
        ERROR_MSG(KRED"signal has problem\n"KNRM);

    SHELL_RUNNING(){
        SHELL_STDOUT(notice());
        if(SHELL_STDIN(buf, MAX_CHAR_SIZE)){
            SHELL_STDOUT(KYEL"\nCtrl+D detected.. bye bye\n"KNRM);
            exit(0);
        }

        replace_newline_to_zero(buf);

        if(do_fork(&pid) == FORK_FAILED){
            ERROR_MSG(KRED"fork failed\n"KNRM);
        }
        else if(IS_CHILD_PROC(pid)){
            do_exec(buf);
            ERROR_MSG(KRED"couldn't execute: %s\n"KNRM, buf);
            exit(COMMAND_NOT_FOUND);
        }

        if(do_wait_child(&pid, &status) == WAIT_FAILED)
            ERROR_MSG(KRED"waitpid has problem\n"KNRM);
    }
    return 0;
}

// print the notice message
char *notice(){
    static char buf[MAX_NOTICE_SIZE];
    struct notice_format rfmt;

    INIT_BUF(buf, sizeof(buf));

    get_current_time(rfmt.now, sizeof(rfmt.now));
    get_host_name(rfmt.host, sizeof(rfmt.host));
    get_user_name(rfmt.user, sizeof(rfmt.user));
    get_current_dir(rfmt.dir, sizeof(rfmt.dir));
    sprintf(buf, KRED"[%s]"KGRN"%s@%s"KBLU"(%s)"KNRM"$", rfmt.now, rfmt.host, rfmt.user, rfmt.dir);

    return buf;
}

int replace_newline_to_zero(char *buf){
    if(buf[strlen(buf) - 1] == '\n'){
        buf[strlen(buf) - 1] = 0;
        return 0;
    }
    return -1;
}

void do_exec(char *buf){
    char *str = strdup(buf), *token;
    int index = 0;
    char *argv[MAX_ARG_SIZE];
    token = strtok(str," \t");
    while(token != NULL){
        argv[index] = strdup(token);
        token = strtok(NULL, " \t");
        index += 1;
    }
    argv[index] = (char*)0;
    execvp(argv[0], argv);
}

void sig_int(int signo){
    SHELL_STDOUT("\nSomeone gives me an interrupt signal.\n");
    SHELL_STDOUT(notice());
}
