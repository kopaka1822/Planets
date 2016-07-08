#pragma once
#include "UDPServer.h"
#include "../Utility/DataContainer.h"
#include <vector>
#include "NetHost.h"

class NetServer : public UDPServer
{
protected:
	struct HostKey
	{
		uint64_t key;
		HostKey() {}
		HostKey(const IPaddress& addr)
		{
			key = uint64_t(addr.host) | (uint64_t(addr.port) << 32);
		}
		bool operator<(const HostKey& rhs) const
		{
			return key < rhs.key;
		}
		bool operator==(const HostKey& rhs) const
		{
			return key == rhs.key;
		}
		bool operator==(const IPaddress& addr)
		{
			return (*this) == HostKey(addr);
		}
	};
public:
	NetServer(PORT listen)
		:
		UDPServer(listen)
	{
		//assert(MIN_REL_SIZE == NetHost::REL_HEADER);
	}
	virtual ~NetServer()
	{
		StopServer();
	}
	static DataContainer GetConRel()
	{
		DataContainer con(UDP_BUFLEN);
		//con += (char)'r';
		//con += (int)0;

		return con;
	}
	static DataContainer GetConUnrel()
	{
		DataContainer con(UDP_BUFLEN);
		con += (char)'u';
		con += (int)0;

		return con;
	}
	static DataContainer GetConRelSmall()
	{
		DataContainer con(100);
		//con += (char)'r';
		//con += (int)0;

		return con;
	}
	static DataContainer GetConUnrelSmall()
	{
		DataContainer con(100);
		con += (char)'u';
		con += (int)0;

		return con;
	}
	std::vector<DataContainer> GetData()
	{
		std::vector<DataContainer> res;
		if (!recvdata.empty())
		{
			res.reserve(20);

			muData.Lock();
			res = std::move(recvdata);
			muData.Unlock();
		}
		return res;
	}

	static void SetMaxPackSize(int size)
	{
		MAX_PACKSIZE = size;
	}
	static int GetMaxPackSize()
	{
		return MAX_PACKSIZE;
	}
protected:
	virtual void HandleData(const IPaddress& addr, char* data, int len) override final
	{
		/*
			r = reliable				-id
			u = unreliable				-id
			p = ping					-timestamp
			q = ping back				-timestamp
			j = join					-
			a = accept					-
			d = denied					-char[] reason
			s = reply (for reliable)	-id
			t = disconnect
		*/
		if (len <= 0)
			return;

		switch (data[0])
		{
		case 'r':
		{	// send back response first
			if (len < MIN_REL_SIZE)
				return;
			PackID id = *(PackID*)(data + 1);
			Timestamp s = *(Timestamp*)(data + 1 + sizeof(PackID));

			SendReliableResponse(addr, id,s);

			// catch errors for damaged packets!
			try
			{
				if (len > MIN_REL_SIZE) // dotn add data with len 0 !
					HandleReliable(addr, *(PackID*)(data + 1), data + MIN_REL_SIZE, len - MIN_REL_SIZE);
			}
			catch (std::logic_error& e)
			{
				HandleDamagedPackage(addr, e);
			}
		}
			break;
		case 'u':
			if (len < MIN_UNREL_SIZE)
				return;
			HandleUnreliable(addr, *(PackID*)(data + 1), data + 1, len - 1);
			break;
		case 'p':
			data[0] = 'q';
			Send(addr, data, len);
			break;
		case 'q':
			if (len != PING_SIZE)
				return;

			HandlePing(addr, *(Timestamp*)(data + 1));
			break;
		case 'j':
			HandleJoin(addr);
			break;
		case 'a':
			HandleAccept(*(int*)(data + 1));
			break;
		case 'd':
			HandleDenied(data + 1);
			break;
		case 's': // reliable respone
			if (len != RELRES_SIZE)
				return;

			HandleReliableResponse(addr, *(PackID*)(data + 1));
			HandlePing(addr, *(Timestamp*)(data + 1 + sizeof(PackID)));
			break;
		case 't':
			HandleDisconnect(addr);
			break;
		}
	}
	virtual void HandleJoin(const IPaddress&)
	{}
	virtual void HandleAccept(int){}
	// reason
	virtual void HandleDenied(const char*){}

	virtual void HandlePing(const IPaddress& addr, Timestamp png) = 0;

	// this may throw std::logic_error for invalid containers
	virtual void HandleReliable(const IPaddress& addr, PackID id, char* data, const int len) = 0;
	// you should remove the host after recieving a damaged package
	virtual void HandleDamagedPackage(const IPaddress& addr, const std::exception& e) = 0;
	virtual void HandleDisconnect(const IPaddress& addr) = 0;

	// 4 byte extra space + data
	virtual void HandleUnreliable(const IPaddress& addr, PackID id, char* data, const int len) = 0;
	// 4 byte extra space + data
	virtual void HandleReliableResponse(const IPaddress& addr, PackID id) = 0;
	void SendUnreliable(char* data, unsigned int len, NetHost& host)
	{
		assert(len >= 5);
		// write id
		PackID id = host.GetUnreliableOut();

		memcpy(data + 1, &id, sizeof(PackID));
		Send(host.GetAddr(), data, len);
	}
	void SendReliable(DataContainer&& con, NetHost& host)
	{
		// queue data
		if (host.AddReliableOutStream(std::move(con)))
		{
			// data should be send
			DataContainer res;

			while (host.GetReliableOutStream(&res))
			{
				// fill header and send
				PackID id = host.GetReliableOut();
				*(char*)res[0] = 'r';
				*(PackID*)res[1] = id;
				*(Timestamp*)res[1 + sizeof(PackID)] = Timer::GetTimestamp();

				// send this container
				Send(host.GetAddr(), (char*)res.start(), res.length());

				host.AddReliableOut(std::move(res));
				host.GetPingStruct().lastSend = Timer::GetTimestamp();
			}
		}
	}
	void UpdateHost(NetHost& host)
	{
		// Reliable Stream Out
		// make sure that stream is send after some ms..

		if (host.GetReliableStreamWaitTime() > std::max(host.GetPing() / 2.0f, 10.0f))
		{
			// dont wait longer than ping / 2 ms
			DataContainer con;
			if (host.GetReliableOutStream(&con))
			{
				// fill header and send
				PackID id = host.GetReliableOut();
				*(char*)con[0] = 'r';
				*(PackID*)con[1] = id;
				*(Timestamp*)con[1 + sizeof(PackID)] = Timer::GetTimestamp();

				// send this container
				Send(host.GetAddr(), (char*)con.start(), con.length());

				host.AddReliableOut(std::move(con));
			}
		}

		// Reliable Resend
		NetHost::PingStruct& ps = host.GetPingStruct();

		host.StartResending();
		while (DataContainer* pCon = host.HasResend())
		{
			if (pCon != nullptr)
			{
				// refresh timestamp
				*(Timestamp*)((*pCon)[1 + sizeof(PackID)]) = Timer::GetTimestamp();
				Send(host.GetAddr(), (char*)pCon->start(), pCon->length());
				ps.lastSend = Timer::GetTimestamp();
			}
		}

		// Ping
		if (timer.ConvertMilli(ps.lastSend) > 1000.0f)
		{
			NetServer::SendPing(host.GetAddr());
			ps.lastSend = Timer::GetTimestamp();
		}
	}
	void SendPing(const IPaddress& addr)
	{
		char buffer[sizeof(Timestamp) + 1];
		BufferWriter w(buffer);
		w.writeChar('p');
		w.write<Timestamp>(Timer::GetTimestamp());

		Send(addr, buffer, w.GetLength());
	}
	void SendDisconnect(const IPaddress& addr)
	{
		char msg = 't';
		for (int i = 0; i < 6; i++)
		{
			Send(addr, &msg, sizeof(char));
		}
	}

	void AddPacket(const char* data, unsigned int len)
	{
		DataContainer con(data, len);
		muData.Lock();
		recvdata.push_back(std::move(con));
		muData.Unlock();
	}
	void AddPacket(DataContainer&& con)
	{
		muData.Lock();
		recvdata.push_back(std::move(con));
		muData.Unlock();
	}

private:
	void SendReliableResponse(const IPaddress& addr, PackID id, Timestamp ts)
	{
		char buffer[1 + sizeof(PackID) + sizeof(Timestamp)];
		BufferWriter w(buffer);
		w.writeChar('s');
		w.write<PackID>(id);
		w.write<Timestamp>(ts);

		Send(addr, buffer, sizeof(buffer));
	}
protected:
	Timer timer; // ping calc
private:
	Mutex muData;
	std::vector< DataContainer > recvdata;

	static int MAX_PACKSIZE;
public:
	static const unsigned int MIN_REL_SIZE = sizeof(char) + sizeof(PackID) + sizeof(Timestamp);
	static const unsigned int RELRES_SIZE = sizeof(char) + sizeof(PackID) + sizeof(Timestamp);
	static const unsigned int MIN_UNREL_SIZE = sizeof(char) + sizeof(PackID);
	static const unsigned int PING_SIZE = sizeof(char) + sizeof(Timestamp);
};
