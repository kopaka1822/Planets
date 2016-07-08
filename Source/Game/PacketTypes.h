#pragma once
#include "../Utility/DataContainer.h"

enum class PacketType
{
	ServerType,
	Username,
	LobbyPlayerlist,
	ClientAllRecieved,
	ClientReady,
	ServerGameStart,
	GameClock,

	// Map Transfer
	MapPlanets,
	MapEntities,
	MapTeams,
	MapComplete,

	// Ingame
	GameSpawn,
	GameDeath,
	GamePlanetCapture,
	GamePlanetHealth,
	GamePositions,

	GameSelect,
	GameClick,
	GameDeselect,
	GameEnd,
	GameSelectGroup,
	GameDeleteGroup,
	GameAddToGroup,
	GameMakeGroup,
	GameSelectAll,
	GameChat,
	GameTeamSurrender,
	GameFilterEntityType,
	GameAllEntityType,

	// Ingame form client
	ClientSelect,
	ClientClick,
	ClientDeselect,
	ClientSelectGroup,
	ClientDeleteGroup,
	ClientAddToGroup,
	ClientMakeGroup,
	ClientSelectAll,
	ClientChat,
	ClientFilterEntityType,
	ClientAllEntityType,

	// Dyn lobby
	LobbyTime,
	LobbyTeamAssign,
	GameForceStop,

	// Clans
	ClientClanRequest,
	ClientClanDestroy,
	GameClanChange,

	// new Entities
	ClientSetPlanetSpawntype,
	GameSetPlanetSpawntpye,

	// Server Start Page
	// 1st Username
	PageNewSection,
	PageUpdateLeft,
	PageUpdateRight,

	ClientButtonpress,

	ClientPlanDefense,
	ServerPlanDefense
};

enum class ServerTypes
{
	OneLobby,
	Redirecting,
	DynClient,   // dynamic number of players
	GameSelectLobby
};

enum class PageObject
{
	Button,
	Label,
	LabelBig
};