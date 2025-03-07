#include "Scene.h"
#include <stdio.h>
#include "Globals.h"



Scene::Scene()
{
	player = nullptr;
    level = nullptr;
	enemiesDefeat = 0;
	currentStage = 1;

	camera.target = { 0, 0 };				 //Center of the screen
	camera.offset = { 0, MARGIN_GUI_Y };	//Offset from the target (center of the screen)
	camera.rotation = 0.0f;					//No rotation
	camera.zoom = 1.0f;						//Default zoom

	debug = DebugMode::OFF;
	drunklifes = 15;
}
Scene::~Scene()
{
	if (player != nullptr)
	{
		player->Release();
		delete player;
		player = nullptr;
	}
    if (level != nullptr)
    {
		level->Release();
        delete level;
        level = nullptr;
    }
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();

	for (Enemy* enem : enemies)
	{
		enem->Release();
		delete enem;
	}
	enemies.clear();
}
AppStatus Scene::Init()
{
	//Create player
	player = new Player({ 0,0 }, State::IDLE, Look::RIGHT);
	if (player == nullptr)
	{
		LOG("Failed to allocate memory for Player");
		return AppStatus::ERROR;
	}
	//Initialise player
	if (player->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Player");
		return AppStatus::ERROR;
	}

	//Create level 
    level = new TileMap();
    if (level == nullptr)
    {
        LOG("Failed to allocate memory for Level");
        return AppStatus::ERROR;
    }
	//Initialise level
	if (level->Initialise() != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}
	//Load level
	if (LoadLevel(1) != AppStatus::OK)
	{
		LOG("Failed to load Level 1");
		return AppStatus::ERROR;
	}
	//Assign the tile map reference to the player to check collisions while navigating
	player->SetTileMap(level);
	font1 = new Text();
	if (font1 == nullptr)
	{
		LOG("Failed to allocate memory for font 1");
		return AppStatus::ERROR;
	}
	//Initialise text font 1
	if (font1->Initialise(Resource::IMG_FONT, "images/font8x8.png", ' ', 8) != AppStatus::OK)
	{
		LOG("Failed to initialise Level");
		return AppStatus::ERROR;
	}

    return AppStatus::OK;
}
AppStatus Scene::LoadLevel(int stage)
{
	int size;
	int x, y, i;
	Tile tile;
	Point pos;
	int *map = nullptr;
	Object *obj;
	Enemy *enem;
	Drunk* super;

	ClearLevel();
	player->Stop();
	size = LEVEL_WIDTH * LEVEL_HEIGHT;
	if (stage == 1)
	{
		map = new int[size] {
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
			10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10,
			10, 10, 74, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 0, 10, 10,
			10, 10, 1, 1, 70, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 70, 0, 0, 1, 1, 10, 10,
			10, 10, 74, 73, 75, 0, 0, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 0, 0, 72, 73, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 1, 1, 70, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 70, 0, 0, 1, 1, 10, 10,
			10, 10, 74, 73, 75, 0, 0, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 0, 0, 72, 73, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 1, 1, 70, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 70, 0, 0, 1, 1, 10, 10,
			10, 10, 74, 73, 75, 0, 0, 72, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 73, 75, 0, 0, 72, 73, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 71, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 10, 10,
			10, 10, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 10, 10

		};
		player->InitScore();
		enemiesDefeat = 3;
	}
	else if (stage == 2)
	{
		map = new int[size] {
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
				30, 31, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 6, 30, 31,
				32, 33, 84, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 82, 83, 83, 83, 83, 83, 85, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 2, 2, 80, 0, 2, 2, 2, 2, 2, 80, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 82, 83, 83, 83, 83, 85, 0, 82, 83, 83, 83, 83, 85, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 80, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 85, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 2, 2, 2, 2, 2, 2, 2, 80, 0, 2, 2, 2, 2, 2, 2, 80, 0, 2, 2, 2, 2, 2, 2, 2, 80, 0, 30, 31,
				32, 33, 81, 0, 82, 83, 83, 83, 83, 83, 83, 85, 0, 82, 83, 83, 83, 83, 83, 85, 0, 82, 83, 83, 83, 83, 83, 83, 85, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 32, 33,
				30, 31, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 30, 31,
				32, 33, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 2, 32, 33
		};
		enemiesDefeat = 4;
	}
	else if (stage == 3)
	{
		map = new int[size] {
			0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0,
			0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0,
				20, 21, 7, 7, 7, 7, 7, 7, 7, 81, 0, 0, 0, 7, 7, 7, 7, 7, 7, 81, 0, 0, 0, 7, 7, 7, 7, 7, 7, 7, 20, 21,
				20, 21, 84, 83, 83, 83, 83, 83, 83, 85, 0, 0, 0, 82, 83, 83, 83, 83, 83, 85, 0, 0, 0, 82, 83, 83, 83, 83, 83, 83, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 3, 3, 3, 3, 3, 3, 3, 80, 0, 0, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 35, 80, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 84, 83, 83, 83, 83, 83, 83, 85, 0, 0, 0, 0, 0, 82, 83, 83, 83, 83, 83, 83, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 81, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 35, 81, 0, 0, 20, 21, 
				20, 21, 81, 0, 0, 35, 3, 3, 3, 3, 3, 3, 3, 3, 80, 0, 0, 0, 3, 3, 3, 3, 3, 3, 3, 3, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 84, 83, 83, 83, 83, 83, 83, 83, 85, 0, 0, 0, 82, 83, 83, 83, 83, 83, 83, 83, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 81, 102, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 102, 35, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 35, 3, 3, 3, 3, 3, 3, 3, 3, 3, 80, 0, 3, 3, 3, 3, 3, 3, 3, 3, 3, 3, 81, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 85, 0, 82, 83, 83, 83, 83, 83, 83, 83, 83, 83, 85, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 3, 3, 3, 3, 3, 80, 0, 3, 3, 3, 80, 0, 0, 0, 0, 0, 0, 0, 3, 3, 3, 80, 0, 3, 3, 3, 3, 3, 20, 21,
				20, 21, 84, 83, 83, 83, 83, 85, 0, 82, 83, 83, 85, 0, 0, 0, 0, 0, 0, 0, 82, 83, 83, 85, 0, 82, 83, 83, 83, 83, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 20, 21,
				20, 21, 3, 3, 3, 3, 3, 3, 35, 80, 0, 0, 0, 35, 3, 3, 3, 3, 35, 80, 0, 0, 0, 35, 3, 3, 3, 3, 3, 3, 20, 21,

				
		};
		enemiesDefeat = 4;
	}
	else if (stage == 4)
	{
		map = new int[size] 
		{
			0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0,
			0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0,
			40, 41, 8, 8, 8, 8, 8, 8, 45, 61, 0, 0, 0, 8, 8, 8, 8, 8, 8, 60, 0, 0, 0, 45, 8, 8, 8, 8, 8, 8, 40, 41,
			42, 43, 64, 63, 63, 63, 63, 63, 45, 61, 0, 0, 0, 62, 63, 63, 63, 63, 63, 65, 0, 0, 0, 45, 64, 63, 63, 63, 63, 63, 42, 43,
			40, 41, 61, 0, 0, 0, 0, 0, 4, 4, 4, 60, 0, 0, 0, 0, 0, 0, 0, 0, 0, 4, 4, 4, 61, 0, 0, 0, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 0, 0, 0, 62, 63, 63, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 63, 65, 0, 0, 0, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 41,
			40, 41, 61, 0, 0, 0, 0, 101, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 101, 0, 0, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 45, 4, 4, 4, 4, 4, 4, 4, 45, 60, 0, 0, 0, 45, 4, 4, 4, 4, 4, 4, 4, 45, 60, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 45, 64, 63, 63, 63, 63, 63, 45, 45, 61, 0, 0, 0, 45, 45, 64, 63, 63, 63, 63, 63, 45, 61, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 45, 61, 0, 0, 0, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 0, 0, 0, 45, 61, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 45, 61, 101, 0, 0, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 0, 101, 0, 45, 61, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 4, 4, 4, 4, 60, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 4, 4, 4, 4, 61, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 62, 63, 63, 63, 65, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 62, 63, 63, 63, 65, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 45, 61, 0, 0, 0, 45, 45, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 43,
			42, 43, 61, 0, 0, 45, 4, 4, 4, 60, 0, 0, 62, 63, 65, 0, 0, 0, 62, 63, 65, 0, 0, 4, 4, 4, 45, 60, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 45, 64, 63, 63, 65, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 62, 63, 63, 45, 61, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 45, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 61, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 45, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 61, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 45, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 45, 61, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 4, 4, 4, 4, 4, 4, 4, 60, 0, 0, 4, 4, 60, 0, 0, 4, 4, 4, 4, 4, 4, 4, 61, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 62, 63, 63, 63, 63, 63, 63, 65, 0, 0, 62, 63, 65, 0, 0, 62, 63, 63, 63, 63, 63, 63, 65, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 41,
			42, 43, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 42, 43,
			40, 41, 61, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 40, 41,
			42, 43, 4, 4, 4, 4, 4, 4, 4, 60, 0, 0, 0, 4, 4, 4, 4, 4, 4, 60, 0, 0, 0, 4, 4, 4, 4, 4, 4, 4, 42, 43,
			


		};
		enemiesDefeat = 4;
	}
	else if (stage == 5)
	{
		map = new int[size] 
		{
		0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0,
		0, 19, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 19, 0,
			50, 50, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 9, 50, 50,
			50, 50, 84, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 83, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 103, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 0, 0, 0, 5, 5, 80, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 0, 0, 0, 82, 83, 85, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 81, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 50, 50,
			50, 50, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 5, 50, 50,
			


		};
		enemiesDefeat = 1;
	}
	else
	{
		//Error level doesn't exist or incorrect level number
		LOG("Failed to load level, stage %d doesn't exist", stage);
		return AppStatus::ERROR;	
	}

	//Entities and objects
	i = 0;
	for (y = 0; y < LEVEL_HEIGHT; ++y)
	{
		for (x = 0; x < LEVEL_WIDTH; ++x)
		{
			tile = (Tile)map[i];
			if (tile == Tile::EMPTY)
			{
				map[i] = 0;
			}
			else if (tile == Tile::PLAYER)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				player->SetPos(pos);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_MUSHROOM)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::MUSHROOM);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_BANANA)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::BANANA);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_ICE_CREAM)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::ICE_CREAM);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_FLAN)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::FLAN);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_CAKE)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::CAKE);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::ITEM_CHERRY)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				obj = new Object(pos, ObjectType::CHERRY);
				objects.push_back(obj);
				map[i] = 0;
			}
			else if (tile == Tile::BUSTER)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				enem = new Enemy(pos, E_State::IDLE, E_Look::RIGHT, E_Type::BUSTER);
				enem -> Initialise();
				enem->SetTileMap(level);
				enem->SetPlayer(player);
				enemies.push_back(enem);
				map[i] = 0;
			}
			else if (tile == Tile::SKELMON)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				enem = new Enemy(pos, E_State::IDLE, E_Look::RIGHT, E_Type::SKELMON);
				enem->Initialise();
				enem->SetTileMap(level);
				enem->SetPlayer(player);
				enemies.push_back(enem);
				map[i] = 0;
			}
			else if (tile == Tile::SUPERDRUNK)
			{
				pos.x = x * TILE_SIZE;
				pos.y = y * TILE_SIZE + TILE_SIZE - 1;
				super = new Drunk(pos, D_State::FALLING, D_Look::RIGHT);
				super->Initialise();
				super->SetTileMap(level);
				super->SetPlayer(player);
				supers.push_back(super);
				map[i] = 0;
			}
			++i;
		}
	}
	pos.x = 4 * TILE_SIZE;
	pos.y = 27 * TILE_SIZE;
	player->SetPos(pos);
	//Tile map
	level->Load(map, LEVEL_WIDTH, LEVEL_HEIGHT);
	delete[] map;

	return AppStatus::OK;
}
void Scene::BubbleShoot()
{
	BubbleTime += GetFrameTime();

	if (IsKeyPressed(KEY_F) && BubbleTime >= .3)
	{
		if (player->IsLookingLeft())
		{
			Bubble* buble = new Bubble(player->GetPos(), Directions::LEFT);
			buble->Initialise();
			bubbles.push_back(buble);
		}
		else
		{
			Bubble* buble = new Bubble(player->GetPos(), Directions::RIGHT);
			buble->Initialise();
			bubbles.push_back(buble);
		}
		BubbleTime = 0;

	}


}
void Scene::RandomItem()
{
	int GetObject = 0;
	int NewPosition = 0;

	for (int i = 0; i < 7; ++i)
	{
		GetObject = GetRandomValue(1, 5);
		NewPosition = GetRandomValue(1, 4);
		
		Point pos;
		Object* obj;

		switch (GetObject)
		{
		case 1:
			switch (NewPosition)
			{
			case 1:
				
				pos.x = 16;
				pos.y = 215;
				break;
			case 2:
				pos.x = 55;
				pos.y = 175;
				break;
			case 3:
				pos.x = 85;
				pos.y = 135;
				break;
			case 4:
				pos.x = 115;
				pos.y = 95;
				break;
			default:
				LOG("No position found.");
				return;
			}
			obj = new Object(pos, ObjectType::MUSHROOM);
			break;
		case 2:
			switch (NewPosition)
			{
			case 1:
				pos.x = 35;
				pos.y = 215;
				break;
			case 2:
				pos.x = 200;
				pos.y = 175;
				break;
			case 3:
				pos.x = 50;
				pos.y = 135;
				break;
			case 4:
				pos.x = 150;
				pos.y = 95;
				break;
			default:
				LOG("No position found.");
				return;
			}
			obj = new Object(pos, ObjectType::BANANA);
			break;
		case 3:
			switch (NewPosition)
			{
			case 1:
				pos.x = 100;
				pos.y = 215;
				break;
			case 2:
				pos.x = 220;
				pos.y = 175;
				break;
			case 3:
				pos.x = 60;
				pos.y = 135;
				break;
			case 4:
				pos.x = 160;
				pos.y = 95;
				break;
			default:
				LOG("No position found.");
				return;
			}
			obj = new Object(pos, ObjectType::ICE_CREAM);
			break;
		case 4:
			switch (NewPosition)
			{
			case 1:
				pos.x = 110;
				pos.y = 215;
				break;
			case 2:
				pos.x = 210;
				pos.y = 175;
				break;
			case 3:
				pos.x = 75;
				pos.y = 135;
				break;
			case 4:
				pos.x = 170;
				pos.y = 95;
				break;
			default:
				LOG("No position found.");
				return;
			}
			obj = new Object(pos, ObjectType::FLAN);
			break;
		case 5:
			switch (NewPosition)
			{
			case 1:
				pos.x = 80;
				pos.y = 215;
				break;
			case 2:
				pos.x = 180;
				pos.y = 175;
				break;
			case 3:
				pos.x = 25;
				pos.y = 135;
				break;
			case 4:
				pos.x = 130;
				pos.y = 95;
				break;
			default:
				LOG("No position found.");
				return;
			}
			obj = new Object(pos, ObjectType::CAKE);
			break;
		case 6:
			switch (NewPosition)
			{
			case 1:
				pos.x = 120;
				pos.y = 215;
				break;
			case 2:
				pos.x = 190;
				pos.y = 175;
				break;
			case 3:
				pos.x = 65;
				pos.y = 135;
				break;
			case 4:
				pos.x = 95;
				pos.y = 95;
				break;
			default:
				LOG("No position found.");
				return;
			}
			obj = new Object(pos, ObjectType::CHERRY);
			break;
		default:
			LOG("No object found.");
			return;
		}
		if (obj != nullptr)
		{
			objects.push_back(obj);
		}
	}
}
void Scene::Update()
{
	Point p1, p2;
	AABB box;
	
	

	//Switch between the different debug modes: off, on (sprites & hitboxes), on (hitboxes) 
	if (IsKeyPressed(KEY_F1))
	{
		debug = (DebugMode)(((int)debug + 1) % (int)DebugMode::SIZE);
	}
	//Debug levels instantly
	if (IsKeyPressed(KEY_ONE))
	{
		LoadLevel(1);
		currentStage = 1;
	}
	else if (IsKeyPressed(KEY_TWO))
	{
		LoadLevel(2);
		currentStage = 2;
	}
	else if (IsKeyPressed(KEY_THREE))
	{
		LoadLevel(3);
		currentStage = 3;
	}
	else if (IsKeyPressed(KEY_FOUR))
	{
		LoadLevel(4);
		currentStage = 4;
	}
	else if (IsKeyPressed(KEY_FIVE))
	{
		LoadLevel(5);
		currentStage = 5;
	}

	for (Bubble* buble : bubbles)
	{
		buble->SetPlayer(player);
	}

	level->Update();
	player->Update();
	EnemyUpdate();
	UpdateBubbles();

	ObjectTime += GetFrameTime();
	if (ObjectTime >= 9.0f) 
	{
		RandomItem();
		ObjectTime = 0;
	}

	CheckCollisions();
	BubbleShoot();
	
	//NextStage Manager
	if (enemiesDefeat == 0)
	{
		TimerStage += GetFrameTime();
		if (TimerStage >= 3.0f)
		{
			currentStage += 1;
			switch (currentStage)
			{
			case 2:
				LoadLevel(2);
				break;
			case 3:
				LoadLevel(3);
				break;
			case 4:
				LoadLevel(4);
				break;
			case 5:
				LoadLevel(5);
				break;
			default:
				break;
			}
			TimerStage = 0;
		}
		
	}

	auto as = bubbles.begin();
	while (as != bubbles.end())
	{
		if (!(*as)->isBubbleAlive())
		{
			delete* as;
			//Erase the object from the vector and get the iterator to the next valid element
			as = bubbles.erase(as);
		}
		else
		{
			//Move to the next object
			++as;
		}
	}

	auto dr = supers.begin();
	while (dr != supers.end())
	{
		if (drunklifes == 0)
		{
			delete* dr;
			//Erase the object from the vector and get the iterator to the next valid element
			dr = supers.erase(dr);
		}
		else
		{
			//Move to the next object
			++dr;
		}
	}
	
}
void Scene::Render()
{
	BeginMode2D(camera);

    level->Render();
	if (debug == DebugMode::OFF || debug == DebugMode::SPRITES_AND_HITBOXES)
	{
		RenderObjects(); 
		player->Draw();
		RenderEnemies();
	}
	if (debug == DebugMode::SPRITES_AND_HITBOXES || debug == DebugMode::ONLY_HITBOXES)
	{
		RenderObjectsDebug(YELLOW);
		player->DrawDebug(GREEN);
		RenderEnemiesDebug(RED);
	}

	EndMode2D();

	RenderGUI();
}
void Scene::Release()
{
    level->Release();
	player->Release();
	ClearLevel();
}
void Scene::CheckCollisions()
{
	AABB player_box, obj_box, enemy_box, bubble_box, DrunkBox;
	
	player_box = player->GetHitbox();
	auto it = objects.begin();
	while (it != objects.end())
	{
		obj_box = (*it)->GetHitbox();
		if(player_box.TestAABB(obj_box))
		{
			player->IncrScore((*it)->Points());
			
			//Delete the object
			delete* it; 
			//Erase the object from the vector and get the iterator to the next valid element
			it = objects.erase(it); 
		}
		else
		{
			//Move to the next object
			++it; 
		}
	}
	auto en = enemies.begin();
	while (en != enemies.end())
	{
		enemy_box = (*en)->GetHitbox();
		if (player_box.TestAABB(enemy_box)&&player->GetState()!=State::DEAD&& (*en)->GetState() != E_State::BUBBLED)
		{
			player->GetHit();
		}
		else
		{
			//Move to the next object
			++en;
		}
	}
	auto dr = supers.begin();
	while (dr != supers.end())
	{
		DrunkBox = (*dr)->GetHitbox();
		if (player_box.TestAABB(DrunkBox) && player->GetState() != State::DEAD)
		{
			player->GetHit();
		}
		else
		{
			//Move to the next object
			++dr;
		}
	}
	auto as = bubbles.begin();
	while (as != bubbles.end())
	{
		bubble_box = (*as)->GetHitbox();
		if (bubble_box.TestAABB(enemy_box))
		{
			delete* as;
			//Erase the object from the vector and get the iterator to the next valid element
			as = bubbles.erase(as);
		}
		else if (bubble_box.TestAABB(DrunkBox))
		{
			delete* as;
			//Erase the object from the vector and get the iterator to the next valid element
			as = bubbles.erase(as);
			drunklifes--;
		}
		else
		{
			//Move to the next object
			++as;
		}
	}
	auto yp = enemies.begin();
	while (yp != enemies.end())
	{
		enemy_box = (*yp)->GetHitbox();
		if (bubble_box.TestAABB(enemy_box)&& (*yp)->GetState() != E_State::BUBBLED)
		{
			(*yp)->Bubbler();
		}
		else if (player_box.TestAABB(enemy_box) && (*yp)->GetState() == E_State::BUBBLED)
		{
			delete* yp;
			//Erase the object from the vector and get the iterator to the next valid element
			yp = enemies.erase(yp);
			--enemiesDefeat;
		}
		else
		{
			//Move to the next object
			++yp;
		}
	}
}
void Scene::ClearLevel()
{
	for (Object* obj : objects)
	{
		delete obj;
	}
	objects.clear();
	for (Enemy* enem : enemies)
	{
		delete enem;
	}
	enemies.clear();
	for (Drunk* super : supers)
	{
		delete super;
	}
	supers.clear();
	for (Bubble* buble : bubbles)
	{
		delete buble;
	}
	bubbles.clear();
}
void Scene::UpdateBubbles()
{
	for (Bubble* buble : bubbles)
	{
		buble->Update();
	}
}
void Scene::EnemyUpdate() const
{
	for (Enemy* enem : enemies)
	{
		enem->Update();
	}
	for (Drunk* super : supers)
	{
		super->Update();
	}
}
void Scene::RenderObjects() const
{
	for (Object* obj : objects)
	{
		obj->Draw();
	}
	for (Bubble* bubl : bubbles)
	{
		bubl->Draw();
	}
	auto it = bubbles.begin();
	while (it != bubbles.end())
	{

		(*it)->Draw();
		++it;
	}
}
void Scene::RenderEnemies() const
{
	for (Enemy* enem : enemies)
	{
		enem->Draw();
	}
	for (Drunk* super : supers)
	{
		super->Draw();
	}
}
void Scene::RenderObjectsDebug(const Color& col) const
{
	for (Object* obj : objects)
	{
		obj->DrawDebug(col);
	}
}
void Scene::RenderEnemiesDebug(const Color& col) const
{
	for (Enemy* enem : enemies)
	{
		enem->DrawDebug(col);
	}
	for (Drunk* super : supers)
	{
		super->DrawDebug(col);
	}
}
void Scene::RenderGUI() const
{
	//Player 1
	font1->Draw(35, 10, TextFormat("1UP"), GREEN);
	font1->Draw(38, 20, TextFormat("%d", player->GetScore()));
	int casos = player->GetLifes()-1;

	if(casos == 1)
		font1->Draw(0, 232, TextFormat("%$"));
	else if(casos == 2)
	{
		font1->Draw(0, 232, TextFormat("%$#"));
	}
	else if(casos == 3)
	{
		font1->Draw(0, 232, TextFormat("%$#+"));
	}
	
	//Player 2
	font1->Draw(204, 10, TextFormat("2UP"), BLUE);
	font1->Draw(210, 20, TextFormat("0"));
	//font1->Draw(245, 220, TextFormat("%d", player->GetLifes()), BLUE);

	//High Score
	font1->Draw(90, 10, TextFormat("HIGH SCORE"), RED);
	font1->Draw(110, 20, TextFormat("%d", player->GetScore()));
}
Player* Scene::GetPlayer()
{
	return player;
}
bool Scene::WinCondition()
{
	if (drunklifes <= 0)
	{
		return true;
	}
	else
	{
		return false;
	}
}