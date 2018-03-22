#ifndef __SERVER_TCP
#define __SERVER_TCP

#include "./Address.hpp"
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <arpa/inet.h>
#include <string.h>

#define LOOPBACK "127.0.0.1"
#define MYSELF_IP "0.0.0.0"
#define MAX_MSG 4096
#define MAX_CONN 1


class ServerTCP {
	private: int sockId; int connId;
	public: ServerTCP();
			~ServerTCP();
			void bind(int porta);
			//void ascolta();
			Address* accetta();
			bool invia(char* msg);
			char* ricevi();
			//void* indica un puntatore ad un messaggio. Nei raw il messaggio può non essere una stringa
			bool inviaRaw(void* msg, int length);
			void* riceviRaw(int* length);
			bool broadcast(bool status);
			void chiudi();
};


/*Costruttore dell' istanza*/
ServerTCP::ServerTCP(){
	this->sockId = socket(AF_INET,
							SOCK_STREAM,
							0);
	connId = -1;

	/*La ascolta ha un vettore e in ogni cella ha una porta sulla quale c'è una connessione. MAX_CONN è il numero di celle e ogni messaggio 
	relativo alle connessioni li salva nella cell apposita*/
	listen(this->sockId, MAX_CONN);
}

/*Distruttore dell' istanza*/
ServerTCP::~ServerTCP(){
	if(connId != -1){
		chiudi();
	}

	close(sockId);
}

/*Metodo che lega il server al socket*/
void ServerTCP::bind(int porta){
	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(LOOPBACK, &myself.sin_addr);
	myself.sin_port = htons(porta);
	for(int i = 0;i < 8;i++){
		myself.sin_zero[i]=0;
	}

	if(bind(sockId, (struct sockaddr*) &myself, (socklen_t) sizeof(struct sockaddr_in))){
		printf("Errore sulla bind().\n");
	}
}

/*Metodo che mette il server iun fase di ascolto*/
/*void ServerTCP::ascolta(){
	
}*/

/*Accetta la conensione quando un client la richiede*/
Address* ServerTCP::accetta(){
	printf("Qua arrivo\n");
	Address* ret;
	struct sockaddr_in client;

	int lenght = sizeof(struct sockaddr);
	connId = accept(sockId,
			(struct sockaddr*) &client, 
			(socklen_t*) &lenght);

	printf("connId vale %d.\n", connId);

	if(connId == -1){
		return NULL;
	}

	ret = new Address(client);
	return ret;
}

/*Metodo chiamato quando si deve inviare una stringa*/
bool ServerTCP::invia(char* msg){
	int bufferLen;
	bufferLen = strlen(msg) + 1;

	return this->inviaRaw((void*) msg, bufferLen);
}

/*Restituisce cosa abiamo ricevuto, va bene se la riceviRaw non da problemi.*/
char* ServerTCP::ricevi(){
	char* buffer;
	int ret;

	buffer = (char*) this->riceviRaw(&ret);
	/*Se il buffer è vuoto ritorno NULL*/
	if(!buffer) return NULL;
	buffer[ret + 1] = '\0';

	return buffer;
}

/*Restituisce false se è ancdato tutto bene, altrimenti restituisce true*/
bool ServerTCP::inviaRaw(void* msg, int length){
	int sended = send(this->connId, 
						msg, 
						length,
						0);
	if(sended != length) return true;

	return false;
}

/*Metodo usato per ricevere dati (stringa, files...)*/
void* ServerTCP::riceviRaw(int* length){
	/*Io metto un buffer che punta ad un area ben definita e non sporca
	Il +1 serve per l'eventuale carattere di fine stringa che viene aggiunto nella ricevi*/
	void* buffer = malloc(MAX_MSG + 1);
	*length = recv(this->connId, 
					buffer, 
					MAX_MSG, 
					0);
	if(*length <= 0){
		/*Evito il memory leak*/
		free(buffer);
		return NULL;
	}

	return buffer;
}

/*Metodo usato per impostare il socket in broacast*/
bool ServerTCP::broadcast(bool status){
	int len_so = sizeof(int);
	int val = status? 1: 0;
	return (!setsockopt(this->sockId, 
						SOL_SOCKET,
						SO_BROADCAST, 
						&val,
						(socklen_t) len_so));
}

/*Chiude la connessione con il client*/
void ServerTCP::chiudi(){
	if(connId == -1) shutdown(connId, SHUT_RDWR);
	connId = -1;
}
#endif //__SERVER_TCP