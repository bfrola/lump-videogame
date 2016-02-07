#include "general.h"

EntityAsteroid::EntityAsteroid(
	const std::string& name, 
	Ogre::Entity* baseEntity,
	float magnitude,
	Ogre::Vector3 startPosition,
	Ogre::Quaternion startOrientation,
	Vector3 movementVelocity,
	Ogre::Quaternion rotationVelocity)
	: SteerableEntity(name, 
		startPosition, 
		startOrientation)
{
	// Creazione di una mesh dedicata	
	Entity* ent = createEntityInstance( baseEntity );

	// Moltiplicatore della scalatura casuale
	mIsStatic = StringConverter::parseBool(
		mLump.getSceneConfigFile()->getSetting("isStatic", name)
	);

	// Scalatura casuale
	float randScale = (float)rand() / (float)RAND_MAX;
	
	// Moltiplicatore della scalatura casuale
	float randScaleRatio = StringConverter::parseReal(
		mLump.getSceneConfigFile()->getSetting("randScaleRatio", name)
	);

	mForwardVector = StringConverter::parseVector3(
		mLump.getSceneConfigFile()->getSetting("forwardVector", name)
	);

	// Scala e vettore avanti della mesh
	
	Vector3 meshScale = StringConverter::parseVector3(
		mLump.getSceneConfigFile()->getSetting("meshScale", name)
	);
	setMeshScale(meshScale * randScale * randScaleRatio);	
	
	float radius = StringConverter::parseReal(
		mLump.getSceneConfigFile()->getSetting("radius", name)
	);
	setRadius(radius * randScale * randScaleRatio );

	float mass = StringConverter::parseReal(
		mLump.getSceneConfigFile()->getSetting("mass", name)
	);
	setMass( mass * randScale * randScaleRatio);

	// Calcolo dell'erientamento casuale	
	mEntityNode->setDirection( newRandVector3( 2 ) );
	//mEntityNode->setVisible( false );
	
//	mVelocity = ( Vec3(0.1f, 0.0f, 0.0f) );

	center = Vec3(startPosition.x, startPosition.y, startPosition.z);

	setMaxSpeed( 300 );
	setMaxForce( 1400 );

	mEnergy = mass * randScale * randScaleRatio / 10;
}

// ---------------------------------------------------------------------------------

EntityAsteroid::~EntityAsteroid(void)
{

}

// ---------------------------------------------------------------------------------

void EntityAsteroid::onUpdate(const float currentTime, const float elapsedTime)
{

}

// ---------------------------------------------------------------------------------

// Update the behaviour
void EntityAsteroid::updateBehaviour(const float currentTime, const float elapsedTime)
{	
	// TODO
}	

// ---------------------------------------------------------------------------------

void EntityAsteroid::onImpactWith( SteerableEntity* impacted, float impactWeight )
{
	if ( mEnergy < 0 )
	{
		// Make the explosion effect
		mLump.getCurrentScenario()->getAsteroidExplPool()->startNextPS( getPosition() );
		mLump.getCurrentScenario()->getEnergyExplPool()->startNextPS( getPosition() );
		
		hide();

		// Move far away (because it cannot be deleted the proximity db token)
		setPosition( Vector3( 1000000, 1000000, 1000000 ) );
		proximityToken->updateForNewPosition( position() );	

		// Use the non static asteroids as energy integrators

		const int randEnergy = 100 * ((float)rand() / (float)RAND_MAX);

		if ( !isStatic() && randEnergy > 50 && 
			mLump.getCurrentScenario()->getPlayerEntity()->getEnergy() <
			mLump.getCurrentScenario()->getPlayerEntity()->getMaxEnergy() )
		{
			mLump.getCurrentScenario()->getPlayerEntity()->setEnergy( 
				mLump.getCurrentScenario()->getPlayerEntity()->getEnergy() + randEnergy );

			if ( mLump.getCurrentScenario()->getPlayerEntity()->getEnergy() >
				mLump.getCurrentScenario()->getPlayerEntity()->getMaxEnergy() )
				mLump.getCurrentScenario()->getPlayerEntity()->setEnergy(
					mLump.getCurrentScenario()->getPlayerEntity()->getMaxEnergy() );


			mLump.getEventsLogger()->log( "Good!", "Energy recovered: +" + 
				StringConverter::toString( randEnergy ) );
		}
		
	}
}