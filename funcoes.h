#pragma once
#ifndef funcoes_h 
#define funcoes_h 

#include <WinSock2.h>
#include <iostream>
#include <stdio.h>
#include <direct.h>
#define GetCurrentDir _getcwd

#define ENDERECO				"127.0.0.1"
#define PORTA					9996
#define TAMANHO_BUFFER			200
#define TEMPO_ESPERA			5000


typedef enum { thelloSend, tEnviarArquivoSend, tEOFSend ,tReceberArquivoSend, trodarCodigoSend};
typedef enum { thelloReq, tEnviarArquivoReq, tEOFReq , tReceberArquivoReq, trodarCodigoReq};

typedef struct mensagem
{
	char buffer[TAMANHO_BUFFER - 5];
	unsigned int tamanhoMsg;
	char opCode;

} tmensagem;

int criarSoquete(int *soquete);
int receber(int soquete, tmensagem *msg);
int enviar(int soquete, tmensagem *msg);
void enviarArquivo(int soquete, FILE *arquivoPtr, tmensagem *msg);
int esperarComando(int soquete);
void preencherStruct(tmensagem *msgPtr, const char *string, char opcode, int len = 0);
void receberArquivo(int soquete, FILE *arquivoPtr, tmensagem *msg);
void rodarComando(int soquete, tmensagem *msg);





#endif