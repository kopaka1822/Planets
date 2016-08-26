#include "UniformGrid.h"
#include "Map.h"

UniformGrid::UniformGrid(int mapX, int mapY, int boxSize, int boxTolerance, int nTeams, const class Map& map)
	:
	width(mapX),
	height(mapY),
	boxsz(boxSize),
	toleran(boxTolerance),
	nTeams(nTeams),
	map(map)
{
	// create Array
	field = new Element*[width * height];

	for (int i = 0; i < width * height; i++)
	{
		field[i] = new Element();
		field[i]->ents.reserve(40); // reserve space
		field[i]->teamsz.assign(nTeams, 0);
	}
}
UniformGrid::~UniformGrid()
{
	for (int i = 0; i < width * height; i++)
	{
		tool::safeDelete(field[i]);
	}
	tool::safeDeleteArray(field);
}
void UniformGrid::clear()
{
	for (int i = 0; i < width * height; i++)
	{
		field[i]->ents.clear();
		field[i]->teamsz.assign(nTeams, 0);
	}
}
void UniformGrid::AddEntity(MapEntity* itm, const PointI& pos)
{
	// push in nearby fields

	int xStart = std::max(0, ((pos.x - toleran)) / boxsz);
	int xEnd = std::min(((pos.x + toleran)) / boxsz, width - 1);
	int yStart = std::max(0, ((pos.y - toleran)) / boxsz);
	int yEnd = std::min(((pos.y + toleran)) / boxsz, height - 1);

	for (int x = xStart; x <= xEnd; x++)
		for (int y = yStart; y <= yEnd; y++)
			field[y * width + x]->ents.push_back(itm);

	// actual pos for ai etc..
	int aX = pos.x / boxsz;
	int aY = pos.y / boxsz;

	aX = tool::clamp(aX, 0, width - 1);
	aY = tool::clamp(aY, 0, height - 1);

	size_t index = aY * width + aX;

	assert(unsigned(itm->getTeam() - 1) < unsigned(nTeams));
	field[index]->teamsz[itm->getTeam() - 1]++;
}
FastVector<MapEntity*>& UniformGrid::GetEntities(PointI pos)
{
	pos.x /= boxsz;
	pos.y /= boxsz;

	pos.x = tool::clamp(pos.x, 0, width - 1);
	pos.y = tool::clamp(pos.y, 0, height - 1);

	return field[pos.y * width + pos.x]->ents;
}
int UniformGrid::GetBoxSize() const
{
	return boxsz;
}
// own m_team
size_t UniformGrid::CountUnits(byte team, PointF pos, float radius)
{
	byte t = team - 1;
	assert(t < nTeams);

	size_t count = 0;
	int xStart = int(pos.x - radius) / boxsz;
	int yStart = int(pos.y - radius) / boxsz;
	int xEnd = int(pos.x + radius) / boxsz;
	int yEnd = int(pos.y + radius) / boxsz;

	xStart = tool::clamp(xStart, 0, width - 1);
	xEnd = tool::clamp(xEnd, 0, width - 1);
	yStart = tool::clamp(yStart, 0, height - 1);
	yEnd = tool::clamp(yEnd, 0, height - 1);

	const float tstrad = (radius + boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * boxsz, y * boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
				count += field[y * width + x]->teamsz[t];
		}
	}
			

	return count;
}
// own m_team + ally
size_t UniformGrid::CountAllyUnits(byte team, PointF pos, float radius)
{
	byte t = team - 1;
	assert(t < nTeams);

	size_t count = 0;
	int xStart = int(pos.x - radius) / boxsz;
	int yStart = int(pos.y - radius) / boxsz;
	int xEnd = int(pos.x + radius) / boxsz;
	int yEnd = int(pos.y + radius) / boxsz;

	xStart = tool::clamp(xStart, 0, width - 1);
	xEnd = tool::clamp(xEnd, 0, width - 1);
	yStart = tool::clamp(yStart, 0, height - 1);
	yEnd = tool::clamp(yEnd, 0, height - 1);

	const float tstrad = (radius + boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * boxsz, y * boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
			{
				for (byte i = 0; i < nTeams; i++)
				{
					if (!map.isAlly(team, i + 1))
						continue;
					// count enemies
					count += field[y * width + x]->teamsz[i];
				}
			}
		}
	}

	return count;
}
// enemies (no allies)
size_t UniformGrid::CountEnemyUnits(byte team, PointF pos, float radius)
{
	byte t = team - 1;
	assert(t < nTeams);

	size_t count = 0;
	int xStart = int(pos.x - radius) / boxsz;
	int yStart = int(pos.y - radius) / boxsz;
	int xEnd = int(pos.x + radius) / boxsz;
	int yEnd = int(pos.y + radius) / boxsz;

	xStart = tool::clamp(xStart, 0, width - 1);
	xEnd = tool::clamp(xEnd, 0, width - 1);
	yStart = tool::clamp(yStart, 0, height - 1);
	yEnd = tool::clamp(yEnd, 0, height - 1);

	const float tstrad = (radius + boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * boxsz, y * boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
			{
				for (byte i = 0; i < nTeams; i++)
				{
					if (map.isAlly(team, i + 1))
						continue;
					// count enemies
					count += field[y * width + x]->teamsz[i];
				}
			}
		}
	}

	return count;
}

PointI UniformGrid::GetEnemyAccumulationPoint(byte team, const PointF& pos, float radius, size_t lowerLimit)
{
	byte t = team - 1;
	assert(t < nTeams);

	int xStart = int(pos.x - radius) / boxsz;
	int yStart = int(pos.y - radius) / boxsz;
	int xEnd = int(pos.x + radius) / boxsz;
	int yEnd = int(pos.y + radius) / boxsz;

	xStart = tool::clamp(xStart, 0, width - 1);
	xEnd = tool::clamp(xEnd, 0, width - 1);
	yStart = tool::clamp(yStart, 0, height - 1);
	yEnd = tool::clamp(yEnd, 0, height - 1);

	const float tstrad = (radius + boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	size_t bestCount = lowerLimit - 1;
	PointI bestBox = PointI(-1, -1);

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * boxsz, y * boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
			{
				size_t count = 0;

				for (byte i = 0; i < nTeams; i++)
				{
					if (map.isAlly(team, i + 1))
						continue;
					// count enemies
					count += field[y * width + x]->teamsz[i];
				}

				if (count > bestCount)
				{
					count = bestCount;
					bestBox = PointI(x, y);
				}
			}
		}
	}

	return bestBox;
}

size_t UniformGrid::CountUnitsInBox(byte team, PointF pos)
{
	assert(unsigned(team - 1) < unsigned(nTeams));

	int x = int(pos.x) / boxsz;
	int y = int(pos.y) / boxsz;

	x = tool::clamp(x, 0, width - 1);
	y = tool::clamp(y, 0, height - 1);

	return field[y * width + x]->teamsz[team - 1];

	
}
size_t UniformGrid::CountUnitsInBoxRaw(byte team, PointI posi)
{
	assert(posi.x >= 0 && posi.x < width);
	assert(posi.y >= 0 && posi.y < height);
	assert(unsigned(team - 1) < unsigned(nTeams));
	return field[posi.y * width + posi.x]->teamsz[team - 1];
}

FastVector<MapEntity*>& UniformGrid::GetEntitiesRaw(const PointI& box)
{
	assert(box.x >= 0 && box.x < width);
	assert(box.y >= 0 && box.y < height);
	return field[box.y * width + box.x]->ents;
}