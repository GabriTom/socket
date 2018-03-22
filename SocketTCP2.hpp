#ifndef __SOCKET_TCP
#define __SOCKET_TCP

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
#define MAX_CONN 50

using namespace std;

class SocketTCP {
	protected: int sockId;
	public: SocketTCP();						  // socket()
			~SocketTCP();						  // close()
			bool broadcast(bool broadcast);		  // setsockopt()
};

SocketTCP::SocketTCP(){
	this->sockId = socket(AF_INET, SOCK_STREAM, 0);
}


SocketTCP::~SocketTCP(){
	close(this->sockId);
}

//True se è impostato a boroadcast
bool SocketTCP::broadcast(bool broadcast){
	bool ret = false;
	int len_so = sizeof(int);
	int val = broadcast? 1: 0;
	if(!setsockopt(this->sockId, 
						SOL_SOCKET,
						SO_BROADCAST, 
						&val,
						(socklen_t) len_so)){
		ret = true;
	}

	return ret;
}







class ConnessioneTCP {
	private: int connId;
				 Address* address;
				 bool server; // Se è falso è un client
	public: ConnessioneTCP(int connId, Address* address, bool client);
			~ConnessioneTCP();					  // shutdown()

			bool invia(char* msg);
			bool inviaRaw(void* msg, int length); // send()
			char* ricevi();						  
			void* riceviRaw(int* length);		  // recv()
};


/*Costruttore dell' istanza*/
ConnessioneTCP::ConnessioneTCP(int connId, Address* address, bool server){
	this->connId = connId;
	this->address = new Address(*address);
	this->server = server;
}

/*Distruttore dell' istanza*/
ConnessioneTCP::~ConnessioneTCP(){
	delete(this->address);

	if(this->server){
		shutdown(this->connId, SHUT_RDWR);
	}
}

//Ritorna vero se è andata a buon fine
bool ConnessioneTCP::invia(char* msg){
	int bufferLen;
	bufferLen = strlen(msg) + 1;

	return this->inviaRaw((void*) msg, bufferLen);
}

//Ritorna vero se è andata a buon fine
bool ConnessioneTCP::inviaRaw(void* msg, int length){
	int sended = send(this->connId, 
						msg, 
						length,
						0);
	if(sended != length){ 
		return false;
	}

	return true;
}

//Restituisce il messaggio che riceve (NULL se qualcosa è andato storto)
char* ConnessioneTCP::ricevi(){
	char* buffer;
	int ret;

	buffer = (char*) this->riceviRaw(&ret);
	/*Se il buffer è vuoto ritorno NULL*/
	if(!buffer) return NULL;
	buffer[ret + 1] = '\0';

	return buffer;
}


void* ConnessioneTCP::riceviRaw(int* length){
	/*Io metto un buffer che punta ad un area ben definita e non sporca
	Il +1 serve per l'eventuale carattere di fine stringa che viene aggiunto nella ricevi*/
	void* buffer = malloc(MAX_MSG + 1);
	*length = recv(this->connId, 
					buffer, 
					MAX_MSG, 
					0);

	//Non ho ricevuto nulla
	if(*length <= 0){
		/*Evito il memory leak*/
		free(buffer);
		return NULL;
	}

	//Se ho ricevuto qualcosa lo ritorno
	return buffer;
}








class ServerTCP: public SocketTCP {
	//private: list <ConnessioneTCP*>* connessioni;
	public: ServerTCP(int port, bool loopback);	  // bind() e listen()
			~ServerTCP();

			ConnessioneTCP* accetta();			  // accept()
			//void rimuoviConnessione();
			//bool inviaTutti(char* msg);
			//bool inviaTuttiRaw(void* msg, int length);
};



//Richiama il costruttore della superclasse
ServerTCP::ServerTCP(int port = 0, bool loopback = false): SocketTCP(){
	//*connessioni = new list<ConnessioneTCP*>;

	struct sockaddr_in myself;
	myself.sin_family = AF_INET;
	inet_aton(loopback? LOOPBACK: MYSELF_IP, &myself.sin_addr);
	myself.sin_port = htons(port);
	for(int i = 0;i < 8;i++){
		myself.sin_zero[i]=0;
	}

	bind(this->sockId, (struct sockaddr*) &myself, (socklen_t) sizeof(struct sockaddr_in));

	listen(this->sockId, MAX_CONN);
}

//Richiama il distruttore di SocketTCP
ServerTCP::~ServerTCP(){
	//this->connessioni->clear();
}

ConnessioneTCP* ServerTCP::accetta(){
	Address* ret;
	struct sockaddr_in mitt;

	int length = sizeof(struct sockaddr_in);
	int connId = accept(this->sockId,
			(struct sockaddr*) &mitt, 
			(socklen_t*) &length);

	if(connId == -1){
		return NULL;
	}

	ret = new Address(mitt);
	ConnessioneTCP* connessione = new ConnessioneTCP(connId, ret, true);

	//this->connessioni->insert(connessione);

	return connessione;
}









class ClientTCP: public SocketTCP {
	private: ConnessioneTCP* connessione;
	public: ClientTCP();
			~ClientTCP();					  

			bool connetti(Address server);		  // connect()
			bool invia(char* msg);
			bool inviaRaw(void* msg, int length);
			char* ricevi();
			void* riceviRaw(int* length);
};


ClientTCP::ClientTCP(){
	//ConnessioneTCP conn = new ConnessioneTCP();
	//this->*connessione = (ConnessioneTCP*) conn;
}

ClientTCP::~ClientTCP(){
	delete(this->connessione);
}

bool ClientTCP::connetti(Address server){
	bool ret;
	struct sockaddr_in structServer = server.getAddress(); 
	ret = connect(this->sockId, 
					(struct sockaddr*) &structServer, 
					(socklen_t) sizeof(struct sockaddr_in));

	if(ret){
		this->connessione = new ConnessioneTCP(sockId, &server, false);
	}

	return ret;
}

bool ClientTCP::invia(char* msg){
	return this->connessione->invia(msg);
}

bool ClientTCP::inviaRaw(void* msg, int length){
	return this->connessione->inviaRaw(msg, length);
}

char* ClientTCP::ricevi(){
	return this->connessione->ricevi();
}

void* ClientTCP::riceviRaw(int* length){
	return this->connessione->riceviRaw(length);
}

/*bool ClientTCP::broadcast(bool broadcast){
	return SocketTCP->broadcast(broadcast);
}*/



#endif //__SOCKET_TCP