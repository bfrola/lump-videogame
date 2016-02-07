#pragma once

#include "Ogre.h"
#include "EntityPlayerShip.h"
#include "FlockPlayer.h"
#include "ScenarioSectorBase.h"
#include "UtilityParticleSet.h"

/**
	\author Bernardino Frola

	\brief  Manage the scenario information, sector list, playership and playerflock reference, planets.

	
 */
class ScenarioBase
{

	// ************************************************************************
	// Types definitions
	// ************************************************************************
public:
	typedef std::vector<ScenarioSectorBase*> SSGroup;
	typedef std::vector<ScenarioSectorBase*>::iterator SSIterator;

	typedef AVGroup LumpObstacleGroup;
	typedef AVIterator LumpObstacleIterator;

	typedef std::vector<SteerableEntity*> SEGroup;
	typedef std::vector<SteerableEntity*>::iterator SEIterator;

	typedef std::vector<FlockBase*> FBGroup;
	typedef std::vector<FlockBase*>::iterator FBIterator;

	// ************************************************************************
	// Constructors/Destructors
	// ************************************************************************
public:
	ScenarioBase(void);
	virtual ~ScenarioBase(void);

	// ************************************************************************
	// Methods
	// ************************************************************************
public:
	
	// Crea il contenuto di base della scena. Contiene le operazioni di inizializzazione generiche
	void enterScenario(void);

	// Aggiorna la scena. Contiene le operazioni di aggiornamento generiche
	void updateScenario(const float currentTime, const float elapsedTime);
	
	// Reiposta la scena
	void resetScenario(void);
	
	// Distruggie la scena
	void exitScenario(void);

	// Refresh the camera position and orientation based on the playership properties
	void updateCamera( const float elapsedTime );

public:

	void increaseCameraDistance(float incRatio);

	void increaseCameraHeight(float incRatio);

	void goToNewSector();
	
	/**
		Start the camera shake effect. The class field ScenarioBase::mShakeCameraStartTime
		will be setted to the current elapsed time value.
		\param <power> The strenght of the shake effect
	*/
	void shakeCamera( float power );

	// ************************************************************************
	// Virtual method - to implements into the class specialization
	// ************************************************************************

	// Aggiorna la scena
	virtual void onUpdate(const float currentTime, const float elapsedTime) = 0;

	// Imposta la scena
	virtual void onEnter() = 0;

	// Resetta la scena
	virtual void onReset() = 0;

	// Distrugge la scena
	virtual void onExit() = 0;

	// Crea le entità
	virtual void createSimpleEntities() = 0;

	// ************************************************************************
	// GET/SET Methods
	// ************************************************************************

	EntityPlayerShip* getPlayerEntity() { return mPlayerEntity; }

	FlockPlayer* getDefencePlayerFlock() { return mDefencePlayerFlock; }

	SEGroup getSingleEntities() { return mSingleEntities; }

	FBGroup getFlocks() { return mFlocks; }	
	
	LumpObstacleGroup getPlanets() { return mPlanets; }

	LumpObstacleGroup getObstacles() { return mObstacles; }

	ProximityDatabase* getProximityDatabase() { return proximityDatabase; }

	float getCameraDistance() { return mCameraDistance; }
	float getCameraHeight() { return mCameraHeight; }

	void setCameraFOVy( Radian FOVy ) { mCameraFOVy = FOVy; }

	ScenarioSectorBase* getCurrentSector() { return mCurrentSector; }

	void setCurrentSector( ScenarioSectorBase* currentScenario );

	SSGroup getSectors() { return mSectors; }

	float getLastBehaviourUpdateTime() { return mLastBehaviourUpdateTime; }
	float getBehaviourUpdateDelay() { return mBehaviourUpdateDelay; }

	UtilityParticleSet* getSparksPool() { return mSparksPool; };
	UtilityParticleSet* getAsteroidExplPool() { return mAsteroidExplPool; };
	UtilityParticleSet* getEnergyExplPool() { return mEnergyExplPool; };
	
	// ************************************************************************
	// Fields
	// ************************************************************************
protected:
	
	// Sector fields
	SSGroup mSectors;
	SSIterator sectors_it;
	ScenarioSectorBase* mCurrentSector;
	ScenarioSectorBase* mPreviousSector;
	ScenarioSectorBase* mHidingSector;


	// Player managed entites
	EntityPlayerShip* mPlayerEntity;
	FlockPlayer* mDefencePlayerFlock;
		
	// Scenario entities /* TEMP *** */
	SEGroup mSingleEntities;
	SEIterator entity_it;
	
	FBGroup mFlocks;	
	FBIterator flock_it;
	
	LumpObstacleGroup mObstacles;
	LumpObstacleIterator obstacle_it;

	LumpObstacleGroup mPlanets;
	LumpObstacleIterator planets_it;
	
	// Spactial database for finding the neighborest sector
	ProximityDatabase* proximityDatabase;
	
	// Configuration parameter

	// Camera parameters
	float mCameraDistance;					//  Distanza dall'oggetto target
	float mCameraHeight;					//	Altezza della camera
	bool mShowCameraTarget;
	// Fattore che decide il tipo di camera da usare
	// se = 1 la camera è in 3a persona
	// se = 0 la camera è guarda sempre nella direzione della navetta
	float mThirdPersonCameraRatio;
	
	// FOVy used for zooming operations
	Radian mCameraFOVy;
	
	// The distance indicator
	SceneNode* mTargetDistanceBar;
	SceneNode* mTargetDistanceIndicator;
	
	// Record the last behaviour update for player ship and player flock
	float mLastBehaviourUpdateTime;
	
	float mBehaviourUpdateDelay;

	float mShakeCameraStartTime;

	UtilityParticleSet* mSparksPool;
	UtilityParticleSet* mAsteroidExplPool;
	UtilityParticleSet* mEnergyExplPool;
};

//**********************************************************************************
//************** Innerclass PositionIndicator
//**********************************************************************************
