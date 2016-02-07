#include "general.h"

EntityPlayerShip::EntityPlayerShip(
	const std::string& name, 
	Ogre::Vector3 startPosition,
	Ogre::Quaternion startOrientation)
	: SteerableEntity(name, 
		startPosition, 
		startOrientation)
{
	// Creazione di una mesh dedicata
	
	std::string meshName = mLump.getConfigFile()->getSetting("meshName", "PlayerShip");
	createEntity( meshName );
	
	// Scala e vettore avanti della mesh
	
	Vector3 meshScale = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("meshScale", "PlayerShip")
	);
	setMeshScale(meshScale);	
	
	mForwardVector = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("forwardVector", "PlayerShip")
	);

	float radius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("radius", "PlayerShip")
	);
	setRadius(radius);

	// Setta la scia dell'oggetto
	
	std::string particleEffect = mLump.getConfigFile()->getSetting("wakeParticleEffect", "PlayerShip");
	createWake( particleEffect );

	speedUpperBound = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("speedUpperBound", "PlayerShip")
	);	

	// Parametri comportamentali
	useAdditiveObstacleAvoidance = StringConverter::parseBool(
		mLump.getConfigFile()->getSetting("useAdditiveObstacleAvoidance", "PlayerShip")
	);	
	
	obstacleAvoidanceMinTimeToCollision = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("obstacleAvoidanceMinTimeToCollision", "PlayerShip")
	);	

	obstacleAvoidanceWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("obstacleAvoidanceWeight", "PlayerShip")
	);	

	neighborsMaxRadius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("neighborsMaxRadius", "PlayerShip")
	);

	float mass = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("mass", "PlayerShip")
	);

	setMass( mass );
	
	float maxForce = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("radius", "PlayerShip")
	);

	setMaxForce(maxForce);	

	float maxSpeed = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("maxSpeed", "PlayerShip")
	);

	setMaxSpeed(maxSpeed);

	mMaxEnergy = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting( "energy", "PlayerShip" )
	);

	mEnergy = mMaxEnergy;

	// Setup target node
	///////////////////////////////////////////////////////////////////////////////

	mTargetNode = mLump.mSceneMgr->getRootSceneNode()->createChildSceneNode( name + "TargetNode" );
	
	meshName = mLump.getConfigFile()->getSetting("targetMeshName", name);

	Entity *targetEntity = mLump.mSceneMgr->createEntity( "FlockTargetEntity", meshName );

	targetEntity->setCastShadows( false );
	mTargetNode->attachObject( targetEntity );
	mTargetNode->setScale( Vector3(0.1, 0.1, 0.1) );

	mTargetNode->setVisible( false );

	// Registering to the sectors proximity database
	///////////////////////////////////////////////////////////////////////////////

	mSectorsProximityToken = mLump.getCurrentScenario()->getProximityDatabase()->allocateToken (this);

	mSectorScanningRadius = StringConverter::parseReal(
		mLump.getSceneConfigFile()->getSetting("sectorScanningRadius", "General")
	);

	steerToForward = steerToBackward = false;
	steerToLeft = steerToRight = false;
	steerToUp = steerToDown = false;

	mLastEnergyValue = mEnergy;
}

// ---------------------------------------------------------------------------------

EntityPlayerShip::~EntityPlayerShip(void)
{
	
}

// ---------------------------------------------------------------------------------

void EntityPlayerShip::onUpdate(const float currentTime, const float elapsedTime)
{
	// First operation check the sector membership
	checkForSectorMembership();
	
	proximityToken->updateForNewPosition ( position() );

	mNeighbors.clear();
	proximityToken->findNeighbors (position(), neighborsMaxRadius, mNeighbors);

	setRepulsionVector( Vector3::ZERO );
	
	checkForCollisions( mNeighbors );

	if ( mGeneralState == STATE_COLLIDING )
	{
		// Dopo un certo numero di secondi lo stato di collisione termina
		if ( mLump.getTotalElapsedTime() > mLastRecordedTime + 2 )
		{
			mLastRecordedTime = 0;
			
			mGeneralState = STATE_RUNNING;

			setMaxSpeed( oldMaxSpeed );
		}
	} 
	
	// State RUNNING
	else if (speed() > 190)
		applyBrakingForce( 1, elapsedTime );
	

	// Update d node
	////////////////////////////////////////////////////////////////////

	// Update the playership orientation
	// Resolve the mismatch of the roll value between the playership and the camera
	setOrientation( mLump.mCamera->getOrientation() );
	Vector3 targetDirection = mLump.mCamera->getDirection();
	float distanceFactor;

	switch ( mBehaviourState )
	{
	/////////////////////////////////////////////////////////////////////////////
	case STATE_SEEKING_CAMERA:
		// Target is distance enough
		distanceFactor = 
			//mLump.getCurrentScenario()->getCameraDistance() * 4  + 
			//mLump.getCurrentScenario()->getCameraHeight() * 2 + 300;
			20000;

		// Check for lateral steering
		if ( isLateralSteering() )
		{
			// Aggiornamento della posizione del target della navetta.
			Vector3 horizSteering = 
				steerToForward ? mLump.mCamera->getDirection() : (
					steerToBackward ? -mLump.mCamera->getDirection() : Vector3::ZERO );

			horizSteering += 
				steerToLeft ? -getRight() : (
					steerToRight ? getRight() : Vector3::ZERO );

			Vector3 vertSteering = 
				steerToUp ? getUp() : (
					steerToDown ? -getUp() : Vector3::ZERO );
			
			targetDirection = horizSteering + vertSteering;
		}

		mTargetNode->setPosition(
			getPosition() +
			targetDirection * distanceFactor
		);

		applySteeringForce( steeringForce(), elapsedTime );

		// Reset steering params
		steerToForward = steerToBackward = false;
		steerToLeft = steerToRight = false;
		steerToUp = steerToDown = false;

		break;
	/////////////////////////////////////////////////////////////////////////////
	case STATE_SEEKING_LOCKED:

		applySteeringForce( velocity(), elapsedTime );

		break;
	}
}

// ---------------------------------------------------------------------------------

bool EntityPlayerShip::isLateralSteering() 
{ 
	return 
		/*steerToForward || */steerToBackward || 
		steerToLeft || steerToRight || 
		steerToUp || steerToDown; 
}

// ---------------------------------------------------------------------------------

void EntityPlayerShip::checkForSectorMembership()
{

	AVGroup sectors; 
	mSectorsProximityToken->findNeighbors (position(), mSectorScanningRadius, sectors);
	
	// Initialize oldDistance to a well known value
	float minDistance = -1;
	ScenarioSectorBase* selectedSector = NULL;
	for (AVIterator sectors_it = sectors.begin(); 
			sectors_it != sectors.end(); ++sectors_it)
	{
		
		if ( typeid( **sectors_it ) != typeid ( ScenarioSectorBase ) )
			continue;
		
		ScenarioSectorBase* sector = (ScenarioSectorBase*) (*sectors_it);
		
		float currDistance = distance( sector->getPosition() );
		
		//mLump.getLogger()->log("Sect", sector->getName() + " " + StringConverter::toString( currDistance ) );
		
		// Check wheather the playership is into the sector
		if ( currDistance < sector->getRadius() )
		{
			// When the playership enter more than one sector
			// select the sector with the shortest radius
			if ( selectedSector == NULL || minDistance > currDistance  )
			{
				selectedSector = sector;
				minDistance = currDistance;
			}

			sector->enter();
		} 
		else
		{
			sector->setHidden();
		}

	}
	
	// Only the selected sector becomes active
	// The method setCurrentSector check for invalid sectors (null or equal to currentSector)
	mLump.getCurrentScenario()->setCurrentSector( selectedSector );
}

// ---------------------------------------------------------------------------------

void EntityPlayerShip::onImpactWith( SteerableEntity* impacted, float impactWeight )
{
	// Passaggio allo stato di collisione
	if ( mGeneralState == STATE_RUNNING )
	{
		// Backup del valore wanderWeight
		//mWanderWeight = 20;

		mLump.getCurrentScenario()->getSparksPool()->startNextPS( getPosition() );

		mGeneralState = STATE_COLLIDING;
		
		// Backup dei valori da ripristinare
		mLastRecordedTime = mLump.getTotalElapsedTime();

		oldMaxSpeed = maxSpeed();

		if (typeid( *impacted ) == typeid( EntityAsteroid ) )
			setMaxSpeed( maxSpeed() + impacted->maxSpeed() );
	} 
	
	// Simulate the impact shaking the camera
	mLump.getCurrentScenario()->shakeCamera( 0.75 + impactWeight / 5 );

	if ( mLastEnergyValue < ( mMaxEnergy / 10 ) && ( mEnergy >= mMaxEnergy / 10 ) )
	{
		mLump.getEventsLogger()->log("Danger", "The ship energy very low!");
	}
	else if ( mLastEnergyValue < mMaxEnergy / 2 && mEnergy >= mMaxEnergy / 2 )
	{
		mLump.getEventsLogger()->log("Warning", "The ship energy is low!");
	}

	mLastEnergyValue = mEnergy;
}

// ---------------------------------------------------------------------------------

Vec3 EntityPlayerShip::steeringForce()
{
	Vec3 steeringForce = Vec3::zero;
		
	// Evita gli ostacoli
	// Sistema non additivo
	if (!useAdditiveObstacleAvoidance)
	{
		//Vec3 avoidance = steerToAvoidObstacles(2.0f, mLump.getCurrentScenario()->getObstacles() );
		Vec3 avoidance = steerToAvoidNeighbors( 
			obstacleAvoidanceMinTimeToCollision, 
			mNeighbors );
		if (avoidance != Vec3::zero)
		{
			return avoidance;
		}
	} 
	else
	// Sistema additivo
	{
		if (obstacleAvoidanceWeight > 0)
			steeringForce += steerToAvoidNeighbors(
				obstacleAvoidanceMinTimeToCollision, 
				mNeighbors )*
			obstacleAvoidanceWeight;
	}
	
	Vector3 targetPos = mTargetNode->getPosition();
	
	// Applicazione parametri della flotta di appartenenza
	steeringForce += steerForSeek(
		Vec3(targetPos.x, targetPos.y, targetPos.z));
	
	return steeringForce;
}

// ---------------------------------------------------------------------------------

// Update the behaviour
void EntityPlayerShip::updateBehaviour(const float currentTime, const float elapsedTime)
{	
	// TODO
}	

// ---------------------------------------------------------------------------------

void EntityPlayerShip::lockDirection()
{
	if ( mBehaviourState == STATE_SEEKING_LOCKED )
		return;

	mBehaviourState = STATE_SEEKING_LOCKED;
	mLump.getLogger()->log(getName(), "View locked" );
} // lockDirection

// ---------------------------------------------------------------------------------

void EntityPlayerShip::unlockDirection()
{
	if ( mBehaviourState == STATE_SEEKING_CAMERA )
		return;

	mBehaviourState = STATE_SEEKING_CAMERA;
	mLump.getLogger()->log(getName(), "View unlocked" );
} // unlockDirection

