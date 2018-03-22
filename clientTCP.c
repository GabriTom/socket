#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

/*Dichiarazione delle costanti*/
#define MAX_MSG 4096

void errore(char* description, int retCode);

/*Client che comunica con un server usando il protocollo TCP*/
int main(int argc, char *argv[])
{
	/*Controllo del corretto passaggio dei parametri al programma*/
	if(argc != 4){
		printf("USAGE: %s IP_SERVER PORTA_SERVER MESSAGGIO\n", argv[0]);
		return(-1);
	}

	/*Dichiarazione variabili*/
	int sockId;
	char buffer[MAX_MSG + 1];
	int bufferLen;

	/*Inizializzo il canale di connessione*/
	printf("Creazione del socket...\n");
	sockId = socket(AF_INET,SOCK_STREAM,0);
	printf("Socket creato.\n");

	/*Creaqzione del server*/
	struct sockaddr_in server;
	server.sin_family = AF_INET;
	inet_aton(argv[1], &server.sin_addr);
	server.sin_port = htons(atoi(argv[2]));
	for(int i = 0;i < 8;i++){
		server.sin_zero[i]=0;
	}

	/*Richiedo la connessione al server*/
	printf("Apertura della connessione...\n");
	if(connect(sockId, (struct sockaddr*) &server, (socklen_t) sizeof(struct sockaddr_in))){
		errore("connect()", -2);
	}
	printf("Connessione stabilita.\n");

	/*Mando un messaggio al server*/
	printf("Invio del messaggio in corso...\n");
	bufferLen = strlen(argv[3]) + 1;
	int ret = send(sockId, argv[3], bufferLen,0);
	if(ret != bufferLen){
		errore("send()", -3);
	}
	printf("Messaggio inviato.\n");

	/*Ricevo un messaggio dal server*/
	printf("In attesa di un messaggio...\n");
	ret = recv(sockId, buffer, MAX_MSG, 0);
	if(ret <= 0){
		errore("recv()",-4);
	}

	/*Accodo al messaggio ricevuto un tappo e lo stampo a video*/
	buffer[ret] = '\0';
	printf("[%s:%d] %s\n", argv[1], atoi(argv[2]), buffer);

	/*Chiusura del socket*/
	close(sockId);

	return 0;
}

/* gestisce gli errori bloccanti */
void errore(char* description, int retCode){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}
