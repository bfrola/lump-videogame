#pragma once

#include "Ogre.h"
#include "SteerableEntity.h"

/**
	\author Bernardino Frola

	\brief This class manage the planet entity.

	
 */
class EntityPlanet :
	public SteerableEntity, 
	public SphericalObstacle
{
public:
	EntityPlanet(const std::string& name, 
		float magnitude = 2.0f,
		Ogre::Vector3 startPosition = Ogre::Vector3::ZERO,
		Ogre::Quaternion startOrientation = Ogre::Quaternion::IDENTITY,
		Vector3 movementVelocity = Ogre::Vector3::ZERO,
		Ogre::Quaternion rotationVelocity = Ogre::Quaternion::ZERO);

	~EntityPlanet(void);

public:
	// Aggiorna il veicolo secondo le specifiche della specializzazione
	void onUpdate(const float currentTime, const float elapsedTime);

	// Update the behaviour
	void updateBehaviour(const float currentTime, const float elapsedTime);

	// Gestisce l'impatto con un elemento
	void onImpactWith( SteerableEntity* impacted, float impactWeight );	
};
