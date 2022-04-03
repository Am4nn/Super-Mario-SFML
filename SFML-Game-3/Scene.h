#pragma once
#include "Common.h"
#include "EntityManager.h"
#include "Action.h"
#include "GameEngine.h"

class GameEngine;

class Scene
{
public:
	GameEngine*					m_game;
	EntityManager				m_entityManager;
	size_t						m_currentFrame = 0;
	std::map<int, std::string>	m_action_map;
	bool						m_paused = false;
	bool						m_hasEnded = false;

	virtual void onEnd() = 0;
	void setPaused(bool paused);

	Scene(GameEngine* gameEngine);

	virtual void update() = 0;
	virtual void sRender() = 0;
	virtual void sDoAction(const Action& action) = 0;
	
	std::map<int, std::string> getActionMap();
	void doAction(const Action& action);
	void simulate(int simulationSpeed);
	void registerAction(int keyCode ,const std::string& action);

	size_t height() const;
	size_t width() const;
	size_t currentFrame() const;
};