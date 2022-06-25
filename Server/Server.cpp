#include "Server.h"

std::vector<SOCKET> Connections;
std::vector<Room>	Rooms;
int					clientCounter = 0;


void setColor(const char* color) {
	if (color == "red")
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED);
	else if (color == "white")
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE);
	else if (color == "blue")
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_BLUE | FOREGROUND_INTENSITY);
	else if (color == "green")
		SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), FOREGROUND_GREEN | FOREGROUND_INTENSITY);

	return;
}

std::vector<std::string> stringSplit(std::string str, std::string delimiter) {
	size_t posStart = 0;
	size_t posEnd;
	std::vector<std::string> splitted;

	while ((posEnd = str.find(delimiter, posStart)) != std::string::npos) {
		splitted.push_back(str.substr(posStart, posEnd - posStart));
		posStart = posEnd + delimiter.length();
	}
	splitted.push_back(str.substr(posStart));

	return splitted;
}

std::string getCurrentTime() {
	std::time_t msgTime = std::time(0);
	char timeStr[50];
	std::strftime(timeStr, sizeof(timeStr), "%F, %T", std::localtime(&msgTime));

	return std::string(timeStr);
}

void sendPacket(SOCKET* socket, Packet* packetType, std::string* message) {
	int messageSize = (*message).size();
	send(*socket, (char*)&(*packetType), sizeof(Packet), NULL);
	send(*socket, (char*)&messageSize, sizeof(int), NULL);
	send(*socket, (*message).c_str(), messageSize, NULL);

	return;
}

void MessageHandler(int userID, std::string msg, int* roomID) {
	std::vector<std::string> splittedMessage = stringSplit(msg, " ");
	std::string message;

	if (splittedMessage[0] == "!exit" && splittedMessage.size() == 1) {
		if (*roomID == -1) {
			setColor("blue");
			printf("%20s | Exiting room: user#%d -> isn't in room\n", getCurrentTime().c_str(), userID);
			setColor("white");
			message = "You are not in any room yet";
			Packet packetType = pServerMessage;
			sendPacket(&Connections[userID], &packetType, &message);
			return;
		}
		for (auto it = Rooms[*roomID].users.begin(); it != Rooms[*roomID].users.end(); it++) {
			if (*it == userID) {
				Rooms[*roomID].users.erase(it);
				break;
			}
		}
		setColor("blue");
		printf("%20s | Exiting room: user#%d -> %s\n", getCurrentTime().c_str(), userID, Rooms[*roomID].name.c_str());
		setColor("white");
		message = "You are exiting room \'" + Rooms[*roomID].name + "\'";
		Packet packetType = pServerMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		*roomID = -1;
		return;
	}

	if (splittedMessage[0] == "!exit" && splittedMessage.size() != 1) {
		setColor("blue");
		printf("%20s | Exiting room: user#%d -> wrong number of parameters\n", getCurrentTime().c_str(), userID);
		setColor("white");
		message = "Wrong number of parameters for the command \'!exit\'";
		Packet packetType = pWarningMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!show" && splittedMessage.size() == 1) {
		if (!Rooms.size()) {
			setColor("blue");
			printf("%20s | Showing rooms: user#%d -> no rooms\n", getCurrentTime().c_str(), userID);
			setColor("white");
			message = "There is no any room yet";
			Packet packetType = pServerMessage;
			sendPacket(&Connections[userID], &packetType, &message);
			return;
		}
		setColor("blue");
		printf("%20s | Showing rooms: user#%d\n", getCurrentTime().c_str(), userID);
		setColor("white");
		for (int i = 0; i < Rooms.size(); i++)
			message += Rooms[i].name + "\n";
		message.pop_back();
		Packet packetType = pServerMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!show" && splittedMessage.size() != 1) {
		setColor("blue");
		printf("%20s | Showing rooms: user#%d -> wrong number of parameters\n", getCurrentTime().c_str(), userID);
		setColor("white");
		message = "Wrong number of parameters for the command \'!show\'";
		Packet packetType = pWarningMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!create" && splittedMessage.size() == 3) {
		message = "You are creating room \'" + splittedMessage[1] + "\'";
		bool nameExists = false;

		for (auto it = Rooms.begin(); it != Rooms.end() && !nameExists; it++) {
			if ((*it).name == splittedMessage[1]) {
				message = "This room name is alredy exists";
				nameExists = true;
			}
		}
		setColor("blue");
		if (!nameExists) {
			Rooms.push_back(Room(splittedMessage[1], splittedMessage[2]));
			printf("%20s | Creating room: user#%d -> \'%s\'\n", getCurrentTime().c_str(), userID, splittedMessage[1].c_str());
		}
		else
			printf("%20s | Creating room: user#%d -> wrong parameters\n", getCurrentTime().c_str(), userID);
		setColor("white");

		Packet packetType = pServerMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!create" && splittedMessage.size() != 3) {
		setColor("blue");
		printf("%20s | Creating room: user#%d -> wrong number of parameters\n", getCurrentTime().c_str(), userID);
		setColor("white");
		message = "Wrong number of parameters for the command \'!create\'";
		Packet packetType = pWarningMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!remove" && splittedMessage.size() == 3) {
		Packet packetType = pServerMessage;
		message = "You are removing room \'" + splittedMessage[1] + "\'";
		bool roomExists = false;
		for (auto it = Rooms.begin(); it != Rooms.end() && !roomExists; it++) {
			if ((*it).name == splittedMessage[1]) {
				roomExists = true;
				if ((*it).password == splittedMessage[2]) {
					Rooms.erase(it);
					break;
				}
				else {
					packetType = pWarningMessage;
					message = "Wrong room password";
				}
			}
		}
		if (!roomExists) {
			packetType = pWarningMessage;
			message = "Wrong room name";
		}
		setColor("blue");
		if (message != "You are removing room \'" + splittedMessage[1] + "\'")
			printf("%20s | Removing room: user#%d -> wrong parameters\n", getCurrentTime().c_str(), userID);
		else
			printf("%20s | Removing room: user#%d -> \'%s\'\n", getCurrentTime().c_str(), userID, splittedMessage[1].c_str());
		setColor("white");
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!remove" && splittedMessage.size() != 3) {
		setColor("blue");
		printf("%20s | Removing room: user#%d -> wrong number of parameters\n", getCurrentTime().c_str(), userID);
		setColor("white");
		message = "Wrong number of parameters for the command \'!remove\'";
		Packet packetType = pWarningMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!open" && splittedMessage.size() == 3) {
		message = "You are opening room \'" + splittedMessage[1] + "\'";
		bool validData = false;
		for (int i = 0; i < Rooms.size() && !validData; i++) {
			if (Rooms[i].name == splittedMessage[1] && Rooms[i].password == splittedMessage[2]) {
				Rooms[i].users.push_back(userID);
				validData = true;
				*roomID = i;
			}
		}
		setColor("blue");
		if (!validData) {
			message = "Wrong room name or password";
			printf("%20s | Opening room: user#%d -> wrong parameters\n", getCurrentTime().c_str(), userID);
		}
		else
			printf("%20s | Opening room: user#%d -> \'%s\'\n", getCurrentTime().c_str(), userID, splittedMessage[1].c_str());
		setColor("white");

		Packet packetType = pServerMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (splittedMessage[0] == "!open" && splittedMessage.size() != 3) {
		setColor("blue");
		printf("%20s | Opening room: user#%d -> wrong number of parameters\n", getCurrentTime().c_str(), userID);
		setColor("white");
		message = "Wrong number of parameters for the command \'!open\'";
		Packet packetType = pWarningMessage;
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	if (*roomID != -1) {
		message = msg;
		if (Rooms[*roomID].users.size() == 1) {
			message = "There is no anyone else in the room";
			Packet packetType = pServerMessage;
			printf("%20s | Sending message: user#%d -> NULL\n", getCurrentTime().c_str(), userID);
			sendPacket(&Connections[userID], &packetType, &message);
		}
		for (int it : Rooms[*roomID].users) {
			if (userID == it)
				continue;
			Packet packetType = pChatMessage;
			printf("%20s | Sending message: user#%d -> user#%d\n", getCurrentTime().c_str(), userID, it);
			sendPacket(&Connections[it], &packetType, &message);
		}
		return;
	}

	if (*roomID == -1) {
		message = "You are not in any room yet. This message wasn't delivired to anyone else";
		Packet packetType = pServerMessage;
		printf("%20s | Sending message: user#%d -> NULL\n", getCurrentTime().c_str(), userID);
		sendPacket(&Connections[userID], &packetType, &message);
		return;
	}

	return;
}

bool processPacket(int index, Packet packetType, int*roomID) {
	switch (packetType) {
	case pChatMessage:
	{
		int msgSize;
		recv(Connections[index], (char*)&msgSize, sizeof(int), NULL);
		char* msg = new char[msgSize + 1];
		msg[msgSize] = '\0';
		recv(Connections[index], msg, msgSize, NULL);
		MessageHandler(index, msg, roomID);
		delete[] msg;
		break;
	}
	default:
	{
		setColor("red");
		printf("RECEIVED AN UNRECOGNIZED TYPE OF PACKET.\n\n");
		setColor("white");
		break;
	}
	}
	return true;
}

void clientHandler(int index) {
	Packet packetType;
	int roomID = -1;

	while (1) {
		if (recv(Connections[index], (char*)&packetType, sizeof(Packet), NULL) <= 0) {
			setColor("red");
			printf("%20s | Client #%d has been disconnected!\n", getCurrentTime().c_str(), index);
			setColor("white");
			if (roomID != -1) {
				for (auto it = Rooms[roomID].users.begin(); it != Rooms[roomID].users.end(); it++) {
					if (*it == index) {
						Rooms[roomID].users.erase(it);
						break;
					}
				}
			}
			break;
		}
		processPacket(index, packetType, &roomID);
	}

	closesocket(index);
	Connections[index] = INVALID_SOCKET;
	if (index == Connections.size() - 1)
		Connections.pop_back();
	return;
}

void main(int argc, char* argv[]) {

	WSAData wsaData;
	if (WSAStartup(MAKEWORD(2, 1), &wsaData) != 0) {
		WSACleanup();
		printf("WSAStartup error\n");
		exit(0);
	}

	SOCKADDR_IN addr;
	int sizeofAddr = sizeof(addr);
	addr.sin_addr.s_addr = inet_addr("127.0.0.1");
	addr.sin_port = htons(1111);
	addr.sin_family = AF_INET;

	SOCKET sListen = socket(AF_INET, SOCK_STREAM, NULL);
	if (sListen == INVALID_SOCKET) {
		WSACleanup();
		printf("Socket creation error\n");
		exit(0);
	}

	bind(sListen, (SOCKADDR*)&addr, sizeofAddr);
	listen(sListen, SOMAXCONN);

	SOCKET newConnection;
	while (clientCounter < MAX_CONNECTIONS) {
		if (!(newConnection = accept(sListen, (SOCKADDR*)&addr, &sizeofAddr))) {
			setColor("red");
			printf("Client connection failure\n\n");
			setColor("white");
			continue;
		}

		clientCounter = 0;
		for (auto sock : Connections)
			clientCounter += (sock != INVALID_SOCKET);

		setColor("green");
		bool reuseSocket = false;
		for (int i = 0; i < Connections.size() && !reuseSocket; i++) {
			if (Connections[i] == INVALID_SOCKET) {
				CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)i, NULL, NULL);
				Connections[i] = newConnection;
				printf("%20s | Client #%d has been connected!\n", getCurrentTime().c_str(), i);
				reuseSocket = true;
			}
		}
		if (!reuseSocket) {
			CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)clientHandler, (LPVOID)(Connections.size()), NULL, NULL);
			Connections.push_back(newConnection);
			clientCounter++;
			printf("%20s | Client #%d has been connected!\n", getCurrentTime().c_str(), clientCounter - 1);
		}
		setColor("white");
	}
	setColor("red");
	printf("\nCONNECTION LIMIT EXCEEDED\nSERVER SHUTDOWN\n\n");
	setColor("white");
	closesocket(sListen);
	closesocket(newConnection);
	return;
}