/*
-----------------------------------------------------------------------------
This source	file is	part of	the	OgreSteer Application.

Author: Spannerman (spannerworx AT gmail DOT com)
URL: http://www.spannerworx.com/

This program is free software; you can redistribute it and/or modify it under
the terms of the GNU Lesser General Public License as published by the Free Software
Foundation; either version 2 of the License, or (at your option) any later
version.

This program is distributed in the hope that it will be useful, but WITHOUT
ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
FOR A PARTICULAR PURPOSE. See the GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License along with
this program; if not, write to the Free Software Foundation, Inc., 59 Temple
Place - Suite 330, Boston, MA 02111-1307, USA, or go to
http://www.gnu.org/copyleft/lesser.txt.
-----------------------------------------------------------------------------
*/

#pragma once

#include "Ogre.h"
#include "SteerableBase.h"
#include "Proximity.h"
#include "UtilityParticleSet.h"

// Definizione dei tipi riguardo il proximity database
typedef AbstractProximityDatabase<AbstractVehicle*> ProximityDatabase;
typedef AbstractTokenForProximityDatabase<AbstractVehicle*> ProximityToken;

using namespace Ogre;

/**
	\author Bernardino Frola

	\brief This class represent the basical entity of the Lump's world and it is a wrapper between
	the OpenSteer's AbstractVehicle implemented by SteerableBase and the Ogre's SceneNode.

	All entities that have a
	position, direction, and the OpenSteer attributes, such as, maxSpeed, maxForce, ect. have
	to extend this class.

	
 */
class SteerableEntity: public SteerableBase
{

public:

	/**
		Standard construnctor getting the basilar parameters.
	*/
	SteerableEntity(
		const std::string& name,	
		// Vehicle name, not unique
		Ogre::Vector3 startPosition = Ogre::Vector3::ZERO,
		Ogre::Quaternion startOrientation = Ogre::Quaternion::IDENTITY);
	
	/**
		
	*/
	virtual ~SteerableEntity();
	
	/**
		Entity rendering states:
		\li visible;
		\li showing: trasiting from hidden to visible;
		\li hidden;
		\li hiding: trasiting from visible to hidden;
	*/
	enum mRenderingStates
	{
		STATE_VISIBLE,
		STATE_SHOWING,
		STATE_HIDDEN, 
		STATE_HIDING
	};

	/**
		Entity running state
		\li running;
		\li colling: haved an impact;
		\li exploding: the entity is dying.
	*/
	enum mGeneralStates 
	{ 
		STATE_RUNNING,
		STATE_COLLIDING,
		STATE_EXPLODING
	};

public:

	/**
		Update the entity position, velocity and other physic-related parameters.
	*/
	void updateEntity(const float currentTime, const float elapsedTime);

	/**
		Set the entity initial state.
	*/
	void resetEntity();

	/**
		Create a entity clone. Use this for crowds.
		\param <baseEntity> The Ogre::Entity to be cloned
		\param <attachToNode> True if the entity have to be attached to the SteerableEntity::mEntityNode
		\return The cloned entity.
	*/
	Ogre::Entity* createEntityInstance(Ogre::Entity* baseEntity, bool attachToNode = true);

	/**
		Create a entity clone. Use this for single objects.
		\param <meshName> The mesh of the entity.
		\param <attachToNode> True if the entity have to be attached to the SteerableEntity::mEntityNode
		\return The created entity.
	*/
	Ogre::Entity* createEntity(const std::string meshName, bool attachToNode = true);
	
	/**
		Add a particle system-based tail.
		\param <wakeEffectName> The name of the scripted effect.
		\param <relativePosition> The position offset.
		\param <wakeName> The name of the Ogre object.

	*/
	void createWake(
		const std::string wakeEffectName, 
		Ogre::Vector3 relativePosition = Ogre::Vector3::ZERO, 
		const std::string wakeName = "Wake"
		);

	/**
		Collision detection function.
		\param <neighbors> The list of neighbor over to check for collision.

		Note: a sub-class that use this method must necessarily to implement the virtual method 
		SteerableEntity::onImpactWith
	*/
	void checkForCollisions( AVGroup neighbors );

	/**
		This method is called when an impact detected in SteerableEntity::checkForCollisions
		occours. Here, is calculated the impactWeight value.

		\param <entity> Impacted object.

	*/
	void impactWith( SteerableEntity* entity );

	/**
		Compute the repulsion vector and impacts dynamics.

		\param <impacted> Impacted object.
		\param <normMass> Mass comparated to the sum of the mass with the impacted object.
		\param <weightedVelocity> Velocity result after the impact.

	*/
	void manageImpact( SteerableEntity* impacted, float normMass, Vector3 weightedVelocity );

	/**
		Calc the distance with a neighbor.
	*/
	float distance( SteerableEntity* neighbor );
	
	/**
		Calc the squared distance with a neighbor.
	*/
	float squaredDistance( SteerableEntity* neighbor );
	
	/**
		Calc the distance with a given position.
	*/
	float distance( Vector3 nodePosition );
	
	/**
		Calc the squared distance with a given position.
	*/
	float squaredDistance( Vector3 nodePosition );

	/**
		Update the position to the spatial database.
	*/
	void registerToProximityDB( ProximityDatabase* pdb );

	/**
		Create a meshed node to show into the HUD radar.
	*/
	Entity* createProximityIndicatorNode( const std::string meshName, Vector3 scale = Vector3(1, 1, 1) );
	
	///////////////////////////////////////////////////////////////////////
	/// Hide/Show entity

	/**
		Set the entity to visible.
	*/
	virtual void show();

	/**
		Set the entity to not visible.
	*/
	virtual void hide();
	
	/**
		Start the smoothed showing of the object. 
		\see SteerableEntity::updateRenderingStatus
	*/
	void startSmoothShowing();

	/**
		Start the smoothed hiding of the object. 
		\see SteerableEntity::updateRenderingStatus
	*/
	void startSmoothHiding();
	

	/**
		Use this function for execute smooth showing/hiding 
		return true when the showing/hiding operation was completed.
	*/
	bool updateRenderingStatus();

	// ************************************************************************
	// Virtual methods
	// ************************************************************************

	/**
		The method SteerableEntity::udateEntity, at its end, call this one.
		The sub-class must insert here the specialized update operations.
	*/
	virtual void onUpdate(const float currentTime, const float elapsedTime) = 0;


	
	/**
		Update the behaviour. The behaviour is updated less frequently of graphic/state update
	*/
	virtual void updateBehaviour(const float currentTime, const float elapsedTime) = 0;

	// Gestisce la collisione con un elemento
	
	/**
		Manage the specialized after impact operations.
	*/
	virtual void onImpactWith( SteerableEntity* impacted, float impactWeight ) = 0;

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************

	std::string getName() { return mName; }

	Ogre::Vector3 getPosition();

	void setPosition(Ogre::Vector3 newPos);

	Ogre::Quaternion getOrientation();
	// Imposta solo l'orientamento dell'entity di Ogre e non applica modifice al vehicle di OpenSteer
	void setOrientation(Ogre::Quaternion newOrientation);

	Ogre::Vector3 getDirection();
	void setDirection( Vector3 direction );

	Ogre::Vector3 getUp();
	Ogre::Vector3 getRight();

	void setMeshScale(Vector3 meshScale);

	Ogre::SceneNode* getEntityNode(){return mEntityNode;}
	const std::string& getEntityName(){return mEntityName;}

	Ogre::Vector3 getForwardVector() {return mForwardVector;}
	void setForwardVector( Vector3 newForwardVector ) { mForwardVector = newForwardVector; };

	void increaseMaxSpeed(float incRatio);
	void increaseSpeed(float incRatio);
	void increaseMaxForce(float incRatio);

	ProximityToken* getProximityToken() { return proximityToken; }
	AVGroup getNeighbors() { return mNeighbors; }

	bool isHighlighted() { return mHighlighted; }
	void setHighlighted( bool highlight ) { mHighlighted = highlight; mEntityNode->showBoundingBox( highlight ); }

	bool isStatic() { return mIsStatic; }
	void setIsStatic( bool isStatic ) { mIsStatic = isStatic; }

	float getEnergy() { return mEnergy; };
	void setEnergy( int energy ) { mEnergy = energy; }

	float getMaxEnergy() { return mMaxEnergy; };
	void setMaxEnergy( int maxEnergy ) { mMaxEnergy = maxEnergy; }

	SceneNode* getProximityIndicatorNode() { return mProximityIndicatorNode; }
	
	// Return the entity's particle system node
	SceneNode* getPSysNode();

	// ************************************************************************
	// Campi
	// ************************************************************************

protected:
	
	/**
		
	*/
	std::string mName;

	
	/**
		
	*/
	Ogre::SceneNode* mEntityNode;
	
	/**
		
	*/
	Ogre::Entity* mEntityEnt;

	/**
		
	*/
	Ogre::Vector3 mInitialEntityPos;
	
	/**
		
	*/
	Ogre::Quaternion mInitialEntityOrient;
	
	/**
		
	*/
	Ogre::Vector3 mForwardVector;
	
	/**
		
	*/

	
	/**
		
	*/
	float mRadius;
	
	/**
		
	*/
	float mMaxForce;
	
	/**
		
	*/
	float mMaxSpeed;

	
	/**
		
	*/
	std::string mEntityName;
	
	/**
		
	*/
	std::string mEntityAnimString;

	
	/**
		
	*/
	bool hasAnimation;
	
	// Riferimento al token del ProximityDatabase
	// da utilizzare quando è richiesto il calcolo del vicinato
	
	/**
		
	*/
	ProximityToken* proximityToken;

	// Vehicle states
	
	/**
		
	*/
	mGeneralStates mGeneralState;

	// Rendering states
	
	/**
		
	*/
	mRenderingStates mRenderingState;
	
	/**
		
	*/
	AVGroup mNeighbors;

	// Generic variables for state controll
	
	/**
		
	*/
	float mLastRecordedTime;
	
	/**
		
	*/
	float mLastWanderingWeight;

	
	/**
		
	*/
	Vector3 mImpactDirection;

	// Update sincronizator
	
	/**
		
	*/
	float mLastUpdateTime;

	
	/**
		
	*/
	float oldMaxSpeed;

	// Is true if the entity is highligthed
	
	/**
		
	*/
	bool mHighlighted;

	// By default only entities into the current active sector are visible.
	// This parameter, if true, allow to keep visible the entity until
	// the player is into the sector range,	even though the sector is not the 
	
	/**
		
	*/
	bool mIsStatic;

	// This node allow to indicate the entity position to the user
	
	/**
		
	*/
	SceneNode* mProximityIndicatorNode;

	// The energy amount of the entity
	
	/**
		
	*/
	float mEnergy;
	
	/**
		
	*/
	float mMaxEnergy;
};
