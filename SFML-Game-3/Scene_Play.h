#pragma once
#include "Scene.h"

class Scene_Play : public Scene
{
private:

	struct PlayerConfig
	{
		float X, Y, CX, CY, SPEED, MAXSPEED, JUMP, GRAVITY;
		std::string WEAPON;
	};

protected:
	
	std::shared_ptr<Entity>	m_player;
	std::string				m_levelPath;
	PlayerConfig			m_playerConfig;
	bool					m_drawTexture = true;
	bool					m_drawCollision = false;
	bool					m_drawGrid = false;
	const Vec2				m_gridSize = { 64.0f, 64.0f };
	sf::Text				m_gridText;
	int						m_scoreCntr = 0;
	int						m_coinCntr = 0;

	sf::CircleShape			m_mouseShape;
	sf::View defView;

	sf::Sprite				sky;
	sf::Sprite				scoreBoard;
	bool					m_end = false;
	sf::Text				m_displayTxt;

	void init(const std::string& levelPath);

	void loadLevel(const std::string& fileName);

	void update();
	void onEnd();

public:
	Scene_Play(GameEngine* gameEngine, const std::string& levelPath);
	Vec2 gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity);
	Vec2 midPixelToGrid(Vec2 pos, std::shared_ptr<Entity> entity);
	void spawnPlayer();
	void hitBlock(std::shared_ptr<Entity> entity);
	void spawnBullet(std::shared_ptr<Entity> e);
	void sMovement();
	void sLifespan();
	void sCollision();
	void sAnimation();
	void sDoAction(const Action& action);
	void drawLine(const Vec2& p1, const Vec2& p2);
	void sRender();
	void flipAndDie(std::shared_ptr<Entity> e);
	void flatAndDie(std::shared_ptr<Entity> e);
	void playerSpawnAtNearPoint();
	bool isInside(const Vec2& pos, std::shared_ptr<Entity> e);
	void spawnMediBult(std::shared_ptr<Entity> e);
	void spawnMediGrow(std::shared_ptr<Entity> e);
};