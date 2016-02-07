#include "general.h"

EntityFlockItem::EntityFlockItem(const std::string& name, 
		Ogre::Entity* baseEntity, 
		FlockBase* newFlockRef,
		Ogre::Vector3 startPosition,
		Ogre::Quaternion startOrientation)
		: SteerableEntity(name, 
			startPosition, 
			startOrientation)
{	
	mFlockRef = newFlockRef;

	setPosition( (Vector3::UNIT_X).randomDeviant( Radian(1) ) );

	mLump.increaseEntityCounter();

	createEntityInstance( baseEntity );
	
	// By default, flockitems are set to active
	active = true;
	
	if ( mFlockRef->useDedicatedPD() )
	{
		registerToProximityDB( mFlockRef->getProximityDatabase() );
		proximityToken->updateForNewPosition( position() );
	} 
	else
	{
		mNeighbors = mFlockRef->getFlockItems();
	}

	
	mNeighborsMaxRadius = 500;
}

// ---------------------------------------------------------------------------------

EntityFlockItem::~EntityFlockItem(void)
{
	
}

// ---------------------------------------------------------------------------------

void EntityFlockItem::onUpdate(const float currentTime, const float elapsedTime)
{

	// Update only if the flock item is active
	if ( !active )
		return;
		
	applySteeringForce( mSteeringForce, elapsedTime );
}

// ---------------------------------------------------------------------------------

// Update the behaviour
void EntityFlockItem::updateBehaviour(const float currentTime, const float elapsedTime)
{	

	//mLump.getLogger()->log( getName(), "Update behaviour" );
	if ( mGeneralState == STATE_COLLIDING )
	{
		// Dopo un certo numero di secondi lo stato di collisione termina
		if ( mLump.getTotalElapsedTime() > mLastRecordedTime + 3 )
		{
			mLastRecordedTime = 0;
			mFlockRef->wanderWeight = mLastWanderingWeight;
			
			mGeneralState = STATE_RUNNING;

			setMaxSpeed( oldMaxSpeed );
		}
	
		applyBrakingForce( 1.0, elapsedTime );
	} 


	////////////////////////////////////////////////
	/// Collision and steering force

	checkForCollisions( mFlockRef->getNeighbors() );
	mSteeringForce = calcSteeringForce();
	
	////////////////////////////////////////////////
	/// dynamic neighborood processing
	
	// The dynamic setup allow to keep the neigborhood size into a certain range

	if ( mFlockRef->useDedicatedPD() )
	{
		// Update the spactial position
		proximityToken->updateForNewPosition( position() );
		
		// Check for the number of neighbors of the last frame
		// If is too much high the mNeighborsMaxRadius is decreased
		if ( mNeighbors.size() > 100 )
		{
			mNeighborsMaxRadius /= 1.5;
		}
		// else is increased
		else if ( mNeighbors.size() < 10 )
		{
			mNeighborsMaxRadius *= 1.5;
		}

		// Find the neighbors
		mNeighbors.clear();
		proximityToken->findNeighbors (position(), mNeighborsMaxRadius, mNeighbors);
	}

	////////////////////////////////////////////////
	/// Energy auto-increasing
	
	if ( mEnergy < mMaxEnergy ) 
		mEnergy += 0.1;
}

// ---------------------------------------------------------------------------------

void EntityFlockItem::onImpactWith( SteerableEntity* impacted, float impactWeight )
{	
	mFlockRef->increaseImpctedFlockItemsNumber();
	
	if ( mEnergy < 1 )
	{
		mFlockRef->setFlockItemActiveState( this, false );
		mEnergy = 0;

		setDirection( newRandVector3( 2 ) );

		return;
	}

	// Passaggio allo stato di collisione
	if ( mGeneralState == STATE_RUNNING )
	{
		// Backup del valore wanderWeight
		mLastWanderingWeight = mFlockRef->wanderWeight;
		
		mLump.getCurrentScenario()->getSparksPool()->startNextPS( getPosition() );
		
		mGeneralState = STATE_COLLIDING;
		
		// Backup dei valori da ripristinare
		mLastRecordedTime = mLump.getTotalElapsedTime();

		oldMaxSpeed = maxSpeed();
	} 
	
	Vector3 repulsionVector = (getPosition() - impacted->getPosition() ) * 2;
	setDirection( repulsionVector.normalisedCopy() );
	// Speed setted into SteerableEntity's method onImpactWithItem
}

// ---------------------------------------------------------------------------------

Vec3 EntityFlockItem::calcSteeringForce()
{
	Vec3 steeringForce = Vec3::zero;
	
	// Evita gli ostacoli
	// Sistema non additivo
	if (!mFlockRef->useAdditiveObstacleAvoidance)
	{
		//Vec3 avoidance = steerToAvoidObstacles(2.0f, mLump.getCurrentScenario()->getObstacles() );
		Vec3 avoidance = steerToAvoidNeighbors( 
			mFlockRef->obstacleAvoidanceMinTimeToCollision, 
			mFlockRef->getNeighbors() );
		if (avoidance != Vec3::zero)
		{
			return avoidance;
		}
	} 
	else
	// Sistema additivo
	{
		if (mFlockRef->obstacleAvoidanceWeight > 0)
			steeringForce += steerToAvoidNeighbors(
				mFlockRef->obstacleAvoidanceMinTimeToCollision, 
				mFlockRef->getNeighbors() )*
			mFlockRef->obstacleAvoidanceWeight;
	}
	
	// Applicazione parametri della flotta di appartenenza
	if ( mFlockRef->targetNode != NULL && mFlockRef->seekingWeight > 0 )
	{
		Vector3 targetNodePos = mFlockRef->targetNode->getPosition();
		steeringForce += steerForSeek(
			Vec3(targetNodePos.x, targetNodePos.y, targetNodePos.z)) * 
		mFlockRef->seekingWeight;
	}

	if ( mFlockRef->targetEntity != NULL && mFlockRef->pursuitingWeight > 0 )
	{
		steeringForce += steerForPursuit( * ( mFlockRef->targetEntity ), 
			mFlockRef->pursuitMaxPredictionTime ) * 
		mFlockRef->pursuitingWeight;
	}
	
	if ( mFlockRef->chaserEntity != NULL && mFlockRef->avasioningWeight > 0 )
	{
		steeringForce += steerForEvasion( * ( mFlockRef->chaserEntity ), 
			mFlockRef->evasionMaxPredictionTime ) * 
		mFlockRef->avasioningWeight;
	}

	if (mFlockRef->wanderWeight > 0)
	{
		float realWanderingWeight;
		float realWanderAngle;
		// In stato di collisione l'item perde controllo
		// ed aumenta il direzionamento casuale
		if ( mGeneralState == STATE_COLLIDING )
		{
			realWanderingWeight = 3000;
			realWanderAngle = 2.0f;
		}
		else
		{
			realWanderingWeight = mFlockRef->wanderWeight;
			realWanderAngle = mFlockRef->wanderAngle;
		}

		steeringForce += steerForWander(
			realWanderAngle) * 
		realWanderingWeight;
	}
	
	if (mFlockRef->separationWeight > 0)
	{
		steeringForce += steerForSeparation( 
			mFlockRef->separationRadius, 
			mFlockRef->separationAngle, 
			mNeighbors ) * 
		mFlockRef->separationWeight;		
	}

	if (mFlockRef->alignmentWeight > 0)
		steeringForce += steerForAlignment (
			mFlockRef->alignmentRadius, 
			mFlockRef->alignmentAngle,
			mNeighbors ) * 
		mFlockRef->alignmentWeight;

	if (mFlockRef->cohesionWeight > 0)
		steeringForce += steerForCohesion (
			mFlockRef->cohesionRadius,
			mFlockRef->cohesionAngle,
			mNeighbors ) * 
		mFlockRef->cohesionWeight;

	if (mFlockRef->boundaryHandling) 
		steeringForce += handleBoundary();

	// Operazioni di debug
	return steeringForce;
}

// ---------------------------------------------------------------------------------

Vec3 EntityFlockItem::handleBoundary (void)
{

	//mLump.getLogger()->log(getName(), "Check boundary");
    // while inside the boundary do noting
    if ( intersectsFlockBoundary() ) 
		return Vec3::zero;
	
	Vector3 boxCenter = ( mFlockRef->getBoundary().getMinimum() + mFlockRef->getBoundary().getMinimum() ) / 2;

    // steer back when outside
    const Vec3 seek = steerForSeek ( Vec3( boxCenter.x, boxCenter.y, boxCenter.z ) ) ;
    const Vec3 lateral = seek.perpendicularComponent (forward ()) * 4000;
	return lateral;
}

// ---------------------------------------------------------------------------------

bool EntityFlockItem::intersectsFlockBoundary (void)
{
	return mFlockRef->getBoundary().intersects( getPosition() );
}

// ---------------------------------------------------------------------------------

void EntityFlockItem::checkForFlockCollisionsBetweenFlockItems( AVGroup neighborsItems ) {

	// Iterate over all items into the flock
	for (AVIterator items_it = neighborsItems.begin(); 
		items_it != neighborsItems.end(); ++items_it) 
	{
		EntityFlockItem* neighborItem = (EntityFlockItem*)(*items_it);
		
		// Don't check for the same item
		if ( neighborItem == this )
			continue;
		
		// Divide by a constant because the radius are greater than the real bounds
		if ( distance( neighborItem ) < radius() + neighborItem->radius() ) {
			impactBetweenFlockItems( neighborItem );
			return;
		}
		
	}
}

// ---------------------------------------------------------------------------------

void EntityFlockItem::impactBetweenFlockItems( SteerableEntity* impacted ) {
	

	/////////////////////////////////////////////
	// Energy reduction

	Vector3 thisDir = this->getDirection().normalisedCopy();
	Vector3 impactedDir = impacted->getDirection().normalisedCopy();

	float impactWeight = ( 1 + thisDir.dotProduct( impactedDir ) ) * ( this->speed() + impacted->speed() ) / 500;
			
	/////////////////////////////////////////////
	// Energy reduction	
	mEnergy -= impactWeight;
	impacted->setEnergy( impacted->getEnergy() - impactWeight );

	/////////////////////////////////////////////
	// Impact dynamics
	
	onImpactWith( impacted, impactWeight );
	impacted->onImpactWith( this, impactWeight );
	
	float combSpeed = 3000;//( speed() + impacted->speed() ) * 2;

	setMaxSpeed( combSpeed );
	setSpeed( combSpeed );
	impacted->setMaxSpeed( combSpeed );
	impacted->setSpeed( combSpeed );
}

// ---------------------------------------------------------------------------------

void EntityFlockItem::checkForResume()
{
	//if ( 
}

void EntityFlockItem::resume()
{
	mFlockRef->setFlockItemActiveState( this, true );
	setEnergy( 1 );
	setSpeed( 0 );
}
