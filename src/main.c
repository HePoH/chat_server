#include "../include/core.h"

int main(){
	pthread_t srv_tid, cln_tid;
	int *srv_stat, *cln_stat;

	sys_log("Server start", INFO, STDOUT_FILENO);

	pthread_create(&srv_tid, NULL, srv_event_hndl, (void*)NULL);
	pthread_create(&cln_tid, NULL, cln_msg_hndl, (void*)NULL);

	pthread_join(srv_tid, (void**)&srv_stat);
	pthread_join(cln_tid, (void**)&cln_stat);

	printf("Server: main thread bye\n");
	exit(EXIT_SUCCESS);
}
