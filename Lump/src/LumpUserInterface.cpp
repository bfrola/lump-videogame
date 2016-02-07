#include "general.h"

const string CONSOLE_SYSTEM_NAME = "System";
const string CONSOLE_USER_NAME = "User";
const string CONSOLE_CMD_QUIT = "quit";
const string CONSOLE_CMD_FPS = "fps";
const string CONSOLE_CMD_LOGGER = "logger";
const string CONSOLE_CMD_HELP = "help";
const string CONSOLE_CMD_KEYS = "commands";
const string CONSOLE_CMD_NOTUNDERSTOOD = "Command not understood. See 'help' for more information.";
const string CONSOLE_SEPARATOR = "---------------------------------------- ";

LumpUserInterface::LumpUserInterface(void)
{
	// Stato inziale
	mControllerState = CONTROLLER_SHUTTLE;

	mMouseSpeedFactor = mOldMouseSpeedFactor = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("mouseSpeed", "General")
	);
	
	mSlowMotionGameSpeed = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("slowMotionGameSpeed", "General")
	);

	gameSpeed = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("initialGameSpeed", "General")
	);

	SLOWMOTION_MAXTIME = 3.0f;
	
	// Initialize to singleshot for the correct set up of the camera fovY
	mAttackMode = ATTACKMODE_SINGLESHOOT;

	mZoomValue = 0.5;
	
	mShowLogger = true;
	mShowFps = true;
	
}

LumpUserInterface::~LumpUserInterface(void)
{
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::initializeHUD(void)
{
	// Initial message
	mLump.getConsole()->log( "", "Lump Game - Designed and developed by Bernardino Frola" );
	mLump.getConsole()->log( "", "Email contact: besaro@tin.it" );
	mLump.getConsole()->log( "", "Web URL: http://bernardinofrola.altervista.org/lump/" );
	mLump.getConsole()->log( "", "" );

	mLump.getConsole()->log( "", "Type 'help' for more information." );
	mLump.getConsole()->log( "---------------------------------------- ", 
		"----------------------------------------" );
	
	// Hide the loading bar overlay
	Overlay* currOverlay = 
		OverlayManager::getSingleton().getByName("Core/LoadOverlay");
	currOverlay->hide();

	// Hide the logger
	cmdLogger();
	cmdFps();

	mLump.getCurrentScenario()->setCameraFOVy( Radian( 1.0f ) );
	
	// The game starts paused
	enterPause();

	
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

bool LumpUserInterface::processUnbufferedKeyInput( InputReader* mInputDevice, const FrameEvent& evt)
{
	
	// Disable the unbuffered input whene the game is paused
	if ( isGamePaused() )
		return true;
	
	// *************************************************
	// Gestione velocità massima
	if (mInputDevice->isKeyDown(KC_W))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->increaseSpeed(0.05);
	} else if (mInputDevice->isKeyDown(KC_S))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->increaseSpeed(-0.05);
	}
		
	if (mInputDevice->isKeyDown(KC_X))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->setSteerToBackward();
	}
	
	// I tasti di movimento laterale spostano l'orientazione del target node
	// rispetto la navetta
	if (mInputDevice->isKeyDown(KC_A))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->setSteerToLeft();
	} else if (mInputDevice->isKeyDown(KC_D))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->setSteerToRight();
	}

	if (mInputDevice->isKeyDown(KC_E))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->setSteerToUp();
	} else if (mInputDevice->isKeyDown(KC_Q))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->setSteerToDown();
	}
	
	// *************************************************
	// Gestione accellerazione
	if (mInputDevice->isKeyDown(KC_R))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->increaseMaxSpeed(0.05);
	}
	if (mInputDevice->isKeyDown(KC_F))
	{
		mLump.getCurrentScenario()->getPlayerEntity()->increaseMaxSpeed(-0.05);
	}

	// *************************************************
	// Gestione velocità massima flotta
	if (mInputDevice->isKeyDown(KC_T))
	{
		mLump.getCurrentScenario()->getDefencePlayerFlock()->increaseMaxSpeed(0.05);
	}
	if (mInputDevice->isKeyDown(KC_G))
	{
		mLump.getCurrentScenario()->getDefencePlayerFlock()->increaseMaxSpeed(-0.05);
	}

	// *************************************************
	// Gestione accellerazione flotta
	if (mInputDevice->isKeyDown(KC_Z))
	{
		mLump.getCurrentScenario()->getDefencePlayerFlock()->increaseMaxForce(-0.03);
	}
	if (mInputDevice->isKeyDown(KC_C))
	{
		mLump.getCurrentScenario()->getDefencePlayerFlock()->increaseMaxForce(0.01);
	}

	// *************************************************
	// Flock speed managing
	
	if (mInputDevice->isKeyDown(KC_SPACE))
	{
		// Increase speed
		mLump.getCurrentScenario()->getPlayerEntity()->lockDirection();
	} 
	else
	{
		mLump.getCurrentScenario()->getPlayerEntity()->unlockDirection();
	}
	
	// *************************************************
	// Camera controller switch
	if (mInputDevice->isKeyDown(KC_LCONTROL))
	{
		mControllerState = CONTROLLER_CAMERA;
	} 
	else 
	{
		mControllerState = CONTROLLER_SHUTTLE;
	}

	// Return true to continue rendering
	return true;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::stopSlowMotion()
{
	//mLastSlowMotionTime = mLump.getTotalElapsedTime() - mLastSlowMotionTime; 
	mLump.getLogger()->log( "Lump", "Slow motion ended" );
	gameSpeed = lastGameSpeed;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::keyClicked(KeyEvent* e)
{
	// *************************************************
	// Console
	if ( isGamePaused() )
	{
		if ( e->getKey() == KC_BACK )
		{	
			if ( mUserCommand.size() > 0 )
				mUserCommand.resize( mUserCommand.size() - 1 );
		} 
		else
		{
			// Compose the user command
			mUserCommand = mUserCommand + e->getKeyChar();
		}

		OverlayElement* currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/UserCommand");
		currOverlay->setCaption( mUserCommand + "_" );
		
		if ( e->getKey() == KC_RETURN )
		{	
			execCommand( );
		}
		else
		if ( e->getKey() == KC_ESCAPE )
		{
			exitPause();
		}

		return;
	}

	// *************************************************
	// Pause
	if ( e->getKey() == KC_ESCAPE )
	{
		enterPause();
	}

	// *************************************************
	// Game exiting

	// DEBUGGING - FINAL VERS EXIT WITH USER COMMAND
	if ( e->getKey() == KC_TAB )
	{	
		mLump.quitGame();
	}

	// If game is paused then others commands are disabled
	if ( isGamePaused() )
	{
		return;
	}

	// *************************************************
	// Slow motion

	if ( e->getKey() == KC_LSHIFT )
	{
		if ( !isGameSlowMotion() )
		{
			// if not expired then resume the slow motion 
			//mLastSlowMotionTime = mLump.getTotalElapsedTime() - mLastSlowMotionTime;
			lastGameSpeed  = gameSpeed;
			gameSpeed = mSlowMotionGameSpeed;
			mLump.getLogger()->log( "Lump", "Slow motion started" );
		}
		else
		{
			stopSlowMotion();
		}
	}

	// *************************************************
	// Change attack mode
}
//-------------------------------------------------------------------------------------
void LumpUserInterface::keyPressed(KeyEvent* e)
{
}
//-------------------------------------------------------------------------------------
void LumpUserInterface::keyReleased(KeyEvent* e)
{
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

bool LumpUserInterface::processUnbufferedMouseInput( 
	InputReader* mInputDevice, const FrameEvent& evt )
{	
	///// Mouse move
	////////////////////////////////////////////////////////////////////////////////////////

	// In the CONTROLLER_CAMERA state the mouse movement manages the camera position
	if (mControllerState != CONTROLLER_CAMERA)
	{
		mRotX = Degree(-mInputDevice->getMouseRelativeX() * mMouseSpeedFactor );
		mRotY = Degree(-mInputDevice->getMouseRelativeY() * mMouseSpeedFactor );
	} else 
	// In the othres states the mouse movement manages the camera direction
	{
		mLump.getCurrentScenario()->increaseCameraDistance( - mInputDevice->getMouseRelativeY() * 0.002 );
		mLump.getCurrentScenario()->increaseCameraHeight( mInputDevice->getMouseRelativeX() * 0.004 );
	}
	
	// Do not handle mouse buttons on pause
	if ( isGamePaused() )
		return true;
	
	///// Mouse buttons
	////////////////////////////////////////////////////////////////////////////////////////
	
	// left button
	if( mInputDevice->getMouseButton( 0 ) )
	{
		// Increase speed
		mLump.getCurrentScenario()->getDefencePlayerFlock()->attack();
		mLump.getCurrentScenario()->getDefencePlayerFlock()->increaseSpeed( 0.02 );
		
		if ( mAttackChargeLevel > 0)
			mAttackChargeLevel += mAttackChargeLevel * evt.timeSinceLastFrame * 5;
		else
			mAttackChargeLevel = 0.01;
		}
	
	else 
	{
		if ( mAttackMode == ATTACKMODE_SINGLESHOOT && mAttackChargeLevel > 0 )
		{
			mLump.getCurrentScenario()->getDefencePlayerFlock()->singleShotAttack( mAttackChargeLevel );

			mAttackChargeLevel = 0;
		}
		mLump.getCurrentScenario()->getDefencePlayerFlock()->idle();
	}
	
	// right button
	if ( mInputDevice->getMouseButton( 1 ) )
	{

		mLump.getCurrentScenario()->getDefencePlayerFlock()->attack();
		

		// Backup the old mMouseSpeedFactor value
		if ( mAttackMode == ATTACKMODE_CONTINUOS )
		{		
			switchAttackModeToSingleShot();
			
			// Zoming setup
			mOldMouseSpeedFactor = mMouseSpeedFactor;
		}

		// *************************************************
		// Zooming		

		mLump.getCurrentScenario()->setCameraFOVy( Radian( mZoomValue ) );

		// Increasing the zoom, the mouse speed decrease
		mMouseSpeedFactor = mLump.mCamera->getFOVy().valueRadians() / 10;	
	} 
	// restore the mMouseSpeedFactor and FOVy values
	// mOldMouseSpeedFactor used as activation flag
	else if ( mAttackMode == ATTACKMODE_SINGLESHOOT )
	{
		switchAttackModeToContinuos();
		
		// Zoming setup
		mLump.getCurrentScenario()->setCameraFOVy( Radian( 1.0f ) );
		mMouseSpeedFactor = mOldMouseSpeedFactor;
		mOldMouseSpeedFactor = -1;
	}


	///// Mouse wheel
	////////////////////////////////////////////////////////////////////////////////////////
	
	// La distanza del target dalla camera è gestita con la rotellina del mouse	
	if (mInputDevice->getMouseRelativeZ() > 0) {
		mLump.getCurrentScenario()->increaseCameraDistance( 0.1f );
	} else if (mInputDevice->getMouseRelativeZ() < 0) {
		mLump.getCurrentScenario()->increaseCameraDistance( -0.1f );
	}

	return true;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::moveCamera() 
{
	// The carrent rotation is based on the current camera orientation

	mLump.mCamera->rotate( mLump.mCamera->getUp(), Radian( mRotX ) );
	mLump.mCamera->rotate( mLump.mCamera->getRight(), Radian( mRotY ) );
	
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::updateHUD( float elapsedTime )
{
	// Update the HUD one time overy two updates
	//////////////////////////////////////////////////////////

	if ( mUpdateCounter )
	{
		mUpdateCounter = !mUpdateCounter;
		return;
	}
		
	mUpdateCounter = !mUpdateCounter;

	OverlayElement* currOverlay;

	// Update the right logger
	//////////////////////////////////////////////////////////
	if ( mShowLogger && mLump.getLogger()->isModified())
	{
		currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/LogPanel/Messages");
		currOverlay->setCaption( mLump.getLogger()->getMsgListString( "\n" ) );		
	}

	// Update the left logger
	//////////////////////////////////////////////////////////
	if ( mLump.getEventsLogger()->isModified())
	{
		currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/EventsPanel/Messages");
		currOverlay->setCaption( mLump.getEventsLogger()->getMsgListString( "\n" ) );		
	}

	// Update the console
	//////////////////////////////////////////////////////////

	if ( isGamePaused() && mLump.getConsole()->isModified())
	{
		currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/ConsoleMessages");
		currOverlay->setCaption( mLump.getConsole()->getMsgListString( "\n" ) );		
	}

	// Console showing/hiding
	//////////////////////////////////////////////////////////

	currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/ConsolePanel");
	if ( isGamePaused() )
	{
		float dist = currOverlay->getTop() - 0.01;
		if ( dist < 0.01 )
			currOverlay->setTop( currOverlay->getTop() - dist * elapsedTime * 15  );
	}
	else
	{
		float dist = currOverlay->getTop() + currOverlay->getHeight() + 0.05;
		if ( dist > 0.01 )
			currOverlay->setTop( currOverlay->getTop() - dist * elapsedTime * 15  );
	}

	// Update the items informations
	//////////////////////////////////////////////////////////
	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/FlockItemsNumber");
	currOverlay->setCaption( 
		StringConverter::toString( 
			mLump.getCurrentScenario()->getDefencePlayerFlock()->getDamagedFlockItemsNumber() )
		+ "/" + StringConverter::toString(
			mLump.getCurrentScenario()->getDefencePlayerFlock()->getActiveFlockItemsNumber() ) 
		+ "/" + StringConverter::toString(
			mLump.getCurrentScenario()->getDefencePlayerFlock()->getFlockItems().size()
		) );

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/FlockItemsCurrentSpeed");
	currOverlay->setCaption( StringConverter::toString(
		(int) mLump.getCurrentScenario()->getDefencePlayerFlock()->speed()
	) );

	// Target flock infos
	//////////////////////////////////////////////////////////
	
	if ( mLump.getCurrentScenario()->getDefencePlayerFlock()->getTargetEntity() != NULL )
	{
		FlockBase* targetFlock = (FlockBase*) mLump.getCurrentScenario()->
			getDefencePlayerFlock()->getTargetEntity();
		
		currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/TargetFlockItemsNumber");
		
		currOverlay->setCaption( 
			StringConverter::toString(
				targetFlock->getDamagedFlockItemsNumber() )
			+ "/" +	StringConverter::toString(
				targetFlock->getActiveFlockItemsNumber() )
			+ "/" + StringConverter::toString(
				targetFlock->getFlockItems().size()
			) );

		currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/TargetFlockEnergy");
		currOverlay->setCaption( StringConverter::toString( 
				targetFlock->getEnergy()
			) + "/" +
			StringConverter::toString( 
				targetFlock->getMaxEnergy()
			) );	

	}
	
	// Attack charge level
	//////////////////////////////////////////////////////////
	
	currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/FlockEnergy");
	currOverlay->setCaption( StringConverter::toString( 
			mLump.getCurrentScenario()->getDefencePlayerFlock()->getEnergy()
		) + "/" +
		StringConverter::toString( 
			mLump.getCurrentScenario()->getDefencePlayerFlock()->getMaxEnergy()
		) );	

	if ( mAttackChargeLevel > 0 && mAttackChargeLevel < 0.8 )
	{
		currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/AttackChargeLevel");
		currOverlay->setWidth( mAttackChargeLevel * 0.7 );

	}
	

	// Slow motion
	//////////////////////////////////////////////////////////
	
	if ( mSlowMotionRemainingTime < SLOWMOTION_MAXTIME )
	{
		currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/SlowMotionPanel");
		
		float slowMotionBarWidth = 0.2;
		slowMotionBarWidth = slowMotionBarWidth * 
				( mSlowMotionRemainingTime / SLOWMOTION_MAXTIME );
		
		currOverlay->setWidth( slowMotionBarWidth );
	}

	// Playership info
	//////////////////////////////////////////////////////////

	currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/PlayerShipEnergy");
	currOverlay->setCaption( StringConverter::toString( 
			mLump.getCurrentScenario()->getPlayerEntity()->getEnergy()
		) + "/" + StringConverter::toString( 
			mLump.getCurrentScenario()->getPlayerEntity()->getMaxEnergy()
		) );

	currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/PlayerShipCurrentSpeed");
	currOverlay->setCaption( StringConverter::toString( 
			(int) mLump.getCurrentScenario()->getPlayerEntity()->speed()
		) );

	currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/PlayerShipNeighborsNumber");
	currOverlay->setCaption( StringConverter::toString( 
			mLump.getCurrentScenario()->getPlayerEntity()->getNeighbors().size()
		) );
	
	// Current sector name
	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/CurrentSector");
	currOverlay->setCaption( "Current side: " + mLump.getCurrentScenario()->getCurrentSector()->getName() );

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/ActiveFlocks");
	currOverlay->setCaption( "Active flocks: " + 
		StringConverter::toString( mLump.getCurrentScenario()->getCurrentSector()->getActiveFlocks() ) );

	// Update the statistics
	//////////////////////////////////////////////////////////
	if ( !mShowFps )
		return;

	const RenderTarget::FrameStats& stats = mLump.mWindow->getStatistics();

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/StatsFps");
	currOverlay->setCaption( "CurrFps: " + StringConverter::toString( stats.lastFPS ) );

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/StatsAvgFps");
	currOverlay->setCaption( "AvgFps: " + StringConverter::toString(	stats.avgFPS ) );

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/StatsTriangle");
	currOverlay->setCaption( "Triangle: " + StringConverter::toString(	stats.triangleCount ) );

	currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/SteerableEntities");
	currOverlay->setCaption( "StEntities#: " + StringConverter::toString( mLump.getEntityCounter() ) );
	
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::updateGameBehaviour( float elapsedTime )
{
	if ( isGameSlowMotion() )
	{
		// read this parameters from config file
		mSlowMotionRemainingTime -= elapsedTime * 0.5;

		if ( mSlowMotionRemainingTime < 0 )
		{
			stopSlowMotion();
		}		
	}
	else if ( mSlowMotionRemainingTime < SLOWMOTION_MAXTIME )
	{
		mSlowMotionRemainingTime += elapsedTime * 0.05;
	}
	
	// Update the neighbors info
	//////////////////////////////////////////////////////////
	
	AVGroup neighbors;// = mLump.getCurrentScenario()->getPlayerEntity()->getNeighbors();
	float radarRadius = 12000;

	mLump.getCurrentScenario()->getPlayerEntity()->getProximityToken()->
		findNeighbors (
			mLump.getCurrentScenario()->getPlayerEntity()->position(), 
			radarRadius, 
			neighbors
		);

	for (AVIterator neighbors_it=neighbors.begin(); 
			neighbors_it != neighbors.end(); ++neighbors_it)
	{
		SteerableEntity* neighbor = (SteerableEntity*)(*neighbors_it);
		
		SceneNode* proximityIndicatorNode = neighbor->getProximityIndicatorNode();
		if ( proximityIndicatorNode != NULL  )
		{
			if ( !mLump.mCamera->isVisible( neighbor->getPosition() ) )
			{
				// Show the indicator node	
				proximityIndicatorNode->setVisible( true, false );
				
				// Calculate the frustum plane at distance 0 from the camera position
				Plane frustumPlane = mLump.mCamera->getFrustumPlane( 0 );

				// Calculate distance vector between camera and the object
				Vector3 neighborDir = mLump.mCamera->getPosition() - neighbor->getPosition();

				// Project the neighborDir to the frustum plane and normalize
				Vector3 neighborDirProjection = frustumPlane.projectVector( neighborDir );
				neighborDirProjection = neighborDirProjection.normalisedCopy();
				
				proximityIndicatorNode->setPosition( mLump.mCamera->getPosition() 
					+ mLump.mCamera->getDirection() * 10
					+ neighborDirProjection * 4.5 );
	
				proximityIndicatorNode->setOrientation( mLump.mCamera->getOrientation() );

			} 
		}

	}
	
	ScenarioBase::SSGroup sectors = mLump.getCurrentScenario()->getSectors();
	for (ScenarioBase::SSIterator sector_it=sectors.begin(); 
			sector_it != sectors.end(); ++sector_it)
	{
		ScenarioSectorBase* sector = (ScenarioSectorBase*)(*sector_it);
		
		SceneNode* proximityIndicatorNode = sector->getProximityIndicatorNode();
		if ( proximityIndicatorNode != NULL && sector->getActiveFlocks() > 0  )
		{
			if ( !mLump.mCamera->isVisible( sector->getPosition() ) )
			{
				// Show the indicator node	
				proximityIndicatorNode->setVisible( true, false );
				
				// Calculate the frustum plane at distance 0 from the camera position
				Plane frustumPlane = mLump.mCamera->getFrustumPlane( 0 );

				// Calculate distance vector between camera and the object
				Vector3 neighborDir = mLump.mCamera->getPosition() - sector->getPosition();

				// Project the neighborDir to the frustum plane and normalize
				Vector3 neighborDirProjection = frustumPlane.projectVector( neighborDir );
				neighborDirProjection = neighborDirProjection.normalisedCopy();
				
				proximityIndicatorNode->setPosition( mLump.mCamera->getPosition() 
					+ mLump.mCamera->getDirection() * 10
					+ neighborDirProjection * 5 );
	
				proximityIndicatorNode->setOrientation( mLump.mCamera->getOrientation() );
			
			} 
		}
	}

	if ( mAttackChargeLevel > 1 )
	{
		mAttackChargeLevel = 0;
	}

}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::execCommand()
{
	// Delete the \n from the end of the command string
	mUserCommand = mUserCommand.substr( 0, mUserCommand.size() - 1 );

	// Show the command
	mLump.getConsole()->log( "", "" );
	mLump.getConsole()->log( CONSOLE_SEPARATOR, CONSOLE_SEPARATOR );
	mLump.getConsole()->log( CONSOLE_USER_NAME, mUserCommand );
	mLump.getConsole()->log( "", "" );
	
	// Select the action to do
	if (  mUserCommand == CONSOLE_CMD_QUIT )
	{
		mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "Goodbye!" );
		mLump.quitGame();
	} 
	else if (  mUserCommand == CONSOLE_CMD_HELP )
	{
		cmdHelp();
	}
	else if (  mUserCommand == CONSOLE_CMD_LOGGER )
	{
		cmdLogger();
	}
	else if (  mUserCommand == CONSOLE_CMD_FPS )
	{
		cmdFps();
	}
	else if (  mUserCommand == CONSOLE_CMD_KEYS )
	{
		cmdKeys();
	}
	else
	{
		mLump.getConsole()->log( CONSOLE_USER_NAME, CONSOLE_CMD_NOTUNDERSTOOD );
	}

	mUserCommand = "";

	OverlayElement* currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/UserCommand");
	currOverlay->setCaption( "_" );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::switchAttackModeToContinuos()
{
	mAttackMode = ATTACKMODE_CONTINUOS;
	
	// This, do not allow the single shot attack when the state is CONTINUOS
	mAttackChargeLevel = 0;

	// Hide the attack charge bar
	OverlayElement* currOverlay = 
	OverlayManager::getSingleton().getOverlayElement("Lump/AttackChargeLevel");
	currOverlay->hide();

	mLump.getCurrentScenario()->getDefencePlayerFlock()->setAiming( false );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::switchAttackModeToSingleShot()
{
	mAttackMode = ATTACKMODE_SINGLESHOOT;
	
	// Reset the attack charge level
	mAttackChargeLevel = 0;
	
	// Reset the attack charge bar
	// In this mode, the attack charge bar appear invisible
	OverlayElement* currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/AttackChargeLevel");
	currOverlay->setWidth( mAttackChargeLevel );

	// Show the attack charge bar
	currOverlay = OverlayManager::getSingleton().getOverlayElement("Lump/AttackChargeLevel");
	currOverlay->show();

	mLump.getCurrentScenario()->getDefencePlayerFlock()->setAiming( true );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::enterPause()
{
	speedAfterPause = gameSpeed;
	gameSpeed = 0;
	mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "game paused" );
	mUserCommand = "";

	OverlayElement* currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/PausePanel");
	currOverlay->show();
	currOverlay->setTop( 0 );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------


void LumpUserInterface::exitPause()
{
	gameSpeed = speedAfterPause; 
	lastGameSpeed = 0;
	mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "game resumed" );

	OverlayElement* currOverlay = 
		OverlayManager::getSingleton().getOverlayElement("Lump/PausePanel");
	currOverlay->hide();
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

void LumpUserInterface::cmdHelp()
{
	mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, 
		"Help" );
	mLump.getConsole()->log( "", 
		"  " + CONSOLE_CMD_QUIT + ": quit the game" );
	mLump.getConsole()->log( "", 
		"  " + 	CONSOLE_CMD_FPS + ": show/hide fps and scene graphic data" );
	mLump.getConsole()->log( "", 
		"  " + CONSOLE_CMD_LOGGER + ": show/hide events logger" );
	mLump.getConsole()->log( "", 
		"  " + CONSOLE_CMD_HELP + ": show this help" );
	mLump.getConsole()->log( "", 
		"  " + CONSOLE_CMD_KEYS + ": show keyboard/mouse configuration" );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------


void LumpUserInterface::cmdKeys()
{
	mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "Keyboard and mouse configuration" );
	mLump.getConsole()->log( "Movement/game", "" );
	mLump.getConsole()->log( "  [Mouse x/y]", "Camera" );
	mLump.getConsole()->log( "  [W] [S]", "Playership engine power (increase/decrease)" );
	mLump.getConsole()->log( "  [A] [D] [X]", "Playership direction (left/right/back)" );
	mLump.getConsole()->log( "  [LSHIFT]", "Start/end slow motion" );
	mLump.getConsole()->log( "  [LCTRL]", "Move camera with mouse" );
	mLump.getConsole()->log( "  [SPACE]", "Lock playership direction" );
	mLump.getConsole()->log( "Flock", "" );
	mLump.getConsole()->log( "  [Mouse Left-button]", "Attack target/Collapse flock" );
	mLump.getConsole()->log( "  [Mouse Right-button]", "Camera zoom/One shot attack mode" );
	mLump.getConsole()->log( "Others", "" );
	mLump.getConsole()->log( "  [ESC]", "Pause/resume game" );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void LumpUserInterface::cmdFps()
{
	OverlayElement* currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/Stats");
	if ( mShowFps )
	{
		currOverlay->hide();
		mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "Statistics hided" );
	}
	else
	{
		currOverlay->show();
		mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "Statistics showed" );
	}


	mShowFps = !mShowFps;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void LumpUserInterface::cmdLogger()
{
	OverlayElement* currOverlay = 
			OverlayManager::getSingleton().getOverlayElement("Lump/LogPanel");
	if ( mShowLogger )
	{
		currOverlay->hide();
		mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "Logger hided" );
	}
	else
	{
		currOverlay->show();
		mLump.getConsole()->log( CONSOLE_SYSTEM_NAME, "Logger showed" );
	}

	mShowLogger = !mShowLogger;
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------