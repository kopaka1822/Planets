#pragma once
#include <map>
#include "NetServer.h"
#include <memory>
#include "NetHost.h"

class Server : public NetServer
{
public:
	struct Event
	{
		int playerID;
		enum Evnt
		{
			Join,
			Disconnect,
			TimedOut
		} e;
	};
public:
	Server(PORT port, unsigned int mPlayer)
		:
		NetServer(port),
		maxPlayer(mPlayer)
	{
		StartServer();
	}
	void SetJoinable(bool b)
	{
		joinable = b;
	}
	bool isJoinable() const
	{
		return joinable;
	}
	void Update()
	{
		// ping + rel...
		muPlayers.Lock();
		for (auto& p : playermap)
		{
			NetServer::UpdateHost(*p.second);
		}

		// timeout
		for (auto p = playermap.begin(), end = playermap.end(); p != end; p++)
		{
			if (p->second->IsTimedOut())
			{
				AddEvent(p->second->GetID(), Event::TimedOut);
				playermap.erase(p);
				break;
			}
		}

		muPlayers.Unlock();
	}
	void SendContainerAllReliable(DataContainer&& con)
	{
		muPlayers.Lock();
		if (!playermap.empty())
		{
			auto e = playermap.begin();
			// iterate through all exept one and clone data
			for (unsigned int i = 1; i < playermap.size(); i++)
			{
				SendReliable(con.duplicate(), *e->second);
				++e;
			}
			assert(e != playermap.end());
			SendReliable(std::move(con), *e->second);
			assert((++e) == playermap.end());
		}
		muPlayers.Unlock();
	}
	void SendContainerAllUnreliable(DataContainer&& con)
	{
		assert(con.length() >= 7);
		if (con.length() < 7)
			printf("container size to small!\n");

		muPlayers.Lock();
		for (auto& p : playermap)
		{
			if (p.second->GetOutReliableCount() < MAX_AWAITING)
				SendUnreliable((char*)con.start(), con.length(), *p.second);
		}
		muPlayers.Unlock();
	}
	void SendContainerReliable(DataContainer&& con, unsigned int id)
	{
		muPlayers.Lock();
		for (auto& p : playermap)
		{
			if (p.second->GetID() == id)
			{
				SendReliable(std::move(con), *p.second);
				break;
			}
		}
		muPlayers.Unlock();
	}
	void SendContainerUnreliable(DataContainer&& con, unsigned int id)
	{
		muPlayers.Lock();
		for (auto& p : playermap)
		{
			if (p.second->GetID() == id)
			{
				if (p.second->GetOutReliableCount() < MAX_AWAITING)
					SendUnreliable((char*)con.start(), con.length(), *p.second);
				break;
			}
		}
		muPlayers.Unlock();
	}

	std::vector< Event > GetEvents()
	{
		std::vector< Event > res;
		if (events.size())
		{
			muEvnts.Lock();
			res = std::move(events);
			muEvnts.Unlock();
		}
		return res;
	}
	int GetMaxPlayers() const
	{
		return maxPlayer;
	}
	int GetNumConnected() const
	{
		return playermap.size();
	}
	float GetPing(int id) const
	{
		float ping = 0.0f;
		muPlayers.Lock();
		for (const auto& p : playermap)
		{
			if (p.second->GetID() == id)
			{
				ping = p.second->GetPing();
				break;
			}
		}
		muPlayers.Unlock();
		return ping;
	}
protected:
	virtual void HandleJoin(const IPaddress& addr) override
	{
		if (joinable)
		{
			HostKey key(addr);

			muPlayers.Lock();
			auto pl = playermap.find(key);
			if (pl == playermap.end())
			{
				// not in map
				if (playermap.size() < maxPlayer)
				{
					// add to playmap

					// find next free id
					unsigned int id = 1;
					bool free = false;

					while (!free)
					{
						free = true;
						for (const auto& p : playermap)
						{
							if (p.second->GetID() == id)
							{
								free = false;
								id++;
								break;
							}
						}
					}


					playermap[key] = std::unique_ptr<NetHost>(new NetHost(addr,id));

					// send accept
					SendAccept(addr);

					AddEvent(id, Event::Join);
				}
				else
				{
					// send full
					SendDeny(addr, "server full");
				}
			}
			else
			{
				// already in map
				// send accept
				SendAccept(addr);
			}
		}
		else
		{
			SendDeny(addr, "not joinable");
		}
		muPlayers.Unlock();
	}
	virtual void HandlePing(const IPaddress& addr, Timestamp png) override
	{
		HostKey key(addr);
		muPlayers.Lock();
		auto pl = playermap.find(key);

		if (pl != playermap.end())
		{
			NetHost::PingStruct& ps = pl->second->GetPingStruct();
			ps.newPing = timer.ConvertMilli(png);
			if (ps.avgPing == 0.0f)
			{
				ps.avgPing = ps.newPing;
			}
			else
			{
				ps.avgPing = 0.8f * ps.avgPing + 0.2f * ps.newPing;
			}
			pl->second->ResetTimeout();
		}
		muPlayers.Unlock();
	}
	virtual void HandleUnreliable(const IPaddress& addr, PackID id, char* data, const int len) override
	{
		HostKey key(addr);
		muPlayers.Lock();
		auto pl = playermap.find(key);

		if (pl != playermap.end())
		{
			pl->second->ResetTimeout();
			if (pl->second->GetLastUnreliableID() < id)
			{
				pl->second->SetLastUnreliableID(id);
				// overwrite extra space with player id
				BufferWriter w(data);
				w.writeInt(pl->second->GetID());

				AddPacket(data, len);
			}
		}
		muPlayers.Unlock();
	}
	virtual void HandleReliable(const IPaddress& addr, PackID id, char* data, const int len) override
	{
		HostKey key(addr);
		LockGuard g(muPlayers);
		auto pl = playermap.find(key);

		if (pl != playermap.end())
		{
			// overwrite extra space with player id
			pl->second->ProcessReliableStream(data, len, id);

			std::vector< DataContainer > cons;
			while (pl->second->HasNextContainer(cons,pl->second->GetID()))
			{
				for (auto& c : cons)
				{
					AddPacket(std::move(c));
				}
			}
		}
	}
	virtual void HandleDamagedPackage(const IPaddress& addr, const std::exception&) override
	{
		// cut connection with this host
		HandleDisconnect(addr);
	}
	virtual void HandleReliableResponse(const IPaddress& addr, PackID id)
	{
		HostKey key(addr);
		muPlayers.Lock();
		auto pl = playermap.find(key);

		if (pl != playermap.end())
		{
			pl->second->ProcessReliableAnswer(id);
		}
		muPlayers.Unlock();
	}
	virtual void HandleDisconnect(const IPaddress& addr) override
	{
		HostKey key(addr);
		LockGuard g(muPlayers);
		auto pl = playermap.find(key);

		if (pl != playermap.end())
		{
			AddEvent(pl->second->GetID(), Event::Disconnect);
			playermap.erase(pl);
		}
	}
private:
	void SendAccept(const IPaddress& addr)
	{
		char buffer[10];
		BufferWriter w(buffer);

		w.writeChar('a');
		w.writeInt(GetMaxPackSize());

		UDPServer::Send(addr, buffer, w.GetLength());
	}
	void SendDeny(const IPaddress& addr, const char* msg)
	{
		char buffer[128];
		BufferWriter w(buffer);

		w.writeChar('d');
		w.writeString(msg);

		UDPServer::Send(addr, buffer, w.GetLength());
	}
	void AddEvent(int id, Event::Evnt e)
	{
		muEvnts.Lock();
		events.push_back(Event{ id, e });
		muEvnts.Unlock();
	}
private:
	bool joinable = true;
	const unsigned int maxPlayer;
	std::map< HostKey, std::unique_ptr<NetHost> > playermap;
	Mutex muPlayers;

	std::vector<Event> events;
	Mutex muEvnts;

	static const unsigned int MAX_AWAITING = 10;// 5 * ping    //15;//25; // 35
};
