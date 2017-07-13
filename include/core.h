#ifndef CORE_H
#define CORE_H

#define _DEFAULT_SOURCE

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <unistd.h>
#include <pthread.h>
#include <fcntl.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME "/queue-chat-server-dev"
#define CLIENT_QUEUE_NAME "/queue-chat-client-dev"

#define QUEUE_FLAGS 0L
#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 10L
#define MAX_MSG_SIZE 8192L
#define QUEUE_CUR_MSG 0L

#define MAX_DATA_MSG 2
#define MAX_TEXT_MSG_SIZE 256

#define MAX_NAME_SIZE 32
#define MAX_TIME_SIZE 64
#define MAX_LOG_MSG_SIZE 256

#define SERVER_PID 1

#define SERVER_MSG_SIZE sizeof(SERVER_MSG)
#define CLIENT_MSG_SIZE sizeof(CLIENT_MSG)

enum CLIENT_EVENTS {
	CONNECTED,
	CHANGE_NAME,
	DISCONNECTED
};

enum CLIENT_MSG_TYPE {
	PUBLIC,
	PRIVATE,
	ADM_MSG
};

enum LOG_MSG_TYPE {
	INFO,
	WARNING,
	ERROR
};

typedef struct srv_msg {
	int msg_type;

	char data_text[MAX_DATA_MSG][MAX_TEXT_MSG_SIZE];
	pid_t data_pid;
} SERVER_MSG;

typedef struct cl_msg {
        int msg_type;

        char snd_name[MAX_NAME_SIZE];
        char rcp_name[MAX_NAME_SIZE];

        pid_t snd_pid;
        pid_t rcp_pid;
} CLIENT_MSG;

typedef struct cil {
	char cln_name[MAX_NAME_SIZE];
	pid_t cln_pid;

	struct cil *next;
	struct cil *prev;
} CLIENTS_INFO_LIST;

void* srv_event_hndl(void* args);
void* cln_msg_hndl(void* args);

char* get_cur_dt(char* format);
void sys_log(char* msg, int type, int fd);

void make_key();
void destructor(void* ptr);

#endif
