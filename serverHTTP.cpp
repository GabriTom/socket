#include "SocketTCP2.hpp"

#define MAX_STR 4096
#define STOP "stop"

typedef struct{
	ServerTCP* mySelf;
	char* dir;
}Param;

void* ascoltaConnessioni();
char* inputStr();
char* trovaFile(char* s);

int main(int argc, char* argv[]){
	/*Dichiarazione variabili*/
	int port;
	char* dir;
	ServerTCP* mySelf;
	pthread_t* threadId;

	/*Controllo se ricevo il giusto numero di parametri*/
	if(argc != 3){
		printf("%s USAGE: PORT DIR", argv[0]);
		return (-1);
	}

	port = atoi(argv[1]);
	&Param.mySelf = new ServerTCP(port, false);
	&Param.dir = argv[2];

	/*Creazione del nuovo thread*/
	pthread_create(&threadId, NULL, ascoltaConnessioni,(void*) param);

	/*Appena riceve stop si ferma*/
	while(!strcmp(msg = inputStr(), STOP)){}

	return 0;
}

/*Metodo che prende una stringa in input*/
char* inputStr(){
	char buffer[MAX_STR + 1];
	int i;

	for(i=0;buffer[i]=getchar();i++){}
	buffer[i+1] = '\0';

	return strdup(buffer);
}

/*Metodo che cerca una parte di stringa*/
char* trovaFile(char* s){
	char ret[MAX_STR];
	int i = 4;
	int j = 0;
	while(msg[i] !=n ' '){
		ret[j] = msg[i];
		i++;
		j++;
	}

	ret[j] = '\0';
	return strdup(ret);
}

/*Metodo che ascolta se ricevo stop da linea di comando*/
void* ascoltaConnessioni(void* param){
	int i;
	Param* param = (Param*) param;
	ServerTCP* mySelf = Param.mySelf;
	File* f;
	char* dir = Param.dir;
	char* response;
	char* request;
	char* nomeFile;
	char* nomeFileTrusted;

	while(1){
		/*Accetto le connessioni che mi arrivano*/
		ConnessioneTCP* conn = mySelf->accetta();
		/*Ricevo il mesaggio*/
		request = mySelf->ricevi();
		//nomeFile = trovaFile(request);
		nomeFile = "www.google.com";
		
		/*Se ricevo / invio index.html*/
		if(nomeFile == "/"){
			nomeFileTrusted = "index.html";
		}else{
			nomeFileTrusted = nomeFile;
		}

		f = fopen(request, "w");



		if(f != NULL){
			fprintf("200 OK\n\n");
			fscanf(f,"%s");
		}else{
			fprintf("404 NOT FOUND");
		}

		fclose(f);


		mySelf->inviaRaw(page);
	}
}