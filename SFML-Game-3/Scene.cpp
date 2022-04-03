#include "Scene.h"

Scene::Scene(GameEngine* gameEngine)
{
	m_game = gameEngine;
}

void Scene::simulate(int simulationSpeed)
{
	// TODO::
	if (!m_hasEnded) { update(); }
}

void Scene::registerAction(int keyCode, const std::string& action)
{
	// TODO::
	m_action_map[keyCode] = action;
}

void Scene::doAction(const Action& action)
{
	// TODO::
	sDoAction(action);
}

std::map<int, std::string> Scene::getActionMap()
{
	return m_action_map;
}

void Scene::setPaused(bool paused)
{
	m_paused = paused;
}

size_t Scene::width() const
{
	return m_game->window().getSize().x;
}

size_t Scene::height() const
{
	return m_game->window().getSize().y;
}

size_t Scene::currentFrame() const
{
	return m_currentFrame;
}