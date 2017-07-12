#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <pthread.h>
#include <fcntl.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME "/queue-chat-server"
#define CLIENT_QUEUE_NAME "/queue-chat-client"

#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES  256L
#define MAX_MSG_SIZE 256L
#define MAX_NAME_SIZE 32

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

typedef struct srv_msg {
	int msg_type;

	char data[MAX_MSG_SIZE];
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

#endif
