#include "general.h"

EntityPlanet::EntityPlanet(
	const std::string& name, 
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
		
	std::string meshName = mLump.getSceneConfigFile()->getSetting("meshName", name);
	Entity* ent = createEntity( meshName );

	ent->setVisible( true );
	
	// Scala e vettore avanti della mesh
	
	Vector3 meshScale = StringConverter::parseVector3(
		mLump.getSceneConfigFile()->getSetting("meshScale", name)
	);
	setMeshScale(meshScale );	
	
	float radius = StringConverter::parseReal(
		mLump.getSceneConfigFile()->getSetting("radius", name)
	);
	setRadius(radius );

	// Calcolo dell'erientamento casuale

	mEntityNode->setDirection( newRandVector3( 2 ) );

	// mVelocity = ( Vec3(0.1f, 0.0f, 0.0f) );
	
	setMass( 10000000 );
	
	center = Vec3(startPosition.x, startPosition.y, startPosition.z);
	mEntityNode->setPosition( startPosition );

	// Registrazione sul proximity database
	proximityToken = mLump.getCurrentScenario()->getProximityDatabase()->allocateToken (this);

	// Impostazione posizione inziale
	proximityToken->updateForNewPosition (position());

}

// ---------------------------------------------------------------------------------

EntityPlanet::~EntityPlanet(void)
{

}

// ---------------------------------------------------------------------------------

void EntityPlanet::onUpdate(const float currentTime, const float elapsedTime)
{
	 applySteeringForce(velocity(), elapsedTime);
}

// ---------------------------------------------------------------------------------

// Update the behaviour
void EntityPlanet::updateBehaviour(const float currentTime, const float elapsedTime)
{	
	// TODO
}	

// ---------------------------------------------------------------------------------

void EntityPlanet::onImpactWith( SteerableEntity* impacted, float impactWeight )
{

}