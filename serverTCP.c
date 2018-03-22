#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <errno.h>
#include <string.h>

/*Dichiarazione delle costanti*/
#define MAX_MSG 4096
#define MAX_CONN 50
#define LOOPBACK "127.0.0.1"

void errore(char* description, int retCode);

/*Server che comunica con un server usando il protocollo TCP*/
int main(int argc, char *argv[])
{
	/*Controllo del corretto passaggio dei parametri al programma*/
	if(argc != 3){
		printf("USAGE: %s PORTA MESSAGGIO.\n", argv[0]);
		return(-1);
	}

	/*Dichiarazione variabili*/
	int sockId, connId;
	char buffer[MAX_MSG + 1];
	int bufferLen;
	int lenght = sizeof(struct sockaddr_in);

	/*Inizializzo il canale di connessione*/
	printf("Creazione del socket...\n");
	sockId = socket(AF_INET,SOCK_STREAM,0);
	printf("Socket creato.\n");

	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(LOOPBACK, &myself.sin_addr);
	myself.sin_port = htons(atoi(argv[1]));
	for(int i = 0;i < 8;i++){
		myself.sin_zero[i]=0;
	}

	printf("Sto facendo la bind...\n");
	if(bind(sockId, (struct sockaddr*) &myself, (socklen_t) lenght)){
		errore("bind()", -2);
	}
	printf("Bind eseguita.\n");

	/*Si mette in ascolto di client che cercano una connessione*/
	listen(sockId, MAX_CONN);
	printf("Ascolto se c'è qualche richiesta di connessione...\n");

	/*Creazione del client*/
	struct sockaddr_in mitt;
	/*mitt.sin_family = AF_INET;
	inet_aton(LOOPBACK, &mitt.sin_addr);
	mitt.sin_port = htons(atoi(argv[1]));
	for(int i = 0;i < 8;i++){
		mitt.sin_zero[i]=0;
	}*/

	lenght = sizeof(struct sockaddr_in);

	printf("Attendo una richiesta di connessione...\n");
	connId = accept(sockId, (struct sockaddr*) &mitt, (socklen_t*) &lenght);
	printf("Connessione accettata.\n");

	/*Ricevo un messaggio dal client*/
	int ret = recv(connId, buffer, MAX_MSG, 0);

	if(ret <= 0){
		errore("recv()",-3);
	}

	/*Accodo al messaggio ricevuto un tappo e lo stampo a video*/
	buffer[ret] = '\0';
	printf("Sulla porta %d ho ricevuto:\n%s\n", atoi(argv[1]), buffer);

	/*Mando un messaggio al client*/
	bufferLen = strlen(argv[2]) + 1;
	printf("Invio la riposta...\n");
	ret = send(connId, argv[2], bufferLen,0);
	if(ret != bufferLen){
		errore("send()", -4);
	}
	printf("Risposta inviata.\n");

	/*Chiusura della connessione*/
	printf("Sto chiudendo la connessione...\n");
	shutdown(connId, SHUT_RDWR);
	printf("Connessione chiusa.\n");

	/*Chiusura del socket*/
	printf("Chiusura del socket...\n");
	close(sockId);
	printf("Socket chiuso.\n");

	return 0;
}

/* gestisce gli errori bloccanti */
void errore(char* description, int retCode){
	printf("ERRORE bloccante su %s.\n", description);
	printf("%d:%s\n", errno, strerror(errno));
	printf("Exiting with retun code %d.\n", retCode);
	exit(retCode);
}