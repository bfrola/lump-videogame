#include "general.h"

FlockEnemySoldier::FlockEnemySoldier(const std::string& name, 
		Vector3 startPosition)
		: FlockBase(name, startPosition)
{
	
	// Set the target node
	targetNode = mLump.mSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3(2000, 0, 0) );
	//targetEntity = mLump.getCurrentScenario()->getPlayerEntity();
	chaserEntity = mLump.getCurrentScenario()->getDefencePlayerFlock();

	// The base position is the initial position
	mBasePosition = getPosition();

	group();
	
	mBehaviourState = STATE_GROUPING;

	mNearTarget = false;

	GROUPING_MAX_TIME = 15.0f;
	GROUPING_VARIANCE_MULT = 2;
	GROUPING_VARIANCE_THRESHOLD = 200;
	GROUPING_TARGET_SPREAD = 100;
	GROUPING_TARGET_MULT = 100.0f;
	GROUPING_TARGET_DISTANCE = 5000;

	ATTACKING_VARIANCE_THRESHOLD = 600;
	ATTACKING_NEAR_TARGET_MAX_TIME = 15.0f;
	ATTACKING_NEAR_TARGET_VARIANCE_MULT = 8.0f;
	ATTACKING_BASEPOS_MAX_DISTANCE = 10000.0f;

	ATTACKING_MAXSPEED_VARIANCE_MIN = 120;
	ATTACKING_MAXSPEED_VARIANCE_MAX = 230;
	ATTACKING_MAXSPEED_NEAR_TARGET_MAX_TIME = 10.0f;
	ATTACKING_MAXSPEED_NEAR_TARGET_VARIANCE_MULT = 15.0f;
}

void FlockEnemySoldier::onUpdateFlock(const float currentTime, const float elapsedTime)
{

	switch ( mBehaviourState )
	{
	/////////////////////////////////////////////////////////////////////////////
	case STATE_ATTACKING:

		// Transacions to STATE_GROUPING
		//------------------------------------------------------------------------

		// Checking for variance
		if ( variance > ATTACKING_VARIANCE_THRESHOLD )
			group( "var low" );
	
		// Checking for substate nearTarget
		if ( distance( targetEntity ) < variance * ATTACKING_NEAR_TARGET_VARIANCE_MULT)
		{
			nearTarget();
		} else
		{		
			farTarget();
		}
		
		// If the flock stays too much time near target group it
		if ( mNearTarget && mLump.getTotalElapsedTime() > 
			mStartNearTargetTime + ATTACKING_NEAR_TARGET_MAX_TIME )
		{
			group( "too much near" );
		}
		
		// If the flock gets out to the base range
		if ( distance( mBasePosition ) > ATTACKING_BASEPOS_MAX_DISTANCE )
		{
			group( "Out of range" );
		}
		
		// Transacions to STATE_ATTACKING_MAXSPEED
		//------------------------------------------------------------------------
		if ( variance < ATTACKING_MAXSPEED_VARIANCE_MIN )
		{
			// Speed up the flock items to the maxSpeed
			for (ent_it = mSingleEntities.begin(); 
			ent_it != mSingleEntities.end(); ++ent_it)
			{
				EntityFlockItem* entity = (EntityFlockItem*) (*ent_it);
				
				// only on active items
				if ( !entity->isActive() )
					continue;
				
				// Set the speed
				entity->setSpeed( maxSpeed() );

			}
			//switchToState( STATE_ATTACKING_MAXSPEED, "var high" );
			attackAtMaxSpeed( mLump.getCurrentScenario()->getPlayerEntity(), "var high" );
		}


		break;
	/////////////////////////////////////////////////////////////////////////////
	case STATE_ATTACKING_MAXSPEED:
		
		// Transacions to GROUPING
		//------------------------------------------------------------------------
		if ( variance > ATTACKING_MAXSPEED_VARIANCE_MAX )
		{
			group( "var low" );
		}

		// Checking for substate nearTarget
		if ( distance( targetEntity ) < variance * ATTACKING_MAXSPEED_NEAR_TARGET_VARIANCE_MULT)
		{
			nearTarget();
		} else
		{		
			farTarget();
		}
		
		// If the flock stays too much time near target group it
		if ( mNearTarget && mLump.getTotalElapsedTime() > 
			mStartNearTargetTime + ATTACKING_MAXSPEED_NEAR_TARGET_MAX_TIME )
		{
			group( "too much near" );
		}

		break;
	//////////////////////////////////////////////////////////////////////////////
	case STATE_GROUPING:
		
		// Check wheather the flock reachs the target point
		if ( distance( targetNode->getPosition() ) < variance * GROUPING_VARIANCE_MULT )
		{
		
		// Transacions to STATE_GROUPING
		//------------------------------------------------------------------------

			// If cannot group (i.e. there is an asteroid on grouping point)
			if ( mLump.getTotalElapsedTime() > mStartGropingTime + GROUPING_MAX_TIME )
			{
				group( "cannot group" );
			}

		// Transacions to STATE_ATTACKING
		//------------------------------------------------------------------------
			if ( variance < GROUPING_VARIANCE_THRESHOLD )
			{
				attack( mLump.getCurrentScenario()->getPlayerEntity(), "var ok" );
			}
		
		}
		// If cannot group and cannot stay near target
		else if ( mLump.getTotalElapsedTime() > mStartGropingTime + GROUPING_MAX_TIME * 2 )
		{
			group( "cannot group" );
		}

		break;
	/*case STATE_GROUPING_UNDERATTACK:
		break;*/
	}

} // onUpdateFlock

//------------------------------------------------------------------------

void FlockEnemySoldier::nearTarget()
{
	// Go to substate nearTarget
	if ( !mNearTarget )
	{
		mStartNearTargetTime = mLump.getTotalElapsedTime();
		mNearTarget = true;
	}
}

void FlockEnemySoldier::farTarget()
{
	// Exit from substate nearTarget
	if ( mNearTarget )
	{
		mStartNearTargetTime = 0;
		mNearTarget = false;
	}
}

//------------------------------------------------------------------------

void FlockEnemySoldier::attack( SteerableEntity* target, string stransactionReason )
{
	targetEntity = target;
	
	// Enable pursuiting
	if ( seekingWeight > 0 )
		seekingWeight*=-1;
	
	// Disable seeking
	if ( pursuitingWeight < 0 )
		pursuitingWeight*=-1;

	switchToState( STATE_ATTACKING, stransactionReason );
}

//------------------------------------------------------------------------

void FlockEnemySoldier::group( string stransactionReason )
{
	farTarget();

	removeEnergyBurst();

	Vector3 targetPosition = getPosition() + newRandVector3( GROUPING_TARGET_SPREAD ) * GROUPING_TARGET_MULT;

	if ( ( targetPosition - mBasePosition ).length() > ATTACKING_BASEPOS_MAX_DISTANCE )
	{
		targetPosition = mBasePosition;
	}
		
	targetNode->setPosition( targetPosition );


	// Disable pursuiting
	if ( pursuitingWeight > 0 )
	{
		pursuitingWeight*=-1;
	}

	// Enable seeking
	if ( seekingWeight < 0 )
	{
		seekingWeight*=-1;	
	}

	// Set the current time
	mStartGropingTime = mLump.getTotalElapsedTime();

	switchToState( STATE_GROUPING, stransactionReason );
}

//------------------------------------------------------------------------

void FlockEnemySoldier::attackAtMaxSpeed( SteerableEntity* target, string stransactionReason )
{
	addEnergyBurst();
	switchToState( STATE_ATTACKING_MAXSPEED, stransactionReason );
}


//------------------------------------------------------------------------

void FlockEnemySoldier::switchToState( mBehaviourStates state, string stransactionReason )
{
	mBehaviourState = state;
	mLump.getLogger()->log(getName(), state2string( mBehaviourState ) + "! " + stransactionReason);

}

//------------------------------------------------------------------------

string FlockEnemySoldier::state2string( mBehaviourStates state )
{
	switch ( mBehaviourState )
	{
	case STATE_ATTACKING:
		return "Attacking";
	case STATE_ATTACKING_MAXSPEED:
		return "Attacking at max speed";
	case STATE_GROUPING:
		return "Grouping";
	/*case STATE_GROUPING_UNDERATTACK:
		return "Grouping under attack";*/
	}
	return "NO STATE";
} 

//------------------------------------------------------------------------


