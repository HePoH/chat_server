#ifndef CORE_H
#define CORE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <sys/types.h>
#include <sys/stat.h>

#include <fcntl.h>
#include <mqueue.h>

#define SERVER_QUEUE_NAME "/queue-chat-server"
#define CLIENT_QUEUE_NAME "/queue-chat-client"

#define QUEUE_PERMISSIONS 0666
#define MAX_MESSAGES 256
#define MAX_MSG_SIZE 256
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

enum CHAT_EVENTS {
	CONNECTED,
	CHANGE_NAME,
	DISCONNECTED
}

typedef struct msg {
	char name[MAX_MSG_SIZE];
	pid_t pid;
} MSG;

typedef struct cil {
	char name[MAX_MSG_SIZE];
	pid_t pid;

	struct cil *next;
	struct cil *prev;
} CLIENTS_INFO_LIST;

#endif
