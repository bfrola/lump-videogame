#include "general.h"

FlockBase::FlockBase(
		const std::string& name, 
		Vector3 startPosition )
		: SteerableEntity(name, startPosition)
{
	// Lettura dei parametri dal config file
	
	// Inizializzazione flotta (parametri comportamentali)
	////////////////////////////////////////////////////////////////
	
	std::string particleEffect = mLump.getConfigFile()->getSetting("wakeParticleEffect", name);
	createWake( particleEffect );
	
	//--------------------------------------------------------------------------
	
	separationRadius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("separationRadius", name)
	);
	separationAngle = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("separationAngle", name)
	);
	separationWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("separationWeight", name)
	);

	//--------------------------------------------------------------------------

	alignmentRadius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("alignmentRadius", name)
	);
	alignmentAngle = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("alignmentAngle", name)
	);
	alignmentWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("alignmentWeight", name)
	);

	//--------------------------------------------------------------------------

	cohesionRadius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("cohesionRadius", name)
	);
	cohesionAngle = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("cohesionAngle", name)
	);
	cohesionWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("cohesionWeight", name)
	);

	//--------------------------------------------------------------------------
	
	wanderAngle = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("wanderAngle", name)
	);
	wanderWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("wanderWeight", name)
	);

	//--------------------------------------------------------------------------

	seekingWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("seekingWeight", name)
	);
	
	//--------------------------------------------------------------------------

	pursuitingWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("pursuitingWeight", name)
	);
	pursuitMaxPredictionTime = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("pursuitMaxPredictionTime", name)
	);

	//--------------------------------------------------------------------------
	
	avasioningWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("avasioningWeight", name)
	);
	evasionMaxPredictionTime = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("evasionMaxPredictionTime", name)
	);

	//--------------------------------------------------------------------------

	useAdditiveObstacleAvoidance = StringConverter::parseBool(
		mLump.getConfigFile()->getSetting("useAdditiveObstacleAvoidance", name)
	);	
	
	obstacleAvoidanceMinTimeToCollision = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("obstacleAvoidanceMinTimeToCollision", name)
	);	

	obstacleAvoidanceWeight = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("obstacleAvoidanceWeight", name)
	);


	neighborsMaxRadius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("neighborsMaxRadius", name)
	);
	
	// Inizializzazione flotta
	////////////////////////////////////////////////////////////////
	
	int itemNumber = StringConverter::parseInt(
		mLump.getConfigFile()->getSetting("itemsNumber", name)
	);

	minActiveItemsNumber = StringConverter::parseInt(
		mLump.getConfigFile()->getSetting("minActiveItemsNumber", name)
	);

	mInterleavedUpdateRangeSize = StringConverter::parseInt(
		mLump.getConfigFile()->getSetting("interleavedUpdateRangeSize", name)
	);

	// Inizializza la flotta
	std::string meshName = mLump.getConfigFile()->getSetting("meshName", name);
	
	// Create the area node as child of the entityNode
	mAreaNode = mEntityNode->createChildSceneNode();

	// Set the cloned entity as the areaNode entity
	Entity* flockEntity = createEntity( meshName );
	flockEntity = flockEntity->clone(name + "Area" 
		+ StringConverter::toString( mLump.getEntityCounter() ) );
	mAreaNode->attachObject( flockEntity );

	// Starts not visible
	mAreaNode->setVisible( false );

	
	Vector3 meshScale = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("meshScale", name)
	);
	setMeshScale(meshScale);

	mForwardVector = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("forwardVector", name)
	);

	float radius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("radius", name)
	);
	setRadius(radius);
	
	// Read the neighbors indicator
	meshName = mLump.getConfigFile()->getSetting("proximityIndicatorMeshName", name);

	meshScale = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("proximityIndicatorMeshScale", name)
	);

	createProximityIndicatorNode( meshName, meshScale );

	// Spactial DB initialization
	// //////////////////////////////////////////////////////////////////////////////
	
	/*
	const Vec3 center;
	const float div = 20.0f;
	const Vec3 divisions (div, 1.0f, div);
	const float diameter = 80.0f; //XXX need better way to get this
	const Vec3 dimensions (diameter, diameter, diameter);
	typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
	mProximityDatabase = new LQPDAV (center, dimensions, divisions);
	*/
	mProximityDatabase = NULL;

	// Flock item initialization
	/////////////////////////////////////////////////////////////////////////
	
	meshName = mLump.getConfigFile()->getSetting("meshName", name + "Item" );
	Entity* sharedEntity = mLump.mSceneMgr->createEntity(name + "Item" 
		 + StringConverter::toString( mLump.getEntityCounter() ), meshName);
	
	
	float itemMaxSpeed = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("maxSpeed", name + "Item" )
	);

	float itemMass = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("mass", name + "Item" )
	);

	float itemMaxForce = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("maxForce", name + "Item" )
	);

	// Scala e vettore avanti della mesh
	Vector3 itemMeshScale = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("meshScale", name + "Item" )
	);
	
	Vector3 itemForwardVector = StringConverter::parseVector3(
		mLump.getConfigFile()->getSetting("forwardVector", name + "Item" )
	);
	
	float itemRadius = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("radius", name + "Item" )
	);

	float itemMaxEnergy = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("energy", name + "Item" )
	);
	
	// Flock have the same max energy of the item
	setMaxEnergy( itemMaxEnergy );
	
	// Setta la scia dell'oggetto item
	std::string itemParticleEffect = mLump.getConfigFile()->getSetting("wakeParticleEffect", name + "Item" );

	for (int i = 0; i < itemNumber; i ++)
	{	
		std::string itemName = name + "Item" + Ogre::StringConverter::toString(i);
		
		EntityFlockItem* item = new EntityFlockItem(itemName, sharedEntity, this);

		item->setMaxSpeed( itemMaxSpeed );
		item->setMass( itemMass );
		item->setMaxForce( itemMaxForce );
		item->setPosition( getPosition() + newRandVector3( 500 ) );
		item->setMeshScale(itemMeshScale);
		item->setForwardVector( itemForwardVector );
		item->setRadius(itemRadius);
		item->setMaxEnergy( itemMaxEnergy );
		item->setEnergy( itemMaxEnergy );

		item->createWake( itemParticleEffect );
		mSingleEntities.push_back( item );	
	}

	// Set the distribuited parameters
	setMaxForce( itemMaxForce );
	setMaxSpeed( itemMaxSpeed );
	setMass( itemMass * itemNumber );
	
	// Inizialmente il numero di flockItem attivi è pari al numero
	// di flockItem nella lista
	mActiveFlockItemsNumber = itemNumber;
	
	// Particle Effects
	/////////////////////////////////////////////////////////////////////////

	// ***TODO Generalise
	mEnergyBurstEffect = mLump.mSceneMgr->createParticleSystem(
		"EnergyBurst" + name + StringConverter::toString( mLump.getEntityCounter() ),
		"Lump/EnergyBurst");

	getPSysNode()->attachObject( mEnergyBurstEffect );
	removeEnergyBurst();

	// Other
	/////////////////////////////////////////////////////////////////////////
	
	// Reset the impacted items number
	mImpctedFlockItemsNumber = 0;
	mLastItemImpactTime = 0;	

	boundaryHandling = false;

	mInterleavedUpdateCounter = 0;
	mInterleavedUpdateRangeSize = 2;
}


// ---------------------------------------------------------------------------------

FlockBase::~FlockBase()
{
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		delete (SteerableEntity*) (*ent_it);
	}
	mSingleEntities.clear();
}

// ---------------------------------------------------------------------------------

void FlockBase::updateBehaviour(const float currentTime, const float elapsedTime)
{
	// ************************************************
	// Check for life condition

	if ( mActiveFlockItemsNumber < minActiveItemsNumber )
	{
		return;
	}

	// ************************************************
	// Initialization

	// Posizione della flotta calcolata mediando le posizioni di tutti i suoi componenti
	Vector3 avgDir = Vector3::ZERO;
	float avgDistance = 0;
	float avgSpeed = 0;
	float avgEnergy = 0;
	Vector3 variance3d = Vector3::ZERO;
	mDamagedFlockItemsNumber = 0;

	// Da notare che position() è aggiornato al valore del ciclo update precedente
	// perchè calcolato mediando tutti i componenti della flock
	mNeighbors.clear();
	proximityToken->findNeighbors (position(), neighborsMaxRadius, mNeighbors);

	if ( mNeighbors.size() < 5 )
		neighborsMaxRadius *= 1.2;
	else if ( mNeighbors.size() > 12 )
		neighborsMaxRadius /= 1.2;
	
	// ************************************************
	// mImpctedFlockItemsNumber holding
	
	// If there is no impact for holdingImpactNumberTimeout seconds 
	// then reset the value of mImpctedFlockItemsNumber to zero
	float holdingImpactNumberTimeout = 0.1;
	if ( mLump.getTotalElapsedTime() > mLastItemImpactTime + holdingImpactNumberTimeout )
		mImpctedFlockItemsNumber = 0;
	
	// ************************************************
	// Items update and distribuite parameters refresh
	
	// Clip the mInterleavedUpdateCounter to mInterleavedUpdateRangeSize - 1
	mInterleavedUpdateCounter ++;
	if ( mInterleavedUpdateCounter > mInterleavedUpdateRangeSize )
		mInterleavedUpdateCounter = 0;

	int entityCounter = 0;

	// Update the item behaviors and distribuited parameters
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		EntityFlockItem* entity = (EntityFlockItem*) (*ent_it);
		
		
		// Effettua l'update solo dei flockItems attivi
		if ( !entity->isActive() )
			continue;
		
		// Interleaved update - update only a group of the flockItems 
		/*mLump.getLogger()->log(getName(), StringConverter::toString(entityCounter) + ", " 
			+ StringConverter::toString(mInterleavedUpdateRangeSize) + ", " 
			+ StringConverter::toString(mInterleavedUpdateCounter)  );*/
		

		if ( entityCounter % mInterleavedUpdateRangeSize == mInterleavedUpdateCounter )
			entity->updateBehaviour(currentTime, elapsedTime);		

		avgDir += entity->getDirection();
		avgSpeed += entity->speed();
		avgEnergy += entity->getEnergy();
		
		// Calculate the abs distance
		variance3d += Vector3( 
			abs(getPosition().x - entity->getPosition().x),
			abs(getPosition().y - entity->getPosition().y),
			abs(getPosition().z - entity->getPosition().z) );
		
		// Dectect the approximate distance from the center of the flock
		float distanceFromTheCenter = distance( entity );
				
		avgDistance += distanceFromTheCenter;

		if ( entity->getEnergy() < entity->getMaxEnergy() / 3 )
		{
			mDamagedFlockItemsNumber ++;
		}
		
		// Manage highlighing 
		/* **** FUNCTIONALITY SUSPENDED
		if (mOldHighLighted != mHighlighted)
			entity->getEntityNode()->showBoundingBox( mHighlighted );*/

		entityCounter ++;
	}

	if (mOldHighLighted != mHighlighted)
		mAreaNode->setVisible( mHighlighted );

	
	// Divide per il numero di flockItems attivi
	if ( mActiveFlockItemsNumber >= 1)
	{
		avgDir /= mActiveFlockItemsNumber;
		setDirection( avgDir );

		avgSpeed /= mActiveFlockItemsNumber;
		setSpeed( avgSpeed );

		avgEnergy /= mActiveFlockItemsNumber;
		setEnergy( avgEnergy );

		variance = (variance3d / mActiveFlockItemsNumber).length();
		
		// Set the area node boudings		
		mAreaNode->setScale( variance3d / mActiveFlockItemsNumber );

	}
	
	// Comportamento specifico della flotta
	onUpdateFlock(currentTime, elapsedTime);

	// Aggiornamento della posizione nel PD
	// Serve per essere individuato dalla findNeighbors
	proximityToken->updateForNewPosition (position());

	mOldHighLighted = mHighlighted;
}


void FlockBase::onUpdate(const float currentTime, const float elapsedTime)
{	
	if ( mActiveFlockItemsNumber < minActiveItemsNumber )
	{
		if ( mProximityIndicatorNode != NULL )
			mProximityIndicatorNode = NULL;
		return;
	}

	Vector3 avgPos = Vector3::ZERO;
	// Update the items state and flock position
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		EntityFlockItem* entity = (EntityFlockItem*) (*ent_it);
		
		// Effettua l'update solo dei flockItems attivi
		if ( !entity->isActive() )
			continue;

		entity->updateEntity(currentTime, elapsedTime);

		avgPos += entity->getPosition();

	}

	if ( mActiveFlockItemsNumber >= 1)
	{
		// Set the distribuited parameters
		avgPos /= mActiveFlockItemsNumber;
		setPosition( avgPos );
	}
}

// ---------------------------------------------------------------------------------

void FlockBase::show()
{
	//mEntityNode->attachObject(mEntityEnt);

	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->show();
	}
}

// ---------------------------------------------------------------------------------

void FlockBase::hide()
{
	//mEntityNode->detachObject(mEntityEnt);

	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->hide();
	}
}

// ---------------------------------------------------------------------------------

void FlockBase::onImpactWith( SteerableEntity* impacted, float impactWeight )
{

}

// ---------------------------------------------------------------------------------

// Sovrascrive il metodo di SteerableBase
void FlockBase::increaseSpeed(float incRatio)
{
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->increaseSpeed( incRatio );
	}
}

// ---------------------------------------------------------------------------------

// Sovrascrive il metodo di SteerableBase
void FlockBase::increaseMaxSpeed(float incRatio)
{
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->increaseMaxSpeed( incRatio );
	}
}

// Sovrascrive il metodo di SteerableBase
void FlockBase::increaseMaxForce(float incRatio)
{
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->increaseMaxForce( incRatio );
	}
}

// ---------------------------------------------------------------------------------

void FlockBase::setFlockItemActiveState( SteerableEntity* item, bool active )
{
	EntityFlockItem* flockItem = (EntityFlockItem*) item;

	bool oldActive = flockItem->isActive();
	flockItem->setActive( active );
	// Se prima era attivo ed è stato disattivato...
	if ( oldActive && !active )
	{
		mActiveFlockItemsNumber --;

		if ( mActiveFlockItemsNumber < minActiveItemsNumber )
		{
			// DIE

			for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
			{
				EntityFlockItem* entity = (EntityFlockItem*) (*ent_it);
				entity->setActive( false );
			}

			mLump.getEventsLogger()->log( "Yeah!", "You have destroyed a flock!" );

			setHighlighted( false );
			removeEnergyBurst();

			mLump.getCurrentScenario()->getCurrentSector()->annotateFlockDestroyed();
		}

	} 
	// Se prima non era attivo ed è stato attivato...
	else if (!oldActive && active)
	{
		mActiveFlockItemsNumber ++;
	}
}

// ---------------------------------------------------------------------------------

void FlockBase::increaseImpctedFlockItemsNumber() { 
	
	// Big impact
	// If there is a high number of impact and there is no an other impact running on
	if ( mImpctedFlockItemsNumber > mActiveFlockItemsNumber * 0.25  )
	{
		if ( mBeforeImpactSpeed > maxSpeed() * 0.30 && 
				variance < 1000 && mLump.getTotalElapsedTime() > mLastRecordedTime ) 
		{
			// Make big explosion
			mLump.getCurrentScenario()->getEnergyExplPool()->startNextPS( getPosition() + newRandVector3( 300 ) );
			
			// Shake the camera 
			// The pawer is based of the distance of the playership to the explosion
			mLump.getCurrentScenario()->shakeCamera( 200 / 
				distance( mLump.getCurrentScenario()->getPlayerEntity() ) 
			);
		}
		
		// Reset time and impacted counters		
		mLastRecordedTime = mLump.getTotalElapsedTime() + 0.5;
		mImpctedFlockItemsNumber = 0;
		mLastItemImpactTime = 0;


		return;
	} 
	// Backup the value of speed and direraction before starting impact
	else if ( mImpctedFlockItemsNumber == 0 )
	{
		mBeforeImpactSpeed = speed();
		mBeforeImpactDirection = getDirection();
	}

	// Increase the value
	mImpctedFlockItemsNumber++; 
	// Reset the holding time
	mLastItemImpactTime = mLump.getTotalElapsedTime();
}

// ---------------------------------------------------------------------------------

// Set the maxSpeed for each item
float FlockBase::setMaxSpeed (float ms)
{
	SteerableBase::setMaxSpeed( ms );

	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->setMaxSpeed( ms );
	}
	return ms;
}

// ---------------------------------------------------------------------------------


float FlockBase::setMaxForce (float mf)
{
	SteerableBase::setMaxForce( mf );

	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*ent_it);
		entity->setMaxForce( mf );
	}
	return mf;
}

// ---------------------------------------------------------------------------------

float FlockBase::getAimPrecision()
{
	if ( targetEntity != NULL )
	{
		return getDirection().dotProduct(
				( targetEntity->getPosition() - getPosition() ).normalisedCopy()
			);
	}
	return -1;
}

// ---------------------------------------------------------------------------------

void FlockBase::addEnergyBurst()
{
	if ( mEnergyBurstEffect == NULL )
		return;

	mEnergyBurstEffect->getEmitter( 0 )->setEnabled( true );
}

// ---------------------------------------------------------------------------------

void FlockBase::removeEnergyBurst()
{
	if ( mEnergyBurstEffect == NULL )
		return;

	mEnergyBurstEffect->getEmitter( 0 )->setEnabled( false );
}
