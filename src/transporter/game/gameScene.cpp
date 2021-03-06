#include "transporter.h"

GameScene::GameScene()
{
	game = NULL;
	surface = NULL;
	car = NULL;
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧

GameScene::~GameScene()
{
	cleanUp();
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧

bit GameScene::init( Game* game )
{
	this->game = game;	
	return true;
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧

void GameScene::cleanUp()
{
	if(car)
	{
		delete car;
		car = NULL;
	}
	if(surface)
	{
		delete surface;
		surface = NULL;
	}
	physicsWorld.cleanUp();
	visualWorld.cleanUp();
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧

void GameScene::createEntities()
{
	surface = new Surface(game);
	surface->init("surface");

	car     = new CarEntity(surface);
	car->init("car");
}