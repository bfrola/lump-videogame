#pragma once

#include "Ogre.h"
#include "SteerableEntity.h"

/**
	\author Bernardino Frola

	\brief This class manage teh ship controlled by the player.

	
 */
class EntityPlayerShip :
	public SteerableEntity
{
public:
	
	/**
		This states influences the player target.
	*/
	enum mBehaviourStates 
	{ 
		STATE_SEEKING_CAMERA,
		STATE_SEEKING_LOCKED,
	};

	/**
		Create a new instance.
	*/
	EntityPlayerShip(const std::string& name, 
		Ogre::Vector3 startPosition = Ogre::Vector3::ZERO,
		Ogre::Quaternion startOrientation = Ogre::Quaternion::IDENTITY);
	
	~EntityPlayerShip(void);

public:

	/**
		Specialized update.
	*/
	void onUpdate(const float currentTime, const float elapsedTime);


	/**
		Update the behavior.
	*/
	void updateBehaviour(const float currentTime, const float elapsedTime);

	// Gestisce l'impatto con un elemento

	/**
		Manage the impacts. Each impact is followed with a camera shaking.
	*/
	void onImpactWith( SteerableEntity* impacted, float impactWeight );
	
	Vec3 steeringForce();
	
	/**
		This method activate the nearest sector to the playership.
	*/
	void checkForSectorMembership();

	/**
		Lock the direction. The ship will not follow the camera direction.
	*/
	void lockDirection();
	
	/**
		Lock the direction. The ship will follow the camera direction.
	*/
	void unlockDirection();

	/**
		True if the ship follow a lateral target.
	*/
	bool isLateralSteering();

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************	

	// Possibile impostare solo valori true, perchè ad ogni fine aggiornamento 
	// sono settati a false
	void setSteerToForward() { steerToForward = true; }
	void setSteerToBackward() { steerToBackward = true; }
	void setSteerToLeft() { steerToLeft = true; }
	void setSteerToRight() { steerToRight = true; }
	void setSteerToUp() { steerToUp = true; }
	void setSteerToDown() { steerToDown = true; }

	void setRepulsionVector( Vector3 repulsionVector ) { mRepulsionVector = repulsionVector; }
	
	bool isSteerToForward() { return steerToForward; }
	bool isSteerToBackward() { return steerToBackward; }
	bool isSteerToLeft() { return steerToLeft; }
	bool isSteerToRight() { return steerToRight; }
	bool isSteerToUp() { return steerToUp; }
	bool isSteerToDown() { return steerToDown; }

	SceneNode* getTargetNode() { return mTargetNode; }
	
	// ************************************************************************
	// Campi
	// ************************************************************************
protected:
	// Massima velocità della navetta

	/**
		Max speed.
	*/
	float speedUpperBound;

	/**
		
	*/
	bool useAdditiveObstacleAvoidance;

	/**
		To use only if useAdditiveObstacleAvoidance == false.
	*/
	float obstacleAvoidanceMinTimeToCollision;

	/**
		Weight on calculating the steering force.
	*/
	float obstacleAvoidanceWeight;

	/**
		Radius in witch to find the neighbors.
	*/
	float neighborsMaxRadius;
	
	/**
		True if the ship follo the forward target.
	*/
	bool steerToForward;

	/**
		True if the ship follo the backward target.
	*/
	bool steerToBackward;


	/**
		True if the ship follo the lef target.
	*/
	bool steerToLeft; 

	/**
		True if the ship follo the right target.
	*/
	bool steerToRight;


	/**
		True if the ship follo the upper target.
	*/
	bool steerToUp;

	/**
		True if the ship follo the downer target.
	*/
	bool steerToDown;


	/**
		Vector result of the impact.
	*/
	Vector3 mRepulsionVector;

	/**
		Target to follow.
	*/
	SceneNode* mTargetNode;


	/**
		Manage the locking camera.
	*/
	mBehaviourStates mBehaviourState;


	/**
		Token of the proximity db.
	*/
	ProximityToken* mSectorsProximityToken;

	/**
		Radius used to find neighbor sectors.
	*/
	float mSectorScanningRadius;


	/**
		Used for internal messages.
	*/
	float mLastEnergyValue;
};

