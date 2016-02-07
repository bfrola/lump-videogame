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

/*
-----------------------------------------------------------------------------
Filename:    Lump.h
Description: A place for me to try out stuff with OGRE.
-----------------------------------------------------------------------------
*/
#pragma once

#include "BaseApplication.h"
#include "ScenarioBase.h"
#include "UtilityLogger.h"
#include "LumpUserInterface.h"
#include "LumpUserInterface.h"
#include <windows.h>

/**
	\author Bernardino Frola

	\brief Lump application main class

 */

class LumpApp : public BaseApplication
{
public:
	LumpApp(void);
	virtual ~LumpApp(void);


protected:
	void createScene( void );
	bool LumpApp::updateScene( const FrameEvent& evt );
	bool processUnbufferedKeyInput( const FrameEvent& evt );
	bool processUnbufferedMouseInput( const FrameEvent& evt );
	void moveCamera( void );

	// ************************************************************************
	// Metodi GET/SET
	// ************************************************************************	
public:
	ConfigFile* getConfigFile() { return mConfigFile; }
	ConfigFile* getSceneConfigFile() { return mSceneConfigFile; }
	UtilityLogger* getLogger() { return mLogger; }
	UtilityLogger* getEventsLogger() { return mEventsLogger; }
	UtilityLogger* getConsole() { return mConsole; }
	float getTotalElapsedTime() { return mTotalElapsedTime; }
	ScenarioBase* getCurrentScenario() { return mCurrentScenario; }
	int getEntityCounter() { return mEntityCounter; }
	void increaseEntityCounter() { mEntityCounter ++; }

	// ************************************************************************
	// Campi
	// ************************************************************************	
	
protected:
	Overlay* mHUDOverlay;

	ScenarioBase* mCurrentScenario;
	float mTotalElapsedTime;
	
	// Accesso al file di configurazione dell'applicazione
	ConfigFile* mConfigFile;
	ConfigFile* mSceneConfigFile;
	
	UtilityLogger* mLogger;
	UtilityLogger* mEventsLogger;
	UtilityLogger* mConsole;

	// Entity counter
	int mEntityCounter;

	// Input processor
	LumpUserInterface* mUserInterface;

};

