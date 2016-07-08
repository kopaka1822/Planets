#include "UIObject.h"
#include "GameState.h"

void UIObject::Register(GameState& g)
{
	g.AddObject(this);
}