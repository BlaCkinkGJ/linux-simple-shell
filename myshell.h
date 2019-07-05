#ifndef __MYSHELL_H__
#define __MYSHELL_H__

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <ctype.h>
#include <pwd.h>
#include <sys/wait.h>
#include <sys/types.h>

/****COMMAND COLOR****/
#define KNRM  "\x1B[0m"
#define KRED  "\x1B[31m"
#define KGRN  "\x1B[32m"
#define KYEL  "\x1B[33m"
#define KBLU  "\x1B[34m"
#define KMAG  "\x1B[35m"
#define KCYN  "\x1B[36m"
#define KWHT  "\x1B[37m"

/****SHELL MACRO****/
#define SHELL_RUNNING() while(1)
#define IS_CHILD_PROC(pid) pid == 0
#define INIT_BUF(buf, size) memset(buf, '\0', size)
#define SHELL_STDOUT(buf) printf("%s", buf)
#define SHELL_STDIN(buf, size) fgets(buf, size, stdin) == NULL
#define ERROR_MSG(msg, ...) fprintf(stderr, (msg), ##__VA_ARGS__)

/****DEFINE MAX SIZE****/
#define MAX_CHAR_SIZE 256
#define MAX_DIR_SIZE 1024
#define MAX_NAME_SIZE 128
#define MAX_TIME_SIZE 768
#define MAX_NOTICE_SIZE 4096
#define MAX_ARG_SIZE 256

/****SIGNAL HANDLING****/
#define INPUT_ERROR -1
#define FORK_FAILED 1
#define WAIT_FAILED 1
#define COMMAND_NOT_FOUND 127

/****SOME STRUCTURE****/
struct notice_format{
    char dir[MAX_DIR_SIZE];
    char now[MAX_TIME_SIZE];
    char host[MAX_NAME_SIZE];
    char user[MAX_NAME_SIZE];
};

struct time_format{
    int hour;
    int min;
    int sec;
};

/****WRAPPING FUNCTION AND USEFUL FUNCTION****/
static inline char *get_current_dir(char *buf, size_t size){
    return getcwd(buf, size);
}

static inline int get_host_name(char *name, size_t len){
    return gethostname(name, len);
}

static inline int get_user_name(char *buf, size_t size){
    struct passwd *pw;
    uid_t uid;

    uid = getuid();
    pw = getpwuid(uid);
    return snprintf(buf, size, "%s", pw->pw_name);
}

static inline int get_current_time(char *buf, size_t size){
    time_t raw_time;
    struct tm *time_info;
    struct time_format cast_time;
    char hour_tag[3] = "AM";

    time(&raw_time);
    time_info = localtime( &raw_time );

    cast_time.hour = time_info->tm_hour > 12 ? time_info->tm_hour % 12 : time_info->tm_hour;
    cast_time.min = time_info->tm_min;
    cast_time.sec = time_info->tm_sec;
    if(!strncpy(hour_tag, (time_info->tm_hour > 11 ? "PM" : "AM"), sizeof(hour_tag)))
        return -1;
    sprintf(buf, "%.2d:%.2d:%.2d%s",
                cast_time.hour,
                cast_time.min,
                cast_time.sec,
                hour_tag);
    return 0;
}

static inline int do_fork(pid_t *pid){
    return ((*pid = fork()) < 0);
}

static inline int do_wait_child(pid_t *pid, int *status){
    return ((*pid = waitpid(*pid, status, 0)) < 0);
}

#endif
