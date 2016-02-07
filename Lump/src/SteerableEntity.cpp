#include "general.h"

using namespace Ogre;

SteerableEntity::SteerableEntity(
	const std::string& name, 
	Ogre::Vector3 startPosition,
	Ogre::Quaternion startOrientation)

:	mEntityName(name),
	mInitialEntityPos(startPosition),
	mInitialEntityOrient(startOrientation),
	mEntityNode(0),
	mEntityEnt(0),
	mForwardVector(Ogre::Vector3::UNIT_X),
	hasAnimation(false)
{
	assert(name != "");

	// Create parent sceneNode.
	mEntityNode = mLump.mSceneMgr->getRootSceneNode()->createChildSceneNode(mInitialEntityPos);
	setPosition(startPosition);

	mName = name;
	mGeneralState = STATE_RUNNING;

	setHighlighted( false );

	mLump.increaseEntityCounter();

	// By default, entities are not static
	mIsStatic = false;
	
	// By defuault, indicator node is NULL
	mProximityIndicatorNode = NULL;
}

// ---------------------------------------------------------------------------------


SteerableEntity::~SteerableEntity() {
}

// ---------------------------------------------------------------------------------

void SteerableEntity::updateEntity(const float currentTime, const float elapsedTime) {
	
	onUpdate(currentTime, elapsedTime);
	
	OpenSteer::AbstractVehicle* vehicle = this;

	// Update the scene node (position and direction)
	mEntityNode->setPosition(Ogre::Vector3(vehicle->position().x,vehicle->position().y,vehicle->position().z));
	Vector3 forwDir = Ogre::Vector3(vehicle->forward().x, vehicle->forward().y, vehicle->forward().z);
	mEntityNode->setDirection(forwDir, Ogre::SceneNode::TS_WORLD, mForwardVector);  

	if ( mProximityIndicatorNode != NULL )
		mProximityIndicatorNode->setVisible( false, false );
}

// ---------------------------------------------------------------------------------

void SteerableEntity::registerToProximityDB( ProximityDatabase* pdb )
{
	// Registration on the proximity DB
	///////////////////////////////////////////////////////////////////////////////

	proximityToken = pdb->allocateToken (this);
	
	// Initial position
	proximityToken->updateForNewPosition (position());
}

// ---------------------------------------------------------------------------------

void SteerableEntity::resetEntity()
{
	mEntityNode->setPosition(mInitialEntityPos);
	mEntityNode->setOrientation(mInitialEntityOrient);

	Vector3 nodeForwardVec = mEntityNode->_getDerivedOrientation() * Vector3::NEGATIVE_UNIT_Z;

	OpenSteer::Vec3 startPos = OpenSteer::Vec3(mInitialEntityPos.x, mInitialEntityPos.y, mInitialEntityPos.z);

	SteerableBase::reset(); // reset the vehicle 

	static_cast<AbstractVehicle*>(this)->setUp(OpenSteer::Vec3(0, 1, 0));
	static_cast<AbstractVehicle*>(this)->setPosition(startPos);

	setSpeed (1.0f);         // speed along Forward direction.
	setMaxForce (mMaxForce);     // steering force is clipped to this magnitude
	setMaxSpeed (mMaxSpeed);     // velocity is clipped to this magnitude
	setRadius (mRadius);
}

// ---------------------------------------------------------------------------------

Ogre::Entity* SteerableEntity::createEntityInstance(Ogre::Entity* baseEntity, bool attachToNode) 
{
	assert( mEntityEnt == NULL ); 
	mEntityEnt = baseEntity->clone( mEntityName + StringConverter::toString( mLump.getEntityCounter() ) );
	mEntityEnt->setCastShadows( false );

	if ( attachToNode )
		mEntityNode->attachObject(mEntityEnt);

	return mEntityEnt;
}

Ogre::Entity* SteerableEntity::createEntity(const std::string meshName, bool attachToNode)
{
	assert( mEntityEnt == NULL ); 
	assert( meshName != "" ); 
	mEntityEnt = mLump.mSceneMgr->createEntity( 
		mEntityName + StringConverter::toString( mLump.getEntityCounter() ) , meshName );
	mEntityEnt->setCastShadows( false );
	
	if ( attachToNode )
		mEntityNode->attachObject(mEntityEnt);

	return mEntityEnt;
}

// ---------------------------------------------------------------------------------


Ogre::Entity* SteerableEntity::createProximityIndicatorNode( const std::string meshName, Vector3 scale )
{
	assert( mProximityIndicatorNode == NULL ); 
	if ( meshName == "" )
		return NULL;

	mProximityIndicatorNode = 
		mLump.mSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3::ZERO );	

	Entity* piEntity = mLump.mSceneMgr->createEntity( 
		meshName + "ProximityIndicatorNode" + StringConverter::toString( mLump.getEntityCounter() ) , meshName );

	mProximityIndicatorNode->attachObject( piEntity );
	mProximityIndicatorNode->setVisible( false, false );
	mProximityIndicatorNode->setScale( scale );

	return piEntity;

}

// ---------------------------------------------------------------------------------

void SteerableEntity::createWake(const std::string wakeEffectName, Vector3 relativePosition, const std::string wakeName)
{
	// Se il valore è nullo non fa niente
	if (wakeEffectName == "")
		return;

	// Sistema particellare per simulare la scia del razzo posteriore
	// Nome dell'effetto concatenato al nome univoco dell'entity
	ParticleSystem* enginePSys = mLump.mSceneMgr->createParticleSystem(
		mEntityName + wakeName + StringConverter::toString( mLump.getEntityCounter() ), 
		wakeEffectName);
	
	// Parte dalla parte posteriore
	Ogre::SceneNode *particleNode = mEntityNode->createChildSceneNode(
		- (mForwardVector * radius()) + relativePosition);
	
	particleNode->attachObject(enginePSys);
	
}

// ---------------------------------------------------------------------------------

bool SteerableEntity::updateRenderingStatus()
{
	// Operation completed
	if ( mRenderingState ==  STATE_VISIBLE || mRenderingState == STATE_HIDDEN )
		return true;

	const float stepDuration = 0.1;
	const float alphaVariation = 0.001;

	// Use mLastRecordedTime for creating a discrete time division
	if ( mLump.getTotalElapsedTime() > mLastRecordedTime + stepDuration )
	{
		// Get the entity diffuse colour 
		// entityColour.a contains the alpha value of the entity's material
		ColourValue entityColour = mEntityEnt->getSubEntity(0)->getMaterial()->
			getTechnique(0)->getPass(0)->getDiffuse();
	
		//mLump.getLogger()->log(getName(), StringConverter::toString( mRenderingState == STATE_HIDING ) );

		switch ( mRenderingState )
		{
		case STATE_SHOWING:
			entityColour.a += alphaVariation;
			
			if ( entityColour.a > 1 )
			{
				entityColour.a = 1;
				// The entity is already attached to the node
				mRenderingState = STATE_VISIBLE;
			}
			break;
		case STATE_HIDING:
			entityColour.a -= alphaVariation;

			if ( entityColour.a < 0 )
			{
				entityColour.a = 0;
				// detach the entity to the node
				hide();
			}
			break;
		}
		// Apply the modified colour
		mEntityEnt->getSubEntity(0)->getMaterial()->
			getTechnique(0)->getPass(0)->setDiffuse( entityColour );

		mLastRecordedTime = mLump.getTotalElapsedTime();
	} // If step has exired

	return false;
}

// ---------------------------------------------------------------------------------

void SteerableEntity::show()
{
	mRenderingState = STATE_VISIBLE;
	mEntityNode->attachObject(mEntityEnt);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::hide()
{
	mRenderingState = STATE_HIDDEN;
	mEntityNode->detachObject(mEntityEnt);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::startSmoothShowing()
{
	if ( mRenderingState != STATE_HIDDEN )
		return;
	
	mEntityNode->attachObject(mEntityEnt);

	mRenderingState = STATE_SHOWING;

	// Set the entity totally invisible (set alpha channel = 0)
	ColourValue entityColour = mEntityEnt->getSubEntity(0)->getMaterial()->
		getTechnique(0)->getPass(0)->getDiffuse();
	entityColour.a = 0;
	// Apply the modified colour
	mEntityEnt->getSubEntity(0)->getMaterial()->
			getTechnique(0)->getPass(0)->setDiffuse( entityColour );

	mLastRecordedTime = mLump.getTotalElapsedTime();
}

// ---------------------------------------------------------------------------------

void SteerableEntity::startSmoothHiding()
{
	if ( mRenderingState != STATE_VISIBLE )
		return;

	mRenderingState = STATE_HIDING;

	// Set the entity totally visible (set alpha channel = 1)
	ColourValue entityColour = mEntityEnt->getSubEntity(0)->getMaterial()->
		getTechnique(0)->getPass(0)->getDiffuse();
	entityColour.a = 1;
	// Apply the modified colour
	mEntityEnt->getSubEntity(0)->getMaterial()->
			getTechnique(0)->getPass(0)->setDiffuse( entityColour );

	mLastRecordedTime = mLump.getTotalElapsedTime();
}

// ---------------------------------------------------------------------------------

void SteerableEntity::checkForCollisions( AVGroup neighbors )
{

	// Questa condizione serve per evitare che si effettui il controllo
	// delle collisioni con se stessi
	if ( neighbors.size() < 2 )
		return;
	bool collide = false;
	for (AVIterator neighbors_it=neighbors.begin(); 
			neighbors_it != neighbors.end(); ++neighbors_it)
	{
		SteerableEntity* neighbor = (SteerableEntity*)(*neighbors_it);
		
		// Evita il calcolo della collisione per se stesso
		if ( neighbor == this )
			continue;

		if ( typeid( *this ) == typeid( EntityFlockItem ) )

		{
			
			EntityFlockItem* flockItem = ( EntityFlockItem* ) this;

			if ( typeid( *flockItem->getFlockRef() ) == typeid( FlockPlayer ) &&
					typeid( *neighbor ) == typeid( FlockEnemySoldier ) ) 
			{				
				FlockBase* flock = ( FlockBase* ) neighbor;
				
				// Don't check for flock items in the same flock
				//if ( flockItem->getFlockRef() == flock )
				//	continue;

				flockItem->checkForFlockCollisionsBetweenFlockItems( flock->getFlockItems() );
				
				continue;
				
			}

			if ( flockItem->getFlockRef() == neighbor )
				continue;
		}

		// Cannot impact flocks
		if ( typeid( *neighbor ) == typeid( FlockPlayer ) || 
				typeid( *neighbor ) == typeid( FlockEnemySoldier ) )
		{
			continue;
		}

		// Calculate the neghbor distance one time
		float neighborDistance = distance( neighbor );

		// Divide by a constant because the radius are greater than the real bounds
		if ( neighborDistance < (radius() + neighbor->radius()) / 1.6 ) {
			impactWith( neighbor );

			return;
		}
	}

	/*mLump.getLogger()->log(getName(), StringConverter::toString(
				 mLump.getCurrentScenario()->getPlanets().size())
				);*/

	AVGroup planets = mLump.getCurrentScenario()->getPlanets();

	for (AVIterator planets_it = planets.begin(); 
			planets_it != planets.end(); planets_it++)
	{
		// Cast to steerableEntity for resolve the radius method ambiguity
		SteerableEntity* planet = (SteerableEntity*)(*planets_it);

		float planetDistance = distance( planet );

		if ( planetDistance < (radius() + planet->radius()) / 1.6 ) {
			impactWith( planet );
			return;
		}
	}
}


// ---------------------------------------------------------------------------------

void SteerableEntity::impactWith( SteerableEntity* impacted )
{
	Vector3 thisDir = this->getDirection().normalisedCopy();
	Vector3 impactedDir = impacted->getDirection().normalisedCopy();
	Vector3 thisVelocity = thisDir * this->speed();
	Vector3 impactedVelocity = impactedDir * impacted->speed();

	float impactWeight = ( 1 + thisDir.dotProduct( impactedDir ) ) * ( this->speed() + impacted->speed() ) / 500;
	
	
	// Reduct the friendly inpacts
	if ( typeid( *impacted ) == typeid( EntityPlayerShip ) &&
		typeid( *this ) == typeid( EntityFlockItem ))
	{
		EntityFlockItem* flockItem = ( EntityFlockItem* ) this;
		if ( typeid( *flockItem->getFlockRef() ) == typeid( FlockPlayer ))
		{
			impactWeight /= 20;
		}
	}

	/////////////////////////////////////////////
	// Energy reduction	
	mEnergy -= impactWeight;
	impacted->setEnergy( impacted->getEnergy() - impactWeight );

	/////////////////////////////////////////////
	// Impact dynamics

	
			
	const float massSum = this->mass() + impacted->mass();
	const float thisNormMass = this->mass() / massSum;
	const float impactedNormMass =	impacted->mass() / massSum;

	Vector3 weightedVelocity = 
		( thisVelocity * thisNormMass + impactedVelocity * impactedNormMass ) / 2;
	
	this->manageImpact( impacted, thisNormMass, weightedVelocity );

	onImpactWith( impacted, impactWeight * impactedNormMass );
	impacted->onImpactWith( this, impactWeight * thisNormMass );

}

void SteerableEntity::manageImpact( SteerableEntity* impacted, float normMass, Vector3 weightedVelocity )
{
	if ( mLump.getTotalElapsedTime() > mLastUpdateTime + 0.05 )
	{
		Vector3 repulsionVector = (this->getPosition() - impacted->getPosition() ).normalisedCopy();
	
		// Tradeoff between current direction and impacted normal direction
		Vector3 impactVector = 
			repulsionVector * ( 3 + speed() / 140 ) + getDirection() * (20 + 2000 * normMass );
		
		setPosition( getPosition() + repulsionVector * speed() / (30 + 500 * normMass) );
		
		setDirection( impactVector.normalisedCopy() );
		setSpeed( speed() * 0.9 );

		mLastUpdateTime = mLump.getTotalElapsedTime();
	}
	
}

// ---------------------------------------------------------------------------------

float SteerableEntity::distance( SteerableEntity* neighbor )
{
	return distance( neighbor->getPosition() );
}

float SteerableEntity::squaredDistance( SteerableEntity* neighbor )
{
	return squaredDistance( neighbor->getPosition() );
}

// ---------------------------------------------------------------------------------

float SteerableEntity::distance( Vector3 nodePosition )
{
	return ( getPosition() - nodePosition ).length();
}

// ---------------------------------------------------------------------------------

float SteerableEntity::squaredDistance( Vector3 nodePosition )
{
	return ( getPosition() - nodePosition ).squaredLength();
}

// ************************************************************************
// Metodi GET/SET
// ************************************************************************

Ogre::Vector3 SteerableEntity::getPosition()
{
	OpenSteer::AbstractVehicle* vehicle = this;
	return Ogre::Vector3(vehicle->position().x,vehicle->position().y,vehicle->position().z);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::setPosition(Ogre::Vector3 newPos)
{
	mEntityNode->setPosition(newPos);
	static_cast<AbstractVehicle*>(this)->setPosition(OpenSteer::Vec3(newPos.x, newPos.y, newPos.z));
}

// ---------------------------------------------------------------------------------

Ogre::Vector3 SteerableEntity::getDirection()
{
	return Ogre::Vector3(forward().x, forward().y, forward().z);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::setDirection( Vector3 direction )
{
	mEntityNode->setDirection( direction );
	setForward( Vec3( direction.x, direction.y, direction.z ) );
}

// ---------------------------------------------------------------------------------

Vector3 SteerableEntity::getUp()
{
	 return getOrientation() * Vector3::UNIT_Y;
}

// ---------------------------------------------------------------------------------

Vector3 SteerableEntity::getRight()
{
	 return getOrientation() * Vector3::UNIT_X;
}

// ---------------------------------------------------------------------------------

Ogre::Quaternion SteerableEntity::getOrientation()
{
	return mEntityNode->getOrientation();
}

void SteerableEntity::setOrientation(Ogre::Quaternion newOrientation)
{
	mEntityNode->setOrientation( newOrientation );
}

// ---------------------------------------------------------------------------------

void SteerableEntity::setMeshScale(Vector3 meshScale)
{
	assert(meshScale != Vector3::ZERO);
	
	mEntityNode->setScale(meshScale);
	mEntityEnt->setNormaliseNormals(true);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::increaseMaxSpeed(float incRatio)
{
	setMaxSpeed(maxSpeed() + maxSpeed() * incRatio);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::increaseSpeed(float incRatio)
{
	setSpeed(speed() + speed() * incRatio);
}

// ---------------------------------------------------------------------------------

void SteerableEntity::increaseMaxForce(float incRatio)
{
	setMaxForce(maxForce() + maxForce() * incRatio);
}

// ---------------------------------------------------------------------------------

SceneNode* SteerableEntity::getPSysNode()
{
	return (SceneNode*) mEntityNode->getChild( 0 );
}
