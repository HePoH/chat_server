#include "../include/core.h"

void* srv_event_hndl(void* args) {
	mqd_t qd_srv;
	struct mq_attr srv_attr;
	ssize_t bts_num;
	SERVER_MSG srv_msg;

	srv_attr.mq_flags = 0L;
	srv_attr.mq_maxmsg = MAX_MESSAGES;
	/*srv_attr.mq_msgsize = MAX_MSG_SIZE;*/
	srv_attr.mq_msgsize = 1024L;
	srv_attr.mq_curmsgs = 0L;

	qd_srv = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &srv_attr);
	if (qd_srv == -1) {
		perror ("Server: mq_open(server)");
		pthread_exit(EXIT_FAILURE);
	}

	while (1) {
		bts_num = mq_receive(qd_srv, (char*)&srv_msg, SERVER_MSG_SIZE, NULL);
		if (bts_num == -1) {
			perror("Server: mq_receive(server)");
			pthread_exit(EXIT_FAILURE);
		}

		printf("Server: mq_receive(server) message received\n");
		printf("Struct: SERVER_MSG { msg_type = %d, data = %s }\n", srv_msg.msg_type, srv_msg.data);

		/*bts_num = mq_send(qd_srv, (char*)&srv_msg, SERVER_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Server: mq_send(server)");
			continue;
		}

		printf("Server: mq_send(server) response sent to client\n");
		printf("Struct: SERVER_MSG { msg_type = %d, data = %s }\n", srv_msg.msg_type, srv_msg.data);*/
	}

	if (mq_close(qd_srv) == -1) {
		perror("Server: mq_close(server)");
		pthread_exit(EXIT_FAILURE);
	}

	if (mq_unlink(SERVER_QUEUE_NAME) == -1) {
		perror("Server: mq_unlink(server)");
		pthread_exit(EXIT_FAILURE);
        }

	printf("Server: server thread bye\n");
	pthread_exit(EXIT_SUCCESS);
}

void* cln_msg_hndl(void* args) {
	mqd_t qd_cln;
	struct mq_attr cln_attr;
	ssize_t bts_num;
	CLIENT_MSG cln_msg;

	cln_attr.mq_flags = 0L;
	cln_attr.mq_maxmsg = MAX_MESSAGES;
	/*cln_attr.mq_msgsize = MAX_MSG_SIZE;*/
	cln_attr.mq_msgsize = sizeof(CLIENT_MSG);
	cln_attr.mq_curmsgs = 0L;

	qd_cln = mq_open(CLIENT_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &cln_attr);
	if (qd_cln == -1) {
		perror ("Server: mq_open(client)");
		pthread_exit(EXIT_FAILURE);
	}

	while (1) {
		bts_num = mq_receive(qd_cln, (char*)&cln_msg, 1024, NULL);
		if (bts_num == -1) {
			perror("Server: mq_receive(client)");
			pthread_exit(EXIT_FAILURE);
		}

		printf("Server: mq_receive(client) message received\n");
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);

		/*bts_num = mq_send(qd_cln, (char*)&cln_msg, CLIENT_MSG_SIZE, 0);
		if (bts_num == -1) {
			perror("Server: mq_send(client)");
			continue;
		}

		printf("Server: mq_send(client) response sent to client\n");
		printf("Struct: CLIENT_MSG { msg_type = %d, sender = %s [PID: %d], recipient = %s [PID: %d] }\n", cln_msg.msg_type, cln_msg.snd_name,
		cln_msg.snd_pid, cln_msg.rcp_name, cln_msg.rcp_pid);*/
	}

	if (mq_close(qd_cln) == -1) {
		perror("Server: mq_close(client)");
		pthread_exit(EXIT_FAILURE);
        }

	if (mq_unlink(CLIENT_QUEUE_NAME) == -1) {
		perror("Server: mq_unlink(client)");
		exit(EXIT_FAILURE);
	}

	printf("Server: client thread bye\n");
	pthread_exit(EXIT_SUCCESS);
}
