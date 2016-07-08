#pragma once
using PackID = unsigned int;
using Timestamp = unsigned long long;
#include <assert.h>
#include <map>
#include <unordered_set>
#include "../Utility/Timer.h"
#include "NetStreamIn.h"
#include "NetStreamOut.h"
#include "UDPServer.h"
#include "../Utility/Mutex.h"
#include "../Utility/LockGuard.h"
#include <list>

class NetHost
{
public:
	struct PingStruct
	{
		Timestamp lastSend; // last try
		float avgPing; // ms
		float newPing;
	};
	struct RelStruct
	{
		RelStruct(){}
		RelStruct(const RelStruct& c) = delete;
		RelStruct& operator=(const RelStruct& c) = delete;
		RelStruct(RelStruct&& m)
		{
			swap(m);
		}
		RelStruct& operator=(RelStruct&& m)
		{
			swap(m);
		}
		DataContainer con;
		PackID id;
		Timestamp lastSend;
		void swap(RelStruct& o)
		{
			std::swap(con, o.con);
			std::swap(id, o.id);
			std::swap(lastSend, o.lastSend);
		}
	};
public:
	NetHost();
	NetHost(const IPaddress& addr, unsigned int id);
	NetHost(const NetHost& c) = delete;
	NetHost& operator=(const NetHost& c) = delete;

	void init(IPaddress addr)
	{
		sockaddr = addr;
		playerID = 0; // indicates client
		_init();
	}
	const IPaddress& GetAddr() const
	{
		return sockaddr;
	}
	unsigned int GetID() const
	{
		return playerID;
	}
	unsigned int GetLastUnreliableID() const
	{
		return lastUnreliableID;
	}
	void SetLastUnreliableID(unsigned int id)
	{
		lastUnreliableID = id;
	}
	float GetPing() const
	{
		return ping.avgPing;
	}
	PingStruct& GetPingStruct()
	{
		return ping;
	}
	PackID GetUnreliableOut()
	{
		return unreliableOutgoing++;
	}
	// in data = data without header
	void ProcessReliableStream(const char* data, unsigned int len, PackID id)
	{
		ResetTimeout();
		if (id < reliableAwaiting)
			return;

		// put in list
		// insert does nothing if element already exists
		if (inTempStorage.find(id) == inTempStorage.end())
			inTempStorage.insert(std::pair<PackID, DataContainer>(id, DataContainer(data, len)));
	}

	// parameter: empty container | front ID will be played in the first 4 bytes if frontID > 0
	// this will throw std::logic_error if data is damaged
	bool HasNextContainer(std::vector<DataContainer>& data, int frontID)
	{
		if (inTempStorage.empty())
			return false;

		auto elm = inTempStorage.find(reliableAwaiting);

		if (elm != inTempStorage.end())
		{
			data = sIn.ProcessStream(std::move(elm->second),frontID);
			inTempStorage.erase(elm);
			reliableAwaiting++;
			return true;
		}
		return false;
	}
	PackID GetReliableOut()
	{
		return reliableOutgoing++;
	}

	// returns true if fragment should be send
	bool AddReliableOutStream(DataContainer&& con)
	{
		// main thread
		return sOut.AddData(std::move(con));
	}

	void AddReliableOut(DataContainer&& con)
	{
		// main thread
		RelStruct s;
		s.id = *(PackID*)con[1];
		s.con = std::move(con);
		s.lastSend = Timer::GetTimestamp();

		// add to list
		muData.Lock();
		outReliable.push_back(std::move(s));
		muData.Unlock();
	}

	bool GetReliableOutStream(DataContainer* dst)
	{
		// main thread
		return sOut.GetCurStream(dst);
	}
	float GetReliableStreamWaitTime() const
	{
		// main thread
		return sOut.GetTimeSinceFirst();
	}


	// resending reliable
	void StartResending()
	{
		muData.Lock(); // nothing may be deleted or inserted during this time
		curReliable = outReliable.begin();
		curCount = 1;
	}
	// returns a nullptr if nothing should be send
	// if the function returns a valid pointer, UnlockReliable has to be called after the data was send!
	DataContainer* HasResend()
	{
		// thread main
		float maxTime = ping.avgPing > 0.0f ?
			ping.avgPing * 1.2f :
			100.0f;
		maxTime = std::max(5.0f, maxTime); // or else we will be stuck in an endless loop...

		while (curReliable != outReliable.end() && curCount <= 10)
		{
			if (timer.ConvertMilli(curReliable->lastSend) > maxTime)
			{
				// send Again
				curReliable->lastSend = Timer::GetTimestamp();
				return &(curReliable++)->con;
			}

			++curReliable;
			++curCount;
		}

		muData.Unlock();
		return nullptr;
	}
	// in
	void ProcessReliableAnswer(PackID id)
	{
		// thread listen
		ResetTimeout();
		muData.Lock();
		for (std::list<RelStruct>::iterator i = outReliable.begin(),
			end = outReliable.end(); i != end; i++)
		{
			if (i->id == id)
			{
				outReliable.erase(i);
				muData.Unlock();
				return;
			}
		}
		muData.Unlock();
	}

	// timeout
	void ResetTimeout()
	{
		dataRecv = true;
	}
	// call this from main thread
	bool IsTimedOut()
	{
		if (dataRecv)
		{
			dataRecv = false;
			timer.StartWatch();
		}
		else
		{
			if (timer.GetTimeSecond() > 10.0f)
				return true;
		}
		return false;
	}
	unsigned int GetOutReliableCount() const
	{
		return outReliable.size();
	}

	void ResetMaxPackSize(size_t size)
	{
		sOut.ResetMax(size);
	}
private:
	void _init()
	{
		ping.avgPing = 0.0f;
		ping.lastSend = 0;
		ping.newPing = 0.0f;

		timer.StartWatch();
	}
private:
	IPaddress sockaddr;
	unsigned int playerID;

	PackID lastUnreliableID = 0; // incomming
	PackID unreliableOutgoing = 1; // outgoing
	PingStruct ping;

	// reliable in
	PackID reliableAwaiting = 1; // incomming
	std::map<PackID, DataContainer> inTempStorage;

	// reliable out
	PackID reliableOutgoing = 1; // outgoing
	std::list<RelStruct> outReliable;
	Mutex muData;
	std::list<RelStruct>::iterator curReliable;
	unsigned int curCount;

	// timeout
	Timer timer;
	bool dataRecv = false;


	// datastream

	NetStreamIn sIn;
	NetStreamOut sOut;

public:
	static const size_t REL_HEADER = sizeof(char) + sizeof(PackID) + sizeof(Timestamp);
};
