#pragma once

#include "FlockBase.h"

using namespace Ogre;

/**
 \brief Specialization of FlockBase class. Implement the enemy flock behavior.
 \author Bernardino Frola

 This class manage the enemy flock behavior.
 This type of flock attack the playership when its enters into the enemy flock territory.
 */
class FlockEnemySoldier :
	public FlockBase
{
public:
	
	/**
	*/
	enum mBehaviourStates 
	{ 
		STATE_ATTACKING,
		//STATE_ATTACKING_TARGET_HIDED,
		//STATE_ATTACKING_TARGET_SAME_DIR, 
		STATE_ATTACKING_MAXSPEED,
		STATE_GROUPING
		//STATE_GROUPING_UNDERATTACK
	};

	FlockEnemySoldier(const std::string& name, 
		Vector3 startPosition );	

	void onUpdateFlock(const float currentTime, const float elapsedTime);

	void attack( SteerableEntity* target, string stransactionReason = "" );

	void attackAtMaxSpeed( SteerableEntity* target, string stransactionReason = "" );

	void group( string stransactionReason = "" );

	void switchToState( mBehaviourStates state, string stransactionReason = "" );

	string state2string( mBehaviourStates state );
	
	// Manage the substate nearTarget
	void nearTarget();
	void farTarget();

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************	


	// ************************************************************************
	// Campi
	// ************************************************************************
protected:
	mBehaviourStates mBehaviourState;
	long mStartGropingTime;
	long mStartNearTargetTime;
	bool mNearTarget;

	/**
		Initial position of the flock. Using this parameter the enemy flock cannot
		exit from its range.
	*/
	Vector3 mBasePosition;
	
	float GROUPING_MAX_TIME;
	int GROUPING_VARIANCE_MULT;
	int GROUPING_VARIANCE_THRESHOLD;
	int GROUPING_TARGET_SPREAD;
	float GROUPING_TARGET_MULT;
	int GROUPING_TARGET_DISTANCE;

	int ATTACKING_VARIANCE_THRESHOLD;
	float ATTACKING_NEAR_TARGET_MAX_TIME;
	float ATTACKING_NEAR_TARGET_VARIANCE_MULT;

	/**
		Max distance allowed from the FlockEnemySoldier::mBasePosition	
	*/

	float ATTACKING_BASEPOS_MAX_DISTANCE;

	int ATTACKING_MAXSPEED_VARIANCE_MIN;
	int ATTACKING_MAXSPEED_VARIANCE_MAX;
	float ATTACKING_MAXSPEED_SPEEDUP_MULT;
	float ATTACKING_MAXSPEED_NEAR_TARGET_MAX_TIME;
	float ATTACKING_MAXSPEED_NEAR_TARGET_VARIANCE_MULT;


};
