#pragma once
#include "../Utility/Thread.h"
#include "../SDL/SDL_net.h"
#include "../System/System.h"

using PORT = Uint16;
#define UDP_BUFLEN 4096 //Max UDP Buffersize

class UDPServer : public Thread
{
public:
	UDPServer(PORT listenPort)
	{
		// open listen port
		sock = SDLNet_UDP_Open(listenPort);
		if (!sock)
			throw SDL_NetException("could not create network socket port already used? ");

		if (SDLNet_ResolveHost(&myAdr, nullptr, listenPort) != 0)
			throw SDL_NetException("could not resolve own adress");
	}
	~UDPServer()
	{
		StopServer();
	}
	void Send(const UDPsocket& sock, char* data, size_t len)
	{
		UDPpacket pack;
		pack.data = (Uint8*)data;
		pack.len = len;

		// send to sockets adress
		int num = SDLNet_UDP_Send(sock, 0, &pack);

		if (num == 0)
			throw SDL_NetException("could not send");
	}
	void Send(const IPaddress& ip, char* data, size_t len)
	{
		UDPpacket pack;
		pack.data = (Uint8*)data;
		pack.len = len;
		pack.address = ip;

		// sned to packet adress
		int num = SDLNet_UDP_Send(sock, -1, &pack);

		if (num == 0)
			throw SDL_NetException("could not send");
	}
	void StartServer()
	{
		if (isListening)
			return;

		// bind listen socket
		listenChannel = SDLNet_UDP_Bind(sock, -1, &myAdr);
		if (listenChannel == -1)
			throw SDL_NetException("could not bind listen socket");

		isListening = true;
		Thread::Begin();
	}
	void StopServer()
	{
		if (sock)
		{
			SDLNet_UDP_Unbind(sock, listenChannel);
			SDLNet_UDP_Close(sock);
			sock = nullptr;
		}
		isListening = false;
		Thread::Join();
	}

protected:
	virtual int ThreadProc() override final
	{
		int numrecv;

		UDPpacket* packet = SDLNet_AllocPacket(UDP_BUFLEN);
		try
		{
			if (!packet)
				throw std::bad_alloc();

			while (isListening)
			{
				numrecv = SDLNet_UDP_Recv(sock, packet);
				if (numrecv == 0)
				{
					// nothing recieved
					System::Sleep(1);
					continue;
				}
				else if (numrecv < 0)
				{
					// error
					throw SDL_NetException("recieving error");
				}

				HandleData(packet->address, (char*)packet->data, packet->len);
			}
		}
		catch (const std::exception& e)
		{
			SetThreadError(e.what());
		}
		catch (...)
		{
			SetThreadError("FATAL ERROR");
		}

		if (packet)
			SDLNet_FreePacket(packet);

		return 0;
	}
	virtual void HandleData(const IPaddress& addr, char* data, int len) = 0;
private:
	UDPsocket sock;
	int listenChannel = 0;
	IPaddress myAdr;
	bool isListening = false;
};
