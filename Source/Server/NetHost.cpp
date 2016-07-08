#include "NetHost.h"
#include "NetServer.h"

int NetServer::MAX_PACKSIZE = 4000;

NetHost::NetHost()
	:
	sOut(REL_HEADER, NetServer::GetMaxPackSize())
{}
NetHost::NetHost(const IPaddress& addr, unsigned int id)
	:
	sOut(REL_HEADER, NetServer::GetMaxPackSize())
{
	sockaddr = addr;
	playerID = id;
	_init();
}