#include "./ClientTCP.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BROADCAST_IP "192.168.6.255"

/*Metodo che testa la classe ClientTCP.cpp*/
int main(int argc, char *argv[])
{
	/*Controllo del corretto passaggio dei parametri al programma*/
	if(argc != 4){
		printf("USAGE: %s IP_SERVER PORTA_SERVER MESSAGGIO\n", argv[0]);
		return(-1);
	}

	/*Istanzio un nuovo oggetto di classe client*/
	ClientTCP* client = new ClientTCP();
	Address* server = new Address(argv[1], atoi(argv[2]));

	/*Richiedo la connessione al server*/
	printf("Mi sto connettendo al server...\n");
	client->connetti(*server);
	printf("Connessione riuscita.\n");

	/*Invio il messaaggio al server*/
	printf("Invio del messaggio al server...\n");
	client->invia(argv[3]);
	printf("Invio riuscito.\n");

	/*Stampo la risposta del server*/
	printf("Attendo una risposta dal server...\n");
	char* resp;
	printf("Ho ricevuto:\n %s\n", resp=client->ricevi());
	free(resp);

	/*Testo la funzione broadcast*/
	if(client->broadcast(true)){
		printf("Il client ora è in broadcast\n");
	}

	/*Provo ad inviare un messaggio in broadcast*/
	server->setIp(BROADCAST_IP);
	client->connetti(*server);
	client->invia(argv[3]);

	/*Richiamo il distruttore della classe client*/
	delete(client);
	delete(server);

	return 0;
}