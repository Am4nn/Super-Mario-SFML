#pragma once
#include "Common.h"
#include "Scene.h"
#include "Physics.h"

class Scene_Edit : public Scene
{
	struct PlayerConfig
	{
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};
	PlayerConfig			m_playerConfig;
	
	std::shared_ptr<Entity>	m_player;
	std::string				m_levelPath;
	sf::CircleShape			m_mouseShape;
	EntityManager			m_entityManager;
	bool					m_drawTexture = true;
	bool					m_drawCollision = false;
	bool					m_drawGrid = false;
	sf::Text				m_gridText;
	const Vec2				m_gridSize = { 64.0f, 64.0f };
	bool					m_somethingDragging = false;

	//

	std::map<std::string, sf::Sprite>	dragMap;
	std::map<std::string, std::string>	dragMap_type;
	sf::Sprite sky;
	sf::Sprite crossX;
	sf::Sprite end;
	bool m_end = false;

	//

public:
	Scene_Edit(GameEngine* gameEngine, const std::string& levelPath);
	void init(const std::string& levelPath);
	void loadLevel(const std::string levelPath);
	void spawnPlayer();
	void update();
	void sAnimation();

	void sMovement();
	void sLifespan();
	void sCollision();
	void sDoAction(const Action& action);
	void sRender();
	void onEnd();

	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	Vec2 midPixelToGrid(Vec2 pos, std::shared_ptr<Entity> entity);
	bool isInside(const Vec2& pos, std::shared_ptr<Entity> e);
	void drawLine(const Vec2& p1, const Vec2& p2);
	void hitBlock(std::shared_ptr<Entity> e);
	void spawnBullet(std::shared_ptr<Entity> e);
};