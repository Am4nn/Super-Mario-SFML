#include "Scene_Edit.h"

Scene_Edit::Scene_Edit(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Edit::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::W, "JUMP");
	registerAction(sf::Keyboard::A, "LEFT");
	registerAction(sf::Keyboard::D, "RIGHT");
	registerAction(sf::Keyboard::P, "PAUSE");
	registerAction(sf::Keyboard::Space, "SHOOT");
	registerAction(sf::Keyboard::Escape, "QUIT");
	registerAction(sf::Keyboard::T, "TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C, "TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G, "TOGGLE_GRID");
	registerAction(sf::Keyboard::Z, "SAVE_LEVEL");

	m_mouseShape.setRadius(8);
	m_mouseShape.setOrigin(8, 8);
	m_mouseShape.setFillColor(sf::Color(255, 0, 0, 190));

	m_gridText.setCharacterSize(14);
	m_gridText.setFont(m_game->assests().getFont("Arial"));

	//

	
	auto a = m_game->assests().getAnimation("Sky");
	sky = a.getSprite();
	sky.setRotation(0.0f);
	sky.setScale(1.2f, 1.2f);

	a = m_game->assests().getAnimation("CrossX");
	crossX = a.getSprite();
	crossX.setRotation(0.0f);
	crossX.setScale(1.0f, 1.0f);

	a = m_game->assests().getAnimation("Ground");
	dragMap["Ground"] = a.getSprite();
	dragMap_type["Ground"] = "tile";

	a = m_game->assests().getAnimation("Block");
	dragMap["Block"] = a.getSprite();
	dragMap_type["Block"] = "tile";

	a = m_game->assests().getAnimation("Question");
	dragMap["Question"] = a.getSprite();
	dragMap_type["Question"] = "tile";

	a = m_game->assests().getAnimation("Brick");
	dragMap["Brick"] = a.getSprite();
	dragMap_type["Brick"] = "tile";

	a = m_game->assests().getAnimation("PipeTall");
	dragMap["PipeTall"] = a.getSprite();
	dragMap_type["PipeTall"] = "tile";
	dragMap["PipeTall"].setScale(1.0f, 0.5f);

	a = m_game->assests().getAnimation("PipeSmall");
	dragMap["PipeSmall"] = a.getSprite();
	dragMap_type["PipeSmall"] = "tile";

	a = m_game->assests().getAnimation("CloudBig");
	dragMap["CloudBig"] = a.getSprite();
	dragMap_type["CloudBig"] = "dec";
	dragMap["CloudBig"].setScale(0.4f, 0.5f);

	a = m_game->assests().getAnimation("CloudSmall");
	dragMap["CloudSmall"] = a.getSprite();
	dragMap_type["CloudSmall"] = "dec";
	dragMap["CloudSmall"].setScale(0.9f, 1.0f);

	a = m_game->assests().getAnimation("Bush");
	dragMap["Bush"] = a.getSprite();
	dragMap_type["Bush"] = "dec";
	dragMap["Bush"].setScale(0.4f, 0.5f);

	a = m_game->assests().getAnimation("NPC_conics");
	dragMap["NPC_conics"] = a.getSprite();
	dragMap_type["NPC_conics"] = "NPC_conics";
	dragMap["NPC_conics"].setScale(1.0f, 1.0f);

	a = m_game->assests().getAnimation("PipeMid");
	dragMap["PipeMid"] = a.getSprite();
	dragMap_type["PipeMid"] = "tile";

	std::cout << levelPath;

	loadLevel(levelPath);
}

bool Scene_Edit::isInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	if (!e->hasComponent<CAnimation>()) { return false; }

	// half size of animation
	auto halfSize = e->getComponent<CAnimation>().animation.getSize() / 2.0f;

	// delta value between both the enemy
	Vec2 delta = (e->getComponent<CTransform>().pos - pos).abs();

	// if click is within x and y bounds of half size then we are inside
	return (delta.x <= halfSize.x) && (delta.y <= halfSize.y);
}

bool isInsideSp(const Vec2& pos, sf::Sprite& e)
{
	// half size of animation
	auto halfSize = Vec2(64.0f, 64.0f) / 2.0f;

	// delta value between both the enemy
	Vec2 delta = (Vec2(e.getPosition().x, e.getPosition().y) - pos).abs();

	// if click is within x and y bounds of half size then we are inside
	return (delta.x <= halfSize.x) && (delta.y <= halfSize.y);
}

Vec2 Scene_Edit::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float b1x = gridX * m_gridSize.x;
	float b1y = height() - (gridY * m_gridSize.y);
	Vec2 size = entity->getComponent<CAnimation>().animation.getSize();
	return Vec2(b1x + size.x / 2.0f, b1y - size.y / 2.0f);
}

Vec2 Scene_Edit::midPixelToGrid(Vec2 pos, std::shared_ptr<Entity> entity)
{
	Vec2 size = entity->getComponent<CAnimation>().animation.getSize();
	float realX = pos.x - size.x / 2.0f, realY = pos.y + size.y / 2.0f;
	float b1x = realX / m_gridSize.x;
	float b1y = (height() - realY) / m_gridSize.y;
	return Vec2(b1x, b1y);
}



void Scene_Edit::loadLevel(const std::string levelPath)
{
	m_entityManager = EntityManager();
	std::ifstream file(levelPath);
	std::string str;

	float x, y;
	while (file.good())
	{
		file >> str;
		if (str == "Tile")
		{
			file >> str >> x >> y;

			if (str == "PipeMid" || str == "PipeSmall")
			{
				auto patrol = m_entityManager.addEntity("patrol_cutter");
				patrol->addComponent<CAnimation>(m_game->assests().getAnimation("Cutter"), true);
				patrol->addComponent<CTransform>(gridToMidPixel(x + 0.15f, y + m_game->assests().getAnimation(str).getSize().y / 64.0f, patrol));
				patrol->addComponent<CBoundingBox>(patrol->getComponent<CAnimation>().animation.getSize());
				std::vector<Vec2> cords;
				Vec2 pos = patrol->getComponent<CTransform>().pos;
				cords.push_back(Vec2(pos.x, pos.y));
				cords.push_back(Vec2(pos.x, pos.y + 2*patrol->getComponent<CAnimation>().animation.getSize().y));
				patrol->addComponent<CPatrol>(cords, 0, 1.0f);
				patrol->addComponent<CDraggable>();
			}

			auto tile = m_entityManager.addEntity("tile");
			tile->addComponent<CAnimation>(m_game->assests().getAnimation(str), true);
			tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));
			tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize());
			tile->addComponent<CDraggable>();
			if (str == "Brick")
			{
				std::string item;
				file >> item;
				tile->addComponent<CBrickItems>(item);
			}
		}
		else if (str == "Dec")
		{
			file >> str >> x >> y;
			auto tile = m_entityManager.addEntity("dec");
			tile->addComponent<CAnimation>(m_game->assests().getAnimation(str), true);
			tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));
			tile->addComponent<CDraggable>();
		}
		else if (str == "NPC_conics")
		{
			float speed;
			file >> speed >> str >> x >> y;
			auto npc = m_entityManager.addEntity("npc_conics");
			npc->addComponent<CAnimation>(m_game->assests().getAnimation(str), true);
			npc->addComponent<CTransform>(gridToMidPixel(x, y, npc));
			npc->addComponent<CBoundingBox>(npc->getComponent<CAnimation>().animation.getSize());
			npc->addComponent<CGravity>(0.75f);
			npc->addComponent<COneDirectional>(true, 5.0f);
			npc->addComponent<CDraggable>();

			npc->getComponent<CTransform>().velocity =   Vec2(speed, 0.0f) *  (npc->getComponent<COneDirectional>().dir ? 1.0f : -1.0f);
		}
		else if (str == "Player")
		{
			file >> m_playerConfig.X >> m_playerConfig.Y >> m_playerConfig.CX >> m_playerConfig.CY;
			file >> m_playerConfig.SPEED >> m_playerConfig.JUMP >> m_playerConfig.MAXSPEED >> m_playerConfig.GRAVITY >> m_playerConfig.WEAPON;
			spawnPlayer();
		}
	}
	std::cout << "   loaded successfully\n";
}

void Scene_Edit::spawnPlayer()
{
	if (m_player) { m_player->destroy(); }

	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assests().getAnimation("Run"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->addComponent<CInput>();
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>("air");
	m_player->addComponent<CDraggable>();
}

void Scene_Edit::hitBlock(std::shared_ptr<Entity> e)
{
	if (e->getComponent<CAnimation>().animation.getName() == "Brick")
	{
		e->addComponent<CAnimation>(m_game->assests().getAnimation("Explosion"), false);
		e->removeComponent<CBoundingBox>();
	}
	else if (e->getComponent<CAnimation>().animation.getName() == "Question")
	{
		auto coin = m_entityManager.addEntity("coin");
		coin->addComponent<CTransform>(e->getComponent<CTransform>().pos + Vec2(0.0f, -64.0f));
		coin->addComponent<CAnimation>(m_game->assests().getAnimation("Coin"), true);
		coin->addComponent<CLifespane>(60, m_currentFrame);

		e->addComponent<CAnimation>(m_game->assests().getAnimation("Question2"), true);
	}
}

void Scene_Edit::spawnBullet(std::shared_ptr<Entity> e)
{
	auto& transform = e->getComponent<CTransform>();
	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CTransform>(transform.pos, Vec2(12 * transform.scale.x, 0), transform.scale, 0.0f);
	bullet->addComponent<CAnimation>(m_game->assests().getAnimation(m_playerConfig.WEAPON), true);
	bullet->addComponent<CBoundingBox>(m_game->assests().getAnimation(m_playerConfig.WEAPON).getSize());
	bullet->addComponent<CLifespane>(60, m_currentFrame);
}


void Scene_Edit::update()
{
	m_entityManager.update();

	if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();
		sAnimation();

		m_currentFrame++;
	}
}

void Scene_Edit::sMovement()
{
	auto& pTransform = m_player->getComponent<CTransform>();
	auto& pState = m_player->getComponent<CState>();
	Vec2 prevPlayerSpeed = pTransform.velocity;
	Vec2 playerInputSpeed(0.0f, pTransform.velocity.y);
	if (m_player->getComponent<CInput>().left)
	{
		playerInputSpeed.x -= m_playerConfig.SPEED;
		pTransform.scale.x = -1.0f;
	}
	if (m_player->getComponent<CInput>().right)
	{
		playerInputSpeed.x += m_playerConfig.SPEED;
		pTransform.scale.x = 1.0f;
	}
	if (m_player->getComponent<CInput>().up && pState.state != "air" && m_player->getComponent<CInput>().canJump)
	{
		playerInputSpeed.y = m_playerConfig.JUMP;
	}

	pTransform.velocity = playerInputSpeed;

	for (auto& npc : m_entityManager.getEntities("npc_conics"))
	{
		npc->getComponent<CTransform>().velocity.x = std::abs(npc->getComponent<CTransform>().velocity.x);
		npc->getComponent<CTransform>().velocity.x *= npc->getComponent<COneDirectional>().dir ? 1 : -1;
	}

	for (auto& patrol : m_entityManager.getEntities("patrol_cutter"))
	{
		if (patrol->getComponent<CTransform>().pos != patrol->getComponent<CPatrol>().vertex[patrol->getComponent<CPatrol>().currentPos]) { continue; }

		size_t& curr = patrol->getComponent<CPatrol>().currentPos;
		size_t next = (curr + 1) % patrol->getComponent<CPatrol>().vertex.size();
		Vec2 vel = patrol->getComponent<CPatrol>().vertex[next] - patrol->getComponent<CPatrol>().vertex[curr];
		vel.normalize() *= patrol->getComponent<CPatrol>().speed;
		patrol->getComponent<CTransform>().velocity = vel;

		curr = next;
	}

	for (auto& e : m_entityManager.getEntities())
	{
		auto& transform = e->getComponent<CTransform>();

		// gravity
		if (e->hasComponent<CGravity>())
		{
			transform.velocity.y += e->getComponent<CGravity>().gravity;
		}

		// cap the entity's speed so it doesn't go infinite
		if (transform.velocity.x < -m_playerConfig.MAXSPEED) { transform.velocity.x = -m_playerConfig.MAXSPEED; }
		if (transform.velocity.x > m_playerConfig.MAXSPEED) { transform.velocity.x = m_playerConfig.MAXSPEED; }
		if (transform.velocity.y < -m_playerConfig.MAXSPEED) { transform.velocity.y = -m_playerConfig.MAXSPEED; }
		if (transform.velocity.y > m_playerConfig.MAXSPEED) { transform.velocity.y = m_playerConfig.MAXSPEED; }

		// update entity position
		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
	}

	// shoot bullets
	if (m_player->getComponent<CInput>().shoot && m_player->getComponent<CInput>().canShoot)
	{
		spawnBullet(m_player);
		m_player->getComponent<CInput>().canShoot = false;
	}

	// update dragging entites
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CDraggable>() && e->getComponent<CDraggable>().dragging)
		{
			// set the position of the mouse position
			e->getComponent<CTransform>().pos = Vec2(m_mouseShape.getPosition().x, m_mouseShape.getPosition().y);
		}
	}
}

void Scene_Edit::sLifespan()
{
	for (auto& e : m_entityManager.getEntities())
	{
		if (e->hasComponent<CLifespane>())
		{
			auto& lifespan = e->getComponent<CLifespane>();
			int framesAlive = (m_currentFrame - lifespan.frameCreated);

			if (framesAlive >= lifespan.lifespan)
			{
				e->destroy();
			}
		}
	}
}

void Scene_Edit::sCollision()
{
	auto& tiles = m_entityManager.getEntities("tile");
	auto& bullets = m_entityManager.getEntities("bullet");
	auto& npcs = m_entityManager.getEntities("npc_conics");

	auto& pTransform = m_player->getComponent<CTransform>();
	auto& pBox = m_player->getComponent<CBoundingBox>();

	// patrol_cutter
	for (auto& patrol : m_entityManager.getEntities("patrol_cutter"))
	{
		if (!patrol->hasComponent<CBoundingBox>()) { continue; }

		// patrol collision with bullet
		for (auto& bullet : bullets)
		{
			Vec2 overlap = Physics::GetOverlap(bullet, patrol);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(bullet, patrol);

			if (overlap.x < 0 || overlap.y < 0) { continue; }

			patrol->destroy();
			bullet->destroy();
		}

		// patrol collision with player
		Vec2 overlap = Physics::GetOverlap(m_player, patrol);
		Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, patrol);

		if (overlap.x < 0 || overlap.y < 0)
		{
			if (overlap.x > 0 && patrol->getComponent<CTransform>().pos.y > patrol->getComponent<CPatrol>().vertex[0].y + patrol->getComponent<CAnimation>().animation.getSize().y)
			{
				patrol->getComponent<CTransform>().velocity = Vec2();
				patrol->getComponent<CPatrol>().currentPos = 1;
				patrol->getComponent<CTransform>().pos = patrol->getComponent<CPatrol>().vertex[patrol->getComponent<CPatrol>().currentPos];
			}
			continue;
		}

		if (prevOverlap.x > 0)
		{
			patrol->destroy();
		}
		else
		{
			spawnPlayer();
		}
	}

	// npc_conics
	for (auto& npc : npcs)
	{
		if (midPixelToGrid(npc->getComponent<CTransform>().pos, npc).x < 0)
		{
			npc->destroy();
		}

		// npc collision with player
		Vec2 overlap = Physics::GetOverlap(m_player, npc);
		Vec2 prevoverlap = Physics::GetPreviousOverlap(m_player, npc);

		if (overlap.x > 0 && overlap.y > 0)
		{
			Vec2 diff = m_player->getComponent<CTransform>().pos - npc->getComponent<CTransform>().pos;

			if (prevoverlap.x > 0 && diff.y < 0)
			{
				npc->destroy();
			}
			else
			{
				spawnPlayer();
			}
			break;
		}

		// npc collision with tiles
		for (auto& tile : tiles)
		{
			if (!tile->hasComponent<CBoundingBox>()) { continue; }

			Vec2 overlap = Physics::GetOverlap(npc, tile);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(npc, tile);
			
			if (overlap.x < 0 || overlap.y < 0) { continue; }

			Vec2 diff = npc->getComponent<CTransform>().pos - tile->getComponent<CTransform>().pos;
			Vec2 shift(0.0f, 0.0f);

			if (prevOverlap.x > 0)
			{
				shift.y += diff.y > 0 ? overlap.y : -overlap.y;
				npc->getComponent<CTransform>().velocity.y = 0;
				if (diff.y < 0)
				{
					npc->getComponent<CTransform>().pos += (tile->getComponent<CTransform>().velocity);
				}
			}
			else if (prevOverlap.y > 0)
			{
				npc->getComponent<COneDirectional>().dir = !npc->getComponent<COneDirectional>().dir;
			}

			npc->getComponent<CTransform>().pos += shift;
		}
	}

	// bullet
	for (auto& bullet : bullets)
	{
		for (auto& tile : tiles)
		{
			if (!tile->hasComponent<CBoundingBox>()) { continue; }

			Vec2 overlap = Physics::GetOverlap(bullet, tile);
			if (overlap.x > 0 && overlap.y > 0)
			{
				bullet->destroy();
				//tile->addComponent<CDraggable>();

				if (tile->getComponent<CAnimation>().animation.getName() == "Brick")
				{
					tile->addComponent<CAnimation>(m_game->assests().getAnimation("Explosion"), false);
					tile->removeComponent<CBoundingBox>();
				}
			}
		}
		
		// npc collision with bullets
		for (auto& npc : npcs)
		{
			Vec2 overlap = Physics::GetOverlap(bullet, npc);
			if (overlap.x > 0 && overlap.y > 0)
			{
				bullet->destroy();
				npc->destroy();
			}
		}
	}

	m_player->getComponent<CState>().state = "air";

	// player pos before it moved
	Vec2 prevPos = pTransform.pos - pTransform.velocity;

	// player collision
	size_t tileNum = 0;
	for (auto& tile : tiles)
	{
		auto& tTransform = tile->getComponent<CTransform>();
		tileNum++;

		// current overlap
		Vec2 overlap = Physics::GetOverlap(m_player, tile);
		Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

		if (overlap.x < 0 || overlap.y < 0) { continue; }

		if (tile->getComponent<CAnimation>().animation.getName() == "Pole" ||
			tile->getComponent<CAnimation>().animation.getName() == "PoleTop")
		{
			auto a = m_game->assests().getAnimation("TheEnd");
			end = a.getSprite();
			m_end = true;
			return;
		}

		Vec2 diff = pTransform.pos - tTransform.pos;
		Vec2 shift(0.0f, 0.0f);

		if (prevOverlap.x > 0)
		{
			shift.y += diff.y > 0 ? overlap.y : -overlap.y;
			pTransform.velocity.y = 0;
			if (diff.y < 0)
			{
				m_player->getComponent<CState>().state = "ground";
				pTransform.pos += (tTransform.velocity);
			}
			else
			{
				hitBlock(tile);
			}
		}

		else if (prevOverlap.y > 0)
		{
			shift.x += diff.x > 0 ? overlap.x : -overlap.x;
			pTransform.velocity.x = 0;
		}

		pTransform.pos += shift;
	}

	// check if player fallen down a hole
	if (m_player->getComponent<CTransform>().pos.y > height())
	{
		spawnPlayer();
	}

	// don't let the player exit the screen to the left
	if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
	}
}

void Scene_Edit::sDoAction(const Action& action)
{
	if (action.type() == "START")
	{
		auto& pInput = m_player->getComponent<CInput>();
		auto& pState = m_player->getComponent<CState>();
		auto& pTransform = m_player->getComponent<CTransform>();


		if (action.name() == "JUMP")
		{
			if (pState.state != "air" && m_player->getComponent<CInput>().canJump)
			{
				pTransform.velocity.y = m_playerConfig.JUMP;
			}
			m_player->getComponent<CInput>().canJump = false;
			m_player->getComponent<CInput>().up = true;
		}
		else if (action.name() == "LEFT") { pInput.left = true; }
		else if (action.name() == "DOWN") { pInput.down = true; }
		else if (action.name() == "RIGHT") { pInput.right = true; }
		else if (action.name() == "SHOOT") { pInput.shoot = true; }
		else if (action.name() == "TOGGLE_TEXTURE") { m_drawTexture = !m_drawTexture; }
		else if (action.name() == "TOGGLE_COLLISION") { m_drawCollision = !m_drawCollision; }
		else if (action.name() == "TOGGLE_GRID") { m_drawGrid = !m_drawGrid; }
		else if (action.name() == "PAUSE") { setPaused(!m_paused); }
		else if (action.name() == "QUIT") { onEnd(); }
		else if (action.name() == "LEFT_CLICK")
		{
			if (m_somethingDragging)
			{
				m_somethingDragging = false;
				for (auto& e : m_entityManager.getEntities())
				{
					if (e->getComponent<CDraggable>().dragging)
					{
						e->getComponent<CDraggable>().dragging = false;

						float xDiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
						float yDiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;

						float x = midPixelToGrid(e->getComponent<CTransform>().pos, e).x;
						float y = midPixelToGrid(e->getComponent<CTransform>().pos, e).y;

						// TODO
						if (x - (int)x <= 0.25) x = (int)x;
						else if (x - (int)x >= 0.75) x = (int)x + 1;
						else if (x - (int)x >= 0.35 && x - (int)x <= 0.65) x = (int)x + 0.5f;

						if (y - (int)y <= 0.25) y = (int)y;
						else if (y - (int)y >= 0.75) y = (int)y + 1;
						else if (y - (int)y >= 0.35 && y - (int)y <= 0.65) y = (int)y + 0.5f;

						e->getComponent<CTransform>().pos = gridToMidPixel(x, y, e);

						if (isInsideSp(action.pos() + Vec2(xDiff,  yDiff), crossX))
						{
							e->destroy();
						}

						break;
					}
				}
			}
			else
			{
				float xDiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
				float yDiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;

				// this is where we actually clicked inside our world
				Vec2 worldPos(action.pos().x + xDiff, action.pos().y + yDiff);

				// check if any entity was clicked
				for (auto& e : m_entityManager.getEntities())
				{
					// if entity is clicked
					if (e->hasComponent<CDraggable>() && isInside(worldPos, e))
					{
						// change its state from not dragging or vice versa
						e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
						m_somethingDragging = true;
						break;
					}
				}

				for (auto& e : dragMap)
				{
					if (m_somethingDragging) { break; }

					if (isInsideSp(worldPos, e.second))
					{
						if (dragMap_type[e.first] == "tile")
						{
							auto tile = m_entityManager.addEntity("tile");
							tile->addComponent<CAnimation>(m_game->assests().getAnimation(e.first), true);
							tile->addComponent<CTransform>(Vec2(e.second.getPosition().x, e.second.getPosition().y));
							tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize());
							tile->addComponent<CDraggable>();
							tile->getComponent<CDraggable>().dragging = true;
							m_somethingDragging = true;
						}
						else if (dragMap_type[e.first] == "dec")
						{
							auto tile = m_entityManager.addEntity("dec");
							tile->addComponent<CAnimation>(m_game->assests().getAnimation(e.first), true);
							tile->addComponent<CTransform>(Vec2(e.second.getPosition().x, e.second.getPosition().y));
							tile->addComponent<CDraggable>();
							tile->getComponent<CDraggable>().dragging = true;
							m_somethingDragging = true;
						}
						else if (dragMap_type[e.first] == "NPC_conics")
						{
							auto tile = m_entityManager.addEntity("npc_conics");
							tile->addComponent<CAnimation>(m_game->assests().getAnimation(e.first), true);
							tile->addComponent<CTransform>(Vec2(e.second.getPosition().x, e.second.getPosition().y));
							tile->addComponent<CDraggable>();
							tile->getComponent<CDraggable>().dragging = true;
							m_somethingDragging = true;
						}
					}
				}
			}
		}
		else if (action.name() == "SAVE_LEVEL")
		{
		std::stringstream sls;
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->tag() == "player") { continue; }

			if (e->tag() == "tile")
			{
				sls << "Tile	";
			}
			else if (e->tag() == "dec")
			{
				sls << "Dec		";
			}
			else if (e->tag() == "npc_conics")
			{
				sls << "NPC_conics		3       ";
			}
			float x = midPixelToGrid(e->getComponent<CTransform>().pos, e).x;
			float y = midPixelToGrid(e->getComponent<CTransform>().pos, e).y;

			sls << e->getComponent<CAnimation>().animation.getName() << "       "
				<< x << "       " << y;
			if (e->getComponent<CAnimation>().animation.getName() == "Brick")
			{
				sls << "		" << e->getComponent<CBrickItems>().itemAnimation;
			}
			sls << std::endl;
		}
		/*
		m_playerConfig.X
		m_playerConfig.Y
		m_playerConfig.CX
		m_playerConfig.CY
		m_playerConfig.SPEED
		m_playerConfig.JUMP
		m_playerConfig.MAXSPEED
		m_playerConfig.GRAVITY
		m_playerConfig.WEAPON
		*/

		sls << std::endl << "Player  "
			<< midPixelToGrid(m_player->getComponent<CTransform>().pos, m_player).x << " "
			<< midPixelToGrid(m_player->getComponent<CTransform>().pos, m_player).y << " "
			<< m_player->getComponent<CBoundingBox>().size.x << " "
			<< m_player->getComponent<CBoundingBox>().size.y << " "
			<< m_playerConfig.SPEED << " "
			<< m_playerConfig.JUMP << " "
			<< m_playerConfig.MAXSPEED << " "
			<< m_player->getComponent<CGravity>().gravity << " "
			<< m_playerConfig.WEAPON;


		std::ofstream sl("res/create_level/saved_level.txt");
		sl << sls.str();
		std::cout << "Level saved to " << "res/create_level/saved_level.txt" << std::endl;
		sl.close();
		}
	}
	else if (action.type() == "END")
	{
		auto& pInput = m_player->getComponent<CInput>();
		auto& pState = m_player->getComponent<CState>();
		auto& pTransform = m_player->getComponent<CTransform>();

		if (action.name() == "JUMP")
		{
			if (m_player->getComponent<CTransform>().velocity.y < 0)
			{
				pTransform.velocity.y = 0;
			}
			m_player->getComponent<CInput>().canJump = true;
			m_player->getComponent<CInput>().up = false;
		}
		else if (action.name() == "LEFT") { pInput.left = false; }
		else if (action.name() == "DOWN") { pInput.down = false; }
		else if (action.name() == "RIGHT") { pInput.right = false; }
		else if (action.name() == "SHOOT") { pInput.shoot = false; pInput.canShoot = true; }
	}

	if (action.name() == "MOUSE_MOVE")
	{
		float xDiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
		float yDiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;

		m_mouseShape.setPosition(action.pos().x + xDiff, action.pos().y + yDiff);
	}
}

void Scene_Edit::sAnimation()
{
	auto& pState = m_player->getComponent<CState>();
	auto& pTransform = m_player->getComponent<CTransform>();
	if (pState.state == "air")
	{
		m_player->addComponent<CAnimation>(m_game->assests().getAnimation("Air"), true);
	}
	else if (pState.state == "ground")
	{
		auto& input = m_player->getComponent<CInput>();
		if ((input.left || input.right) && !(input.left && input.right))
		{
			if (m_player->getComponent<CAnimation>().animation.getName() != "Run")
			{
				m_player->addComponent<CAnimation>(m_game->assests().getAnimation("Run"), true);
			}
		}
		else
		{
			m_player->addComponent<CAnimation>(m_game->assests().getAnimation("Stand"), true);
		}
	}

	for (auto& e : m_entityManager.getEntities())
	{
		if (!e->hasComponent<CAnimation>()) { continue; }

		auto& cAnimation = e->getComponent<CAnimation>();

		// if animation is not repeating then its completed , destroy the entity
		if (cAnimation.animation.hasEnded() && !cAnimation.repeat)
		{
			e->destroy();
		}
		else
		{
			cAnimation.animation.update();
		}
	}
}

void Scene_Edit::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	line->color = sf::Color(255, 0, 0, 200);
	m_game->window().draw(line, 2, sf::Lines);
}

void Scene_Edit::sRender()
{
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = fmax(m_game->window().getSize().x / 2.0f, pPos.x);
	sf::View view = m_game->window().getView();
	view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);
	m_game->window().setView(view);

	float xDiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
	float yDiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;
	if (!m_paused) {

		sky.setPosition(width() / 2.0f + xDiff, height() / 2.0f + yDiff);
		dragMap["Ground"].setPosition(xDiff + 32, yDiff + 34);
		dragMap["Block"].setPosition(xDiff + 32 + 66, yDiff + 34);
		dragMap["Question"].setPosition(xDiff + 32 + 66 * 2, yDiff + 34);
		dragMap["Brick"].setPosition(xDiff + 32 + 66 * 3, yDiff + 34);
		dragMap["PipeTall"].setPosition(xDiff + 32 + 66 * 4, yDiff + 34);
		dragMap["PipeSmall"].setPosition(xDiff + 32 + 66 * 5, yDiff + 34);
		dragMap["CloudBig"].setPosition(xDiff + 32 + 66 * 6, yDiff + 34);
		dragMap["CloudSmall"].setPosition(xDiff + 32 + 66 * 7, yDiff + 34);
		dragMap["Bush"].setPosition(xDiff + 32 + 66 * 8, yDiff + 34);
		dragMap["NPC_conics"].setPosition(xDiff + 32 + 66 * 9, yDiff + 34);
		dragMap["PipeMid"].setPosition(xDiff + 32 + 66 * 10, yDiff + 34);

		m_game->window().draw(sky);
		m_game->window().draw(dragMap["Ground"]);
		m_game->window().draw(dragMap["Block"]);
		m_game->window().draw(dragMap["Question"]);
		m_game->window().draw(dragMap["Brick"]);
		m_game->window().draw(dragMap["PipeTall"]);
		m_game->window().draw(dragMap["PipeSmall"]);
		m_game->window().draw(dragMap["CloudBig"]);
		m_game->window().draw(dragMap["CloudSmall"]);
		m_game->window().draw(dragMap["Bush"]);
		m_game->window().draw(dragMap["NPC_conics"]);
		m_game->window().draw(dragMap["PipeMid"]);
	}
	if (m_somethingDragging)
	{
		crossX.setPosition(xDiff + width() - 48.0f, yDiff + 46.0f);
		m_game->window().draw(crossX);
	}

	// draw all entity texture / animation
	if (m_drawTexture)
	{
		for (auto& e : m_entityManager.getEntities())
		{
			if (e->tag() == "player" || e->tag() == "tile") { continue; }

			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CTransform>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}

		for (auto& e : m_entityManager.getEntities("tile"))
		{
			auto& transform = e->getComponent<CTransform>();

			if (e->hasComponent<CTransform>())
			{
				auto& animation = e->getComponent<CAnimation>().animation;
				animation.getSprite().setRotation(transform.angle);
				animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
				animation.getSprite().setScale(transform.scale.x, transform.scale.y);
				m_game->window().draw(animation.getSprite());
			}
		}

		auto& transform = m_player->getComponent<CTransform>();
		auto& animation = m_player->getComponent<CAnimation>().animation;
		animation.getSprite().setRotation(transform.angle);
		animation.getSprite().setPosition(transform.pos.x, transform.pos.y);
		animation.getSprite().setScale(transform.scale.x, transform.scale.y);
		m_game->window().draw(animation.getSprite());
	}

	// draw the grid
	if (m_drawGrid)
	{
		float leftX = m_game->window().getView().getCenter().x - width() / 2;
		float rightX = leftX + width() + m_gridSize.x;
		float nextGridX = leftX - ((int)leftX % (int)m_gridSize.x);

		for (float x = nextGridX; x < rightX; x += m_gridSize.x)
		{
			drawLine(Vec2(x, 0), Vec2((float)x, (float)height()));
		}
		for (float y = 0; y < height(); y += m_gridSize.y)
		{
			drawLine(Vec2(leftX, height() - y), Vec2(rightX, height() - y));

			for (float x = nextGridX; x < rightX; x += m_gridSize.x)
			{
				std::string xCell = std::to_string((int)x / (int)m_gridSize.x);
				std::string yCell = std::to_string((int)y / (int)m_gridSize.y);
				m_gridText.setString("(" + xCell + "," + yCell + ")");
				m_gridText.setFillColor(sf::Color(255, 0, 0, 200));
				m_gridText.setPosition(x + 3, height() - y - m_gridSize.y + 2);

				m_game->window().draw(m_gridText);
			}
		}
	}

	// draw bounding boxes
	if (m_drawCollision)
	{
		for (auto& e : m_entityManager.getEntities())
		{

			if (e->hasComponent<CBoundingBox>())
			{
				auto& box = e->getComponent<CBoundingBox>();
				auto& transform = e->getComponent<CTransform>();
				sf::RectangleShape rect;
				rect.setSize(sf::Vector2f(box.size.x - 1, box.size.y - 1));
				rect.setOrigin(sf::Vector2f(box.halfSize.x, box.halfSize.y));
				rect.setOutlineColor(sf::Color(0, 0, 0, 200));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_end)
	{
		end.setPosition(width() / 2.0f + xDiff, height() / 2.0f + yDiff);
		m_game->window().draw(end);
		m_hasEnded = true;
	}

	//m_game->window().draw(m_mouseShape);
}

void Scene_Edit::onEnd()
{
	m_hasEnded = true;
	m_game->changeScene("MENU", nullptr, true);
}