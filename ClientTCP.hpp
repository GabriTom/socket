#ifndef __CLIENT_TCP
#define __CLIENT_TCP

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


class ClientTCP {
	private: int sockId;
	public: ClientTCP();
			~ClientTCP();
			bool connetti(Address server);
			bool invia(char* msg);
			char* ricevi();
			//void* indica un puntatore ad un messaggio. Nei raw il messaggio può non essere una stringa
			bool inviaRaw(void* msg, int length);
			void* riceviRaw(int* length);
			bool broadcast(bool status);
};

/*Costruttore dell' istanza*/
ClientTCP::ClientTCP(){
	this->sockId = socket(AF_INET,
							SOCK_STREAM,
							0);
}

/*Distruttore dell' istanza*/
ClientTCP::~ClientTCP(){
	close(this->sockId);
}

/*Restituisce falso nel caso in cui sia andato tutto bene, altrimenti vero se si sono verificati degli errori.*/
bool ClientTCP::connetti(Address server){
	struct sockaddr_in structServer = server.getAddress(); 
	return connect(this->sockId, 
					(struct sockaddr*) &structServer, 
					(socklen_t) sizeof(struct sockaddr_in));
}


bool ClientTCP::invia(char* msg){
	int bufferLen;
	bufferLen = strlen(msg) + 1;

	return this->inviaRaw((void*) msg, bufferLen);
}

/*Restituisce cosa abiamo ricevuto, va bene se la riceviRaw non da problemi.*/
char* ClientTCP::ricevi(){
	char* buffer;
	int ret;

	buffer = (char*) this->riceviRaw(&ret);
	/*Se il buffer è vuoto ritorno NULL*/
	if(!buffer) return NULL;
	buffer[ret + 1] = '\0';

	return buffer;
}

/*Restituisce false se è ancdato tutto bene, altrimenti restituisce true*/
bool ClientTCP::inviaRaw(void* msg, int length){
	int sended = send(this->sockId, 
						msg, 
						length,
						0);
	if(sended != length) return true;

	return false;
}

void* ClientTCP::riceviRaw(int* length){
	/*Io metto un buffer che punta ad un area ben definita e non sporca
	Il +1 serve per l'eventuale carattere di fine stringa che viene aggiunto nella ricevi*/
	void* buffer = malloc(MAX_MSG + 1);
	*length = recv(this->sockId, 
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

bool ClientTCP::broadcast(bool status){
	int len_so = sizeof(int); 
	int val = status? 1: 0;
	return (!setsockopt(this->sockId, 
						SOL_SOCKET,
						SO_BROADCAST, 
						&val,
						(socklen_t) len_so));
}



#endif //__CLIENT_TCP