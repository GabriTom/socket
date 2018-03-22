#include "SocketTCP2.hpp"

/*Dichiarazione delle costanti*/
#define MAX_MSG 4096
#define MAX_CONN 50
#define LOOPBACK "127.0.0.1"
#define NOME_FILE "index.html"
#define HTTP_PORT 80

char* trovaPagina(char* s);

/*Server che comunica con un server usando il protocollo TCP*/
int main(int argc, char *argv[])
{
	char* msg;
	char* addr;
	char* nomeFile;
	int length;
	FILE* f;
	char* paginaSupport;

	if(argc != 3){
		printf("USAGE: %s INDIRIZZO NOME_FILE.\n", argv[0]);
		return(-1);
	}

	addr = argv[1];
	if(argv[2] == "/"){
		nomeFile = NOME_FILE;
	}else{
		nomeFile = argv[2];
	}

	ClientTCP* mySelf = new ClientTCP();
	Address* server = new Address(addr,HTTP_PORT);

	printf("Provo a connettermi...\n");
	if(mySelf->connetti(*server))
	printf("Connessione riuscita!\n");

	/*???????*/
	int lunghezza = strlen(nomeFile)+1 + strlen(addr)+1 + 48;
	printf("%i\n",strlen(nomeFile));
	msg = (char*)malloc(lunghezza);


	msg = strcat("GET ",nomeFile);
	msg = strcat(msg," HTTP/1.1\n Host: ");
	msg = strcat(msg,addr);
	msg = strcat(msg,"\n User-Agent: Mozilla/5.0 (compatible; Konqueror/3.2; Linux) (KHTML, like Gecko)\n\n");
	printf("Invio il messaggio...\n");
	mySelf->invia(msg);
	printf("Messaggio inviato!\n");

	printf("Ricevo il messaggio...\n");
	char* pagina;
	//Gli passo un intero e poi lui lo valorizza
	pagina = (char*) mySelf->riceviRaw(&length);

	//Ricerca 200 OK
	if(!strstr(pagina, "200 OK")){
		printf("Errore di counicazione");
	}else{
		//Scrittura solo html nella pagina
		printf("Pagina ricevuta!\n",pagina);
	}

	//Scrittura della pagina su file
	printf("Ora scrivo la pagina su file...\n");
	f = fopen(nomeFile, "w");
	paginaSupport = trovaPagina(pagina);

	fprintf(f,"%s",paginaSupport);
	fclose(f);
	printf("Pagina scritta!\n");

	printf("Elimino gli oggetti utilizzati.\n");
	delete(mySelf);
	delete(server);
	free(pagina);
	printf("Oggetti eliminati!\n");

	return 0;
}


char* trovaPagina(char* s){
	char* ret;
	int len = strlen(s);
	int i=0;

	/*Continua fino a che non ci sono due a capo di fila e non si è raggiunta la fine del messaggio*/
	while((s[i]!='\n' || s[i+1]!='\n') && i<len-1){
		i++;
	}
	i+=2;

	/*Inserisce in ret i caratteri che compongono la pagina*/
	for(int j=0;j<len-i;j++){
		ret[j] = s[i];
		i++;
	}

	/*Restituisce la pagina duplicata*/
	return strdup(ret);
}