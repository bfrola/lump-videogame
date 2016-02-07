#pragma once

#include "ScenarioBase.h"
#include "Ogre.h"

/**
	\author Bernardino Frola

	\brief Provide a particular implamentation of ScenarioBase.

	This class initialize the object of the scene. To change the scene over the possibilities
	provided by the configuration file, you must to write an other class similar to 
	ScenarioBaseImpl.
 */
class ScenarioBaseImpl : public ScenarioBase
{
public:

	ScenarioBaseImpl(void);
	~ScenarioBaseImpl(void);

public:

	// Update scene
	void onUpdate(const float currentTime, const float elapsedTime);

	// Set up scene
	void onEnter();

	// Reset scene
	void onReset();

	// Destroy scene
	void onExit();

	// Create scene specific Entities
	void createSimpleEntities();
};
