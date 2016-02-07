/*
-----------------------------------------------------------------------------
This source file is part of OGRE
(Object-oriented Graphics Rendering Engine)
For the latest info, see http://www.ogre3d.org/

Copyright (c) 2000-2005 The OGRE Team
Also see acknowledgements in Readme.html

You may use this sample code for anything you like, it is not covered by the
LGPL like the rest of the engine.
-----------------------------------------------------------------------------
*/

#include "Lump.h"
#include "general.h"

#pragma comment(lib, "user32")

//-------------------------------------------------------------------------------------
LumpApp::LumpApp(void)
{	
	// Inizializzazione del gestore e caricamento del file di configurazione
	mConfigFile = new ConfigFile();
	mConfigFile->load( "Lump.cfg" );

	mSceneConfigFile = new ConfigFile();
	mSceneConfigFile->load( mLump.getConfigFile()->getSetting("sceneCofigFile", "General") );

	// Selezione dello scenario iniziale
	mCurrentScenario = new ScenarioBaseImpl();

	// Contatore del tempo trascorso
	mTotalElapsedTime = 0;

	// Impostazione del logger
	int loggerSize = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("size", "Logger")
	);

	mLogger = new UtilityLogger( loggerSize );

	loggerSize = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("size", "Console")
	);

	mConsole = new UtilityLogger( loggerSize );

	mEventsLogger = new UtilityLogger( 5 );
	
	string loggerLevel = mLump.getConfigFile()->getSetting("level", "Logger");
	string loggerSenderFilter = mLump.getConfigFile()->getSetting("senderFilter", "Logger");

	mLogger->setLevelString( loggerLevel );
	mLogger->setSenderFilter( loggerSenderFilter );

	// Delclare the input event processor
	mUserInterface = new LumpUserInterface();
}
//-------------------------------------------------------------------------------------
LumpApp::~LumpApp(void)
{
	delete mCurrentScenario;
}

//-------------------------------------------------------------------------------------
void LumpApp::createScene(void)
{
	mHUDOverlay = OverlayManager::getSingleton().getByName("Lump/HUD");
	mHUDOverlay->show();

	mCurrentScenario->enterScenario();
	
	// Lettura del valore iniziale della velocità di gioco

	float clipDistance = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("nearClipDistance", "General")
	);

	mCamera->setNearClipDistance( clipDistance );

	clipDistance = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("farClipDistance", "General")
	);

	mCamera->setFarClipDistance( clipDistance );

	float FOVy = StringConverter::parseReal(
		mLump.getConfigFile()->getSetting("FOVy", "General")
	);

	mCamera->setFOVy( Radian(FOVy) );
	
	// Elimina il riferimento al file di configurazione
	// in modo da renderlo utilizzabile solo in fase di configurazione 
	// e non in fase di aggiornamento della scena
	delete mConfigFile;

	mEventProcessor->addKeyListener( mUserInterface );

	// Setup the HUD
	mUserInterface->initializeHUD();	
}

//-------------------------------------------------------------------------------------
bool LumpApp::updateScene(const FrameEvent& evt)
{
	// Calcola il tempo trascorso da passare alla libreria OpenSteer
	// Il tempo trascorso è moltiplicato per il fattore gameSpeed
	float elapsedTime = evt.timeSinceLastFrame * mUserInterface->getGameSpeed();

	if ( !mUserInterface->isGamePaused() )
		mTotalElapsedTime = mTotalElapsedTime + elapsedTime;

	mCurrentScenario->updateCamera( evt.timeSinceLastFrame );
	
	mCurrentScenario->updateScenario( mTotalElapsedTime, elapsedTime );
	
	mUserInterface->updateGameBehaviour( elapsedTime );

	// Game speed indipendent
	mUserInterface->updateHUD( evt.timeSinceLastFrame );

	return true;
}

//-------------------------------------------------------------------------------------
bool LumpApp::processUnbufferedKeyInput(const FrameEvent& evt)
{	
	return mUserInterface->processUnbufferedKeyInput( mInputDevice, evt );
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
bool LumpApp::processUnbufferedMouseInput(const FrameEvent& evt)
{
	return mUserInterface->processUnbufferedMouseInput( mInputDevice, evt );
}
//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------
void LumpApp::moveCamera()
{
	mUserInterface->moveCamera();
}

//-------------------------------------------------------------------------------------
//-------------------------------------------------------------------------------------

//******************************************************************
// MAIN
//******************************************************************

LumpApp mLump;

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
#define WIN32_LEAN_AND_MEAN
#include "windows.h"
#endif

#ifdef __cplusplus
	extern "C" {
#endif

#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
		INT WINAPI WinMain( HINSTANCE hInst, HINSTANCE, LPSTR strCmdLine, INT )
#else
		int main(int argc, char *argv[])
#endif
		{
			// Create application object

			SET_TERM_HANDLER;

			try {
				mLump.go();
			} catch( Ogre::Exception& e ) {
#if OGRE_PLATFORM == OGRE_PLATFORM_WIN32
				MessageBox( NULL, e.getFullDescription().c_str(), "An exception has occured!", MB_OK | MB_ICONERROR | MB_TASKMODAL);
#else
				std::cerr << "An exception has occured: " <<
					e.getFullDescription().c_str() << std::endl;
#endif
			}

			return 0;
		}

#ifdef __cplusplus
	}
#endif
