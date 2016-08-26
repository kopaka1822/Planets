#pragma once
#include "../Utility/Rect.h"
#include "../Utility/LinkedIDList.h"
#include "MapPlanet.h"
#include "MapEntity.h"
#include <functional>
#include "../Utility/Line.h"
#include "../Utility/Mutex.h"
#include <map>
#include "../Utility/Thread.h"
#include "UniformGrid.h"
#include "../Utility/Exception.h"
#include "../Utility/DataContainer.h"

using byte = unsigned char;

using PlanetID = short;
using EntIterator = LinkedIDList<MapEntity>::element;

class Map
{
	friend class PlayerAI;
	friend class PlanetTask;
	friend class MapDrawer;
	friend class MiniMap;
	friend class WeakAI;
public:
	enum class ClanInfo
	{
		Ally,
		noAlly,

		// server
		sendRequest, // the other one send a request
		awaiting // waiting for the answer of the other one
	};
	enum class GameType
	{
		AllvAll,
		UnholyAlliance, // you can form a m_team with one person
		Allicance, // teams specified in map
		NONE = 0xFF
	};
public:
	Map(int nPlayers, int nPlans, float width, float height, GameType ty, const std::vector< byte >& pclans);
	virtual ~Map();

	virtual bool Select(PointF center, float r2, byte team);
	virtual void SelectAll(byte team);
	//tries to add units to an existing m_group
	virtual void AddToGroup(byte team, int group);
	//tries to make a new m_group with selected units
	virtual void MakeGroup(byte team, int group);
	virtual void SelectGroup(byte team, int group);
	virtual void DeleteGroup(byte team, int group);
	// returns true if planet was targetet
	virtual bool Click(PointF pt, byte team) = 0;
	virtual void ClickRight(byte team);
	virtual void Update(float dt) = 0;
	virtual void SetAllPlanetsOnDefense(byte team);
	// returns true if anything was selected
	virtual bool FilterEntityType(byte team, MapObject::EntityType)
	{
		for (const auto& e : ents[team - 1])
		{
			if (e.selected())
			{
				return true;
			}
		}

		for (const auto& p : plans)
		{
			if (p->getTeam() == team)
			{
				if (p->selected())
				{
					return true;
				}
			}
		}

		return false;
	}
	virtual void SelectAllEntityType(byte team, MapObject::EntityType et)
	{
		for (auto& e : ents[team - 1])
		{
			if (e.getEntityType() == et)
			{
				e.forceSelect();
			}
			else
			{
				e.deselect();
			}
		}

		for (auto& p : plans)
		{
			if (p->getTeam() == team)
			{
				if (p->getEntityType() == et)
					p->forceSelect();
				else
					p->deselect();
			}
		}
	}
	virtual byte GameEnd() const = 0;//returns winner m_team
	virtual bool GameStart() const
	{
		return true;
	}
	float GetWidth() const
	{
		return mWidth;
	}
	float GetHeight() const
	{
		return mHeight;
	}
	unsigned int GetNPlans() const
	{
		return plans.size();
	}
	unsigned int CountTeamPlans(byte team) const
	{
		int n = 0;
		for (const auto& p : plans)
		{
			if (p->getTeam() == team)
				++n;
		}
		return n;
	}
	virtual float GetGameTime() const
	{
		return 0.0f; // for remote map
	}
	byte GetMaxPlayer() const
	{
		return nPlayers;
	}
	virtual void SetPlanetSpawnType(byte team, MapObject::EntityType t);
	// multiplayer
	virtual void AddPacket(DataContainer&&){}
	int CountAllyPlanets(byte team)
	{
		if (unsigned(team - 1) >= nPlayers)
			return 0;

		int c = 0;
		for (const auto& p : plans)
		{
			if (isAlly(team, p->getTeam()))
				c++;
		}
		return c;
	}
protected:
	//this will be used in every child of map
	const size_t nPlayers;
	const size_t nPlans;
	const float mWidth; //map width + height
	const float mHeight;
	UniformGrid grid;
	const GameType gameType;

	std::vector< MapPlanet* > plans;
	std::vector< LinkedIDList<MapEntity> > ents;


	std::vector< PointF > plansDefend; //this will safe some processor time | this saves the position of attacking enemie
	Mutex muEnts; //protect the Ents!

	//clans (multiplayer)
	std::vector< std::vector< ClanInfo >> clanInfo;

protected:
	///////////////////////////////////////////////////
	////////////// Map Helper Funcs ///////////////////
	///////////////////////////////////////////////////

	//Collision
	bool borderCol(const PointF& p) const;
	const MapPlanet* GetColPlan(const PointF& pt) const;
	void RefreshGrid();
	const MapEntity* GetColEnt(PointF pt, unsigned int id, byte team);
	//Update
	void SetCrowdEntVel(float ds, MapEntity& curEnt, const unsigned int ID);

	//Defend
	PointF GetEntDefend(PlanetID pID);
	inline void ResetPlanDef()
	{
		plansDefend.assign(nPlans, PointF{ 0.0f, 0.0f });
	}
	void SetTargetAvoidingPlanets(const PointF& vP, PointF& vT);
	void SetPrimaryEntVel(float ds, MapEntity& curEnt);
	bool col(const PointF& pt, unsigned int id, byte team);

	void SetEntPosition(MapEntity& curEnt, const float dt);

public:
	//clan helper functions
	bool isAlly(byte team1, byte team2) const;

public:
	///////////////////////////////////////////////////
	////////////// EVENTS /////////////////////////////
	///////////////////////////////////////////////////
	class EventReciever
	{
	public:
		virtual void Event_EntitySpawn(PlanetID pID,MapEntity& e){};
		virtual void Event_EntityKilled(MapEntity& e){};
		virtual void Event_PlanetAttacked(PlanetID pID, const MapEntity& e){};

		// culprit may be null if it was not captured by an entity
		virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit){};
		virtual ~EventReciever(){}
	};

private:
	std::vector< EventReciever* > evntRecv;
public:
	void addEventReciever(EventReciever* recv)
	{
		evntRecv.push_back(recv);
	}

protected:

	virtual void Event_EntitySpawn(PlanetID pID, MapEntity& e)
	{
		for (auto& r : evntRecv)
		{
			r->Event_EntitySpawn(pID, e);
		}
	};
	virtual void Event_EntityKilled(MapEntity& e)
	{
		for (auto& r : evntRecv)
		{
			r->Event_EntityKilled(e);
		}
	};
	virtual void Event_PlanetAttacked(PlanetID pID, const MapEntity& e)
	{
		for (auto& r : evntRecv)
		{
			r->Event_PlanetAttacked(pID, e);
		}
	};
	virtual void Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit)
	{
		for (auto& r : evntRecv)
		{
			r->Event_PlanetCaptured(pID,newTeam,oldTeam, culprit);
		}
	};

public:
	///////////////////////////////////////////////////
	////////////// PLAYER AI //////////////////////////
	///////////////////////////////////////////////////
	inline MapPlanet* getPlanP(PlanetID pID)
	{
		if (pID < 0 || pID >= (PlanetID)plans.size())
			return nullptr;
		return plans[pID];
	}
	inline MapPlanet& getPlan(PlanetID pID)
	{
		assert(pID >= 0);
		assert((unsigned)pID < plans.size());
		return *plans[pID];
	}
	inline unsigned int countUnits(byte team) const
	{
		assert(team > 0 && team <= nPlayers);
		return ents[team - 1].length();
	}
	inline unsigned int countPlanets(byte team) const
	{
		unsigned int c = 0;
		for (const auto& p : plans)
		{
			if (p->getTeam() == team)
				c++;
		}
		return c;
	}
private:
	///////////////////////////////////////////////////
	////////////// collision //////////////////////////
	///////////////////////////////////////////////////
	inline MapEntity** GetNextFOVEnt(MapEntity** start, MapEntity** end,
		const byte& myTeam, const PointF& vP, const PointF& vV,const MapEntity* me)
	{
		while (start != end) //end is always the same
		{
			if (isAlly((*start)->getTeam(), myTeam))
			{
				const PointF dist = (*start)->getPos() - vP;

				if (dist.lengthSq() < (float)MapEntity::FOV2)
				{
					//inside fov, facing in same direction?
					if (vV * dist > 0)
					{
						//same direction
						return start;
					}
				}
			}
			++start;
		}
		return end;
	}

	class Updater : public Thread
	{
		class Insane : public Thread //because it is insane to start a threads inside a thread
		{
		public:
			Insane(EntIterator start, EntIterator end,
				std::function<void(EntIterator, EntIterator)>& func)
				:
				Thread(),
				start(start), end(end), func(func)
			{}
			virtual ~Insane()
			{}
		protected:
			virtual int ThreadProc() override
			{
				func(start, end);
				return 0;
			}
		private:
			EntIterator start;
			EntIterator end;
			std::function<void(EntIterator, EntIterator)>& func;
		};
	public:
		Updater(std::function<void(EntIterator, EntIterator)>& updFunc, LinkedIDList<MapEntity>& ents)
			:
			Thread(),
			updFunc(updFunc),
			ents(ents)
		{}
		virtual ~Updater()
		{}
		virtual int ThreadProc() override
		{
			if (ents.length() == 0)
				return 0;

			//start other threads lololol
			const unsigned int dx = 400; //each thread handles dx ents
			const unsigned int nThreads = (ents.length() - 1) / dx;

			EntIterator cur = ents.begin();

			Insane** ppi = nullptr;
			if (nThreads)
			{
				ppi = new Insane*[nThreads];
				for (unsigned int i = 0; i < nThreads; ++i)
				{
					EntIterator start = cur;
					for (unsigned int u = 0; u < dx; ++u) { ++cur; };

					ppi[i] = new Insane(start, cur, updFunc);
					ppi[i]->Begin();
				}
			}

			updFunc(cur, ents.end());

			if (nThreads)
			{
				for (unsigned int i = 0; i < nThreads; ++i)
				{
					ppi[i]->Join();
					delete ppi[i];
					ppi[i] = nullptr;
				}
				delete[] ppi;
			}

			return 0;
		}
	private:
		//const byte m_team;
		std::function<void(EntIterator, EntIterator)>& updFunc;
		LinkedIDList<MapEntity>& ents;
	};

protected:
	void UpdateEntsWithUpdater(std::function<void(EntIterator, EntIterator)>& func)
	{
		Updater** ppUpd = new Updater*[nPlayers - 1];

		//start & create Threads
		for (size_t index = 0; index < nPlayers - 1; ++index)
		{
			ppUpd[index] = new Updater(func, ents[index]);
			ppUpd[index]->Begin();
		}

		//Stop Threads
		//updFunc(nPlayers - 1); // run one proc on own thread
		Updater lastUpd(func, ents[nPlayers - 1]);
		lastUpd.ThreadProc();

		for (size_t index = 0; index < nPlayers - 1; ++index)
		{
			ppUpd[index]->Join();
			delete ppUpd[index];
			ppUpd[index] = nullptr;
		}
		delete[] ppUpd;
		ppUpd = nullptr;
	}
};
