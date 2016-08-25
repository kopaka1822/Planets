#pragma once
#include "Drawing.h"
#include "Input.h"
#include <vector>
//#include "UIButtonText.h"
#include "UILabel.h"
#include <initializer_list>
#include "UIObject.h"

class GameState : public Input::EventReceiver
{
	friend Input; // touching the Input Reciever Functions
public:
	enum class states //c++ 11 enum bitches
	{
		Undefined = 0,
		Exit,
		Menu,
		Options,
		Single,
		ServerSelect,
		Connect,
		MultiLobbyFixed,
		MultiLobbyDyn,
		GameMulti,
		MultiName,
		ServerStartPage,
		StateFirstStart,

		GameSingle,
		GameOver, // graph
		GameOver2, // table

		Extras,
		Editor,
		EditorTeams,
		ExtraTheme,
		Achiev,
		Color,
		LevelpackEditor,
		LevelpackWin,

		Back,
		Pause,
		Tutorial,
		TutoialGame,
		TutorialGame2,
		TutorialProposal
	};
public:
	virtual ~GameState()
	{}
	virtual void ComposeFrame(Drawing& draw) = 0;
	virtual void ExecuteCode(float dt) = 0;
	virtual void Event_Resize(const PointF& ul, const PointF& dim){}

	bool isFinished() const
	{
		return (nxtState != states::Undefined);
	}
	states GetNextState()
	{
		states t = nxtState;
		nxtState = states::Undefined;
		return t;
	}
	virtual bool BackgroundUpdate()
	{
		return false;
	}
	virtual std::unique_ptr<GameState> GetPrev()
	{
		return nullptr;
	}
	void AddObject(UIObject* recv)
	{
		inRec.push_back(recv);
	}
protected:
	void SetNextState(states state)
	{
		assert(state != states::Undefined);

		nxtState = state;
	}

	void AlignObjects(PointF start, float distance, std::initializer_list< UIObject* > objs)
	{
		for (auto& o : objs)
		{
			o->setOrigin(start);
			start.y += distance + o->getMetrics().y;
		}
	}
private:
	states nxtState = states::Undefined;
	std::vector<UIObject*> inRec; //Input Reciever
};
