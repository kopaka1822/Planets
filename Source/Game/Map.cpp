#include "Map.h"

Map::Map(int nPlayers, int nPlans, float width, float height, GameType ty, const std::vector< byte >& pclans)
	:
	nPlayers(nPlayers),
	nPlans(nPlans),
	mWidth(width),
	mHeight(height),
	grid(int(width / 30.0f), int(height / 30.0f), 30, 20, nPlayers, *this),
	gameType(ty)
{
	ents.assign(nPlayers, LinkedIDList<MapEntity>());

	// clans
	std::vector< ClanInfo > vec;
	vec.assign(nPlayers, ClanInfo::noAlly);

	clanInfo.assign(nPlayers, vec);
	for (int i = 0; i < nPlayers; i++)
	{
		// team 1 is ally with himself
		clanInfo[i][i] = ClanInfo::Ally;
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
					clanInfo[curP][i] = ClanInfo::Ally;
			}
		}
	}
}

Map::~Map()
{
	for (auto& p : plans)
		tool::safeDelete(p);
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// USER INPUT /////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Map::Select(PointF center, float r2, byte team)
{
	//check if anything was selected in the first place...
	if (r2 < 6400.0f) //80 px squared
	{
		for (auto& e : ents[team - 1])
		{
			float er2 = (e.GetPos() - center).lengthSq();

			if (er2 <= r2)
			{
				return true;
			}
		}
		//Planet Selected?
		for (auto& p : plans)
		{
			if (p->GetTeam() == team)
			{
				float pr2 = (p->GetPos() - center).lengthSq();

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

void Map::SelectAll(byte team)
{
	for (auto& e : ents[team - 1])
	{
		e.forceSelect();
	}
	//Planet Selected?
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
			p->forceSelect();
	}
}

void Map::AddToGroup(byte team, int group)
{
	for (auto& e : ents[team - 1])
	{
		if (e.selected())
			e.groupAssign(group);
	}

	for (auto& p : plans)
	{
		if (p->selected())
			p->groupAssign(group);
	}
}

void Map::MakeGroup(byte team, int group)
{
	for (auto& e : ents[team - 1])
	{
		if (e.selected())
			e.groupAssign(group);
		else
			e.groupDestroy(group);
	}

	for (auto& p : plans)
	{
		if (p->selected())
			p->groupAssign(group);
		else
			p->groupDestroy(group);
	}
}

void Map::SelectGroup(byte team, int group)
{
	for (auto& e : ents[team - 1])
	{
		e.groupSelect(group);
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
			p->groupSelect(group);
	}
}

void Map::DeleteGroup(byte team, int group)
{
	for (auto& e : ents[team - 1])
	{
		e.groupDestroy(group);
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
			p->groupDestroy(group);
	}
}

void Map::ClickRight(byte team)
{
	for (auto& e : ents[team - 1])
	{
		e.deselect();
	}
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
			p->deselect();
	}
}

void Map::SetAllPlanetsOnDefense(byte team)
{
	for (auto& p : plans)
	{
		if (p->GetTeam() == team)
		{
			PointF tar;
			tar.x = float(p->GetID());
			p->setTarget(tar, MapObject::tgPlanetDefend);
			p->SetEntityType(MapObject::entityType::etNormal);
		}
	}
}

void Map::SetPlanetSpawnType(byte team, MapObject::entityType t)
{
	assert(t != MapObject::entityType::etNone);

	for (auto& p : plans)
	{
		if (p->GetTeam() == team && p->selected())
		{
			p->SetEntityType(t);
		}
	}
}

////////////////////////////////////////////////////////////////////////////////
/////////////////////////// COLLISION AND PATHFINDING //////////////////////////
////////////////////////////////////////////////////////////////////////////////

bool Map::borderCol(const PointF& p) const
{
	const float b = 5.0f; //border
	if (p.x < b || p.x > mWidth - b || p.y < b || p.y > mHeight - b)
		return true;
	return false;
}

const MapPlanet* Map::GetColPlan(const PointF& pt) const
{
	for (const auto& p : plans)
	{
		if (p->isColliding(pt))
			return p;
	}
	return nullptr;
}

void Map::RefreshGrid()
{
	grid.clear();
	// sort entities
	for (size_t i = 0; i < nPlayers; i++)
	{
		for (auto& e : ents[i])
		{
			grid.AddEntity(&e, e.GetPos());
		}
	}
}

const MapEntity* Map::GetColEnt(PointF pt, unsigned int id, byte team)
{
	auto& list = grid.GetEntities(pt);
	for (auto& e : list)
	{
		if (e->isColliding(pt))
		{
			if (e->GetID() == id && e->GetTeam() == team)
				continue;

			return e;
		}
	}
	return nullptr;
}

void Map::SetCrowdEntVel(float ds, MapEntity& curEnt, const unsigned int ID)
{
	ds *= curEnt.GetSpeedModifier();
	VectorF vV = curEnt.GetVel();

	if (vV == PointF{ 0.0f, 0.0f })
		return;

	const VectorF& vP = curEnt.GetPos();
	const byte team = curEnt.GetTeam();

	FastVector< MapEntity* > nearby;
	FastVector< MapEntity* >& gridlist = grid.GetEntities(vP);

	MapEntity** ppe = GetNextFOVEnt(gridlist.begin(), gridlist.end(), team, vP, vV, &curEnt);

	while (ppe != gridlist.end())
	{
		nearby.push_back(*ppe);
		ppe = GetNextFOVEnt(++ppe, gridlist.end(), team, vP, vV, &curEnt);
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
				PointF clPoint = route.closestPoint(en->GetPos());

				if (en->isColliding(clPoint))
				{
					if (route.whichSide(en->GetPos()) == LineF::right)
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
				if (route.vectorDir(en->GetPos(), en->GetVel()) == LineF::towardLine)
				{
					en->keepEvading();

					if (route.whichSide(en->GetPos()) == LineF::right)
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
				VectorF eV = en->GetVel();
				float result = (route.vecNorm() + eV.normalize()).lengthSq();

				if (result > 3.6f)//3.414f)
				{
					//same direction
					VectorF vPE = en->GetPos() - vP;
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
					const PointF eP = en->GetPos();

					std::function<void()> toClose = [&vP, ds, &eP, route, &en, &vV]()
					{
						VectorF vPE = (eP - vP).normalize() * ds;
						if (route.whichSide(eP) == LineF::left)
						{
							en->SetVel(vPE.CW90());
							vV = vPE.CCW90();
						}
						else
						{
							en->SetVel(vPE.CCW90());
							vV = vPE.CW90();
						}
					};


					if (route.vectorDir(eP, en->GetVel()) == LineF::awayFromLine)
					{
						continue;
					}


					if (vV * en->GetVel() < 0)
					{
						VectorF vPE = (eP - vP).normalize() * ds;
						if (vPE.whichSideVec(vV) == PointF::right)
						{
							vV = vPE.CW90();
							en->SetVel((-vPE).CW90());
						}
						else
						{
							vV = vPE.CCW90();
							en->SetVel((-vPE).CCW90());
						}

					}
					else
					{
						//same direction
						if ((vP - eP).lengthSq() > 2 * MapEntity::MIN_DIST2)
						{
							if (route.whichSide(eP) == LineF::left)
							{
								vV = (en->GetPos() - vP).CCW45().normalize() * ds;
							}
							else
							{
								vV = (en->GetPos() - vP).CW45().normalize() * ds;
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
				if ((vP - en->GetPos()).lengthSq() < 2 * MapEntity::MIN_DIST2)
				{
					PointF clPoint = route.closestPoint(en->GetPos());

					if (en->isColliding(clPoint))
					{
						en->setEvade(curEnt.GetTarget() * ds); //evade in same direction
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

	curEnt.SetVel(vV);
}

PointF Map::GetEntDefend(PlanetID pID)
{
	assert(pID >= 0);
	assert((unsigned)pID < nPlans);
	PointF& result = plansDefend[pID];

	if (result.x == 0.0f)
	{
		//not searched ->search next ent
		const int r = (int)plans[pID]->GetRadius() + 50;
		const int r2 = r * r;

		const byte team = plans[pID]->GetTeam();
		assert(team != 0);

		const PointF& pos = plans[pID]->GetPos();

		for (size_t i = 0; i < nPlayers; ++i)
		{
			if (isAlly(i + 1, team))
				continue;

			for (const auto& e : ents[i])
			{
				if ((e.GetPos() - pos).lengthSq() < r2)
				{
					result = e.GetPos();
					return result;
				}
			}
		}

		//nothing found -> return to planet
		result = plans[pID]->GetPos();
		return result;
	}
	else
	{
		return result;
	}
}

void Map::SetTargetAvoidingPlanets(const PointF& vP, PointF& vT)
{
	std::vector< std::pair< float, const MapPlanet*> > colPlans;

	const LineFixedF route = LineFixedF(vP, vT);

	//collect planets with intersection
	for (const auto& p : plans)
	{
		float scalar = route.intersectCircle(p->GetPos(), p->GetRadius());
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
	if (vT == plan->GetPos())
	{
		//target reached
		return;
	}
	VectorF vOP = vP - plan->GetPos(); //vector from planet midpoint to vP
	float angle = acosf(plan->GetRadius() / vOP.length());
	vOP = vOP.normalize() * plan->GetRadius();

	if (route.whichSide(plan->GetPos()) == _Line<float>::side::left)
	{
		//dodge right
		vOP = vOP.rot(angle);
	}
	else
	{
		vOP = vOP.rot(-angle);
	}

	//set new target
	vT = vOP + plan->GetPos();
}

void Map::SetPrimaryEntVel(float ds, MapEntity& curEnt)
{
	ds *= curEnt.GetSpeedModifier();
	switch (curEnt.GetState())
	{
	case MapEntity::sTarget:
	{
		PointF vT; //target
		const PointF& vP = curEnt.GetPos();

		if (curEnt.GetTargetType() == MapObject::tgPlanetDefend)
		{
			const PlanetID pID = (PlanetID)curEnt.GetTarget().x;
			//check if planet belongs to the team
			if (isAlly(plans[pID]->GetTeam(), curEnt.GetTeam()))
			{
				vT = Map::GetEntDefend(pID);
				if (vT == plans[pID]->GetPos())
				{
					//spin around planet if near enough
					if (plans[pID]->isInDefend(vP))
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
			vT = curEnt.GetTarget();
		}


		//see if point and target are already close enough
		if (curEnt.GetTargetType() == MapEntity::tgPlanet)
		{
			PlanetID pID = (PlanetID)vT.x;
			vT = plans[pID]->GetPos();

			if (plans[pID]->isNearby(vP) && curEnt.GetMood())
			{
				vT = vP + (vT - vP).CW90();
			}
		}
		else if (curEnt.GetTargetType() == MapEntity::tgPoint)
		{
			if (vP.isClose(vT, 25))
			{
				curEnt.TargetPointReached();
			}
		}

		SetTargetAvoidingPlanets(vP, vT);

		PointF vVnorm = (vT - vP).normalize();
		PointF vV = vVnorm * ds;

		{//Colliding with planet?
			const MapPlanet* obj = GetColPlan(vP + vV);
			if (obj != nullptr)
			{//Colliding with planet!
				if (obj->GetPos() == vT)
				{
					//this is the target planet
					if (isAlly(curEnt.GetTeam(), obj->GetTeam()))
					{
						//Entity reached his goal
						if (curEnt.GetTargetType() != MapObject::tgPlanetDefend)
						{
							vT.x = (float)obj->GetID();
							curEnt.setTarget(vT, MapObject::tgPlanetDefend);
							curEnt.SetVel({ 0.0f, 0.0f });
							return;
						}
						return;
					}
					else
					{
						//Entity attacks planet
						curEnt.DisableVelCorrect();
						PointF vPOnorm = (obj->GetPos() - vP).normalize();
						if (curEnt.GetMood())
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
					PointF vPOnorm = (obj->GetPos() - vP).normalize();
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

		curEnt.SetVel(vV);
	}
	break;

	case MapEntity::sEvade:
		curEnt.SetVel(curEnt.GetTarget() * ds);
		break;

	default: //dont move
		curEnt.ZeroVelocity();
		break;
	}
}
bool Map::col(const PointF& pt, unsigned int id, byte team)
{
	if (GetColPlan(pt) != nullptr)
		return true;

	if (GetColEnt(pt, id, team) != nullptr)
		return true;

	return borderCol(pt);
}

void Map::SetEntPosition(MapEntity& curEnt, const float dt)
{
	const PointF& pos = curEnt.GetPos();
	const VectorF& vel = curEnt.GetVel();

	if (!col(pos + vel, curEnt.GetID(), curEnt.GetTeam()))
	{
		curEnt.UpdatePosition(dt);
		return;
	}
	else
	{
		curEnt.ChangeMood();
		if (curEnt.GetMood())
		{
			if (!col(pos + vel.CW90(), curEnt.GetID(), curEnt.GetTeam()))
			{
				curEnt.SetVel(vel.CW90());
				curEnt.UpdatePosition(dt);
				return;
			}
			return;
		}
		else
		{
			if (!col(pos + vel.CCW90(), curEnt.GetID(), curEnt.GetTeam()))
			{
				curEnt.SetVel(vel.CCW90());
				curEnt.UpdatePosition(dt);
				return;
			}

			return;
		}
		return;
	}
}

bool Map::isAlly(byte team1, byte team2) const
{
	if (byte(team1 - 1) >= nPlayers || byte(team2 - 1) >= nPlayers)
		//if (team1 == 0 || team2 == 0 || team1 > nPlayers || team2 > nPlayers)
		return false;

	if (clanInfo[team1 - 1][team2 - 1] == ClanInfo::Ally)
		return true;
	else
		return false;
}