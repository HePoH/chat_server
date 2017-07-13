#include "../include/core.h"
#include "../include/list.h"

pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void make_key() {
	int rtn;

	rtn = pthread_key_create(&key, destructor);
	if (rtn != 0) {
		perror("Server: pthread_key_create");
		pthread_exit((void*)EXIT_FAILURE);
	}
}

void destructor(void* ptr) {
	free(ptr);
}

char* get_cur_dt(char* format) {
	time_t rt;
	struct tm* ti;
	char* dt;
	int rtn;

	rtn = pthread_once(&key_once, make_key);
	if (rtn != 0) {
		perror("Server: pthread_once");
		pthread_exit((void*)EXIT_FAILURE);
	}

	dt = pthread_getspecific(key);
	if (dt == NULL) {
		dt = malloc(MAX_TIME_SIZE * sizeof(char));

		rtn = pthread_setspecific(key, dt);
		if (rtn != 0) {
			perror("Server: pthread_setspecific");
			pthread_exit((void*)EXIT_FAILURE);
		}
	}

	time(&rt);
	ti = localtime(&rt);

	strftime(dt, MAX_TIME_SIZE, format, ti);

	return dt;
}

void sys_log(char* msg, int type, int fd) {
	char *cur_dt, sys_msg[MAX_LOG_MSG_SIZE];
	cur_dt = get_cur_dt("[%D][%T]");

	switch(type) {
		case INFO:
			snprintf(sys_msg, MAX_LOG_MSG_SIZE, "%s [%s]: %s\n", cur_dt, "INFORMATION", msg);
			break;

		case WARNING:
			snprintf(sys_msg, MAX_LOG_MSG_SIZE, "%s [%s]: %s\n", cur_dt, "WARNING", msg);
			break;

		case ERROR:
			snprintf(sys_msg, MAX_LOG_MSG_SIZE, "%s [%s]: %s\n", cur_dt, "ERROR", msg);
			break;
	}

	write(fd, &sys_msg, strlen(sys_msg));
}

void* srv_event_hndl(void* args) {
	int srv_qid;
	key_t srv_key;
	ssize_t bts_num;
	SERVER_MSG srv_msg;
	CLIENT_INFO* cln_info;
	list_node_t* cln_node;

	srv_key = ftok(SERVER_KEY_FILE, PROJECT_ID);
	if (srv_key == -1) {
		perror("Server: ftok(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	srv_qid = msgget(srv_key, IPC_CREAT | QUEUE_PERMISSIONS);
	if (srv_qid == -1) {
		perror("Server: msgget(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	while (1) {
		bts_num = msgrcv(srv_qid, (void*)&srv_msg, SERVER_MSG_SIZE, SERVER_PID, 0);
		if (bts_num == -1) {
			perror("Server: msgrcv(server)");
			pthread_exit((void*)EXIT_FAILURE);
		}

		sys_log("Server: msgrcv(server) message received", INFO, STDOUT_FILENO);
		printf("Struct: SERVER_MSG { msg_type = %d, data[0] = %s, data[1] = %s, pid = %d }\n", srv_msg.msg_type, srv_msg.data_text[0], srv_msg.data_text[1], srv_msg.data_pid);

		switch(srv_msg.msg_type) {
			case CONNECTED:
					cln_info = malloc(sizeof(CLIENT_INFO));
					if (cln_info == NULL) {
						perror("Server: malloc(CLIENT_INFO)");
						pthread_exit((void*)EXIT_FAILURE);
					}

					strncpy(cln_info->cln_name, srv_msg.data_text[0], MAX_NAME_SIZE);
					cln_info->cln_pid = srv_msg.data_pid;

					cln_node = list_node_new((void*)cln_info);
					list_rpush(cln_list, cln_node);

					/* ----------------------------------------------- */
					list_node_t *node;
					list_iterator_t *it = list_iterator_new(cln_list, LIST_HEAD);

					printf("\nCLIENTS LIST:\n");

					while ((node = list_iterator_next(it))) {
						printf("Struct: CLIENT_INFO { cln_name = %s, cln_pid = %d }\n", ((CLIENT_INFO*)node->val)->cln_name, ((CLIENT_INFO*)node->val)->cln_pid);

					}
					/* ----------------------------------------------- */
					break;

			case CHANGE_NAME:
					break;

			case DISCONNECTED:
					break;

			default:
					break;
		}

		/*srv_msg.pid = srv_msg.data_pid;

		bts_num = msgsnd(srv_qid, (void*)&srv_msg, SERVER_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Server: msgsnd(server)");
			continue;
		}

		sys_log("Server: msgsnd(server) response sent to client", INFO, STDOUT_FILENO);
		printf("Struct: SERVER_MSG { msg_type = %d, data[0] = %s, data[1] = %s, pid = %d }\n", srv_msg.msg_type, srv_msg.data_text[0], srv_msg.data_text[1], srv_msg.data_pid);*/
	}


	if (msgctl(srv_qid, IPC_RMID, NULL) == -1) {
		perror("Server: msgctl(server)");
		pthread_exit((void*)EXIT_FAILURE);
        }

	sys_log("Server: server thread bye", INFO, STDOUT_FILENO);
	pthread_exit((void*)EXIT_SUCCESS);
}

void* cln_msg_hndl(void* args) {
	int cln_qid;
	key_t cln_key;
	ssize_t bts_num;
	CLIENT_MSG cln_msg;

	cln_key = ftok(CLIENT_KEY_FILE, PROJECT_ID);
	if (cln_key == -1) {
		perror("Server: ftok(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	cln_qid = msgget(cln_key, IPC_CREAT | QUEUE_PERMISSIONS);
	if (cln_qid == -1) {
		perror("Server: msgget(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	while (1) {
		bts_num = msgrcv(cln_qid, (void*)&cln_msg, CLIENT_MSG_SIZE, SERVER_PID, 0);
		if (bts_num == -1) {
			perror("Server: msgrcv(client)");
			pthread_exit((void*)EXIT_FAILURE);
		}

		sys_log("Server: mq_receive(client) message received", INFO, STDOUT_FILENO);
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);

		switch(cln_msg.msg_type) {
			case PUBLIC:
					break;

			case PRIVATE:
					break;

			case ADM_MSG:
					break;

			default:
					break;
		}

		bts_num = msgsnd(cln_qid, (void*)&cln_msg, CLIENT_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Server: msgsnd(client)");
			continue;
		}

		sys_log("Server: msgsnd(client) response sent to client", INFO, STDOUT_FILENO);
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);
	}

	if (msgctl(cln_qid, IPC_RMID, NULL) == -1) {
		perror("Server: msgctl(cln)");
		pthread_exit((void*)EXIT_FAILURE);
        }

	sys_log("Server: client thread bye", INFO, STDOUT_FILENO);
	pthread_exit((void*)EXIT_SUCCESS);
}
