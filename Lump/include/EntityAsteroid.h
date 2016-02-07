#pragma once

#include "Ogre.h"
#include "SteerableEntity.h"

/**
	\author Bernardino Frola

	\brief This class manage any type of asteroid.

 */
class EntityAsteroid :
	public SteerableEntity, 
	public SphericalObstacle
{
	// ************************************************************************
	// Constructors/Destructors
	// ************************************************************************
public:

	/**
		Create a new asteroid.
	*/
	EntityAsteroid(const std::string& name, 
		Ogre::Entity* baseEntity,			// Entity da clonare
		float magnitude = 2.0f,
		Ogre::Vector3 startPosition = Ogre::Vector3::ZERO,
		Ogre::Quaternion startOrientation = Ogre::Quaternion::IDENTITY,
		Vector3 movementVelocity = Ogre::Vector3::ZERO,
		Ogre::Quaternion rotationVelocity = Ogre::Quaternion::ZERO);
	
	~EntityAsteroid(void);

	// ************************************************************************
	// Methods
	// ************************************************************************
public:
	/**
		Do nothing.
	*/
	void onUpdate(const float currentTime, const float elapsedTime);

	/**
		Do nothing.
	*/
	void updateBehaviour(const float currentTime, const float elapsedTime);

	/**
		Explode if the energy is lower than 0. For blue asteroids, on exploding, the
		playership's energy is recoverd by a random value into the range [50, 100]
	*/
	void onImpactWith( SteerableEntity* impacted, float impactWeight );	

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************

	// ************************************************************************
	// Fields
	// ************************************************************************
};
