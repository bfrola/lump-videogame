#include "general.h"

// Selection options
static const int SELECTION_MAX_DISTANCE = 100000;
static const int SELECTION_MIN_DISTANCE = 50;
static const int SELECTION_FLOCK_MIN_VARIANCE = 700;

ScenarioSectorBase::ScenarioSectorBase( 
		const std::string& sectorName, 
		Vector3 position, 
		float radius )
{
	mName = sectorName;
	mPosition = position;
	setRadius( radius );

	// Spactial DB initialization
	// //////////////////////////////////////////////////////////////////////////////
	
	const Vec3 center;
	const float div = 50.0f;
	const Vec3 divisions (div, div, div);
	const float diameter = radius * 1.1f * 2; //XXX need better way to get this
	const Vec3 dimensions (diameter, diameter, diameter);
	typedef LQProximityDatabase<AbstractVehicle*> LQPDAV;
	mProximityDatabase = new LQPDAV (center, dimensions, divisions);

	// Registration on the sectors proximity DB
	///////////////////////////////////////////////////////////////////////////////

	mProximityToken = mLump.getCurrentScenario()->getProximityDatabase()->allocateToken (this);
	
	// Initial position
	mProximityToken->updateForNewPosition ( 
		Vec3( mPosition.x, mPosition.y, mPosition.z ) );

	mRenderingState = STATE_VISIBLE;

	mShowActiveSector = StringConverter::parseBool(
		mLump.getSceneConfigFile()->getSetting( "showActiveSector", "General" )
	);

	// Proximiy indicator node creation
	mProximityIndicatorNode = 
		mLump.mSceneMgr->getRootSceneNode()->createChildSceneNode( Vector3::ZERO );	
	
	mLump.increaseEntityCounter();
	
	const string meshName = "SectorIndicator.mesh";

	Entity* piEntity = mLump.mSceneMgr->createEntity( 
		meshName + "ProximityIndicatorNode" + StringConverter::toString( mLump.getEntityCounter() ) , meshName );

	mProximityIndicatorNode->attachObject( piEntity );
	mProximityIndicatorNode->setVisible( false, false );
	mProximityIndicatorNode->setScale( 0.002, 0.002, 0.002 );
}

// ---------------------------------------------------------------------------------

ScenarioSectorBase::~ScenarioSectorBase(void)
{
		// Delete all the instatiated entities
	for (entity_it = mSingleEntities.begin(); 
			entity_it != mSingleEntities.end(); ++entity_it)
	{
		delete (*entity_it);
	}
	mSingleEntities.clear();
	
	// Flocks
	for (flock_it = mFlocks.begin(); 
			flock_it != mFlocks.end(); ++flock_it)
	{
		delete (*flock_it);
	}
	mFlocks.clear();
	
	// Obstacles
	for (obstacle_it = mObstacles.begin(); 
			obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		delete (*obstacle_it);
	}
	mObstacles.clear();

	// Planets
	for (planets_it = mPlanets.begin(); 
			planets_it != mPlanets.end(); ++planets_it)
	{
		delete (*planets_it);
	}
	mPlanets.clear();
}

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::setActive()
{
	if ( mRenderingState != STATE_VISIBLE )
		return;

	mLump.getLogger()->log(getName(), "From " + getState2string() + " to active" );

	mRenderingState = STATE_ACTIVE;

	for (entity_it = mSingleEntities.begin(); 
			entity_it != mSingleEntities.end(); ++entity_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*entity_it);
		entity->show();
	}
	
	// Flocks
	for (flock_it = mFlocks.begin(); 
			flock_it != mFlocks.end(); ++flock_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*flock_it);
		entity->show();
	}
	
	for (obstacle_it = mObstacles.begin(); 
				obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*obstacle_it);
		
		// Show non static obstacles
		if ( !entity->isStatic() )
		{
			entity->startSmoothShowing();
		}

		// Highlight the active sector (show the asteroids bounding box)
		if ( mShowActiveSector ) 
		{
			entity->getEntityNode()->showBoundingBox( true );
		}
	}
}

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::enter()
{
	// Allowed transaction to STATE_VISIBLE from all other states
	if ( mRenderingState != STATE_HIDDEN )
		return;
	
	mLump.getLogger()->log(getName(), "From " + getState2string() + " to visible" );
	
	mRenderingState = STATE_VISIBLE;

	// Show only static obstacles 
	for (obstacle_it = mObstacles.begin(); 
			obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*obstacle_it);
		if ( entity->isStatic() ) 
		{
			entity->startSmoothShowing();
		}
	}
}

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::exit()
{
	// Allowed transaction to STATE_VISIBLE from all other states
		if ( mRenderingState != STATE_ACTIVE )
		return;
	
	mLump.getLogger()->log(getName(), "From " + getState2string() + " to visible" );
	
	mRenderingState = STATE_VISIBLE;

	// Hides single entities and flocks

	for (entity_it = mSingleEntities.begin(); 
			entity_it != mSingleEntities.end(); ++entity_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*entity_it);
		entity->hide();
	}
	
	// Flocks
	for (flock_it = mFlocks.begin(); 
			flock_it != mFlocks.end(); ++flock_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*flock_it);
		entity->hide();
	}
	
	// Hide non static obstacles
	for (obstacle_it = mObstacles.begin(); 
				obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*obstacle_it);

		if ( !entity->isStatic() )
		{
			entity->startSmoothHiding();
		}
		
		// hide the active sector highlighting (hide the asteroids bounding box)
		if ( mShowActiveSector ) 
		{
			entity->getEntityNode()->showBoundingBox( false );
		}
	}
}

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::setHidden()
{
	if ( mRenderingState == STATE_HIDDEN )
		return;

	if ( mRenderingState == STATE_ACTIVE )
		exit();
	
	mLump.getLogger()->log(getName(), "From " + getState2string() + " to hidden" );

	mRenderingState = STATE_HIDDEN;
	
	// Obstacles
	for (obstacle_it = mObstacles.begin(); 
			obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*obstacle_it);
		if ( entity->isStatic() )
		{
			entity->startSmoothHiding();
		}
	}
}

// ---------------------------------------------------------------------------------

// TODO: delete this method
Vector3 ScenarioSectorBase::getRealObjectPosition( const std::string objectName )
{
	return mPosition + 	
		StringConverter::parseVector3(
			mLump.getSceneConfigFile()->getSetting( objectName + "_position" , mName)
		);
}

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::createSectorScene( void )
{
	OverlayElement* currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Core/LoadPanel/Description");
	currOverlay->setCaption( "Loading scenario ("  + getName() + ")" );
	
	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Core/LoadPanel/Bar/Progress");
	
	const float remainderSpace = 0.4 - currOverlay->getWidth();
	const float progressBarIncrement = remainderSpace / mLump.getCurrentScenario()->getSectors().size();
	
	currOverlay->setWidth(
			currOverlay->getWidth() + progressBarIncrement);

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Core/LoadPanel/Comment");
	currOverlay->setCaption( "Asteroids" );
	mLump.mWindow->update();

	// Create scene
	// //////////////////////////////////////////////////////////////////////////////

	StringVector::iterator strList_it;
	StringVector::iterator instanceList_it;

	// ****************************************
	// Asteroids	
	
	StringVector strList = mLump.getSceneConfigFile()->getMultiSetting( "asteroids", mName );

	for (StringVector::iterator strList_it = strList.begin(); 
			strList_it != strList.end(); ++strList_it)
	{
		// Temporarily is used the last sector
		String className = (String)(*strList_it);

		mLump.getLogger()->log(" Added class ", className);

		// Get all the instances declared
		// To specify the instance put _instance after the class name
		// followed by the instance position
		StringVector instanceList = 
			mLump.getSceneConfigFile()->getMultiSetting( className + "_instance" , mName );
		for (StringVector::iterator instanceList_it = instanceList.begin(); 
			instanceList_it != instanceList.end(); ++instanceList_it)
		{

			// Get the instance position
			Vector3 instancePosition = mPosition +
				StringConverter::parseVector3(
				(String)(*instanceList_it) );
			
			mLump.getLogger()->log(" Added instance ", (*instanceList_it));
			
			// Asteroids parameter

			// For each asteroid get the properties
			int asteroidsNum = StringConverter::parseInt(
				mLump.getSceneConfigFile()->getSetting( "asteroidsNum", className )
			);

			int asteroidsRange = StringConverter::parseInt(
				mLump.getSceneConfigFile()->getSetting( "asteroidsRange", className )
			);
			
			// Get the mesh
			mLump.increaseEntityCounter();
			std::string meshName = mLump.getSceneConfigFile()->getSetting( "meshName", className );
			std::string entityName = "AsteroidBase" + StringConverter::toString( mLump.getEntityCounter() );
			Entity* asteroidsBaseEntity = mLump.mSceneMgr->createEntity( entityName,  meshName );
			
			// Now set as asteroid material a clone of the original material 
			// This allow the alpha tuning of the asteroid's material
			string materialCopyName = className + mName + StringConverter::toString( mLump.getEntityCounter() ); 

			MaterialPtr materialCopy = asteroidsBaseEntity->getSubEntity(0)->
				getMaterial()->clone( materialCopyName );
			
			// Load the material
			materialCopy->load();

			asteroidsBaseEntity->getSubEntity(0)->setMaterialName( materialCopyName );

			// The position is into the section content descriptor
			// Create the asteroid system
			EntityAsteroid* stEntity;

			int radius = 0;
			bool radiusIncreasing = true;

			int initialAsteroidsNum = asteroidsNum;

			for (int i = 0; i < asteroidsNum; i++)
			{
				float rad = ( (float) i ) / 5;


				if ( radiusIncreasing )
					radius ++;
				else
					radius --;
				
				Vector3 asteroidPosition;
				
				// Circular set
				if ( initialAsteroidsNum <= asteroidsNum )
				{
					// The position is relative to the sector position
					asteroidPosition = Vector3( 
						cos( rad ) * radius * asteroidsRange,
						sin( rad ) * radius * asteroidsRange, 
						( i - asteroidsNum / 2 ) * asteroidsRange )
						+ instancePosition + newRandVector3( 500 ) ;
					
					if ( i > 0 && ( stEntity->distance( asteroidPosition ) < 1000 ) )
					{
						asteroidsNum ++;
						continue;
					}

					//if ( radiusIncreasing && ( asteroidPosition - instancePosition ).length() > 30000 )
					if ( radiusIncreasing && i > asteroidsNum / 2 )
					{
						radiusIncreasing = false;
					}
				}
				// Random set
				else
				{
					asteroidPosition = Vector3( 
						instancePosition + newRandVector3( asteroidsRange * 100 ) ) ;
				}

				stEntity = new EntityAsteroid(
					className,  
					asteroidsBaseEntity, 
					0, 
					asteroidPosition
					);
				stEntity->registerToProximityDB( mProximityDatabase );
				mObstacles.push_back( stEntity );
			}
		}
	}

	currOverlay->setCaption( "Planets" );
	mLump.mWindow->update();
	
	// ****************************************
	// Planets 

	strList = mLump.getSceneConfigFile()->getMultiSetting( "planet", mName );

	for (StringVector::iterator strList_it = strList.begin(); 
			strList_it != strList.end(); ++strList_it)
	{
		// Temporarily is used the last sector
		String className = (String)(*strList_it);
		
		mLump.getLogger()->log(" Added class ", className);
	
		StringVector instanceList = 
			mLump.getSceneConfigFile()->getMultiSetting( className + "_instance" , mName );
		for (StringVector::iterator instanceList_it = instanceList.begin(); 
			instanceList_it != instanceList.end(); ++instanceList_it)
		{
			// Get the instance position
			Vector3 instancePosition = mPosition +
				StringConverter::parseVector3(
				(String)(*instanceList_it) );
			
			mLump.getLogger()->log(" Added instance ", (*instanceList_it));

			// Add the object to the world
			mPlanets.push_back( new EntityPlanet(
				className, 
				10, 
				instancePosition
			) );	

		} // instances iterator
	} // classes iterator

	currOverlay->setCaption( "Enemies" );
	mLump.mWindow->update();

	// ****************************************
	// Enemies (soldier) 

	mActiveFlocks = 0;
		
	strList = mLump.getSceneConfigFile()->getMultiSetting( "soldierFlock", mName );

	for (StringVector::iterator strList_it = strList.begin(); 
			strList_it != strList.end(); ++strList_it)
	{
		// Temporarily is used the last sector
		String className = (String)(*strList_it);
	
		mLump.getLogger()->log(" Added class ", className);

		StringVector instanceList = 
			mLump.getSceneConfigFile()->getMultiSetting( className + "_instance" , mName );
		for (StringVector::iterator instanceList_it = instanceList.begin(); 
			instanceList_it != instanceList.end(); ++instanceList_it)
		{

			// Get the instance position
			Vector3 instancePosition = mPosition +
				StringConverter::parseVector3(
				(String)(*instanceList_it) );

			mLump.getLogger()->log(" Added instance ", (*instanceList_it));
			
			FlockEnemySoldier* fbEntity;
			fbEntity = new FlockEnemySoldier( className, instancePosition );
			fbEntity->registerToProximityDB( mProximityDatabase );
			mFlocks.push_back( fbEntity );

			mActiveFlocks ++;
		
		}		

	}
		

	// ****************************************
	// Enemies (scout) 

	currOverlay->setCaption( "Hide all entities" );
	mLump.mWindow->update();

	// ****************************************
	// Other 
	
	// Initialization: Hide all 
	// //////////////////////////////////////////////////////////////////////////////

	// Set the state to STATE_VISIBLE to allow the state transatcion to Hidden
	for (entity_it = mSingleEntities.begin(); 
			entity_it != mSingleEntities.end(); ++entity_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*entity_it);
		entity->hide();
	}
	
	// Flocks
	for (flock_it = mFlocks.begin(); 
			flock_it != mFlocks.end(); ++flock_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*flock_it);
		entity->hide();
	}
	
	// Obstacles
	for (obstacle_it = mObstacles.begin(); 
			obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		SteerableEntity* entity = (SteerableEntity*) (*obstacle_it);
		entity->hide();
	}

	mRenderingState = STATE_HIDDEN;
}

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::updateSector(const float currentTime, const float elapsedTime)
{
	// Update obstacles
	for (obstacle_it = mObstacles.begin(); 
			obstacle_it != mObstacles.end(); ++obstacle_it)
	{
		SteerableEntity* entity = (SteerableEntity*)(*obstacle_it);
		entity->updateRenderingStatus();
	}

	// Do not update far sectors
	if ( mRenderingState != STATE_ACTIVE)
		return;

	// Aggiorna tutte le entità dello scenario
	for (entity_it = mSingleEntities.begin(); 
			entity_it != mSingleEntities.end(); ++entity_it)
	{
		(*entity_it)->updateEntity(currentTime, elapsedTime);
	}
	
	Ray mouseRay( mLump.mCamera->getPosition(), mLump.mCamera->getDirection() );
		
	FlockBase* selectedFlock = NULL;
	float minInterectesDistance = SELECTION_MAX_DISTANCE;

	// Flocks
	for (flock_it = mFlocks.begin(); 
			flock_it != mFlocks.end(); ++flock_it)
	{
		(*flock_it)->updateEntity(currentTime, elapsedTime);
		
		Sphere itemSphere = Sphere( (*flock_it)->getPosition(), (*flock_it)->getVariance() );
		std::pair<bool, Real> intersectsResult = mouseRay.intersects( itemSphere );
		
		// TODO fix this
		if ( mLump.getTotalElapsedTime() > 
			mLump.getCurrentScenario()->getLastBehaviourUpdateTime() + 
			mLump.getCurrentScenario()->getBehaviourUpdateDelay() / 2 )
		{
			(*flock_it)->updateBehaviour( currentTime, elapsedTime );
		}
		
		// Flock selection
		/////////////////////////////////////////////////////////////////////////////
		
		// The player flocks cannot be selectable
		if ( typeid( **flock_it ) == typeid( FlockPlayer ) )
			continue;

		// Only compacted flocks are selctable
		bool selectable = intersectsResult.first && (*flock_it)->getVariance() < SELECTION_FLOCK_MIN_VARIANCE;
		
		// Get the neareast flock
		if ( selectable && intersectsResult.second < minInterectesDistance)
		{
			selectedFlock = (*flock_it);
			minInterectesDistance = intersectsResult.second;
		}
		
		(*flock_it)->setHighlighted( false );		

	}

	//mLastBehaviourUpdateTime = mLump.getTotalElapsedTime();
	
	// Highlight the nearest flock only if is sufficiently far
	if ( minInterectesDistance > SELECTION_MIN_DISTANCE )
		mLump.getCurrentScenario()->getDefencePlayerFlock()->
			setSelectedEnemy( selectedFlock, minInterectesDistance );
	
}

//------------------------------------------------------------------------

string ScenarioSectorBase::getState2string( )
{
	switch ( mRenderingState )
	{
	case STATE_ACTIVE:
		return "Active";
	case STATE_VISIBLE:
		return "Visible";
	case STATE_HIDDEN:
		return "Hidden";
	}
	return "NO STATE";
} 

// ---------------------------------------------------------------------------------

void ScenarioSectorBase::annotateFlockDestroyed()
{
	mActiveFlocks --;
}

// ---------------------------------------------------------------------------------

