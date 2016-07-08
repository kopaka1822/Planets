#pragma once
#include "Thread.h"
#include <SDL_net.h>
#include "SDL_Exception.h"

using PORT = Uint16;

class UDPServer : public Thread
{
public:
	UDPServer(PORT listenPort)
	{
		sock = SDLNet_UDP_Open(listenPort);
		if (!sock)
			throw SDL_NetException("could not create network socket");
	}
	~UDPServer()
	{
		Shutdown();
	}
	void Shutdown()
	{
		if (sock)
		{
			SDLNet_UDP_Close(sock);
			sock = nullptr;
		}
	}
	virtual int ThreadProc() override
	{
		return 0;
	}
	
private:
	UDPsocket sock;
};