#pragma once

#pragma comment(lib, "ws2_32.lib")
#pragma warning(disable: 4996)
#define _WINSOCK_DEPRECATED_NO_WARNINGS

#include <winsock2.h>
#include <windows.h>
#include <ctime>
#include <string>
#include <vector>
#include <iostream>

enum Packet {
	pChatMessage,
	pServerMessage,
	pWarningMessage
};

void setColor(const char* color);

std::vector<std::string> stringSplit(std::string str, std::string delimiter);

std::string getCurrentTime();

void addTimeNickname(std::string* word, std::string* message);

bool connectToServer(std::string address, int port);

void sendPacket(SOCKET* socket, Packet* packetType, std::string* message);

bool processInstructions(std::vector<std::string> splittedMessage);

bool processPacket(Packet packetType);

void clientHandler();