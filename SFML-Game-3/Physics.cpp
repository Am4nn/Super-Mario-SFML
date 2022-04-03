#include "Physics.h"
#include "Components.h"

Vec2 Physics::GetOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// return the overlap rectamge size of bounding boxes of entity a and b
	// if no bounding box then return 0,0

	if (!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) { return Vec2(); }

	Vec2 dist = (a->getComponent<CTransform>().pos - b->getComponent<CTransform>().pos).abs();

	Vec2 boxSize = a->getComponent<CBoundingBox>().halfSize + b->getComponent<CBoundingBox>().halfSize;

	return Vec2(boxSize.x - dist.x, boxSize.y - dist.y);
}

Vec2 Physics::GetPreviousOverlap(std::shared_ptr<Entity> a, std::shared_ptr<Entity> b)
{
	// return the previous overlap rectangle size of the bounding boxes of entity a and b
	//		 previous overlap uses the entity's previous position
	// if no bounding box then return 0,0

	if(!a->hasComponent<CBoundingBox>() || !b->hasComponent<CBoundingBox>()) { return Vec2(); }

	Vec2 dist = (a->getComponent<CTransform>().prevPos - b->getComponent<CTransform>().prevPos).abs();

	Vec2 boxSize = a->getComponent<CBoundingBox>().halfSize + b->getComponent<CBoundingBox>().halfSize;

	return Vec2(boxSize.x - dist.x, boxSize.y - dist.y);
}