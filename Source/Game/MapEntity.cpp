#include "MapEntity.h"

const int MapEntity::RADIUS = 2;
const int MapEntity::RADIUS2 = RADIUS * RADIUS;
const int MapEntity::MIN_DIST = 2 * RADIUS + 2;
const int MapEntity::MIN_DIST2 = MIN_DIST * MIN_DIST;

const int MapEntity::NEAR_DIST = 2 * MIN_DIST;
const int MapEntity::NEAR_DIST2 = NEAR_DIST * NEAR_DIST;

const int MapEntity::SPEED = 30;

const int MapEntity::DAMAGE = 4;
//const float MapEntity::DAMAGE_INTV;

const int MapEntity::MAX_CHARGES = 10; //shall be used if the game is laggy

const int MapEntity::MAX_HP = 8;

const int MapEntity::FOV = (MIN_DIST * 3) / 2; //Field of vision
const int MapEntity::FOV2 = FOV * FOV;

const float MapEntity::DAMAGE_INTV = 0.5f;
