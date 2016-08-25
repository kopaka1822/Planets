#include "UIObject.h"
#include "GameState.h"

void UIObject::registerMe(GameState& g)
{
	g.AddObject(this);
}