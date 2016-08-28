#include "Map.h"
#include "../Utility/Line.h"
#include "../Utility/Exception.h"

Map::Map(int nPlayers, int nPlans, float width, float height, GameType ty, const std::vector< TeamID >& pclans)
	:
	m_nPlayers(nPlayers),
	m_nPlans(nPlans),
	m_mWidth(width),
	m_mHeight(height),
	m_grid(int(width / 30.0f), int(height / 30.0f), 30, 20, nPlayers, *this),
	m_gameType(ty)
{
	m_ents.assign(nPlayers, LinkedIDList<MapEntity>());

	// clans
	std::vector< ClanInfo > vec;
	vec.assign(nPlayers, ClanInfo::noAlly);

	m_clanInfo.assign(nPlayers, vec);
	for (int i = 0; i < nPlayers; i++)
	{
		// m_team 1 is ally with himself
		m_clanInfo[i][i] = ClanInfo::Ally;
	}

	if (ty == GameType::Allicance)
	{
		// take map info
		if (pclans.size() < (size_t)nPlayers)
			throw Exception("map invalid parameters - clans.size() < nPlayer");

		for (int curP = 0; curP < nPlayers; curP++)
		{
			byte curClan = pclans[curP];

			for (int i = 0; i < nPlayers; i++)
			{
				if (pclans[i] == curClan)
					m_clanInfo[curP][i] = ClanInfo::Ally;
			}
		}
	}
}

Map::~Map()
{
	for (auto& p : m_plans)
		tool::safeDelete(p);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// USER INPUT /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Map::select(PointF center, float r2, TeamID team)
{
	//check if anything was selected in the first place...
	if (r2 < 6400.0f) //80 px squared
	{
		for (auto& e : m_ents[team - 1])
		{
			float er2 = (e.getPos() - center).lengthSq();

			if (er2 <= r2)
			{
				return true;
			}
		}
		//Planet Selected?
		for (auto& p : m_plans)
		{
			if (p->getTeam() == team)
			{
				float pr2 = (p->getPos() - center).lengthSq();

				if (pr2 <= r2)
				{
					return true;
				}
			}
		}
		return false;
	}
	return true;
}

void Map::selectAll(TeamID team)
{
	for (auto& e : m_ents[team - 1])
	{
		e.forceSelect();
	}
	//Planet Selected?
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
			p->forceSelect();
	}
}

void Map::addToGroup(TeamID team, GroupID group)
{
	for (auto& e : m_ents[team - 1])
	{
		if (e.selected())
			e.groupAssign(group);
	}

	for (auto& p : m_plans)
	{
		if (p->selected())
			p->groupAssign(group);
	}
}

void Map::makeGroup(TeamID team, GroupID group)
{
	for (auto& e : m_ents[team - 1])
	{
		if (e.selected())
			e.groupAssign(group);
		else
			e.groupDestroy(group);
	}

	for (auto& p : m_plans)
	{
		if (p->selected())
			p->groupAssign(group);
		else
			p->groupDestroy(group);
	}
}

void Map::selectGroup(TeamID team, GroupID group)
{
	for (auto& e : m_ents[team - 1])
	{
		e.groupSelect(group);
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
			p->groupSelect(group);
	}
}

void Map::deleteGroup(TeamID team, GroupID group)
{
	for (auto& e : m_ents[team - 1])
	{
		e.groupDestroy(group);
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
			p->groupDestroy(group);
	}
}

void Map::deselectTarget(TeamID team)
{
	for (auto& e : m_ents[team - 1])
	{
		e.deselect();
	}
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
			p->deselect();
	}
}

void Map::setAllPlanetsOnDefense(TeamID team)
{
	for (auto& p : m_plans)
	{
		if (p->getTeam() == team)
		{
			PointF tar;
			tar.x = float(p->getID());
			p->setTarget(tar, MapObject::tgPlanetDefend);
			p->setEntityType(MapObject::EntityType::etNormal);
		}
	}
}

void Map::setPlanetSpawnType(TeamID team, MapObject::EntityType t)
{
	assert(t != MapObject::EntityType::etNone);

	for (auto& p : m_plans)
	{
		if (p->getTeam() == team && p->selected())
		{
			p->setEntityType(t);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// COLLISION AND PATHFINDING //////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Map::borderCol(const PointF& p) const
{
	const float b = 5.0f; //border
	if (p.x < b || p.x > m_mWidth - b || p.y < b || p.y > m_mHeight - b)
		return true;
	return false;
}

const MapPlanet* Map::getColPlan(const PointF& pt) const
{
	for (const auto& p : m_plans)
	{
		if (p->isColliding(pt))
			return p;
	}
	return nullptr;
}

void Map::refreshGrid()
{
	m_grid.clear();
	// sort entities
	for (size_t i = 0; i < m_nPlayers; i++)
	{
		for (auto& e : m_ents[i])
		{
			m_grid.addEntity(&e, e.getPos());
		}
	}
}

const MapEntity* Map::getColEnt(PointF pt, GameID id, TeamID team)
{
	auto& list = m_grid.getEntities(pt);
	for (auto& e : list)
	{
		if (e->isColliding(pt))
		{
			if (e->getID() == id && e->getTeam() == team)
				continue;

			return e;
		}
	}
	return nullptr;
}

void Map::setCrowdEntVel(float ds, MapEntity& curEnt, GameID ID)
{
	ds *= curEnt.getSpeedModifier();
	VectorF vV = curEnt.getVel();

	if (vV == PointF{ 0.0f, 0.0f })
		return;

	const VectorF& vP = curEnt.getPos();
	const byte team = curEnt.getTeam();

	FastVector< MapEntity* > nearby;
	FastVector< MapEntity* >& gridlist = m_grid.getEntities(vP);

	MapEntity** ppe = getNextFOVEnt(gridlist.begin(), gridlist.end(), team, vP, vV, &curEnt);

	while (ppe != gridlist.end())
	{
		nearby.push_back(*ppe);
		ppe = getNextFOVEnt(++ppe, gridlist.end(), team, vP, vV, &curEnt);
	}

	if (nearby.length() == 0)
		return; //no nearby entities



	switch (curEnt.GetState())
	{
	case MapEntity::sTarget:
		for (auto& en : nearby)
		{
			const LineFixedOneF route = LineFixedOneF(vP, vP + vV);
			switch (en->GetState())
			{
			case MapEntity::sLazy:
			{
				//would they collide?
				PointF clPoint = route.closestPoint(en->getPos());

				if (en->isColliding(clPoint))
				{
					if (route.whichSide(en->getPos()) == LineF::right)
					{
						en->setEvade(vV.CW90()); //evade in this direction
					}
					else
					{
						en->setEvade(vV.CCW90());
					}
				}
			}
			break;

			case MapEntity::sEvade:
				if (route.vectorDir(en->getPos(), en->getVel()) == LineF::towardLine)
				{
					en->keepEvading();

					if (route.whichSide(en->getPos()) == LineF::right)
					{
						vV = vV.CCW45();
					}
					else
					{
						vV = vV.CW45();
					}

				}
				break;
			case MapEntity::sTarget:
			{
				//which direction?
				VectorF eV = en->getVel();
				float result = (route.vecNorm() + eV.normalize()).lengthSq();

				if (result > 3.6f)//3.414f)
				{
					//same direction
					VectorF vPE = en->getPos() - vP;
					if (vPE.whichSideVec(vV) == PointF::left)
					{
						vV = vV.CCW45();
					}
					else
					{
						vV = vV.CW45();
					}
				}
				else
				{
					//~perpendicular
					const PointF eP = en->getPos();

					std::function<void()> toClose = [&vP, ds, &eP, route, &en, &vV]()
					{
						VectorF vPE = (eP - vP).normalize() * ds;
						if (route.whichSide(eP) == LineF::left)
						{
							en->setVel(vPE.CW90());
							vV = vPE.CCW90();
						}
						else
						{
							en->setVel(vPE.CCW90());
							vV = vPE.CW90();
						}
					};


					if (route.vectorDir(eP, en->getVel()) == LineF::awayFromLine)
					{
						continue;
					}


					if (vV * en->getVel() < 0)
					{
						VectorF vPE = (eP - vP).normalize() * ds;
						if (vPE.whichSideVec(vV) == PointF::right)
						{
							vV = vPE.CW90();
							en->setVel((-vPE).CW90());
						}
						else
						{
							vV = vPE.CCW90();
							en->setVel((-vPE).CCW90());
						}

					}
					else
					{
						//same direction
						if ((vP - eP).lengthSq() > 2 * MapEntity::MIN_DIST2)
						{
							if (route.whichSide(eP) == LineF::left)
							{
								vV = (en->getPos() - vP).CCW45().normalize() * ds;
							}
							else
							{
								vV = (en->getPos() - vP).CW45().normalize() * ds;
							}
						}
						else
						{
							//to close
							toClose();

						}

					}
				}
			}
			break;
			default:
				break;
			}
		}
		break;

	case MapEntity::sEvade:
		for (auto& en : nearby)
		{
			const LineFixedOneF route = LineFixedOneF(vP, vP + vV);
			switch (en->GetState())
			{
			case MapEntity::sLazy:
			{	//would they collide?
				if ((vP - en->getPos()).lengthSq() < 2 * MapEntity::MIN_DIST2)
				{
					PointF clPoint = route.closestPoint(en->getPos());

					if (en->isColliding(clPoint))
					{
						en->setEvade(curEnt.getTarget() * ds); //evade in same direction
					}
				}

			}
			break;
			default:
				break;
			}
		}
		break;
	}

	curEnt.setVel(vV);
}

PointF Map::getEntDefend(PlanetID pID)
{
	assert(pID >= 0);
	assert((unsigned)pID < m_nPlans);
	PointF& result = m_plansDefend[pID];

	if (result.x == 0.0f)
	{
		//not searched ->search next ent
		const int r = (int)m_plans[pID]->getRadius() + 50;
		const int r2 = r * r;

		const byte team = m_plans[pID]->getTeam();
		assert(team != 0);

		const PointF& pos = m_plans[pID]->getPos();

		for (size_t i = 0; i < m_nPlayers; ++i)
		{
			if (isAlly(i + 1, team))
				continue;

			for (const auto& e : m_ents[i])
			{
				if ((e.getPos() - pos).lengthSq() < r2)
				{
					result = e.getPos();
					return result;
				}
			}
		}

		//nothing found -> return to planet
		result = m_plans[pID]->getPos();
		return result;
	}
	else
	{
		return result;
	}
}

void Map::setTargetAvoidingPlanets(const PointF& vP, PointF& vT)
{
	std::vector< std::pair< float, const MapPlanet*> > colPlans;

	const LineFixedF route = LineFixedF(vP, vT);

	//collect planets with intersection
	for (const auto& p : m_plans)
	{
		float scalar = route.intersectCircle(p->getPos(), p->getRadius());
		if (scalar > 0.0f)
		{
			//intersection
			colPlans.push_back(std::make_pair(scalar, p));
		}
	}

	if (colPlans.size() == 0)
		return;

	const MapPlanet* plan = colPlans[0].second;

	if (colPlans.size() > 1)
	{
		//find planet with closest scalar
		float scalar = colPlans[0].first;

		for (const auto& e : colPlans)
		{
			if (e.first < scalar)
			{
				scalar = e.first;
				plan = e.second;
			}
		}
	}

	//determine new target
	//left or right dodging?
	if (vT == plan->getPos())
	{
		//target reached
		return;
	}
	VectorF vOP = vP - plan->getPos(); //vector from planet midpoint to vP
	float angle = acosf(plan->getRadius() / vOP.length());
	vOP = vOP.normalize() * plan->getRadius();

	if (route.whichSide(plan->getPos()) == _Line<float>::side::left)
	{
		//dodge right
		vOP = vOP.rot(angle);
	}
	else
	{
		vOP = vOP.rot(-angle);
	}

	//set new target
	vT = vOP + plan->getPos();
}

void Map::setPrimaryEntVel(float ds, MapEntity& curEnt)
{
	ds *= curEnt.getSpeedModifier();
	switch (curEnt.GetState())
	{
	case MapEntity::sTarget:
	{
		PointF vT; //target
		const PointF& vP = curEnt.getPos();

		if (curEnt.getTargetType() == MapObject::tgPlanetDefend)
		{
			const PlanetID pID = (PlanetID)curEnt.getTarget().x;
			//check if planet belongs to the m_team
			if (isAlly(m_plans[pID]->getTeam(), curEnt.getTeam()))
			{
				vT = Map::getEntDefend(pID);
				if (vT == m_plans[pID]->getPos())
				{
					//spin around planet if near enough
					if (m_plans[pID]->isInDefend(vP))
					{
						vT = vP + (vT - vP).CW90();
					}
				}
			}
			else
			{
				//attack planet if you dont own it anymore
				//curEnt.setTarget(plans[pID]->GetPos(), MapObject::tgPlanet);
				vT.x = (float)pID;
				curEnt.setTarget(vT, MapObject::tgPlanet);
			}

		}
		else //if targetType != PlanetDefend
		{
			vT = curEnt.getTarget();
		}


		//see if point and target are already close enough
		if (curEnt.getTargetType() == MapEntity::tgPlanet)
		{
			PlanetID pID = (PlanetID)vT.x;
			vT = m_plans[pID]->getPos();

			if (m_plans[pID]->isNearby(vP) && curEnt.getMood())
			{
				vT = vP + (vT - vP).CW90();
			}
		}
		else if (curEnt.getTargetType() == MapEntity::tgPoint)
		{
			if (vP.isClose(vT, 25))
			{
				curEnt.targetPointReached();
			}
		}

		setTargetAvoidingPlanets(vP, vT);

		PointF vVnorm = (vT - vP).normalize();
		PointF vV = vVnorm * ds;

		{//Colliding with planet?
			const MapPlanet* obj = getColPlan(vP + vV);
			if (obj != nullptr)
			{//Colliding with planet!
				if (obj->getPos() == vT)
				{
					//this is the target planet
					if (isAlly(curEnt.getTeam(), obj->getTeam()))
					{
						//Entity reached his goal
						if (curEnt.getTargetType() != MapObject::tgPlanetDefend)
						{
							vT.x = (float)obj->getID();
							curEnt.setTarget(vT, MapObject::tgPlanetDefend);
							curEnt.setVel({ 0.0f, 0.0f });
							return;
						}
						return;
					}
					else
					{
						//Entity attacks planet
						curEnt.disableVelCorrect();
						PointF vPOnorm = (obj->getPos() - vP).normalize();
						if (curEnt.getMood())
						{
							//go right;
							vV = vPOnorm.CW90() * ds;
						}
						else
						{
							//go left
							vV = vPOnorm.CCW90() * ds;
						}
					}
				}
				else
				{
					//dodge left or right?
					PointF vPOnorm = (obj->getPos() - vP).normalize();
					if (vVnorm.cross(vPOnorm) > 0)
					{
						//go right;
						vV = vPOnorm.CW90() * ds;
					}
					else
					{
						//go left
						vV = vPOnorm.CCW90() * ds;
					}
				}
			}
		}//Planet case closed

		curEnt.setVel(vV);
	}
	break;

	case MapEntity::sEvade:
		curEnt.setVel(curEnt.getTarget() * ds);
		break;

	default: //dont move
		curEnt.zeroVelocity();
		break;
	}
}
bool Map::col(const PointF& pt, GameID id, TeamID team)
{
	if (getColPlan(pt) != nullptr)
		return true;

	if (getColEnt(pt, id, team) != nullptr)
		return true;

	return borderCol(pt);
}

void Map::setEntPosition(MapEntity& curEnt, const float dt)
{
	const PointF& pos = curEnt.getPos();
	const VectorF& vel = curEnt.getVel();

	if (!col(pos + vel, curEnt.getID(), curEnt.getTeam()))
	{
		curEnt.updatePosition(dt);
		return;
	}
	else
	{
		curEnt.changeMood();
		if (curEnt.getMood())
		{
			if (!col(pos + vel.CW90(), curEnt.getID(), curEnt.getTeam()))
			{
				curEnt.setVel(vel.CW90());
				curEnt.updatePosition(dt);
				return;
			}
			return;
		}
		else
		{
			if (!col(pos + vel.CCW90(), curEnt.getID(), curEnt.getTeam()))
			{
				curEnt.setVel(vel.CCW90());
				curEnt.updatePosition(dt);
				return;
			}

			return;
		}
		return;
	}
}

bool Map::isAlly(TeamID team1, TeamID team2) const
{
	if (byte(team1 - 1) >= m_nPlayers || byte(team2 - 1) >= m_nPlayers)
		//if (team1 == 0 || team2 == 0 || team1 > nPlayers || team2 > nPlayers)
		return false;

	if (m_clanInfo[team1 - 1][team2 - 1] == ClanInfo::Ally)
		return true;
	else
		return false;
}