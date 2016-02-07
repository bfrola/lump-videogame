// ---------------------------------------
// 2008-2010 Bernardino Frola
// ---------------------------------------
//
// Update history
// Bernardino Frola 
//  23-06-2008: Created

#pragma once
#include "Ogre.h"

using namespace std;
using namespace Ogre;

/**
	\brief This class collects and manages a pool of particle system.
*/
class UtilityParticleSet
{
public:
	
	// ************************************************************************
	// Inner classes
	// ************************************************************************

	/**
		\brief A UtilityParticleSet inner-bean class.
		
		It contains:
		\li Ogre::SceneNode at whicth is attached the particle system
		\li float, that contains the particle system's start time
	*/
	class ParticleInfo 
	{
	public:
		ParticleInfo(SceneNode* psNode, float expirationTime);
		~ParticleInfo();

		SceneNode* getPSNode() { return mPSNode; };
		float getExpirationTime() { return mExpirationTime; };
		void setExpirationTime( float expirationTime ) { mExpirationTime = expirationTime; }

	protected:
		SceneNode* mPSNode;
		float mExpirationTime;
	};
		
	/**
		The pool is a array of UtilityParticleSet::ParticleInfo	
	*/
	typedef vector<ParticleInfo*> particlesList;
	
	/**
		Iterator over the psystems
	*/
	typedef vector<ParticleInfo*>::const_iterator particlesList_it;

	/**
		\param <poolSize> Size of the pool.
		\param <objectName> The ogre's entity name.
		\param <effectName> The resource effect name.
		\param <alternateEffectName> Used for compose alternated effects.
		\param <effectDuration> How much each psystem can run after it is started and before it is hided.
	*/
	UtilityParticleSet( int poolSize, float effectDuration, string objectName, 
		string effectName, 
		string alternateEffectName = "" );
public:
	~UtilityParticleSet(void);
	
	/**
		This hide all the expired psystems.

		A psystem is expired when currentTime > startTime + mEffectDuration 
	*/
	void checkForExpiredPS();

	/**
		Show the next available psystem of the pool.
		\param <position> The position where to place the psystem.
		\return The Ogre::SceneNode at which is attached the psystem.
	*/
	SceneNode* startNextPS( Vector3 position );

	// ************************************************************************
	// GET/SET Methods
	// ************************************************************************	

	/**
		\return UtilityParticleSet::mActivePSNumber
	*/
	int getActivePSNumber() { return mActivePSNumber; }

	// ************************************************************************
	// Fields
	// ************************************************************************
protected:

	/**
		The pool of psystems.
	*/
	particlesList mPSPool;

	/**
		A reference to the last used psystem.
	*/
	particlesList_it mCurrentPS;

	/**
		Hold the effect duration.	
	*/
	float mEffectDuration;

	/**
		Hold the number or current running psystems of the pool.
	*/
	int mActivePSNumber;
	
	
};



