#include "general.h"

const float IDLE_SPEED = 800.0f;
const float IDLE_MAXFORCE = 200.0f;
const float IDLE_BOXSIZE = 300.0f;
const float IDLE_BOXDISTANCE = 2500.0f;

const float ATTACK_SPEED = 2000.0f;
const float ATTACK_MAXFORCE = 550.0f;
const float ATTACK_SEEKINGWEIGHT = 2000.0f;
const float ATTACK_DEFAULTTARGETDISTANCE = 3000.0f;

const float SINGLESHOT_SPEED = 10000.0f;
const float SINGLESHOT_BASESPEED = 4000.0f;
const float SINGLESHOT_TARGET_RIGHT_OFFSET = 400.0f;
const float SINGLESHOT_TARGET_DOWN_OFFSET = 400.0f;
const float SINGLESHOT_TARGET_Z_OFFSET = 2000.0f;
const float SINGLESHOT_DEFAULTTARGETDISTANCE = 10000.0f;
const float SINGLESHOT_RANDFACTOR = 0.05;

const float RESUME_DISTANCE = 1000.0f;

FlockPlayer::FlockPlayer(const std::string& name, 
		Vector3 startPosition)
		: FlockBase(name, startPosition)
{
	
	std::string meshName = mLump.getConfigFile()->getSetting("targetMeshName", name);

	Entity *targetEntity = mLump.mSceneMgr->createEntity( name + "TargetEntity" + 
		StringConverter::toString( mLump.getEntityCounter() ), meshName );
	targetEntity->setCastShadows( false );

	targetNode = mLump.mSceneMgr->getRootSceneNode()->createChildSceneNode( name + "TargetNode" + 
		StringConverter::toString( mLump.getEntityCounter() ));
	
	targetNode->attachObject( targetEntity );
	targetNode->setScale(Vector3(0.2, 0.2, 0.2));
	targetNode->setVisible( true );

	mTargetDistance = 1000;
	
	// Evade from player ship
	chaserEntity = mLump.getCurrentScenario()->getPlayerEntity();
	
	// Start from idle state
	attack();

} // FlockPlayer

// ---------------------------------------------------------------------------------

void FlockPlayer::increaseTargetDistance(float incRatio)
{
	float newValue = mTargetDistance + mTargetDistance * incRatio;
	if ( incRatio > 0 || newValue > mLump.getCurrentScenario()->getCameraDistance() / 2 )
		mTargetDistance = newValue;
	
} // increaseAttackTargetDistance

// ---------------------------------------------------------------------------------

void FlockPlayer::setSelectedEnemy( FlockBase* selectedFlock, float flockDistance )
{

	targetEntity = selectedFlock;

	if ( mBehaviourState == STATE_IDLE )
		return;

	// Here, mBehaviourState == STATE_ATTACK or mBehaviourState == STATE_SINGLESHOT

	if ( selectedFlock == NULL )
	{
		if ( mTargetDistance != ATTACK_DEFAULTTARGETDISTANCE )
				mTargetDistance = ATTACK_DEFAULTTARGETDISTANCE;

		return;
	}	

	// Here, selectedFlock != NULL
	
	// Set the target distance to the flock center
	mTargetDistance = flockDistance + selectedFlock->getVariance();
	selectedFlock->setHighlighted( true );

} // setSelectedEnemy

// ---------------------------------------------------------------------------------

void FlockPlayer::onUpdateFlock(const float currentTime, const float elapsedTime)
{

	switch ( mBehaviourState )
	{
	/////////////////////////////////////////////////////////////////////////////
	case STATE_IDLE:

		break;
	/////////////////////////////////////////////////////////////////////////////
	
	case STATE_ATTACKING:
		// Update the target node, based on the position and direction of the main camera
		
		// If aiming the target is decentred in order to
		// do not hide the line of sight between the target and player
		if ( mAiming )
		{
			targetNode->setPosition( 
				mLump.mCamera->getPosition() + 
				mLump.mCamera->getDirection() * SINGLESHOT_TARGET_Z_OFFSET + 
				mLump.mCamera->getRight() * SINGLESHOT_TARGET_RIGHT_OFFSET -
				mLump.mCamera->getUp() * SINGLESHOT_TARGET_DOWN_OFFSET );
		}
		else
		{
			targetNode->setPosition( 
				mLump.mCamera->getPosition() + 
				mLump.mCamera->getDirection() * mTargetDistance );
		}
	
		// Resume the inactive flock items 
		for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
		{
			EntityFlockItem* entity = (EntityFlockItem*) (*ent_it);
			if ( !entity->isActive() )
			{
				// If the flock item is near the flock center, then resume it.
				if ( distance( entity ) < RESUME_DISTANCE )
				{
					entity->resume();
				}
			}
		}

		break;

	/////////////////////////////////////////////////////////////////////////////
	case STATE_SINGLESHOT:

		mLump.getLogger()->log( "PFl", "ONESHOT" );	

		mLump.getLogger()->log( "PFl", StringConverter::toString( mLump.getTotalElapsedTime())  );	
		mLump.getLogger()->log( "PFl", StringConverter::toString( mSingleShotEndTime)  );	

		if ( mLump.getTotalElapsedTime() > mSingleShotEndTime || 
			mImpctedFlockItemsNumber > mSingleEntities.size() / 5 )
		{
			mBehaviourState = STATE_ATTACKING;
			idle();
		}

		break;

	/////////////////////////////////////////////////////////////////////////////
	case STATE_DEFENCING:

		// Put the target node between player ship and enemy (70% near player ship)

		targetNode->setPosition(  
			targetEntity->getPosition() * 0.3 + 
			mLump.getCurrentScenario()->getPlayerEntity()->getPosition() * 0.7 );

		break;
	}
	
	// Set up the boundaries
	// The player flock stay into a square towards the playership
	if ( boundaryHandling )
	{
		// TODO read from config
		Vector3 mBoxSize = Vector3( IDLE_BOXSIZE, IDLE_BOXSIZE, IDLE_BOXSIZE );
		float mBoxDistance = IDLE_BOXDISTANCE;
		
		Vector3 boxCenter = mLump.getCurrentScenario()->getPlayerEntity()->getDirection() * mBoxDistance;

		Vector3 minCorner = mLump.getCurrentScenario()->getPlayerEntity()->getPosition() + 
			boxCenter - mBoxSize;
		Vector3 maxCorner = mLump.getCurrentScenario()->getPlayerEntity()->getPosition() +
			boxCenter + mBoxSize;
		
		// Set the boundary min and max
		mBoundary.setMinimum( minCorner );
		mBoundary.setMaximum( maxCorner );
	}

} // onUpdateFlock

// ---------------------------------------------------------------------------------

void FlockPlayer::defence()
{
	if ( targetEntity == NULL )
		return;

	if ( mBehaviourState != STATE_DEFENCING )
		mBehaviourState = STATE_DEFENCING;
} // defence

// ---------------------------------------------------------------------------------

void FlockPlayer::attack()
{
	if ( mBehaviourState == STATE_ATTACKING || mLump.getTotalElapsedTime() < mSingleShotEndTime )
		return;
		
	mBehaviourState = STATE_ATTACKING;

	seekingWeight = ATTACK_SEEKINGWEIGHT;
	boundaryHandling = false;
	setMaxSpeed( ATTACK_SPEED );
	setMaxForce( ATTACK_MAXFORCE );
	
	// FIXME: HardCode
	addEnergyBurst();
} // attack

// ---------------------------------------------------------------------------------

void FlockPlayer::idle()
{
	if ( mBehaviourState == STATE_IDLE || mBehaviourState == STATE_SINGLESHOT )
		return;
		
	mBehaviourState = STATE_IDLE;

	seekingWeight = -1.0f;
	boundaryHandling = true;
	setMaxSpeed( IDLE_SPEED );
	setMaxForce( IDLE_MAXFORCE );
	
	// FIXME: HardCode
	removeEnergyBurst();
} // attack

// ---------------------------------------------------------------------------------

void FlockPlayer::singleShotAttack( float attackChargeLevel )
{
	if ( mBehaviourState == STATE_SINGLESHOT )
		return;

	mBehaviourState = STATE_SINGLESHOT;

	mSingleShotEndTime = mLump.getTotalElapsedTime() + 3;

	if ( targetEntity == NULL )
		mTargetDistance = SINGLESHOT_DEFAULTTARGETDISTANCE;
	
	// Direct the shot toward the camera direction
	Vector3 targetPosition = mLump.mCamera->getPosition() + 
		mLump.mCamera->getDirection() * mTargetDistance;

	Vector3 shotDirection = ( targetPosition - getPosition() ).normalisedCopy();
		
	// Set the whoole flock direction towards the target
	for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
	{
		EntityFlockItem* entity = (EntityFlockItem*) (*ent_it);

		entity->setDirection( shotDirection + newRandVector3( SINGLESHOT_RANDFACTOR ) );
		entity->setMaxSpeed( SINGLESHOT_SPEED * attackChargeLevel + SINGLESHOT_BASESPEED );
		entity->setSpeed( SINGLESHOT_SPEED * attackChargeLevel + SINGLESHOT_BASESPEED );
	}

}

// ---------------------------------------------------------------------------------

