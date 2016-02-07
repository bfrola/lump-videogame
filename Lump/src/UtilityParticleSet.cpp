// ---------------------------------------
// 2008-2010 Bernardino Frola
// ---------------------------------------
//
// Update history
// Bernardino Frola 
//  23-06-2008: Created

#include "general.h"

UtilityParticleSet::UtilityParticleSet( 
	int poolSize, float effectDuration, string objectName, 
	string effectName, string alternateEffectName )
{
	SceneManager* mSceneMgr = mLump.mSceneMgr;
	ParticleSystem* pSys;

	for (int i = 0; i < poolSize; i ++)
	{	

		if ( alternateEffectName != "" && i % 2 == 1 )
		{
			pSys = mSceneMgr->createParticleSystem(
				objectName + StringConverter::toString( i ), alternateEffectName);
		}
		else
		{
			pSys = mSceneMgr->createParticleSystem(
				objectName + StringConverter::toString( i ), effectName);
		}
	
		// Starts disabled
		// pSys->getEmitter( 0 )->setEnabled( true );
		
		Ogre::SceneNode *particleNode = mSceneMgr->getRootSceneNode()->createChildSceneNode();
		particleNode->attachObject( pSys );

		// Starts not visible
		particleNode->setVisible( false );

		mPSPool.push_back( new ParticleInfo( particleNode, -1) );
	}
	
	mEffectDuration = effectDuration;
	mCurrentPS = mPSPool.begin();
}

// ---------------------------------------------------------------------------------


UtilityParticleSet::~UtilityParticleSet(void)
{
	mPSPool.clear();
}

// ---------------------------------------------------------------------------------

SceneNode* UtilityParticleSet::startNextPS( Vector3 position )
{
	particlesList_it current = mCurrentPS ++;
	if ( mCurrentPS == mPSPool.end() )
		mCurrentPS = mPSPool.begin();
	
	ParticleInfo* pInfo = ( *current );
	
	// If is not already visible, increase the counter
	if ( pInfo->getExpirationTime() < 0 )
		mActivePSNumber ++;
	
	ParticleSystem* pSys = (ParticleSystem*) pInfo->getPSNode()->getAttachedObject( 0 );

	// Reset the particle system (if is still running)
	pSys->getEmitter( 0 )->setEnabled( false );
	pSys->getEmitter( 0 )->setEnabled( true );
	
	// Set the expiration time to current time + effect duration
	pInfo->setExpirationTime( mLump.getTotalElapsedTime() + mEffectDuration );

	pInfo->getPSNode()->setVisible( true );
	pInfo->getPSNode()->setPosition( position );

	return pInfo->getPSNode();
}

// ---------------------------------------------------------------------------------

void UtilityParticleSet::checkForExpiredPS()
{
	// Do nothing if there aren't running psystems
	if ( mActivePSNumber == 0 )
		return;

	for (particlesList_it psNode = mPSPool.begin(); 
			psNode != mPSPool.end(); ++psNode)
	{
		ParticleInfo* pInfo = ( *psNode );
		
		
		// If the ps is expired then disable it
		if ( pInfo->getExpirationTime() > 0 && mLump.getTotalElapsedTime() > pInfo->getExpirationTime() )
		{
			ParticleSystem* pSys = (ParticleSystem*) pInfo->getPSNode()->getAttachedObject( 0 );

			// Stop the particles emission
			pSys->getEmitter( 0 )->setEnabled( false );
			pInfo->setExpirationTime( -1 );
			
			// Hide the psystem
			pInfo->getPSNode()->setVisible( false );

			mActivePSNumber--;
		}
	}
}

// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------
// ---------------------------------------------------------------------------------

UtilityParticleSet::ParticleInfo::ParticleInfo(SceneNode* psNode, float expirationTime)
{
	mPSNode = psNode;
	mExpirationTime = expirationTime;
}	

// ---------------------------------------------------------------------------------

UtilityParticleSet::ParticleInfo::~ParticleInfo()
{
	
}

// ---------------------------------------------------------------------------------