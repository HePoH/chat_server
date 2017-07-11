#include "../include/core.h"

int main(){
	mqd_t qd_server, qd_client;
	struct mq_attr srv_attr, cln_attr;

	char in_buffer[MSG_BUFFER_SIZE];
	char out_buffer[MSG_BUFFER_SIZE];

	srv_attr.mq_flags = 0;
	srv_attr.mq_maxmsg = MAX_MESSAGES;
	srv_attr.mq_msgsize = MAX_MSG_SIZE;
	srv_attr.mq_curmsgs = 0;

	cln_attr.mq_flags = 0;
	cln_attr.mq_maxmsg = MAX_MESSAGES;
	cln_attr.mq_msgsize = MAX_MSG_SIZE;
	cln_attr.mq_curmsgs = 0;

	qd_server = mq_open(SERVER_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &srv_attr);
	if (qd_server == -1) {
		perror ("Server: mq_open(server)");
		exit(EXIT_FAILURE);
	}

	qd_client = mq_open(CLIENT_QUEUE_NAME, O_RDWR | O_CREAT, QUEUE_PERMISSIONS, &cln_attr);
	if (qd_client == -1) {
		perror ("Server: mq_open(client)");
		exit(EXIT_FAILURE);
	}

	while (1) {
		if (mq_receive(qd_server, in_buffer, MSG_BUFFER_SIZE, NULL) == -1) {
			perror("Server: mq_receive(server)");
			exit(EXIT_FAILURE);
		}

		printf("Received: %s\n", in_buffer);
		printf("Server: message received.\n");

		sprintf(out_buffer, "%ld", 321);
		if (mq_send(qd_client, out_buffer, strlen(out_buffer), 0) == -1) {
			perror("Server: Not able to send message to client");
			continue;
		}

		printf("Server: response sent to client.\n");
	}

	if (mq_close(qd_server) == -1) {
		perror("Server: mq_close(server)");
		exit(EXIT_FAILURE);
	}

	if (mq_close(qd_client) == -1) {
		perror("Server: mq_close(server)");
		exit(EXIT_FAILURE);
	}

	if (mq_unlink(SERVER_QUEUE_NAME) == -1) {
		perror("Server: mq_unlink(server)");
		exit(EXIT_FAILURE);
	}

	if (mq_unlink(CLIENT_QUEUE_NAME) == -1) {
		perror("Server: mq_unlink(client)");
		exit(EXIT_FAILURE);
	}

	printf("Server: bye\n");
	exit(EXIT_SUCCESS);
}
