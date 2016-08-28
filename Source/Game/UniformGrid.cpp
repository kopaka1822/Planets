#include "UniformGrid.h"
#include "Map.h"

UniformGrid::UniformGrid(int mapX, int mapY, int boxSize, int boxTolerance, int nTeams, const class Map& map)
	:
	m_width(mapX),
	m_height(mapY),
	m_boxsz(boxSize),
	m_toleran(boxTolerance),
	m_nTeams(nTeams),
	m_map(map)
{
	// create Array
	m_field = new Element*[m_width * m_height];

	for (int i = 0; i < m_width * m_height; i++)
	{
		m_field[i] = new Element();
		m_field[i]->ents.reserve(40); // reserve space
		m_field[i]->teamsz.assign(nTeams, 0);
	}
}
UniformGrid::~UniformGrid()
{
	for (int i = 0; i < m_width * m_height; i++)
	{
		tool::safeDelete(m_field[i]);
	}
	tool::safeDeleteArray(m_field);
}
void UniformGrid::clear()
{
	for (int i = 0; i < m_width * m_height; i++)
	{
		m_field[i]->ents.clear();
		m_field[i]->teamsz.assign(m_nTeams, 0);
	}
}
void UniformGrid::addEntity(MapEntity* itm, const PointI& pos)
{
	// push in nearby fields

	int xStart = std::max(0, ((pos.x - m_toleran)) / m_boxsz);
	int xEnd = std::min(((pos.x + m_toleran)) / m_boxsz, m_width - 1);
	int yStart = std::max(0, ((pos.y - m_toleran)) / m_boxsz);
	int yEnd = std::min(((pos.y + m_toleran)) / m_boxsz, m_height - 1);

	for (int x = xStart; x <= xEnd; x++)
		for (int y = yStart; y <= yEnd; y++)
			m_field[y * m_width + x]->ents.push_back(itm);

	// actual pos for ai etc..
	int aX = pos.x / m_boxsz;
	int aY = pos.y / m_boxsz;

	aX = tool::clamp(aX, 0, m_width - 1);
	aY = tool::clamp(aY, 0, m_height - 1);

	size_t index = aY * m_width + aX;

	assert(unsigned(itm->getTeam() - 1) < unsigned(m_nTeams));
	m_field[index]->teamsz[itm->getTeam() - 1]++;
}
FastVector<MapEntity*>& UniformGrid::getEntities(PointI pos)
{
	pos.x /= m_boxsz;
	pos.y /= m_boxsz;

	pos.x = tool::clamp(pos.x, 0, m_width - 1);
	pos.y = tool::clamp(pos.y, 0, m_height - 1);

	return m_field[pos.y * m_width + pos.x]->ents;
}
int UniformGrid::getBoxSize() const
{
	return m_boxsz;
}
// own m_team
size_t UniformGrid::countUnits(TeamID team, PointF pos, float radius)
{
	byte t = team - 1;
	assert(t < m_nTeams);

	size_t count = 0;
	int xStart = int(pos.x - radius) / m_boxsz;
	int yStart = int(pos.y - radius) / m_boxsz;
	int xEnd = int(pos.x + radius) / m_boxsz;
	int yEnd = int(pos.y + radius) / m_boxsz;

	xStart = tool::clamp(xStart, 0, m_width - 1);
	xEnd = tool::clamp(xEnd, 0, m_width - 1);
	yStart = tool::clamp(yStart, 0, m_height - 1);
	yEnd = tool::clamp(yEnd, 0, m_height - 1);

	const float tstrad = (radius + m_boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * m_boxsz, y * m_boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
				count += m_field[y * m_width + x]->teamsz[t];
		}
	}
			

	return count;
}
// own m_team + ally
size_t UniformGrid::countAllyUnits(TeamID team, PointF pos, float radius)
{
	byte t = team - 1;
	assert(t < m_nTeams);

	size_t count = 0;
	int xStart = int(pos.x - radius) / m_boxsz;
	int yStart = int(pos.y - radius) / m_boxsz;
	int xEnd = int(pos.x + radius) / m_boxsz;
	int yEnd = int(pos.y + radius) / m_boxsz;

	xStart = tool::clamp(xStart, 0, m_width - 1);
	xEnd = tool::clamp(xEnd, 0, m_width - 1);
	yStart = tool::clamp(yStart, 0, m_height - 1);
	yEnd = tool::clamp(yEnd, 0, m_height - 1);

	const float tstrad = (radius + m_boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * m_boxsz, y * m_boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
			{
				for (TeamID i = 0; i < m_nTeams; i++)
				{
					if (!m_map.isAlly(team, i + 1))
						continue;
					// count enemies
					count += m_field[y * m_width + x]->teamsz[i];
				}
			}
		}
	}

	return count;
}
// enemies (no allies)
size_t UniformGrid::countEnemyUnits(TeamID team, PointF pos, float radius)
{
	byte t = team - 1;
	assert(t < m_nTeams);

	size_t count = 0;
	int xStart = int(pos.x - radius) / m_boxsz;
	int yStart = int(pos.y - radius) / m_boxsz;
	int xEnd = int(pos.x + radius) / m_boxsz;
	int yEnd = int(pos.y + radius) / m_boxsz;

	xStart = tool::clamp(xStart, 0, m_width - 1);
	xEnd = tool::clamp(xEnd, 0, m_width - 1);
	yStart = tool::clamp(yStart, 0, m_height - 1);
	yEnd = tool::clamp(yEnd, 0, m_height - 1);

	const float tstrad = (radius + m_boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * m_boxsz, y * m_boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
			{
				for (byte i = 0; i < m_nTeams; i++)
				{
					if (m_map.isAlly(team, i + 1))
						continue;
					// count enemies
					count += m_field[y * m_width + x]->teamsz[i];
				}
			}
		}
	}

	return count;
}

PointI UniformGrid::getEnemyAccumulationPoint(TeamID team, const PointF& pos, float radius, size_t lowerLimit)
{
	byte t = team - 1;
	assert(t < m_nTeams);

	int xStart = int(pos.x - radius) / m_boxsz;
	int yStart = int(pos.y - radius) / m_boxsz;
	int xEnd = int(pos.x + radius) / m_boxsz;
	int yEnd = int(pos.y + radius) / m_boxsz;

	xStart = tool::clamp(xStart, 0, m_width - 1);
	xEnd = tool::clamp(xEnd, 0, m_width - 1);
	yStart = tool::clamp(yStart, 0, m_height - 1);
	yEnd = tool::clamp(yEnd, 0, m_height - 1);

	const float tstrad = (radius + m_boxsz / 2);
	const float tstrad2 = tstrad * tstrad;

	size_t bestCount = lowerLimit - 1;
	PointI bestBox = PointI(-1, -1);

	for (int x = xStart; x <= xEnd; x++)
	{
		for (int y = yStart; y <= yEnd; y++)
		{
			// is box in radius?
			PointF mid = PointF(x * m_boxsz, y * m_boxsz);
			if ((mid - pos).lengthSq() < tstrad2)
			{
				size_t count = 0;

				for (byte i = 0; i < m_nTeams; i++)
				{
					if (m_map.isAlly(team, i + 1))
						continue;
					// count enemies
					count += m_field[y * m_width + x]->teamsz[i];
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

size_t UniformGrid::countUnitsInBox(TeamID team, PointF pos)
{
	assert(unsigned(team - 1) < unsigned(m_nTeams));

	int x = int(pos.x) / m_boxsz;
	int y = int(pos.y) / m_boxsz;

	x = tool::clamp(x, 0, m_width - 1);
	y = tool::clamp(y, 0, m_height - 1);

	return m_field[y * m_width + x]->teamsz[team - 1];

	
}
size_t UniformGrid::countUnitsInBoxRaw(TeamID team, PointI posi)
{
	assert(posi.x >= 0 && posi.x < m_width);
	assert(posi.y >= 0 && posi.y < m_height);
	assert(unsigned(team - 1) < unsigned(m_nTeams));
	return m_field[posi.y * m_width + posi.x]->teamsz[team - 1];
}

FastVector<MapEntity*>& UniformGrid::getEntitiesRaw(const PointI& box)
{
	assert(box.x >= 0 && box.x < m_width);
	assert(box.y >= 0 && box.y < m_height);
	return m_field[box.y * m_width + box.x]->ents;
}