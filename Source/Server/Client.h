#pragma once
#include "NetServer.h"
#include "../Utility/GameError.h"

class Client : public NetServer
{
public:
	enum class States
	{
		notConnected,
		Connecting,
		Connected,
		Timeout,
		Disconnected,
		Denied,
		Error
	};
public:
	Client()
		:
		NetServer(0)
	{
		StartServer();
	}
	virtual ~Client()
	{
	}
	void Connect(const std::string& ip, PORT p)
	{
		IPaddress addr;
		if (SDLNet_ResolveHost(&addr, ip.c_str(), p) != 0)
		{
			throw GameError("could not resolve ip adress");
		}
		server.init(addr);
		serverKey = HostKey(addr);
		init = true;
		timer.StartWatch(); // join request timer
	}
	// main thread
	void Update()
	{
		if (!init) return;

		if (Thread::HasThreadError())
		{
			SetClientError(GetThreadError());
		}

		switch (state)
		{
		case Client::States::notConnected:
			// ping
		{
			if (tries > 11)
			{
				state = States::Timeout;
				break;
			}

			NetHost::PingStruct& ps = server.GetPingStruct();
			if (ps.newPing > 0.0f)
			{
				state = States::Connecting;
				break;
			}

			if (timer.ConvertMilli(ps.lastSend) > 2000.0f)
			{
				SendServerPing();
				tries++;
			}

		}
			break;
		case Client::States::Connecting:
			if (timer.GetTimeSecond() > 0.5f)
			{
				SendJoin();
				timer.StartWatch();
				// request
			}
			break;
		case Client::States::Connected:
			// ping etc.
			NetServer::UpdateHost(server);

			// check timeout
			if (server.IsTimedOut())
				state = States::Timeout;
			break;
		default:
			break;
		}
	}
	States GetState() const
	{
		return state;
	}
	void SendServerPing()
	{
		NetServer::SendPing(server.GetAddr());
		NetHost::PingStruct& ps = server.GetPingStruct();
		ps.lastSend = Timer::GetTimestamp();
	}
	float GetPing()
	{
		return server.GetPing();
	}
	std::string GetError() const
	{
		return error;
	}
	void SendReliable(DataContainer&& con)
	{
		NetServer::SendReliable(std::move(con), server);
	}
	void SendUnreliable(DataContainer&& con)
	{
		NetServer::SendUnreliable((char*)con.start(), con.length(), server);
	}
	void SendDisconnect()
	{
		if (state == States::Connected)
			NetServer::SendDisconnect(server.GetAddr());
		state = States::Disconnected;
	}
protected:
	// listen thread
	virtual void HandlePing(const IPaddress& addr, Timestamp png) override
	{
		NetHost::PingStruct& ps = server.GetPingStruct();
		ps.newPing = timer.ConvertMilli(png);
		if (ps.avgPing == 0.0f)
		{
			ps.avgPing = ps.newPing;
		}
		else
		{
			ps.avgPing = 0.8f * ps.avgPing + 0.2f * ps.newPing;
		}
		server.ResetTimeout();
	}

	// 4 byte extra space + data
	virtual void HandleUnreliable(const IPaddress& addr, PackID id, char* data, const int len) override
	{
		server.ResetTimeout();
		if (server.GetLastUnreliableID() < id)
		{
			server.SetLastUnreliableID(id);
			// skip extra space
			AddPacket(data + 4, len - 4);
		}
	}
	
	// no extra space
	virtual void HandleReliable(const IPaddress& addr, PackID id, char* data, const int len) override
	{
		if (serverKey == addr)
		{
			server.ProcessReliableStream(data, len, id);

			std::vector< DataContainer > cons;
			while (server.HasNextContainer(cons, -1))
			{
				for (auto& c : cons)
				{
					AddPacket(std::move(c));
				}
			}
		}
	}
	// you should remove the host after recieving a damaged package
	virtual void HandleDamagedPackage(const IPaddress& addr, const std::exception& e) override
	{
		// cut connection with server
		if(serverKey == addr)
			SetClientError(std::string("recieved damaged package: ") + e.what());
	}
	virtual void HandleReliableResponse(const IPaddress& addr, PackID id)
	{
		if(serverKey == addr)
			server.ProcessReliableAnswer(id);
	}
	virtual void HandleAccept(int val)
	{
		// val == max pack size
		SetMaxPackSize(val);
		server.ResetMaxPackSize(val);
		state = States::Connected;
	}
	virtual void HandleDenied(const char* reason)
	{
		error = reason;
		state = States::Denied;
	}
	virtual void HandleDisconnect(const IPaddress& addr) override
	{
		if (serverKey == addr)
		{
			state = States::Disconnected;
		}
	}

private:
	void SendJoin()
	{
		char msg = 'j';
		Send(server.GetAddr(), &msg, 1);
	}
	void SetClientError(const std::string& s)
	{
		error = s;
		state = States::Error;
	}
private:
	bool init = false;
	States state = States::notConnected;
	NetHost server;
	HostKey serverKey;

	Timer timer;
	int tries = 0;
	std::string error;
};