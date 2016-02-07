#pragma once

#include "Ogre.h"
#include "FlockPlayer.h"

/**
	\author Bernardino Frola

	\brief Manage an indipendent world bucket. Used for the spatial division of the world.

	This class needs the AbstractVehicle extension for registering it into the global proximity database.

	
 */
class ScenarioSectorBase : SteerableBase
{

	// ************************************************************************
	// Types definitions
	// ************************************************************************
public:
	typedef AVGroup LumpObstacleGroup;
	typedef AVIterator LumpObstacleIterator;

	typedef std::vector<SteerableEntity*> SEGroup;
	typedef std::vector<SteerableEntity*>::iterator SEIterator;

	typedef std::vector<FlockBase*> FBGroup;
	typedef std::vector<FlockBase*>::iterator FBIterator;

	// Renderig states
	enum mRenderingStates
	{	
		STATE_ACTIVE,	// Visible and updating
		STATE_VISIBLE,	// Only visible but not updating
		STATE_HIDDEN	// Completely disabled (hidden and not updating)
	};

	// ************************************************************************
	// Constructors/Destructors
	// ************************************************************************
public:
	ScenarioSectorBase( const std::string& sectorName, Vector3 position, float radius );
	virtual ~ScenarioSectorBase(void);

	// ************************************************************************
	// Methods
	// ************************************************************************
public:
	// Crea il contenuto di base della scena. Contiene le operazioni di inizializzazione generiche
	void createSectorScene(void);

	// Aggiorna la scena. Contiene le operazioni di aggiornamento generiche
	void updateSector(const float currentTime, const float elapsedTime);

	Vector3 getRealObjectPosition( const std::string objectName );
	
	/**
		State tranaction visible->active
	*/
	void setActive();
	
	/**
		State tranaction hidden->visible
	*/
	void enter();
	
	/**
		State tranaction active->visible
	*/
	void exit();

	/**
		State tranaction hidden->visible
	*/
	void setHidden();

	std::string getState2string();
	
	/**
		Called when the player destroys a flock.
	*/
	void annotateFlockDestroyed();

	// ************************************************************************
	// Metodi virtuali - da implementare nelle specializzazioni
	// ************************************************************************
	
	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************	

	SEGroup getSingleEntities() { return mSingleEntities; }

	FBGroup getFlocks() { return mFlocks; }	
	
	LumpObstacleGroup getPlanets() { return mPlanets; }

	LumpObstacleGroup getObstacles() { return mObstacles; }

	ProximityDatabase* getProximityDatabase() { return mProximityDatabase; }

	ProximityToken* getProximityToken() { return mProximityToken; }

	Vector3 getPosition() { return mPosition; }

	void setPosition( Vector3 position ) { mPosition = position; }

	std::string getName() { return mName; }

	float getRadius() { return radius(); }

	SceneNode* getProximityIndicatorNode() { return mProximityIndicatorNode; }

	int getActiveFlocks() { return mActiveFlocks; }

	// ************************************************************************
	// Campi
	// ************************************************************************
protected:
	std::string mName;

	// Rendering states
	mRenderingStates mRenderingState;

	// Sector entities: single entities, flocks and obstacles
	// These entities "lives" into the sector
	SEGroup mSingleEntities;
	SEIterator entity_it;
	
	FBGroup mFlocks;	
	FBIterator flock_it;
	
	LumpObstacleGroup mObstacles;
	LumpObstacleIterator obstacle_it;

	LumpObstacleGroup mPlanets;
	LumpObstacleIterator planets_it;

	// Finds the neighborood
	ProximityDatabase* mProximityDatabase;
	// Position (OpenSteer object)
	Vector3 mPosition;

	ProximityToken* mProximityToken;

	bool mShowActiveSector;	

	int mActiveFlocks;

	SceneNode* mProximityIndicatorNode;
};
