#include "EntityManager.h"

EntityManager::EntityManager() {}

void EntityManager::update()
{
	// add entities from m_entitiesToAdd to m_entities
	for (auto& e : m_entitiesToAdd)
	{
		// add entity in m_entities
		m_entities.push_back(e);

		// add entity in m_entityMap
		m_entityMap[e->tag()].push_back(e);
	}

	// clear m_entitiesToAdd
	m_entitiesToAdd.clear();

	// remove entity from m_entities
	removeDeadEntities(m_entities);

	// remove entity from m_entityMap
	for (auto& e : m_entityMap)
	{
		removeDeadEntities(e.second);
	}
}

std::shared_ptr<Entity> EntityManager::addEntity(const std::string& tag)
{
	// create entity
	auto entity = std::shared_ptr<Entity>(new Entity(m_totalEntities++, tag));
	// mark it to be added to m_entity
	m_entitiesToAdd.push_back(entity);
	// return the shared_ptr of new entity created
	return entity;
}

const EntityVec& EntityManager::getEntities()
{
	return m_entities;
}

const EntityVec& EntityManager::getEntities(const std::string& tag)
{
	return m_entityMap[tag];
}

void EntityManager::removeDeadEntities(EntityVec& vec)
{
	vec.erase(std::remove_if(vec.begin(), vec.end(), [](const std::shared_ptr<Entity>& entity) {
		return !entity->isActive();
		}), vec.end());
}