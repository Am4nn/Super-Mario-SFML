#include "Scene_Menu.h"
#include "Scene_Edit.h"

Scene_Menu::Scene_Menu(GameEngine* gameEngine)
	: Scene(gameEngine)
{
	init();
}

void Scene_Menu::init()
{
	registerAction(sf::Keyboard::W, "UP");
	registerAction(sf::Keyboard::A, "UP");
	registerAction(sf::Keyboard::Up, "UP");
	registerAction(sf::Keyboard::Left, "UP");
	registerAction(sf::Keyboard::S, "DOWN");
	registerAction(sf::Keyboard::D, "DOWN");
	registerAction(sf::Keyboard::Down, "DOWN");
	registerAction(sf::Keyboard::Right, "DOWN");
	registerAction(sf::Keyboard::Enter, "PLAY");
	registerAction(sf::Keyboard::Escape, "QUIT");

	m_title = "Mario";
	m_menuStrings.push_back("Level 1");
	m_menuStrings.push_back("Level 2");
	m_menuStrings.push_back("Level 3");
	m_menuStrings.push_back("Load Level");
	m_menuStrings.push_back("Create Level");

	m_levelPaths.push_back("res/levels/level1.txt");
	m_levelPaths.push_back("res/levels/level2.txt");
	m_levelPaths.push_back("res/levels/level3.txt");
	m_levelPaths.push_back("res/save_level/saved_level.txt");
	m_levelPaths.push_back("res/create_level/saved_level.txt");

	auto a = m_game->assests().getAnimation("Castle");
	sky = a.getSprite();
	sky.setScale(1.2f, 1.2f);

	m_posMenuStrings.push_back(sf::Vector2f(10.0f + 0 * 250.0f, 110.0f + 1.3 * 100.0f));
	m_posMenuStrings.push_back(sf::Vector2f(10.0f + 1 * 250.0f, 110.0f + 1.5 * 100.0f));
	m_posMenuStrings.push_back(sf::Vector2f(10.0f + 2 * 250.0f, 110.0f + 1.7 * 100.0f));
	m_posMenuStrings.push_back(sf::Vector2f(10.0f, 110.0f + 3 * 100.0f));
	m_posMenuStrings.push_back(sf::Vector2f(10.0f, 110.0f + 4 * 100.0f));

	myName.setCharacterSize(75);
	myName.setString("- Aman Arya");
	myName.setPosition(880.0f, 110.0f + 3.5f * 100.0f);
	myName.setFont(m_game->assests().getFont("Name"));
	myName.setFillColor(sf::Color(255, 99, 71));

	std::cout << "\nMENU scene created\n";
}

void Scene_Menu::update()
{
	m_entityManager.update();
}

void Scene_Menu::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		if (action.name() == "UP")
		{
			if (m_selectedMenuIndex > 0) { m_selectedMenuIndex--; }
			else { m_selectedMenuIndex = m_menuStrings.size() - 1; }
		}
		else if (action.name() == "DOWN")
		{
			m_selectedMenuIndex = (m_selectedMenuIndex + 1) % m_menuStrings.size();
		}
		else if (action.name() == "PLAY")
		{
			if (m_menuStrings[m_selectedMenuIndex] == "Create Level") 
			{
				m_game->changeScene("EDIT", std::make_shared<Scene_Edit>(m_game, m_levelPaths[m_selectedMenuIndex]));
			}
			else
			{
				m_game->changeScene("PLAY", std::make_shared<Scene_Play>(m_game, m_levelPaths[m_selectedMenuIndex]));
			}
		}
		else if (action.name() == "QUIT")
		{
			onEnd();
		}
	}
}

void Scene_Menu::sRender()
{
	// clear the window to a blue
	m_game->window().setView(m_game->window().getDefaultView());
	m_game->window().clear(sf::Color(100, 100, 255));

	sky.setPosition(width() / 2.0f, height() / 2.0f); 
	m_game->assests().getFragShader_ptr("BlackSun")->setUniform("time", clock.getElapsedTime().asSeconds() - 2.0f);
	m_game->window().draw(sky, m_game->assests().getFragShader_ptr("BlackSun"));

	// draw the game title in top - left of the screen
	m_menuText.setCharacterSize(110);
	m_menuText.setFont(m_game->assests().getFont("Title"));
	m_menuText.setString(m_title);
	m_menuText.setFillColor(sf::Color(255, 223, 79));
	m_menuText.setPosition(sf::Vector2f(450.0f, 20.0f));
	m_game->window().draw(m_menuText);

	m_menuText.setFont(m_game->assests().getFont("Menu"));
	m_menuText.setCharacterSize(48);
	// draw all of the menu options
	for (size_t i = 0; i < m_menuStrings.size(); i++)
	{
		m_menuText.setString(m_menuStrings[i]);
		m_menuText.setFillColor(i == m_selectedMenuIndex ? sf::Color::White : sf::Color(5,25,20,255));
		m_menuText.setPosition(m_posMenuStrings[i]);
		m_game->window().draw(m_menuText);
	}

	// draw the controls in the bottom - left
	m_menuText.setCharacterSize(30);
	m_menuText.setFillColor(sf::Color::Cyan);
	m_menuText.setString("move : ' w '  ' a '  ' s '  ' d '      |      play : ' enter '      |      back : ' esc '");
	m_menuText.setPosition(sf::Vector2f(10.0f, 700.0f));
	m_game->window().draw(m_menuText);


	m_game->window().draw(myName);
}

void Scene_Menu::onEnd()
{
	m_hasEnded = true;
	m_game->quit();
}