#pragma once

#include "Entity.h"

using EntityVec = std::vector<std::shared_ptr<Entity>>;
using EntityMap = std::map<std::string, EntityVec>;

class EntityManager
{

public:

	EntityManager();
	void update();
	std::shared_ptr<Entity> addEntity(const std::string& tag);
	const EntityVec& getEntities();
	const EntityVec& getEntities(const std::string& tag);

private:

	EntityVec m_entities;
	EntityVec m_entitiesToAdd;
	EntityMap m_entityMap;
	size_t m_totalEntities = 0;

	void removeDeadEntities(EntityVec& vec);
};