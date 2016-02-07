#pragma once
#include "general.h"
#include "GL/glut.h"

/**
	\author Bernardino Frola

	\brief This class manage the single flock item. 
	
	All the behavioral parameter are stored into the flock whitch the item belongs to.
	

	
 */
class EntityFlockItem :
	public SteerableEntity
{
public:
	EntityFlockItem(const std::string& name, 
		Ogre::Entity* baseEntity,			// Entity da clonare
		FlockBase* newFlockRef,				// Riferimento allo stormo di appartenenza
		Ogre::Vector3 startPosition = Ogre::Vector3::ZERO,
		Ogre::Quaternion startOrientation = Ogre::Quaternion::IDENTITY);

	~EntityFlockItem(void);

public:
	
	void onUpdate(const float currentTime, const float elapsedTime);

	void updateBehaviour(const float currentTime, const float elapsedTime);

	/**
		Check for inpacts between flocks items.
	*/
	void checkForFlockCollisionsBetweenFlockItems( AVGroup neighborsItems );
	
	/**
		When a flock item is inactive, the player can resume it.
	*/
	void checkForResume();

	void impactBetweenFlockItems( SteerableEntity* impacted );

	/**
		Manage the item impact.
	*/
	void onImpactWith( SteerableEntity* impacted, float impactWeight );
	
	/**
		Calculate the steering force based on the flock parameters.
	*/
	Vec3 calcSteeringForce(void);

	/**
		Vector pointing to the center of the boudary box.
	*/
	Vec3 handleBoundary(void);

	/**
		Chech for boudary condition.
	*/
	bool intersectsFlockBoundary(void);

	/**
		\return The steering vector to behave as a flock component
	*/
	Vec3 steerToFlock (void);

	/**
		Calling this method the flock item becomes active.
		Used for flock resume behavior.
	*/
	void resume();
	
	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************	

	void setActive(bool newActive) { active = newActive; }
	bool isActive() { return active; }
	FlockBase* getFlockRef() { return mFlockRef; }
	
	// ************************************************************************
	// Campi
	// ************************************************************************

private:
	
	/**
		Force applied to the vehicle.
	*/
	Vec3 mSteeringForce;
	
	/**
		Radius for neighborhood searching.
	*/
	float mNeighborsMaxRadius;

protected:
	
	/**
		Flock whitch the item belongs to.
	*/
	FlockBase* mFlockRef;	
	
	
	/**
		Activation flag.
	*/
	bool active;
};

