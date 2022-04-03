#pragma once
#include "Common.h"

class Action
{
	std::string m_name = "NONE";
	std::string m_type = "NONE";
	Vec2		m_pos = Vec2(0.0f,0.0f);

public:
	Action();
	Action(const std::string& name, const std::string& type);
	Action(const std::string& name, const std::string& type, const Vec2& pos);
	Action(const std::string& name, const Vec2& pos);

	const std::string& name() const;
	const std::string& type() const;
	std::string toString() const;
	const Vec2& pos() const;
};