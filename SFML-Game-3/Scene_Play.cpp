#include "Scene_Play.h"
#include "Common.h"
#include "Physics.h"
#include "Assests.h"
#include "GameEngine.h"
#include "Components.h"
#include "Action.h"

#include <format>

Scene_Play::Scene_Play(GameEngine* gameEngine, const std::string& levelPath)
	: Scene(gameEngine), m_levelPath(levelPath)
{
	init(m_levelPath);
}

void Scene_Play::init(const std::string& levelPath)
{
	registerAction(sf::Keyboard::W,		"JUMP");
	registerAction(sf::Keyboard::A,		"LEFT");
	registerAction(sf::Keyboard::D,		"RIGHT");
	registerAction(sf::Keyboard::P,		"PAUSE");
	registerAction(sf::Keyboard::Space,	"SHOOT");
	registerAction(sf::Keyboard::Escape,"QUIT");
	registerAction(sf::Keyboard::T,		"TOGGLE_TEXTURE");
	registerAction(sf::Keyboard::C,		"TOGGLE_COLLISION");
	registerAction(sf::Keyboard::G,		"TOGGLE_GRID");
	registerAction(sf::Keyboard::Z,		"SAVE_LEVEL");

	m_mouseShape.setRadius(8);
	m_mouseShape.setOrigin(8, 8);
	m_mouseShape.setFillColor(sf::Color(255, 0, 0, 190));

	m_gridText.setCharacterSize(12);
	m_gridText.setFont(m_game->assests().getFont("Arial"));

	m_displayTxt.setCharacterSize(40);
	m_displayTxt.setFont(m_game->assests().getFont("Score"));
	m_displayTxt.setFillColor(sf::Color(196, 30, 58));

	auto a = m_game->assests().getAnimation("Sky");
	sky = a.getSprite();
	sky.setScale(1.2f, 1.2f);

	std::cout << levelPath;

	loadLevel(levelPath);
}

bool Scene_Play::isInside(const Vec2& pos, std::shared_ptr<Entity> e)
{
	if (!e->hasComponent<CAnimation>()) { return false; }

	// half size of animation
	auto halfSize = e->getComponent<CAnimation>().animation.getSize() / 2.0f;

	// delta value between both the enemy
	Vec2 delta = (e->getComponent<CTransform>().pos - pos).abs();

	// if click is within x and y bounds of half size then we are inside
	return (delta.x <= halfSize.x) && (delta.y <= halfSize.y);
}


Vec2 Scene_Play::gridToMidPixel(float gridX, float gridY, std::shared_ptr<Entity> entity)
{
	float b1x = gridX * m_gridSize.x;
	float b1y = height() - (gridY * m_gridSize.y);
	Vec2 size = entity->getComponent<CAnimation>().animation.getSize();
	return Vec2(b1x + size.x / 2.0f, b1y - size.y / 2.0f);
}

Vec2 Scene_Play::midPixelToGrid(Vec2 pos, std::shared_ptr<Entity> entity)
{
	Vec2 size = entity->getComponent<CAnimation>().animation.getSize();
	float realX = pos.x - size.x / 2.0f, realY = pos.y + size.y / 2.0f;
	float b1x = realX / m_gridSize.x;
	float b1y = (height() - realY) / m_gridSize.y;
	return Vec2(b1x , b1y);
}

void Scene_Play::loadLevel(const std::string& filename)
{
	m_entityManager = EntityManager();
	std::ifstream file(filename);
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
				cords.push_back(Vec2(pos.x, pos.y + 2 * patrol->getComponent<CAnimation>().animation.getSize().y));
				patrol->addComponent<CPatrol>(cords, 0, 1.0f);
				patrol->addComponent<CDraggable>();
			}

			auto tile = m_entityManager.addEntity("tile");
			tile->addComponent<CAnimation>(m_game->assests().getAnimation(str), true);
			tile->addComponent<CTransform>(gridToMidPixel(x, y, tile));
			tile->addComponent<CBoundingBox>(tile->getComponent<CAnimation>().animation.getSize());
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
			npc->addComponent<COneDirectional>(true, speed);
			npc->addComponent<CDraggable>();

			npc->getComponent<CTransform>().velocity = Vec2(npc->getComponent<COneDirectional>().speed, 0.0f) * (npc->getComponent<COneDirectional>().dir ? 1.0f : -1.0f);
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

void Scene_Play::spawnPlayer()
{
	if (m_player) { m_player->destroy(); }

	m_player = m_entityManager.addEntity("player");
	m_player->addComponent<CAnimation>(m_game->assests().getAnimation("Run"), true);
	m_player->addComponent<CTransform>(gridToMidPixel(m_playerConfig.X, m_playerConfig.Y, m_player));
	m_player->addComponent<CInput>();
	m_player->addComponent<CBoundingBox>(Vec2(m_playerConfig.CX, m_playerConfig.CY));
	m_player->addComponent<CGravity>(m_playerConfig.GRAVITY);
	m_player->addComponent<CState>("air", "small", false);

	m_scoreCntr -= 100;
	if (m_scoreCntr < 0) m_scoreCntr = 0;
}
void Scene_Play::spawnMediBult(std::shared_ptr<Entity> e)
{
	auto item = m_entityManager.addEntity("medi_bult");
	item->addComponent<CAnimation>(m_game->assests().getAnimation(e->getComponent<CBrickItems>().itemAnimation), true);
	item->addComponent<CTransform>(e->getComponent<CTransform>().pos + Vec2(0.0f, -58.0f));
	item->addComponent<CBoundingBox>(item->getComponent<CAnimation>().animation.getSize());
}

void Scene_Play::spawnMediGrow(std::shared_ptr<Entity> e)
{
	auto item = m_entityManager.addEntity("medi_grow");
	item->addComponent<CAnimation>(m_game->assests().getAnimation(e->getComponent<CBrickItems>().itemAnimation), true);
	item->addComponent<CTransform>(e->getComponent<CTransform>().pos + Vec2(0.0f, -58.0f));
	item->addComponent<CBoundingBox>(item->getComponent<CAnimation>().animation.getSize());
	item->addComponent<CGravity>(m_playerConfig.GRAVITY);
	item->addComponent<COneDirectional>(true, m_entityManager.getEntities("npc_conics").at(0)->getComponent<COneDirectional>().speed);

	item->getComponent<CTransform>().velocity = Vec2(item->getComponent<COneDirectional>().speed, 0.0f) * (item->getComponent<COneDirectional>().dir ? 1.0f : -1.0f);
}

void Scene_Play::hitBlock(std::shared_ptr<Entity> e)
{
	if (e->getComponent<CAnimation>().animation.getName() == "Brick")
	{
		if (e->getComponent<CBrickItems>().itemAnimation == "null")
		{
			if (m_player->getComponent<CState>().size == "big")
			{
				e->addComponent<CAnimation>(m_game->assests().getAnimation("Explosion"), false);
				e->removeComponent<CBoundingBox>();
			}
		}
		else
		{
			if (e->getComponent<CBrickItems>().itemAnimation == "MediGrow")
			{
				spawnMediGrow(e);
			}
			else
			{
				spawnMediBult(e);
			}
			e->getComponent<CBrickItems>().itemAnimation = "null";
		}
	}
	else if (e->getComponent<CAnimation>().animation.getName() == "Question")
	{
		auto coin = m_entityManager.addEntity("coin");
		coin->addComponent<CTransform>(e->getComponent<CTransform>().pos + Vec2(0.0f, -64.0f));
		coin->addComponent<CAnimation>(m_game->assests().getAnimation("Coin"), true);
		coin->addComponent<CLifespane>(60, m_currentFrame);

		e->addComponent<CAnimation>(m_game->assests().getAnimation("Question2"), true);
		m_coinCntr++;
		m_scoreCntr += 20;
	}
}

void Scene_Play::spawnBullet(std::shared_ptr<Entity> e)
{
	auto& transform = e->getComponent<CTransform>();
	auto bullet = m_entityManager.addEntity("bullet");
	bullet->addComponent<CTransform>(transform.pos, Vec2(12 * transform.scale.x, 0), transform.scale, 0.0f);
	bullet->addComponent<CAnimation>(m_game->assests().getAnimation(m_playerConfig.WEAPON), true);
	bullet->addComponent<CBoundingBox>(m_game->assests().getAnimation(m_playerConfig.WEAPON).getSize());
	bullet->addComponent<CLifespane>(60, m_currentFrame);
}

void Scene_Play::update()
{
	m_entityManager.update();

	sAnimation();
	if (!m_paused)
	{
		sMovement();
		sLifespan();
		sCollision();

		m_currentFrame++;
	}
}

void Scene_Play::sMovement()
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

	// update direction according to CPatrol
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

		// update direction according to COneDirectional
		if (e->hasComponent<COneDirectional>())
		{
			transform.velocity.x = std::abs(transform.velocity.x);
			transform.velocity.x *= e->getComponent<COneDirectional>().dir ? 1 : -1;
		}

		// gravity
		if (e->hasComponent<CGravity>())
		{
			transform.velocity.y += e->getComponent<CGravity>().gravity;
		}

		// cap the entity's speed so it doesn't go infinite
		if (transform.velocity.x < -m_playerConfig.MAXSPEED) { transform.velocity.x = -m_playerConfig.MAXSPEED; }
		if (transform.velocity.x >  m_playerConfig.MAXSPEED) { transform.velocity.x =  m_playerConfig.MAXSPEED; }
		if (transform.velocity.y < -m_playerConfig.MAXSPEED) { transform.velocity.y = -m_playerConfig.MAXSPEED; }
		if (transform.velocity.y >  m_playerConfig.MAXSPEED) { transform.velocity.y =  m_playerConfig.MAXSPEED; }

		// update entity position
		transform.prevPos = transform.pos;
		transform.pos += transform.velocity;
	}

	// shoot bullets
	if (m_player->getComponent<CInput>().shoot && m_player->getComponent<CInput>().canShoot && m_player->getComponent<CState>().shootAbility)
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

void Scene_Play::sLifespan()
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
		if (e->hasComponent<CInvincible>())
		{
			auto& lifespan = e->getComponent<CInvincible>();
			int framesAlive = (m_currentFrame - lifespan.frameCreated);

			if (framesAlive >= lifespan.lifespan)
			{
				e->removeComponent<CInvincible>();
			}
		}
	}

	if (m_player->hasComponent<CTransitionState>())
	{
		if (m_player->getComponent<CTransitionState>().lifespan)
			m_player->getComponent<CTransitionState>().lifespan--;
		else
			m_player->removeComponent<CTransitionState>();
	}
}

void Scene_Play::flipAndDie(std::shared_ptr<Entity> e)
{
	if (e->tag() == m_player->tag() && m_player->getComponent<CState>().size == "big")
	{
		m_player->getComponent<CState>().shootAbility = false;
		m_player->getComponent<CState>().size = "small";
		m_player->addComponent<CInvincible>(60, m_currentFrame);
	}
	else
	{
		e->getComponent<CTransform>().scale.y = -1.0f;
		e->getComponent<CTransform>().velocity = Vec2(0.0f, -10.0f);
		e->removeComponent<CBoundingBox>();
	}
}

void Scene_Play::flatAndDie(std::shared_ptr<Entity> e)
{
	std::string ch = "Chapta_" + e->getComponent<CAnimation>().animation.getName();

	auto f = m_entityManager.addEntity("chapta");
	f->addComponent<CTransform>(e->getComponent<CTransform>().pos);
	f->addComponent<CAnimation>(m_game->assests().getAnimation(ch), true);
	f->addComponent<CLifespane>(60, m_currentFrame);

	e->destroy();
}

void Scene_Play::playerSpawnAtNearPoint()
{
	float minDist = INFINITY;
	Vec2 minVec = gridToMidPixel(2, 6, m_player);

	for (auto& e : m_entityManager.getEntities("tile"))
	{
		if ((e->getComponent<CAnimation>().animation.getName() == "PipeSmall"		||
			e->getComponent<CAnimation>().animation.getName() == "PipeMid")		&&
			e->getComponent<CTransform>().pos.x <= m_player->getComponent<CTransform>().pos.x)
		{
			float dist = e->getComponent<CTransform>().pos.distSq(m_player->getComponent<CTransform>().pos);
			if (minDist > dist)
			{
				minDist = dist;
				minVec = e->getComponent<CTransform>().pos;
			}
		}
	}

	spawnPlayer();
	m_player->getComponent<CTransform>().pos = { minVec.x, minVec.y - 64 * 6 };
}

void Scene_Play::sCollision()
{
	auto& tiles = m_entityManager.getEntities("tile");
	auto& bullets = m_entityManager.getEntities("bullet");
	auto& npcs = m_entityManager.getEntities("npc_conics");

	auto& pTransform = m_player->getComponent<CTransform>();
	auto& pBox = m_player->getComponent<CBoundingBox>();

	// medicine to grow collision detection
	for (auto& e : m_entityManager.getEntities("medi_bult"))
	{
		// e collision with player
		Vec2 overlap = Physics::GetOverlap(m_player, e);
		Vec2 prevoverlap = Physics::GetPreviousOverlap(m_player, e);

		if (overlap.x > 0 && overlap.y > 0)
		{
			// if collision happens then player gets shooting abilities
			m_player->getComponent<CState>().shootAbility = true;
			m_player->addComponent<CTransitionState>(60, m_currentFrame / 60.0f, false, true);
			e->destroy();
			m_scoreCntr += 10;
		}
	}

	// medicine to grow collision detection
	for (auto& e : m_entityManager.getEntities("medi_grow"))
	{
		if (midPixelToGrid(e->getComponent<CTransform>().pos, e).x < 0 || midPixelToGrid(e->getComponent<CTransform>().pos, e).y > height())
		{
			e->destroy();
		}

		// e collision with player
		Vec2 overlap = Physics::GetOverlap(m_player, e);
		Vec2 prevoverlap = Physics::GetPreviousOverlap(m_player, e);

		if (overlap.x > 0 && overlap.y > 0)
		{
			// if collision happens then player grow
			m_player->getComponent<CState>().size = "big";
			m_player->addComponent<CTransitionState>(60, m_currentFrame / 60.0f, true, false);
			e->destroy();
			m_scoreCntr += 10;
		}

		// e collision with tiles
		for (auto& tile : tiles)
		{
			if (!tile->hasComponent<CBoundingBox>()) { continue; }

			Vec2 overlap = Physics::GetOverlap(e, tile);
			Vec2 prevOverlap = Physics::GetPreviousOverlap(e, tile);

			if (overlap.x < 0 || overlap.y < 0) { continue; }

			Vec2 diff = e->getComponent<CTransform>().pos - tile->getComponent<CTransform>().pos;
			Vec2 shift(0.0f, 0.0f);

			if (prevOverlap.x > 0)
			{
				shift.y += diff.y > 0 ? overlap.y : -overlap.y;
				e->getComponent<CTransform>().velocity.y = 0;
				if (diff.y < 0)
				{
					e->getComponent<CTransform>().pos += (tile->getComponent<CTransform>().velocity);
				}
			}
			else if (prevOverlap.y > 0)
			{
				e->getComponent<COneDirectional>().dir = !e->getComponent<COneDirectional>().dir;
			}

			e->getComponent<CTransform>().pos += shift;
		}
	}

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
			m_scoreCntr += 25;
		}

		// patrol collision with player
		if (!m_player->hasComponent<CBoundingBox>()		||
			 m_player->hasComponent<CInvincible>()		||
			 patrol->hasComponent<CInvincible>())		{ continue; }

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
			m_scoreCntr += 25;
		}
		else
		{
			flipAndDie(m_player);
		}
	}

	// npc_conics
	for (auto& npc : npcs)
	{
		if (midPixelToGrid(npc->getComponent<CTransform>().pos, npc).x < 0 || midPixelToGrid(npc->getComponent<CTransform>().pos, npc).y > height())
		{
			npc->destroy();
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

		// npc collision with player
		if (!m_player->hasComponent<CBoundingBox>()		||
			 m_player->hasComponent<CInvincible>()		||
			 npc->hasComponent<CInvincible>())			{ continue; }

		Vec2 overlap = Physics::GetOverlap(m_player, npc);
		Vec2 prevoverlap = Physics::GetPreviousOverlap(m_player, npc);

		if (overlap.x > 0 && overlap.y > 0)
		{
			Vec2 diff = m_player->getComponent<CTransform>().pos - npc->getComponent<CTransform>().pos;

			if (prevoverlap.x > 0 && diff.y < 0)
			{
				flatAndDie(npc);
				//npc->destroy();
				m_scoreCntr += 35;
			}
			else
			{
				flipAndDie(m_player);
			}
			break;
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
					hitBlock(tile);
				}
			}
		}

		// npc collision with bullets
		for (auto& npc : npcs)
		{
			if (npc->hasComponent<CInvincible>()) { continue; }

			Vec2 overlap = Physics::GetOverlap(bullet, npc);
			if (overlap.x > 0 && overlap.y > 0)
			{
				bullet->destroy();
				flipAndDie(npc);
				//npc->destroy();
				m_scoreCntr += 35;
			}
		}
	}

	m_player->getComponent<CState>().state = "air";

	// player pos before it moved
	Vec2 prevPos = pTransform.pos - pTransform.velocity;

	// player collision with tiles
	size_t tileNum = 0;
	for (auto& tile : tiles)
	{
		auto& tTransform = tile->getComponent<CTransform>();
		tileNum++;

		if (!m_player->hasComponent<CBoundingBox>()) { continue; }

		// current overlap
		Vec2 overlap = Physics::GetOverlap(m_player, tile);
		Vec2 prevOverlap = Physics::GetPreviousOverlap(m_player, tile);

		if (overlap.x < 0 || overlap.y < 0) { continue; }

		if (tile->getComponent<CAnimation>().animation.getName() == "Pole" ||
			tile->getComponent<CAnimation>().animation.getName() == "PoleTop")
		{
			auto a = m_game->assests().getAnimation("ScoreBoard");
			scoreBoard = a.getSprite();
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
		playerSpawnAtNearPoint();
	}

	// don't let the player exit the screen to the left
	if (m_player->getComponent<CTransform>().pos.x < m_player->getComponent<CBoundingBox>().halfSize.x)
	{
		m_player->getComponent<CTransform>().pos.x = m_player->getComponent<CBoundingBox>().halfSize.x;
	}
}

void Scene_Play::sDoAction(const Action& action)
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
		//else if (action.name() == "LEFT_CLICK")
		//{
		//	std::cout << "Left - Click : " << action.toString() << "\n";
			
		//	float xDiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
		//	float yDiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;

			// this is where we actually clicked inside our world
			//Vec2 worldPos(action.pos().x + xDiff, action.pos().y + yDiff);

			// check if any entity was clicked
			//for (auto& e : m_entityManager.getEntities())
			//{
			//	// if entity is clicked
			//	if (e->hasComponent<CDraggable>() && isInside(worldPos, e))
			//	{
			//		// change its state from not dragging or vice versa
			//		e->getComponent<CDraggable>().dragging = !e->getComponent<CDraggable>().dragging;
			//		std::cout << "Entity Clicked : " << e->tag() << "\n";
			//		break;
			//	}
			//}
		//}	
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


			std::ofstream sl("res/save_level/saved_level.txt");
			sl << sls.str();
			std::cout << "Level saved to " << "res/save_level/saved_level.txt" << std::endl;
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

void Scene_Play::sAnimation()
{
	
	auto& pState = m_player->getComponent<CState>();
	auto& pTransform = m_player->getComponent<CTransform>();

	static std::string isBig;
	static std::string state;
	if (m_player->getComponent<CState>().size == "big") 
	{
		isBig = "Big";
		if (m_player->hasComponent<CBoundingBox>())
		{
			m_player->getComponent<CBoundingBox>().size = m_game->assests().getAnimation("AirBig").getSize();
			m_player->getComponent<CBoundingBox>().halfSize = m_player->getComponent<CBoundingBox>().size / 2.0f;
		}
	}
	else
	{
		isBig = "";
		if (m_player->hasComponent<CBoundingBox>())
		{
			m_player->getComponent<CBoundingBox>().size = m_game->assests().getAnimation("Air").getSize();
			m_player->getComponent<CBoundingBox>().halfSize = m_player->getComponent<CBoundingBox>().size / 2.0f;
		}
	}

	if (pState.state == "air")
	{
		m_player->addComponent<CAnimation>(m_game->assests().getAnimation("Air" + isBig), true);
	}
	else if (pState.state == "ground")
	{
		auto& input = m_player->getComponent<CInput>();
		if ((input.left || input.right) && !(input.left && input.right))
		{
			state = "Run" + isBig;
			if (m_player->getComponent<CAnimation>().animation.getName() != state)
			{
				m_player->addComponent<CAnimation>(m_game->assests().getAnimation(state), true);
			}
		}
		else
		{
			state = "Stand" + isBig;
			m_player->addComponent<CAnimation>(m_game->assests().getAnimation(state), true);
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

void Scene_Play::drawLine(const Vec2& p1, const Vec2& p2)
{
	sf::Vertex line[] = { sf::Vector2f(p1.x, p1.y), sf::Vector2f(p2.x, p2.y) };
	line->color = sf::Color(255, 0, 0, 200);
	m_game->window().draw(line, 2, sf::Lines);
}

float noise1d(float value)
{
	return cos(value + cos(value * 90.0f) * 100.0f) * 0.5f + 0.5f;
}

void Scene_Play::sRender()
{
	if (!m_paused) { m_game->window().clear(sf::Color(100, 100, 255)); }
	else { m_game->window().clear(sf::Color(50, 50, 150)); }

	auto& pPos = m_player->getComponent<CTransform>().pos;
	float windowCenterX = fmax(m_game->window().getSize().x / 2.0f, pPos.x);

	//sf::View view = m_game->window().getView();
	//view.setCenter(windowCenterX, m_game->window().getSize().y - view.getCenter().y);

	sf::View view;
	view.setCenter(windowCenterX, m_game->window().getSize().y / 2.0f);
	view.setSize(m_game->window().getSize().x, m_game->window().getSize().y);

	if (m_currentFrame / 60.0f <= 1.0f)
	{
		view.setRotation((0.5f - noise1d(m_currentFrame / 60.0f)));
	}
	else
	{
		view.setRotation(0.0f);
	}
	m_game->window().setView(view);

	float xDiff = m_game->window().getView().getCenter().x - m_game->window().getSize().x / 2.0f;
	float yDiff = m_game->window().getView().getCenter().y - m_game->window().getSize().y / 2.0f;


	sky.setPosition(width() / 2.0f + xDiff, height() / 2.0f + yDiff);
	if (!m_paused) 
	{
		if (m_currentFrame / 60.0f <= 2.0f)
		{
			m_game->assests().getFragShader_ptr("SkyOpen")->setUniform("time", m_currentFrame / 60.0f);
			m_game->window().draw(sky, m_game->assests().getFragShader_ptr("SkyOpen"));
		}
		else
		{
			m_game->window().draw(sky);
		}
	}

	if (!m_hasEnded)
	{
		m_displayTxt.setPosition(110.0f + xDiff, 20.0f + yDiff);
		std::string curr_time = std::to_string(m_currentFrame / 60.0f);
		curr_time = (curr_time.substr(0, curr_time.find(".") + 3));
		curr_time = "Time : " + curr_time;
		curr_time += "			Score : " + std::to_string(m_scoreCntr);
		curr_time += "			Coins : " + std::to_string(m_coinCntr);
		m_displayTxt.setString(curr_time);
		m_game->window().draw(m_displayTxt);
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
		

		if (m_player->hasComponent<CInvincible>())
		{
			m_game->assests().getFragShader_ptr("Blinking")->setUniform("time", m_currentFrame / 60.0f);
			m_game->window().draw(animation.getSprite(), m_game->assests().getFragShader_ptr("Blinking"));
		}
		else if (m_player->hasComponent<CTransitionState>())
		{
			if (m_player->getComponent<CTransitionState>().sizeTransition)
			{
				m_game->assests().getFragShader_ptr("PlayerTransition")->setUniform("condition", true);
				m_game->assests().getFragShader_ptr("PlayerTransition")->setUniform("time", m_currentFrame / 60.0f - m_player->getComponent<CTransitionState>().timeCreated);
				m_game->window().draw(animation.getSprite(), m_game->assests().getFragShader_ptr("PlayerTransition"));
			}
			else if (m_player->getComponent<CTransitionState>().shootTransition)
			{
				m_game->assests().getFragShader_ptr("PlayerTransition")->setUniform("condition", false);
				m_game->assests().getFragShader_ptr("PlayerTransition")->setUniform("time", m_currentFrame / 60.0f - m_player->getComponent<CTransitionState>().timeCreated);
				m_game->window().draw(animation.getSprite(), m_game->assests().getFragShader_ptr("PlayerTransition"));
			}
		}

		else
		{
			m_game->window().draw(animation.getSprite());
		}
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
				rect.setOutlineColor(sf::Color(255, 255, 255, 255));
				rect.setPosition(transform.pos.x, transform.pos.y);
				rect.setFillColor(sf::Color(0, 0, 0, 0));
				rect.setOutlineThickness(1);
				m_game->window().draw(rect);
			}
		}
	}

	if (m_end)
	{
		scoreBoard.setPosition(width() / 2.0f + xDiff, height() / 2.0f + yDiff);
		m_game->window().draw(scoreBoard);

		sf::Sprite marioImg;
		auto a = m_game->assests().getAnimation("ScorePic");
		marioImg = a.getSprite();
		marioImg.setPosition(xDiff + 850.0f, yDiff + 400.0f);

		sf::Text t;
		sf::Text s;
		sf::Text c;
		sf::Text n;

		t.setCharacterSize(60);
		t.setFont(m_game->assests().getFont("Super"));
		t.setFillColor(sf::Color::Black);

		s.setCharacterSize(60);
		s.setFont(m_game->assests().getFont("Super"));
		s.setFillColor(sf::Color::Black);

		c.setCharacterSize(60);
		c.setFont(m_game->assests().getFont("Super"));
		c.setFillColor(sf::Color::Black);

		n.setCharacterSize(40);
		n.setFont(m_game->assests().getFont("Name"));
		//n.setFillColor(sf::Color(255, 215, 0));
		n.setFillColor(sf::Color(rand() % 255, rand() % 255, rand() % 255, 255));

		std::string curr_time = std::to_string(m_currentFrame / 60.0f);
		curr_time = (curr_time.substr(0, curr_time.find(".") + 3));
		t.setString("Time      :     " + curr_time);
		s.setString("Score     :     " + std::to_string(m_scoreCntr));
		c.setString("Coins     :     " + std::to_string(m_coinCntr));
		n.setString("- Aman Arya");

		t.setPosition(xDiff + 350.0f, yDiff + 250.0f);
		s.setPosition(xDiff + 350.0f, yDiff + 250.0f + 100.0f);
		c.setPosition(xDiff + 350.0f, yDiff + 250.0f + 200.0f);
		n.setPosition(xDiff + 825.0f, yDiff + 560.0f);

		m_game->window().draw(t);
		m_game->window().draw(s);
		m_game->window().draw(c);
		m_game->window().draw(n);
		m_game->window().draw(marioImg);

		m_hasEnded = true;
		sAnimation();
	}

	//m_game->window().draw(m_mouseShape);
}

void Scene_Play::onEnd()
{
	m_hasEnded = true;
	m_game->changeScene("MENU", nullptr, true);
}