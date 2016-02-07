#include "general.h"

ScenarioBase::ScenarioBase() 
{
	// Spactial DB initialization
	// //////////////////////////////////////////////////////////////////////////////
	
	const Vec3 center;
	const float div = 10.0f;
	const Vec3 divisions (div, div, div);
	const float diameter = 1e6f; //XXX need better way to get this
	const Vec3 dimensions (diameter, diameter, diameter);
	typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
	proximityDatabase = new LQPDAV (center, dimensions, divisions);

	mCameraDistance = 600;
	mCameraHeight = 100;
}

// ---------------------------------------------------------------------------------

ScenarioBase::~ScenarioBase() 
{
	// Sectors
	for (sectors_it = mSectors.begin(); 
			sectors_it != mSectors.end(); ++sectors_it)
	{
		delete (*sectors_it);
	}
	mSectors.clear();

	delete proximityDatabase;

}

// ---------------------------------------------------------------------------------

void ScenarioBase::setCurrentSector( ScenarioSectorBase* currentSector )
{
	if ( currentSector == NULL || currentSector == mCurrentSector )
		return;
	
	// load the new sector and delete the old sector
	currentSector->setActive();

	if ( mCurrentSector != NULL )
	{
		mCurrentSector->exit(); 

		mLump.getLogger()->log("Sector", 
					"change from " + mCurrentSector->getName() +
					" to " + currentSector->getName() );
	}

	mCurrentSector = currentSector; 

	// Register to the proximity database of the current sector
	mPlayerEntity->registerToProximityDB( mCurrentSector->getProximityDatabase() );
	mDefencePlayerFlock->registerToProximityDB( mCurrentSector->getProximityDatabase() );

	mLump.getEventsLogger()->log( "Position", "You have changed sector" );

}

// ---------------------------------------------------------------------------------

// Aggiorna il contenuto dello scenario
void ScenarioBase::updateScenario( const float currentTime, const float elapsedTime )
{		
	// Entity and current sector update
	// //////////////////////////////////////////////////////////////////////////////

	mPlayerEntity->updateEntity( currentTime, elapsedTime );
	mDefencePlayerFlock->updateEntity( currentTime, elapsedTime ); 

	if ( mLump.getTotalElapsedTime() > mLastBehaviourUpdateTime + mBehaviourUpdateDelay )
	{
		mPlayerEntity->updateBehaviour( currentTime, elapsedTime );
		mDefencePlayerFlock->updateBehaviour( currentTime, elapsedTime ); 
		mLastBehaviourUpdateTime = mLump.getTotalElapsedTime();

		mSparksPool->checkForExpiredPS();
		mEnergyExplPool->checkForExpiredPS();
		mAsteroidExplPool->checkForExpiredPS();
	}
	
	for (sectors_it = mSectors.begin(); 
			sectors_it != mSectors.end(); ++sectors_it)
	{
		ScenarioSectorBase* sector = (ScenarioSectorBase*) *sectors_it;
		sector->updateSector( currentTime, elapsedTime );
	}

	// Update the asteroid in the playership neighborood
	// //////////////////////////////////////////////////////////////////////////////
	
	/*AVGroup playerShipneighbors = mPlayerEntity->getNeighbors();

	for (AVIterator neighbors_it=playerShipneighbors.begin(); 
			neighbors_it != playerShipneighborfs.end(); ++neighbors_it)
	{
		SteerableEntity* neighbor = (SteerableEntity*)(*neighbors_it);
		
		if ( typeid( *neighbor ) == typeid( EntityAsteroid ) ) 
		{
			neighbor->updateEntity( currentTime, elapsedTime );
		}
	}*/
	
	onUpdate(currentTime, elapsedTime);

} // updateScenario

// ---------------------------------------------------------------------------------

void ScenarioBase::enterScenario()
{
	SceneManager *mSceneMgr = mLump.mSceneMgr;
	
	// Scenario reset
	// //////////////////////////////////////////////////////////////////////////////

	// Chiama il metodo specifico di inizializzazione
	onEnter();

	resetScenario();

	mBehaviourUpdateDelay =  StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("behaviourUpdateDelay", "General")
	);

	// Load the particle system pools
	mSparksPool = new UtilityParticleSet( 20, 2, "Sparks", 
		"Lump/Sparks", "Lump/Sparks2" );
	mEnergyExplPool = new UtilityParticleSet( 3, 4, "EnergyExplosion", 
		"Lump/EnergyExplosion", "Lump/EnergyExplosion2" );
	mAsteroidExplPool = new UtilityParticleSet( 4, 4, "AsteroidExplosion", 
		"Lump/AsteroidExplosion" );

} // enterScenario

// ---------------------------------------------------------------------------------

void ScenarioBase::resetScenario()
{	
	// TODO: operazioni generiche di reset
	
	// Chiama il metodo specifico di reset
	onReset();
}

// ---------------------------------------------------------------------------------

void ScenarioBase::exitScenario()
{	
	// TODO: operazioni generiche di uscita
	
	// Chiama il metodo specifico di uscita
	onExit();
}

// ---------------------------------------------------------------------------------

void ScenarioBase::increaseCameraDistance(float incRatio)
{
	mCameraDistance += mCameraDistance * incRatio;
}

// ---------------------------------------------------------------------------------

void ScenarioBase::increaseCameraHeight(float incRatio)
{
	mCameraHeight += mCameraHeight * incRatio;
}

// ---------------------------------------------------------------------------------

void ScenarioBase::updateCamera( const float elapsedTime )
{	
	
	////// Main camera 
	//////////////////////////////////////////////////////////////////////////////////
	
	Vector3 cameraDirection;
	cameraDirection = mLump.mCamera->getDirection();

	// Ottiene il vettore diretto indietro del veicolo dell'utente
	Vector3 relativeForwardPosition = 
		- cameraDirection * 
		mCameraDistance;
	
	// Ottiene il vettore diretto verso l'alto del veicolo dell'utente
	Vector3 relativeUpperPosition = 
		mLump.mCamera->getUp() *
		mCameraHeight;

	Vector3 shakeEffect;
	float diffTime = mLump.getTotalElapsedTime() - mShakeCameraStartTime;
	if ( diffTime < 0 )
	{	
		shakeEffect = newRandVector3( diffTime * diffTime * 100 );
	}

	mLump.mCamera->setPosition(mPlayerEntity->getPosition() + 
		relativeForwardPosition + 
		relativeUpperPosition + 
		shakeEffect
	);

	// *************************************************
	// Linear smooth zooming
	
	Radian FOVyDistance = mLump.mCamera->getFOVy() - mCameraFOVy;
	if ( FOVyDistance < - Radian(0.01) || FOVyDistance > Radian(0.01)  )
		 mLump.mCamera->setFOVy( mLump.mCamera->getFOVy() - Radian( FOVyDistance * elapsedTime * 10 ) );

	////// Front camera 
	//////////////////////////////////////////////////////////////////////////////////
	
	/*mLump.mFrontCamera->setPosition( mPlayerEntity->getPosition() );
	mLump.mFrontCamera->setDirection( Vector3( mPlayerEntity->forward().x, 
		mPlayerEntity->forward().y, 
		- mPlayerEntity->forward().z )
		);*/


} // updateCamera


// ---------------------------------------------------------------------------------

void ScenarioBase::shakeCamera( float power )
{	
	mShakeCameraStartTime = mLump.getTotalElapsedTime() + power;
}