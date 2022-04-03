#pragma once

#include "Animation.h"

class Component
{
public:
	bool has = false;
};

class CTransform : public Component
{
public:
	Vec2 pos = { 0.0f, 0.0f };
	Vec2 prevPos = { 0.0f, 0.0f };
	Vec2 velocity = { 0.0f, 0.0f };
	Vec2 scale = { 1.0f, 1.0f };
	float angle = 0.0f;

	CTransform() {}
	CTransform(const Vec2& pos)
		: pos(pos) {}
	CTransform(const Vec2& pos, const Vec2& velocity, const Vec2& scale, float angle)
		: pos(pos), velocity(velocity), angle(angle), scale(scale) {}
};

class CBoundingBox : public Component
{
public:
	Vec2 size;
	Vec2 halfSize;
	CBoundingBox() {}
	CBoundingBox(const Vec2& size)
		: size(size), halfSize(size.x / 2.0f, size.y / 2.0f) {}
};

class CScore : public Component
{
public:
	int score = 0;
	CScore() {}
	CScore(int score) : score(score) {}
};

class CLifespane : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CLifespane() {}
	CLifespane(int duration, int frame)
		: lifespan(duration), frameCreated(frame) {}
};

class CInput : public Component
{
public:
	bool up = false;
	bool down = false;
	bool left = false;
	bool right = false;
	bool shoot = false;
	bool canShoot = true;
	bool canJump = true;

	CInput() {}
};

class CAnimation : public Component
{
public:
	Animation animation;
	bool repeat = false;
	CAnimation() {}
	CAnimation(const Animation& animation, bool repeat)
		: animation(animation), repeat(repeat) {}
};

class CGravity : public Component
{
public:
	float gravity = 0.0f;
	CGravity() {}
	CGravity(float gravity) : gravity(gravity) {}
};

class CState : public Component
{
public:
	std::string state = "air";
	std::string size = "small";
	bool	shootAbility = false;

	CState() {}
	CState(const std::string& size) : size(size) {}
	CState(bool shootAbility) : shootAbility(shootAbility) {}
	CState(const std::string& state, const std::string& size, bool shootAbility)
		: state(state), size(size), shootAbility(shootAbility) {}
};

class CTransitionState : public Component
{
public:
	bool	sizeTransition = false;
	bool	shootTransition = false;
	int lifespan = 0;
	int timeCreated = 0;
	CTransitionState() {}
	CTransitionState(int duration, int timeCreated, bool sizeTransition, bool shootTransition)
		: lifespan(duration), timeCreated(timeCreated), sizeTransition(sizeTransition), shootTransition(shootTransition) {}
};

class CInvincible : public Component
{
public:
	int lifespan = 0;
	int frameCreated = 0;
	CInvincible() {}
	CInvincible(int duration, int frameCreated)
		: lifespan(duration), frameCreated(frameCreated) {}
};

class CDraggable : public Component
{
public:
	bool dragging = false;
	CDraggable() {}
	CDraggable(bool dragging) : dragging(dragging) {}
};

class COneDirectional : public Component
{
public:
	bool dir = false;
	float speed = 0.0f;
	COneDirectional() {}
	COneDirectional(bool dir, float speed)
		: dir(dir), speed(speed) {}
};

class CFollowPlayer : public Component
{
public:
	Vec2 home = { 0.0f, 0.0f };
	float speed = 0.0f;
	CFollowPlayer() {}
	CFollowPlayer(const Vec2& home, float speed)
		: speed(speed), home(home) {}
};

class CPatrol : public Component
{
public:
	std::vector<Vec2> vertex;
	size_t currentPos = 0;
	float speed = 0.0f;
	CPatrol() {}
	CPatrol(std::vector<Vec2>& vertex, size_t currentPos, float speed)
		: vertex(vertex), currentPos(currentPos), speed(speed) {}
};

class CBrickItems : public Component
{
public:
	std::string itemAnimation = "null";

	CBrickItems() {}
	CBrickItems(const std::string& itemAnimation) : itemAnimation(itemAnimation) {}
};