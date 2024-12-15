#include "Entity.h"

uint32_t Entity::counter = 0;

Entity::Entity()
{
	id = counter;
	counter++;
}

Entity::~Entity()
{
}
