#include "./ServerTCP.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define BROADCAST_IP "192.168.6.255"

/*Metodo che testa la classe serverTCP.cpp*/
int main(int argc, char *argv[])
{
	/*Controllo del corretto passaggio dei parametri al programma*/
	if(argc != 3){
		printf("USAGE: %s PORTA MESSAGGIO\n", argv[0]);
		return(-1);
	}

	/*Istanzio un nuovo oggetto di classe server*/
	ServerTCP* server = new ServerTCP();
	Address* client;

	int porta = atoi(argv[1]);

	printf("Eseguo la bind sulla porta %d.\n", porta);
	server->bind(porta);
	printf("Bind eseguita.\n");

	/*Ascolto se ci sono nuove connessioni*/
	printf("Ascolto per nuove connessioni...\n");
	server->ascolta();
	printf("Ho ricevuto una richiesta di connessione.\n");

	/*Richiedo la connessione al server*/
	printf("Accetto la connessione...\n");
	client = server->accetta();
	printf("Connessione accettata.\n");

	/*Ricevo il messaggio dal client*/
	printf("Ricevo il messaggio dal client...\n");
	char* resp;
	printf("Ho ricevuto:\n %s\n", resp=server->ricevi());
	free(resp);

	/*Invio la risposta al client*/
	printf("Invio la risposta al client...\n");
	server->invia(argv[2]);
	printf("Risposta inviata.\n");

	/*Testo la funzione broadcast*/
	if(server->broadcast(true)){
		printf("Il server ora è in broadcast.\n");
	}

	/*Provo ad inviare un messaggio in broadcast*/
	/*client->setIp(BROADCAST_IP);
	server->invia(argv[2]);

	/*Richiamo il distruttore della classe server*/
	delete(server);

	return 0;
}