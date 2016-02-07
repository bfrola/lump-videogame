#include "general.h"

Ogre::Vector3 newRandVector3( float range )
{
	const float randX = (float)rand() / (float)RAND_MAX;
	const float randY = (float)rand() / (float)RAND_MAX;
	const float randZ = (float)rand() / (float)RAND_MAX;

	return Vector3(
		range * randX - range / 2, 
		range * randY - range / 2, 
		range * randZ - range / 2
	);
}