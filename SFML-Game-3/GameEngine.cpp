#include "GameEngine.h"
#include "Assests.h"
#include "Scene_Menu.h"
#include "Scene_Play.h"

void GameEngine::init(const std::string& path)
{
	m_assests.loadFromFile(path);

	m_window.create(sf::VideoMode(1280, 768), "Mario - in the Cataclysmic World");
	m_window.setFramerateLimit(60);

	if (!sf::Shader::isAvailable())
	{
		std::cout << "Error : shaders not available !\n";
	}

	changeScene("MENU", std::make_shared<Scene_Menu>(this));
}

void GameEngine::update()
{
	if (!isRunning()) { return; }
	if (m_sceneMap.empty()) { return; }

	sUserInput();
	currentScene()->simulate(m_simulationSpeed);
	currentScene()->sRender();
	m_window.display();
}

void GameEngine::sUserInput()
{
	sf::Event event;
	while (m_window.pollEvent(event))
	{
		if (event.type == sf::Event::Closed)
		{
			quit();
		}

		if (event.type == sf::Event::KeyPressed)
		{
			if (event.key.code == sf::Keyboard::X)
			{
				sf::Texture ss;
				ss.create(m_window.getSize().x, m_window.getSize().y);
				ss.update(m_window);

				std::ifstream current_name("res//Screenshots//current_name.txt");
				size_t current_name_uint;
				current_name >> current_name_uint;
				std::stringstream ss_path;
				ss_path << "res//Screenshots//game_screenshot" << current_name_uint << ".png";
				current_name.close();
				std::ofstream current_name_override("res//Screenshots//current_name.txt");
				current_name_override << (current_name_uint + 1);

				if (ss.copyToImage().saveToFile(ss_path.str()))
				{
					std::cout << "Screenshot saved to " << ss_path.str() << std::endl;
				}
			}
		}

		if (event.type == sf::Event::KeyPressed || event.type == sf::Event::KeyReleased)
		{
			if (!currentScene()->getActionMap().count(event.key.code)) { continue; }

			// START == KeyPressed , END == KeyReleased
			const std::string actionType = (event.type == sf::Event::KeyPressed) ? "START" : "END";

			// look up the action and send action to the scene
			currentScene()->doAction(Action(currentScene()->getActionMap().at(event.key.code), actionType));
		}

		// Mouse Action
		auto mpos = sf::Mouse::getPosition(m_window);
		Vec2 pos(mpos.x, mpos.y);
		if (event.type == sf::Event::MouseButtonPressed)
		{
			switch (event.mouseButton.button)
			{
				case sf::Mouse::Left: {currentScene()->doAction(Action("LEFT_CLICK", "START", pos)); break; }
				case sf::Mouse::Middle: {currentScene()->doAction(Action("MIDDLE_CLICK", "START", pos)); break; }
				case sf::Mouse::Right: {currentScene()->doAction(Action("RIGHT_CLICK", "START", pos)); break; }
				default: break;
			}
		}

		if (event.type == sf::Event::MouseButtonReleased)
		{
			switch (event.mouseButton.button)
			{
			case sf::Mouse::Left: {currentScene()->doAction(Action("LEFT_CLICK", "END", pos)); break; }
			case sf::Mouse::Middle: {currentScene()->doAction(Action("MIDDLE_CLICK", "END", pos)); break; }
			case sf::Mouse::Right: {currentScene()->doAction(Action("RIGHT_CLICK", "END", pos)); break; }
			default: break;
			}
		}

		if (event.type == sf::Event::MouseMoved)
		{
			currentScene()->doAction(Action("MOUSE_MOVE", Vec2((float)event.mouseMove.x, (float)event.mouseMove.y)));
		}
	}
}

std::shared_ptr<Scene> GameEngine::currentScene()
{
	return m_sceneMap[m_currentScene];
}

GameEngine::GameEngine(const std::string& path)
{
	init(path);
}

void GameEngine::changeScene(const std::string& sceneName, std::shared_ptr<Scene> scene, bool endCurrentScene)
{
	if (scene)
	{
		m_sceneMap[sceneName] = scene;
	}
	else
	{
		if (m_sceneMap.find(sceneName) == m_sceneMap.end())
		{
			std::cerr << "Warning : Scene does not exists : " << sceneName << std::endl;
			return;
		}
	}

	if (endCurrentScene)
	{
		m_sceneMap.erase(m_sceneMap.find(m_currentScene));
	}

	m_currentScene = sceneName;
	std::cout << "Current scene : " << m_currentScene << std::endl << std::endl;
}

void GameEngine::quit()
{
	m_running = false;
}

void GameEngine::run()
{
	while (m_running)
	{
		update();
	}
}

sf::RenderWindow& GameEngine::window()
{
	return m_window;
}

Assests& GameEngine::assests()
{
	return m_assests;
}

bool GameEngine::isRunning()
{
	return m_running && m_window.isOpen();
}