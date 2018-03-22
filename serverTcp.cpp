#include "SocketTCP2.hpp"
#include <list>


/*Dichiarazione delle costanti*/
#define MAX_MSG 4096
#define MAX_CONN 50
#define LOOPBACK "127.0.0.1"

using namespace std;

/*Server che comunica con un server usando il protocollo TCP*/
int main(int argc, char *argv[])
{
	int port;
	char* msg;
	bool loopback = false;

	if(argc != 4){
		printf("USAGE: %s PORTA MESSAGGIO LOOPBACK(1=true, 0=false).\n", argv[0]);
		return(-1);
	}

	port = atoi(argv[1]);
	msg = argv[2];
	if(atoi(argv[3]) == 1){
		loopback = true;
	}

	ServerTCP* mySelf = new ServerTCP(port, loopback);
	list <ConnessioneTCP*> connessioni;

	/*Accetta due connessioni*/
	printf("Accetto la prima connessione.\n");
	connessioni.push_back(mySelf->accetta());
	printf("Accetto la seconda connessione.\n");
	connessioni.push_back(mySelf->accetta());
	printf("Connessioni accettate!\n");

	/*Stampa ciò che riceve dalle due connessioni*/
	for(ConnessioneTCP* conn: connessioni){
		char* rec = conn->ricevi();
		printf("Ho ricevuto: %s\n", rec);
	}

	/*Risponde alle due connessioni*/
	printf("Ora invio la risposta\n");
	for(ConnessioneTCP* conn: connessioni){
		conn->invia(msg);
	}
	printf("Risposte inviate!\n");

	/*Elimina ogni singola connessione*/
	/*printf("Ora elimino le connessioni.\n");
	for(ConnessioneTCP* conn: connessioni){
		delete(conn);
	}
	printf("Connessioni eliminate!\n");*/

	/*Cancella il vettore e la struttura del server*/
	printf("Elimino il vettore e la struttura e le connessioni al suo interno.\n");
	//delete(&connessioni);
	delete(mySelf);
	printf("Eliminazione avvenuta con successo. Fine.\n");

	return 0;
}
