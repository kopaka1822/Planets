#include "LevelToolApp.h"
#include "StateMenu.h"
#include "StateReadSave.h"
#include "StateSaveAnalyse.h"

using State = std::unique_ptr< ConsoleState >;

std::unique_ptr< ConsoleState > LevelToolApp::GetInitState()
{
	return State(new StateMenu());
}
std::unique_ptr< ConsoleState > LevelToolApp::GetState(int code)
{
	switch (code)
	{
	case LevelToolApp::menu:
		return State(new StateMenu());
	case LevelToolApp::readSave:
		return State(new StateReadSave());
	case LevelToolApp::analyseSaves:
		return State(new StateSaveAnalyse());
	}
	return State();
}