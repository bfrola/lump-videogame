#pragma once

#include "FlockBase.h"

using namespace Ogre;


/**
	\author Bernardino Frola

	\brief Specialization of FlockBase class. Implement the player controlled flock behavior.

	This class provide the player controlled flock behavior.
 */
class FlockPlayer :
	public FlockBase
{
	// ************************************************************************
	// Types definitions
	// ************************************************************************
public:
	enum mBehaviourStates 
	{ 
		STATE_SINGLESHOT,
		STATE_ATTACKING,
		STATE_DEFENCING, 
		STATE_IDLE
	};

	// ************************************************************************
	// Constructors/Destructors
	// ************************************************************************

	FlockPlayer(const std::string& name, 
		Vector3 startPosition );

	// ************************************************************************
	// Methods
	// ************************************************************************

	void onUpdateFlock(const float currentTime, const float elapsedTime);

	// Called when the player select an enemy flock 
	void setSelectedEnemy( FlockBase* selectedFlock, float flockDistance );

	void showImpactCamera( bool show );

	void defence();
	void attack();
	void idle();

	void singleShotAttack( float attackChargeLevel );

	// ************************************************************************
	// GET/SET Methods
	// ************************************************************************
	void increaseTargetDistance(float incRatio);
	
	float getTargetDistance() { return mTargetDistance; }

	void setTargetDistance( float targetDistance ) { mTargetDistance = targetDistance; }

	void setAiming( bool aiming ) { mAiming = aiming; };
	
	/**
		Used by flock items for resume behavior.
	*/
	//bool isAttacking() { return mBehaviourState == STATE_ATTACKING; }

	// ************************************************************************
	// Fields
	// ************************************************************************
protected:
	float mTargetDistance;

	/**
		Attacking/idle/singleshot attack
	*/
	mBehaviourStates mBehaviourState;

	/**
		Ent time of the single shot attack.
	*/
	float mSingleShotEndTime;

	/**
		True if the player is charging the single-shot attack.
	*/
	bool mAiming;
};
