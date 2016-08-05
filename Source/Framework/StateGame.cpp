#include "StateGame.h"

StateGame::StateGame(byte myTeam, byte maxTeams)
	:
	btnMiniOnOff(true),
	gs(Database::GetGameStruct()),
	sBox(gs.pMap->GetNPlans(), gs.pMap->CountTeamPlans(1), gs.myTeam),
	btnSA("A", FONT_MED),
	btnSN("D", FONT_MED),
	btnChat("C", FONT_MED),
	score(maxTeams),
	myTeam(myTeam),
	btnPlayer("P", FONT_MED),
	tipPause(btnPause, FONT_SMALL),
	tipSA(btnSA, FONT_SMALL),
	tipSN(btnSN, FONT_SMALL),
	tipChat(btnChat, FONT_SMALL),
	tipPlayer(btnPlayer, FONT_SMALL),
	tipMap(btnMiniOnOff, FONT_SMALL),
	tablist(Database::GetGameStruct().nTeams, FONT_SMALL, gs.ty != Map::GameType::UnholyAlliance),

	btnFilterEnt(Database::GetTexture(Database::Particle)),
	btnFilterBomb(Database::GetTexture(Database::ParticleExpl)),
	btnFilterSabo(Database::GetTexture(Database::ParticleSabo)),
	btnFilterSpeed(Database::GetTexture(Database::ParticleTank)),
	btnDefPlanets(Database::GetTexture(Database::Shield)),

	tipEnt(btnFilterEnt, FONT_SMALL),
	tipBomb(btnFilterBomb, FONT_SMALL),
	tipSpeed(btnFilterSpeed, FONT_SMALL),
	tipSabo(btnFilterSabo, FONT_SMALL),
	tipDefPlan(btnDefPlanets, FONT_SMALL)
{
	assert(gs.pMap != nullptr);

	tablist.AdjustToPlayers();

	byte curTeam = 1;
	for (auto& b : tablist.GetButtons())
	{
		AddObject(b);
		if (gs.pMap->isAlly(curTeam, myTeam))
			b->SetState(UIClanButton::state::ally);

		curTeam++;
	}

	tablist.SetVisible(false);



	gs.pMap->addEventReciever(&sBox);
	gs.pMap->addEventReciever(&score);

	pMapDraw = std::unique_ptr< MapDrawer >(new MapDrawer(*gs.pMap, myTeam));
	sBox.Init(pMapDraw.get());

	pMiniMap = std::unique_ptr< MiniMap >( new MiniMap(*gs.pMap, *pMapDraw) );

	msLast = Input::GetMouseXY();

	AddObject(pMiniMap.get());
	AddObject(&btnPause);
	AddObject(&btnMiniOnOff);
	AddObject(&btnPlayer);
	AddObject(&tablist);

	AddObject(&btnSA);
	AddObject(&btnSN);

	AddObject(&btnFilterEnt);
	AddObject(&btnFilterBomb);
	AddObject(&btnFilterSabo);
	AddObject(&btnFilterSpeed);
	AddObject(&btnDefPlanets);

	AddObject(&tipSA);
	AddObject(&tipSN);
	AddObject(&tipChat);
	AddObject(&tipPlayer);
	AddObject(&tipPause);
	AddObject(&tipMap);
	AddObject(&tipDefPlan);

	AddObject(&tipEnt);
	AddObject(&tipBomb);
	AddObject(&tipSabo);
	AddObject(&tipSpeed);

	tipSA.AddLine("Select All");
	tipSN.AddLine("Deselect All");
	tipPause.AddLine("Pause Menu");
	tipPlayer.AddLine("Playerlist");
	tipChat.AddLine("Disable/Enable Chat");
	tipMap.AddLine("Minimize/Maximize Minimap");

	tipEnt.AddLine("Filter by Entities");
	tipBomb.AddLine("Filter by Bomber");
	tipSpeed.AddLine("Filter by Speeder");
	tipSabo.AddLine("Filter by Saboteur");
	tipDefPlan.AddLine("Set all planets on defense");

	btnChat.Disable();

	pMapDraw->GetEntitySpawnSelect()->Register(*this);

	score.AddBranch(*gs.pMap); // stamp for time = 0
}

StateGame::~StateGame()
{
	Drawing::GetDraw().SetStarfieldColor(Color::White());
}

////////////////////////////////////////////////////////////////
 /////////////////////////// EVENTS /////////////////////////////
////////////////////////////////////////////////////////////////

void StateGame::Event_MouseDown(Input::MouseKey k, const PointF& pos)
{
	if (pMapDraw->GetRect().PointInside(pos) && !pMiniMap->GetRect().PointInside(pos))
	{
		if (k == Input::Middle)
		{
			bScolling = true;
			scollLast = pos;
		}
	}
}
void StateGame::Event_MouseUp(Input::MouseKey k, const PointF& pos)
{
	if (k == Input::Middle)
	{
		bScolling = false;
	}

}
void StateGame::Event_GameKeyDown(Input::GameKey k, const PointF& p)
{
	if (!bMapInput)
		return;

	switch (k)
	{
	case Input::GK_SELECTALL:
		gs.pMap->SelectAll(gs.myTeam);
		break;

	case Input::GK_SELECT:
		if (pMapDraw->GetRect().PointInside(p) && !pMiniMap->GetRect().PointInside(p))
		{
			bMouseDown = true;
			msDown = pMapDraw->toModel(p);
			msLast = msDown;
		}
		else
		{
			bMouseDown = false;
		}
		if (Settings::GetGamekey(Input::GK_SELECT) != Settings::GetGamekey(Input::GK_DESELECT))
			break;
	case Input::GK_DESELECT:
		if (pMapDraw->GetRect().PointInside(p) && !pMiniMap->GetRect().PointInside(p))
			gs.pMap->ClickRight(gs.myTeam);
		break;
	case Input::GK_FILTERENT:
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etNormal))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etNormal);
		}
		break;
	case Input::GK_FILTERBOMB:
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etBomber))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etBomber);
		}
		break;
	case Input::GK_FILTERSPEED:
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etSpeeder))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etSpeeder);
		}
		break;
	case Input::GK_FILTERSABO:
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etSaboteur))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etSaboteur);
		}
		break;
	case Input::GK_ADDGROUP:
		bAddGroupDown = true;
		break;
	case Input::GK_MAKEGROUP:
		bMakeGroupDown = true;
		break;
	case Input::GK_PLANDEFENSE:
		gs.pMap->SetAllPlanetsOnDefense(myTeam);
		break;
	}
}
void StateGame::Event_GameKeyUp(Input::GameKey k, const PointF& p)
{
	if (!bMapInput)
		return;

	bool bSameKey = Settings::GetGamekey(Input::GK_CLICK) == Settings::GetGamekey(Input::GK_SELECT);
	bool bIns = pMapDraw->GetRect().PointInside(p);
	switch (k)
	{
	case Input::GK_CLICK:
		if (bIns)
		{
			SetTarget(pMapDraw->toModel(p));
		}

		break;
	case Input::GK_SELECT:
		if (bMouseDown)
		{
			bMouseDown = false;
			msUp = pMapDraw->toModel(p);
			HandleMouseClick(bIns && bSameKey);
		}
		break;
	case Input::GK_ADDGROUP:
		bAddGroupDown = false;
		break;
	case Input::GK_MAKEGROUP:
		bMakeGroupDown = false;
		break;
	}
}
void StateGame::Event_MouseMove(const PointF& pos)
{
	if (bScolling)
	{
		float dx = scollLast.x - pos.x;
		float dy = scollLast.y - pos.y;

		dx = pMapDraw->toModel(dx);
		dy = pMapDraw->toModel(dy);

		scollLast = pos;
		pMapDraw->MoveCam(dx, dy);
	}

	msLast = pMapDraw->toModel(pos);
}

void StateGame::Event_Resize(const PointF& ul, const PointF& dim)
{
	const float scalar = Framework::GetScalar();

	pMapDraw->SetOrigin(ul);
	pMapDraw->SetMetrics(dim + PointF(0, -70.0f));

	pMapDraw->Zoom(1.0f); //Adjust cam

	ctrlBar = RectF(pMapDraw->GetRect().BottomLeft(), ul + dim);

	AlignButtons(ctrlBar.TopLeft() + PointF(10.0f, 10.0f),
	{ ctrlBar.getHeight() - 20.0f, ctrlBar.getHeight() - 20.0f }, 10.0f,
	std::initializer_list < UIObject* > {
		&btnPause, &btnSA, &btnSN, &btnPlayer, &btnFilterEnt, &btnFilterBomb, &btnFilterSpeed, 
			&btnFilterSabo, &btnDefPlanets, &btnChat
	});

	pMiniMap->SetMetrics({ 200.0f * scalar, 200.0f * scalar });
	SetMiniMap(btnMiniOnOff.GetState());


	btnMiniOnOff.SetMetrics(PointF(50.0f, 50.0f));
	btnMiniOnOff.SetOrigin({ (pMiniMap->GetRect().BottomLeft() - btnMiniOnOff.GetMetrics()).x, ctrlBar.TopRight().y + 10.0f });
	tablist.AdjustToScreen();
}
void StateGame::Event_KeyDown(SDL_Scancode code)
{
	if (!bMapInput)
		return;

	if (code >= SDL_SCANCODE_1 && code <= SDL_SCANCODE_9)
	{
		byte group = code - SDL_SCANCODE_1 + 1;
		if (bAddGroupDown)
		{
			//add to group
			gs.pMap->AddToGroup(gs.myTeam, group);
			gs.pMap->SelectGroup(gs.myTeam, group);
		}
		else if (bMakeGroupDown)
		{
			gs.pMap->MakeGroup(gs.myTeam, group);
		}
		else
		{
			//select group
			gs.pMap->SelectGroup(gs.myTeam, group);
		}
	}
}
//positive = forwards | negative = backwards
void StateGame::Event_MouseWheel(float amount, const PointF& pos)
{
	if (amount > 0)
	{
		pMapDraw->Zoom(std::pow(1.1f, float(amount)), pos);
	}
	else if (amount < 0)
	{
		pMapDraw->Zoom(std::pow(0.9f, float(-amount)), pos);
	}
}

void StateGame::HandleMouseClick(bool bInside)
{
	if (!bMapInput)
		return;

	if (msDown.isEqual(msUp, 5.0f) && bInside)
	{
		//Click event
		SetTarget(msDown);
	}
	else
	{
		//select event
		const PointF p = (msDown + msUp) / 2.0f;
		float r2 = ((msDown - msUp) / 2.0f).lengthSq();

		if (!gs.pMap->Select(p, r2, gs.myTeam))
		{
			//nothing was selected -> if bIns => Click && Select on same key
			if (bInside)
			{
				SetTarget(p);
			}
		}
		else
		{
			// something was selected!
			// message for tutorial
			Event_UnitsSelected();
		}
	}
}
void StateGame::SetTarget(const PointF& p)
{
	if (!bMapInput)
		return;

	bool planTargeted = false;
	UIGridEntity& grid = *pMapDraw->GetEntitySpawnSelect();
	if (grid.isEnabled())
	{
		if (!grid.GetRect().PointInside(Input::GetMouseXY()))
		{
			planTargeted = gs.pMap->Click(p, gs.myTeam);
		}
	}
	else
	{
		planTargeted = gs.pMap->Click(p, gs.myTeam);
	}

	if (planTargeted && Settings::DeselectOnTargetSelect())
		gs.pMap->ClickRight(gs.myTeam);
}

  ////////////////////////////////////////////////////////////////
 /////////////////////////// MAP & Buttons //////////////////////
////////////////////////////////////////////////////////////////

void StateGame::DrawMap(Drawing& draw)
{
	CalcStarfieldColor(draw);

	pMapDraw->Draw(draw, Input::GetMouseXY());

	if (bMouseDown)
	{
		float r = pMapDraw->fromModel(((msDown - msLast) / 2.0f).length());
		PointF p1 = pMapDraw->fromModel(msDown);
		PointF p2 = pMapDraw->fromModel(msLast);

		PointF p = (p1 + p2) / 2.0f;
		draw.DrawCircle(p, r, Color::Green(), 4.0f);
	}

	draw.DrawRect(ctrlBar, Color::GetTeamColor(gs.myTeam).mix(Color::Black()));

	btnPause.Draw(draw);
	btnSA.Draw(draw);
	btnSN.Draw(draw);

	btnFilterEnt.Draw(draw);
	btnFilterBomb.Draw(draw);
	btnFilterSabo.Draw(draw);
	btnFilterSpeed.Draw(draw);
	btnDefPlanets.Draw(draw);

	if (btnChat.isEnabled())
		btnChat.Draw(draw);

	if (btnPlayer.isEnabled())
		btnPlayer.Draw(draw);

	pMiniMap->Draw(draw);
	btnMiniOnOff.Draw(draw);

	tipPlayer.Draw(draw);
	tipChat.Draw(draw);
	tipSN.Draw(draw);
	tipSA.Draw(draw);
	tipDefPlan.Draw(draw);
	tipPause.Draw(draw);
	tipMap.Draw(draw);

	tipEnt.Draw(draw);
	tipBomb.Draw(draw);
	tipSpeed.Draw(draw);
	tipSabo.Draw(draw);

	tablist.Draw(draw);

	/*
	FONT_SMALL.SetColor(Color::White());
	FONT_SMALL.Text(std::string("map: ") + std::to_string(mapUpdate), { 10, 90 });
	FONT_SMALL.Text(std::string("mapdraw: ") + std::to_string(mapDrawUpdate), { 10, 130 });

	FONT_SMALL.Text(std::string("AI: ") + std::to_string(MAP_AI), { 10, 170 });
	FONT_SMALL.Text(std::string("planets: ") + std::to_string(MAPU_PLANS), { 10, 210 });
	FONT_SMALL.Text(std::string("all ents: ") + std::to_string(MAPU_ENTS), { 10, 250 });
	FONT_SMALL.Text(std::string("e primary: ") + std::to_string(MAPU_PRIM_VEL), { 10, 290 });
	FONT_SMALL.Text(std::string("e crowd: ") + std::to_string(MAPU_CROWD), { 10, 330 });
	FONT_SMALL.Text(std::string("e pos: ") + std::to_string(MAPU_POS), { 10, 370 });
	FONT_SMALL.Text(std::string("e attk: ") + std::to_string(MAPU_ATTK), { 10, 410 });
	FONT_SMALL.Text(std::string("e die: ") + std::to_string(MAPU_DIE), { 10, 450 });
	FONT_SMALL.Text(std::string("time: ") + std::to_string(time), { 10, 490 });
	FONT_SMALL.Text(std::string("units: ") + std::to_string(MAP_UNITS), { 10, 530 });
	*/
}
void StateGame::UpdateMap(float dt)
{
	sBox.Reset();

	gs.pMap->Update(dt);

	pMapDraw->Update(dt);

	sBox.Update(gs.pMap->CountAllyPlanets(myTeam));

	if(gs.pMap->GameStart())
		score.Update(dt, *gs.pMap);

	byte winner = gs.pMap->GameEnd();

	if (winner != 0)
	{
		score.SaveInDatabase(gs.pMap->isAlly(winner, gs.myTeam));
		gs.time = GetCurGametime();

		if (gs.lvlPackLevel)
		{
			if (score.IsWinner())
			{
				// won a levelpack game
				SetNextState(states::LevelpackWin);
			}
			else
			{
				SetNextState(states::GameOver);
			}
		}
		else
		{
			SetNextState(states::GameOver);
		}
	}
}
void StateGame::UpdateGameButtons()
{
	if (btnPause.Pushed())
	{
		SetNextState(states::Pause);
	}

	if (btnMiniOnOff.Pushed())
		SetMiniMap(btnMiniOnOff.GetState());

	if (btnSA.Pushed() && bMapInput)
		gs.pMap->SelectAll(gs.myTeam);

	if (btnSN.Pushed() && bMapInput)
		gs.pMap->ClickRight(gs.myTeam);

	if (btnFilterEnt.Pushed() && bMapInput)
	{
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etNormal))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etNormal);
		}
	}

	if (btnFilterBomb.Pushed() && bMapInput)
	{
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etBomber))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etBomber);
		}
	}

	if (btnFilterSabo.Pushed() && bMapInput)
	{
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etSaboteur))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etSaboteur);
		}
	}

	if (btnFilterSpeed.Pushed() && bMapInput)
	{
		if (!gs.pMap->FilterEntityType(myTeam, MapObject::etSpeeder))
		{
			gs.pMap->SelectAllEntityType(myTeam, MapObject::etSpeeder);
		}
	}

	if (btnDefPlanets.Pushed() && bMapInput)
	{
		gs.pMap->SetAllPlanetsOnDefense(myTeam);
	}

	UIGridEntity& entSel = *pMapDraw->GetEntitySpawnSelect();
	if (bEntitySelector)
	{
		if (entSel.isEnabled() && bMapInput)
		{
			if (entSel.Pushed() && entSel.GetCurType() != MapObject::entityType::etNone)
			{
				gs.pMap->SetPlanetSpawnType(myTeam, entSel.GetCurType());
				Event_EntityTypeChanged();
			}
		}
	}
	else
	{
		entSel.Disable();
	}
	

	if (btnPlayer.Pushed())
	{
		// show players
		tablist.SetVisible(!tablist.isVisible());
	}
}

void StateGame::AlignButtons(PointF start, const PointF& dim, float dist, const std::initializer_list< UIObject* >& btns)
{
	for (auto& b : btns)
	{
		b->SetMetrics(dim);
		b->SetOrigin(start);
		start += PointF(dim.x + dist, 0.0f);
	}
}

void StateGame::SetMiniMap(bool active)
{
	const PointF ul = Framework::DrawStart();
	const PointF dim = Framework::DrawWidth();
	if (active)
	{
		pMiniMap->SetOrigin(PointF(ul.x + dim.x - pMiniMap->GetMetrics().x - 10.0f,
			ul.y + dim.y - 10.0f - pMiniMap->GetMetrics().y));
	}
	else
	{
		pMiniMap->SetOrigin(PointF(ul.x + dim.x - pMiniMap->GetMetrics().x - 10.0f,
			ul.y + dim.y - ctrlBar.getHeight() + 10.0f));
	}
}
float StateGame::GetCurGametime()
{
	return score.GetGametime();
}


void StateGame::CalcStarfieldColor(Drawing& draw)
{
	static const int minUnits = 40;
	bool minUnitsReached = false;

	size_t curTeamMax = 0;
	size_t maxUnits = 0;
	size_t totalUnits = 0;
	for(int i = 0; i < gs.nTeams; i++)
	{
		if(gs.pMap->countUnits(i + 1) > minUnits && gs.pMap->countUnits(i + 1) > maxUnits)
		{
			maxUnits = gs.pMap->countUnits(i + 1);
			curTeamMax = i + 1;
		}
		totalUnits += gs.pMap->countUnits(i + 1);
	}

	const size_t halfUnits = totalUnits / 2;
	if(curTeamMax != 0 && maxUnits > halfUnits)
	{
		float percent = float(maxUnits - halfUnits) / float(halfUnits); // 0 - 1
		Color c = Color::GetTeamColor(curTeamMax).mix(Color::White(),percent);
		draw.SetStarfieldColor(c);
	}
	else
	{
		draw.SetStarfieldColor(Color::White());
	}
}

////////////////////////////////////////////////////////////////
 /////////////////////////// SOUND //////////////////////////////
////////////////////////////////////////////////////////////////

StateGame::SoundBox::SoundBox(int maxPlans, int myPlans, int Team)
	:
	maxPlans(maxPlans),
	myPlans(myPlans),
	myTeam(Team)
{
	lvl = CalcSoundLvl();
	Sound::Theme(lvl);

	if (Database::GetGameStruct().pClient != nullptr)
		firstPlanet = true;
}

void StateGame::SoundBox::Init(MapDrawer* pDrawer)
{
	pMapDrawer = pDrawer;
}
void StateGame::SoundBox::Event_EntitySpawn(PlanetID pID, MapEntity& e) 
{
	if (!bSpawn)
	{
		bSpawn = true;
		Sound::Effect(Sound::S_SPAWN);
		return;
	}
}
void StateGame::SoundBox::Event_EntityKilled(MapEntity& e) 
{
	if (!bDied)
	{
		bDied = true;
		Sound::Effect(Sound::S_DIE);
		return;
	}
}
//virtual void Event_PlanetAttacked(PlanetID pID, const EntityID& eID){};

void StateGame::SoundBox::Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit) 
{
	if (newTeam != 0)
	{
		if (firstPlanet)
		{
			firstPlanet = false;
			if (newTeam == myTeam)
				Settings::UnlockAchievement(Settings::A_FirstPlanet);
		}
		Sound::Effect(Sound::S_CAPTURE);
	}
	else
	{
		bool setBlurring = false;

		// new team == 0
		// if previous team was you and the enemy send an saboteur, play sudden.wav
		if (oldTeam == myTeam && culprit != nullptr && culprit->GetEntityType() == MapObject::entityType::etSaboteur)
		{
			Sound::Effect(Sound::S_SUDDEN);
			setBlurring = true;
		}
		// if previous team was an enemy and you captured with saboteur play horns
		else if (culprit != nullptr && culprit->GetTeam() == myTeam && culprit->GetEntityType() == MapObject::entityType::etSaboteur)
		{
			Sound::Effect(Sound::S_HORNS);
			setBlurring = true;
			Settings::UnlockAchievement(Settings::A_MLG);
		}
		else
			Sound::Effect(Sound::S_EXPLOSION);

		if (setBlurring)
		{
			pMapDrawer->SetPlanetBlur(pID);
		}
	}
}
void StateGame::SoundBox::Update(int nPlans)
{
	if (nPlans != myPlans)
	{
		myPlans = nPlans;
		int l = CalcSoundLvl();
		if (l != lvl)
		{
			lvl = l;
			Sound::Theme(lvl);
		}
	}
}
void StateGame::SoundBox::Reset()
{
	bDied = false;
	bSpawn = false;
}

unsigned int StateGame::SoundBox::CalcSoundLvl()
{
	if (maxPlans < 3) return Sound::ThemeMax();
	if (myPlans == 0) return 0;
	float res = float(myPlans - 1) / float(maxPlans - 2) * float(Sound::ThemeMax());
	int r = (int)res;
	return r + 1;
}

  ////////////////////////////////////////////////////////////////
 ////////////////////////// SCOREBOARD //////////////////////////
////////////////////////////////////////////////////////////////

StateGame::Scoreboard::Scoreboard(int maxTeams)
	:
	maxTeams(maxTeams)
{
	initVec(entSpawned);
	initVec(entKilled);
	initVec(entLost);
	initVec(planCaptured);
	initVec(planLost);
}
void StateGame::Scoreboard::Event_EntitySpawn(PlanetID pID, MapEntity& e) 
{
	entSpawned[e.GetTeam() - 1]++;
}
void StateGame::Scoreboard::Event_EntityKilled(MapEntity& e) 
{
	entLost[e.GetTeam() - 1]++;
}
//virtual void Event_PlanetAttacked(PlanetID pID, const EntityID& eID){};
void StateGame::Scoreboard::Event_PlanetCaptured(PlanetID pID, byte newTeam, byte oldTeam, const MapEntity* culprit) 
{
	if (oldTeam != 0)
	{
		planLost[oldTeam - 1]++;
	}
	if (newTeam != 0)
	{
		planCaptured[newTeam - 1]++;
	}
}
void StateGame::Scoreboard::SaveInDatabase(bool isWinner)
{
	GameResults r;
	r.isWinner = isWinner;
	this->isWinner = isWinner;

	for (int i = 0; i < maxTeams; i++)
	{
		GameResults::stats s;
		s.entDie = entLost[i];
		s.entSpawn = entSpawned[i];
		s.planCap = planCaptured[i];
		s.planLost = planLost[i];

		r.data.push_back(s);
	}

	r.dtUnitCount = 10.0f;
	r.unitCounts = std::move(unitCounts);

	Database::SetResults(std::move(r));
}
bool StateGame::Scoreboard::IsWinner() const
{
	return isWinner;
}
void StateGame::Scoreboard::initVec(std::vector<unsigned int>& v)
{
	v.assign(maxTeams, 0);
}
void StateGame::Scoreboard::Update(float dt, const Map& map)
{
	recordTime += dt;
	if (recordTime > 10.0f)
	{
		recordTime -= 10.0f;

		AddBranch(map);
	}

	sumTime += dt;
}
void StateGame::Scoreboard::AddBranch(const Map& map)
{
	std::vector< unsigned int > ucount;
	ucount.assign(maxTeams, 0);

	for (int i = 0; i < maxTeams; i++)
	{
		ucount[i] = map.countUnits(i + 1);
	}

	unitCounts.push_back(std::move(ucount));
}

float StateGame::Scoreboard::GetGametime()
{
	return sumTime;
}
