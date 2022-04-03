#pragma once

#include "GameEngine.h"
#include "Scene_Play.h"

class Scene_Menu : public Scene
{
public:
	Scene_Menu(GameEngine*	gameEngine);
	void					init();
	void					onEnd();
	void					update();
	void					sDoAction(const Action& action);
	void					sRender();

private:
	std::string						m_title;
	std::vector<std::string>		m_menuStrings;
	std::vector<std::string>		m_levelPaths;
	sf::Text						m_menuText;
	int								m_selectedMenuIndex = 0;
	sf::Sprite						sky;
	sf::Clock						clock;
	std::vector<sf::Vector2f>		m_posMenuStrings;
	sf::Text						myName;
};