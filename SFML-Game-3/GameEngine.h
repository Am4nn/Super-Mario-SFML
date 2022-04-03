#pragma once

#include "Common.h"
#include "Scene.h"
#include "Assests.h"

class Scene;

using SceneMap = std::map<std::string , std::shared_ptr<Scene>>;


class GameEngine
{
protected:
	sf::RenderWindow	m_window;
	Assests				m_assests;
	std::string			m_currentScene;
	SceneMap			m_sceneMap;
	size_t				m_simulationSpeed = 1;
	bool				m_running = true;

	void init(const std::string& path);
	void update();

	void sUserInput();

	std::shared_ptr<Scene> currentScene();

public:

	GameEngine(const std::string& path);

	void changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene = false);

	void quit();
	void run();

	sf::RenderWindow& window();
	Assests& assests();
	bool isRunning();
};