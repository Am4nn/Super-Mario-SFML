#pragma once

#include <tuple>
#include "Common.h"
#include "Components.h"

class EntityManager;

typedef std::tuple<
	CTransform,
	CLifespane,
	CInput,
	CBoundingBox,
	CAnimation,
	CGravity,
	CState,
	CInvincible,
	CBrickItems,
	CScore,
	CTransitionState,
	CDraggable,
	COneDirectional,
	CPatrol,
	CFollowPlayer
> ComponentTuple;

class Entity
{
public:

	friend class EntityManager;

	// private member access functions
	bool					isActive() const;
	const std::string&		tag() const;
	const size_t			id() const;
	void					destroy();

	template<typename T>
	bool hasComponent() const
	{
		return getComponent<T>().has;
	}

	template<typename T, typename... TArgs>
	T& addComponent(TArgs&&... mArgs)
	{
		auto& component = getComponent<T>();
		component = T(std::forward<TArgs>(mArgs)...);
		component.has = true;
		return component;
	}

	template<typename T>
	T& getComponent()
	{
		return std::get<T>(m_components);
	}

	template<typename T>
	const T& getComponent() const
	{
		return std::get<T>(m_components);
	}

	// TODO: check ...
	template<typename T>
	void removeComponent()
	{
		getComponent<T>().has = false;
	}


private:
	bool			m_active = true;
	std::string		m_tag = "default";
	size_t			m_id = 0;
	ComponentTuple	m_components;

	// constructor is private so only EntityManager can create an entity
	Entity(const size_t id, const std::string& tag);
};