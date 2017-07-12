#include "../include/core.h"

pthread_key_t key;
pthread_once_t key_once = PTHREAD_ONCE_INIT;

void make_key() {
	int rtn;

	rtn = pthread_key_create(&key, destructor);
	if (rtn != 0) {
		perror("pthread_key_create");
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
		perror("pthread_once");
		pthread_exit((void*)EXIT_FAILURE);
	}

	dt = pthread_getspecific(key);
	if (dt == NULL) {
		dt = malloc(MAX_TIME_SIZE * sizeof(char));

		rtn = pthread_setspecific(key, dt);
		if (rtn != 0) {
			perror("pthread_setspecific");
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
	mqd_t qd_srv;
	struct mq_attr srv_attr;
	ssize_t bts_num;
	SERVER_MSG srv_msg;

	srv_attr.mq_flags = 0L;
	srv_attr.mq_maxmsg = MAX_MESSAGES;
	srv_attr.mq_msgsize = SERVER_MSG_SIZE;
	srv_attr.mq_curmsgs = 0L;

	qd_srv = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &srv_attr);
	if (qd_srv == -1) {
		perror("Server: mq_open(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	while (1) {
		bts_num = mq_receive(qd_srv, (char*)&srv_msg, SERVER_MSG_SIZE, NULL);
		if (bts_num == -1) {
			perror("Server: mq_receive(server)");
			pthread_exit((void*)EXIT_FAILURE);
		}

		sys_log("Server: mq_receive(server) message received", INFO, STDOUT_FILENO);
		printf("Struct: SERVER_MSG { msg_type = %d, data = %s }\n", srv_msg.msg_type, srv_msg.data);

		/*bts_num = mq_send(qd_srv, (char*)&srv_msg, SERVER_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Server: mq_send(server)");
			continue;
		}

		sys_log("Server: mq_send(server) response sent to client", INFO, STDOUT_FILENO);
		printf("Struct: SERVER_MSG { msg_type = %d, data = %s }\n", srv_msg.msg_type, srv_msg.data);*/
	}

	if (mq_close(qd_srv) == -1) {
		perror("Server: mq_close(server)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	if (mq_unlink(SERVER_QUEUE_NAME) == -1) {
		perror("Server: mq_unlink(server)");
		pthread_exit((void*)EXIT_FAILURE);
        }

	sys_log("Server: server thread bye", INFO, STDOUT_FILENO);
	pthread_exit((void*)EXIT_SUCCESS);
}

void* cln_msg_hndl(void* args) {
	mqd_t qd_cln;
	struct mq_attr cln_attr;
	ssize_t bts_num;
	CLIENT_MSG cln_msg;

	cln_attr.mq_flags = 0L;
	cln_attr.mq_maxmsg = MAX_MESSAGES;
	cln_attr.mq_msgsize = CLIENT_MSG_SIZE;
	cln_attr.mq_curmsgs = 0L;

	qd_cln = mq_open(CLIENT_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &cln_attr);
	if (qd_cln == -1) {
		perror("Server: mq_open(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	while (1) {
		bts_num = mq_receive(qd_cln, (char*)&cln_msg, CLIENT_MSG_SIZE, NULL);
		if (bts_num == -1) {
			perror("Server: mq_receive(client)");
			pthread_exit((void*)EXIT_FAILURE);
		}

		sys_log("Server: mq_receive(client) message received", INFO, STDOUT_FILENO);
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);

		/*bts_num = mq_send(qd_cln, (char*)&cln_msg, CLIENT_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Server: mq_send(client)");
			continue;
		}

		sys_log("Server: mq_send(client) response sent to client", INFO, STDOUT_FILENO);
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);*/
	}

	if (mq_close(qd_cln) == -1) {
		perror("Server: mq_close(client)");
		pthread_exit((void*)EXIT_FAILURE);
        }

	if (mq_unlink(CLIENT_QUEUE_NAME) == -1) {
		perror("Server: mq_unlink(client)");
		pthread_exit((void*)EXIT_FAILURE);
	}

	sys_log("Server: client thread bye", INFO, STDOUT_FILENO);
	pthread_exit((void*)EXIT_SUCCESS);
}
