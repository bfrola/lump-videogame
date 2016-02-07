#pragma once

#include "SteerableEntity.h"
#include "Ogre.h"

using namespace Ogre;

/**
	\brief Abstract class thath manage the generic flock entity.
	\date dec 2007 - jul 2008
	\version 1.0
 */
class FlockBase :
	public SteerableEntity
{
public:
	/**
		\param <name> The entity name corresponding to the config file section from which to read 
		the flock configuration.
		\param <startPosition> The initial position of the flock.
	*/
	FlockBase(const std::string& name, 
		Vector3 startPosition
	);
	
	/**
		Destroy all the item of the flock.
	*/
	~FlockBase( void );
	
	/**
		Increase the max speed of all flock item.
		\param <incRatio> Specify the ratio of increment ( newValue = oldValue * incRatio ).
	*/
	void increaseMaxSpeed(float incRatio);		// Overwrite the SteerableEntity's method

	/**
		Increase the speed of all flock item.
		\param <incRatio> Specify the ratio of increment ( newValue = oldValue * incRatio ).
	*/
	void increaseSpeed(float incRatio);			// Overwrite the SteerableEntity's method

	/**
		Increase the max force of all flock item.
		\param <incRatio> Specify the ratio of increment ( newValue = oldValue * incRatio ).
	*/
	void increaseMaxForce(float incRatio);		// Overwrite the SteerableEntity's method
	
	/**
		Change the active flag of a spefied flock item. Activation/deactivation of a flock item
		must be excecuted thourougth this function, in order to update FlockBase::mActiveFlockItemsNumber.
		\param <item> Reference to the item to whitch change the active flag.
		\param <active> The active flag value .
	*/
	void setFlockItemActiveState( SteerableEntity* item, bool active );

	/**
		Update the flock position (average of items) and rendering status
	*/
	void onUpdate(const float currentTime, const float elapsedTime);

	/**
		Update the flock behavior and a set of distribuited parameters, such as:
		\li variance (flock space spread);
		\li flock speed (average of items);
		\li flock direction (average of items);
		
		Moreover update the spatial database token and check for flock live conditions.
	*/
	void updateBehaviour(const float currentTime, const float elapsedTime);

	/**
		Unused method.
	*/
	void onImpactWith( SteerableEntity* impacted, float impactWeight );

	/**
		When a flock item impact anything then call this funtion. It check for wheather or not 
		show big explosion, by countering the how many flock items have been impacted and 
		is not exploding yet.
	*/
	void increaseImpctedFlockItemsNumber( void );

	/**
		Set the flock items visibility flag to true.
	*/
	void show();	// Overwrite the SteerableEntity's method

	/**
		Set the flock items visibility flag to false.
	*/
	void hide();	// Overwrite the SteerableEntity's method
	 
	/**
		\return the dot product between the distance vector and the flock direction
		used for indicating wheater or not the flock is aiming the target
	*/
	float getAimPrecision();
	
	/**
		Add the energy burst particle effect to the flock.
	*/
	void addEnergyBurst();

	/**
		Remove the energy burst particle effect from the flock.
	*/
	void removeEnergyBurst();

	bool useDedicatedPD() { return mProximityDatabase != NULL; }

	// ************************************************************************
	// Metodi virtuali - da implementare nelle specializzazioni
	// ************************************************************************

	/**
		This method is used from class specialization for implement the custimized behavior
	*/
	virtual void onUpdateFlock(const float currentTime, const float elapsedTime) = 0;

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************	
	
	/**
		\return The number of max flock items.
	*/
	AVGroup getFlockItems() { return mSingleEntities; }

	/**
		\return FlockBase::mActiveFlockItemsNumber.
	*/
	int getActiveFlockItemsNumber() { return mActiveFlockItemsNumber; }

	/**
		\return FlockBase::mDamagedFlockItemsNumber
	*/
	int getDamagedFlockItemsNumber() { return mDamagedFlockItemsNumber; }

	/**
		\return FlockBase::variance.
	*/
	float getVariance() { return variance; }

	/**
		\return A scene node scaled to the flock variance and traslated to the flock position.
	*/
	SceneNode* getAreaNode() { return mAreaNode; }

	/**
		\return <targetNode>. Used for the seeking behavior.
	*/
	SceneNode* getTargetNode() { return targetNode; }

	/**
		\return FlockBase::targetEntity. Used for the pursuiting behavior. 
	*/
	SteerableEntity* getTargetEntity() { return targetEntity; }

	/**
		\return The flock bounding box boundary.
	*/
	AxisAlignedBox getBoundary() { return mBoundary; }

	/**
		\param <boundary> The flock bounding box boundary. 
	*/
	void setBoundary( AxisAlignedBox boundary ) { mBoundary = boundary; }

	/**
		Set the max speed of all flock items.
		\param <ms> Max speed new value.
	*/
	// Set the properties for each item (ovverride the SteerableEntity method)
	float setMaxSpeed (float ms);	

	/**
		Set the max force of all flock items.
		\param <mf> Max force new value.
	*/
	float setMaxForce (float mf);

	ProximityDatabase* getProximityDatabase() { return mProximityDatabase; }
	
	// ************************************************************************
	// Campi
	// ************************************************************************
protected:
	
	/**
		AVGroup (Abstract Vehicle Gruop - std::vector) containing the flock items.
	*/
	AVGroup mSingleEntities;

	/**
		AVIterator (Abstract Vehicle Iterator - std::vector::iterator) over flock items
	*/
	AVIterator ent_it;

	/**
		\warning Unused
	*/
	Real mSeparationLevel;

	/**
		\warning Unused
	*/
	Real mCohesionLevel;
	
	/**
		 The number of active flock items 
	*/
	int mActiveFlockItemsNumber;

	/**
		The number of damaged flock items.
		A flock item is damaged when its current energy less than 1/3 of the max energy value.
	*/
	int mDamagedFlockItemsNumber;

	/**
		The number of flock items that have been impacted anything.
	*/
	int mImpctedFlockItemsNumber;

	/**
		 Manage the holding time for mImpctedFlockItemsNumber
	*/
	float mLastItemImpactTime;

	/**
		Backups the speed befor the impact, because the impact reduct the speed.
	*/
	float mBeforeImpactSpeed;

	/**
		Backups the direction befor the impact, because the impact change the direction.
	*/
	Vector3 mBeforeImpactDirection;
	
	/**
		Used for checking wheather the flock was highLighted
	*/
	bool mOldHighLighted;

	/**
		The flock space spread value.
	*/
	float variance;

	/**
		This node encapsulate the flock items.
		Its scale is proportional to the variance.
	*/
	SceneNode* mAreaNode;
	
	/**
		The bounding box representing the flock boundary 
	*/
	AxisAlignedBox mBoundary;
		
	/**
		Particle system for the energy attack  effect
	*/
	ParticleSystem* mEnergyBurstEffect;

	/**
		A flock have a minimun number of active flock. Under this threshold the 
		flock is dead.
	*/
	float minActiveItemsNumber;
	
	/**
		Behavior interation counter. Used for interleaved behavior update of the flock items.
		It range in [ 0, FlockBase::mInterleavedUpdateRangeSize ]
	*/
	int mInterleavedUpdateCounter;

public:
	/**
		Flocking neighborhood influence. Used for the flocking behavior.

		Separation permit the collision avoidance between the flock items.

		This value is part of flock configuration, read from config file.
	*/
	float separationRadius;

	/**
		Flock items's view angle. Used for the flocking behavior.
		This value is part of flock configuration, read from config file.
	*/
	float separationAngle;

	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float separationWeight;

	/**
		Flocking neighborhood influence. Used for the flocking behavior.
		This value is part of flock configuration, read from config file.
	*/
	float alignmentRadius;

	/**
		Flock items's view angle. Used for the flocking behavior.
		This value is part of flock configuration, read from config file.
	*/
	float alignmentAngle;

	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float alignmentWeight;

	/**
		Flocking neighborhood influence. Used for the flocking behavior.
		This value is part of flock configuration, read from config file.
	*/
	float cohesionRadius;

	/**
		Flock items's view angle. Used for the flocking behavior.
		This value is part of flock configuration, read from config file.
	*/
	float cohesionAngle;

	/**
		Flock items's view angle. Used for the flocking behavior.
		This value is part of flock configuration, read from config file.
	*/
	float cohesionWeight;

	/**
		Max steering angle for wandering. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/	
	float wanderAngle;

	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float wanderWeight;
	
	// Inseguimento del target utente

	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float seekingWeight;

	/**
		True if the the flock items have to be contained into the flock boundaries.
	*/
	bool boundaryHandling;
	
	// pursuit and evade from vehicles
	
	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float pursuitingWeight;

	/**
		Amount of time (in second) for future prediction. Used for the pursuiting behavior.
	*/
	float pursuitMaxPredictionTime;

	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float avasioningWeight;

	/**
		Amount of time (in second) for future prediction. Used for the evasion behavior.
		This value is part of flock configuration, read from config file.
	*/
	float evasionMaxPredictionTime;
	
	// Se true il comportamento di obstacleAvoidance è sommato agli altri
	// Se false il comportamento è seguito in modo esclusivo in presenza di ostacoli

	/**
		If this option is true the the obstacle avoidance is compounded with others behaviors.
		Otherwise, if false, the obstacle avoidance have the priority on others behaviors.
		This value is part of flock configuration, read from config file.
	*/
	bool useAdditiveObstacleAvoidance;

	// Da usare solo se useAdditiveObstacleAvoidance = false

	/**
		Effect only uf FlockBase::useAdditiveObstacleAvoidance == false. 
		Is the amount of time (in second) use to predict the future obstacle impact. 
		This value is part of flock configuration, read from config file.
	*/
	float obstacleAvoidanceMinTimeToCollision;

	/**
		Weight used for behavior compounding. Used for the wandering behavior.
		This value is part of flock configuration, read from config file.
	*/
	float obstacleAvoidanceWeight;


	/**
		Max neighboors sensing radius. Passed directly to the spactial database neighborood query.
		This value is part of flock configuration, read from config file.
	*/
	float neighborsMaxRadius;

	/**
		Speed burst value on attacking.
		This value is part of flock configuration, read from config file.
	*/
	float maxSpeedBurst;

	/**
		Used for the seeking behavior.
	*/
	SceneNode* targetNode;
	
	/**
		Used for the pursuiting behavior. This behavior needs the SteerableEntity
		reference instead of the SceneNode reference because the pursuiting behavior takes into
		account the target future position, calculated with speed, direction and position.
	*/
	SteerableEntity* targetEntity;

	/**
		Used for the evading behavior. This behavior needs the SteerableEntity, 
		for the same reason expaned for FlockBase::targetEntity. 
	*/
	SteerableEntity* chaserEntity;

	/**
		This is a fundamental parameter for the performance optimization.

		On calling FlockBase::updateBehavior method is updated the behavior of flock items
		that satisfy this condition:

		flockItems_index MOD FlockBase::mInterleavedUpdateRangeSize == FlockBase::mInterleavedUpdateCounter

		where:
		
		\li flockItems_index is the index of flock, it range in [ 0, FlockBase::mInterleavedUpdateCounter [
		\li FlockBase::interleaveUpdateCounter is the behavior index, it 
		range in [ 0, FlockBase::mInterleavedUpdateRangeSize ]

	*/
	int mInterleavedUpdateRangeSize;

	ProximityDatabase* mProximityDatabase;
	
};
