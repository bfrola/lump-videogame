#pragma once

#include <ogre.h>
#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>
#include <OgreStringConverter.h>
#include <OgreException.h>

using namespace Ogre;

/**
	\author Bernardino Frola
	\brief This class manage the user interaction and the game status
*/
class LumpUserInterface : public KeyListener
	
{
public:
	// ************************************************************************
	// Types definitions
	// ************************************************************************

	/**
		Attack modes.
		\li ATTACKMODE_CONTINUOS: the flock follow the target in every time
		\li ATTACKMODE_SINGLESHOOT: charging the attack, flock make only a big, directly attack
	*/
	enum mEnumAttackModes
	{
		ATTACKMODE_CONTINUOS,
		ATTACKMODE_SINGLESHOOT
	};
	
	// Enumerazione degli stati del sistema di controllo della navetta
	enum mEnumControllerStates 
	{ 
		CONTROLLER_SHUTTLE,	// Segue la navetta - valore di default
		CONTROLLER_CAMERA,	// Stato di regolazione altezza e distanza della camera
		CONTROLLER_FREELOOK	// La navetta non segue la direzione della camera, serve per 
							// comandare con più facilità la playerFlock
	};

	// ************************************************************************
	// Constructors/Destructors
	// ************************************************************************
public:
	LumpUserInterface(void);
	virtual ~LumpUserInterface(void);

	// ************************************************************************
	// Methods
	// ************************************************************************
public:
	// Unbuffered input (mouse and keyboard)
	bool processUnbufferedKeyInput( InputReader* mInputDevice, const FrameEvent& evt );
	bool processUnbufferedMouseInput( InputReader* mInputDevice, const FrameEvent& evt );

	// Buffered input (keyboard)
	void keyClicked(KeyEvent* e);
	void keyPressed(KeyEvent* e);
	void keyReleased(KeyEvent* e);

	void moveCamera();
	
	/**
		When the user types a string into the console, this function recognize and execute
		the associated command.
	*/
	void execCommand();
	
	/**
		Update the HUD's components.
	*/
	void updateGameBehaviour( float elapsedTime );

	/**
		Like LumpUserInterface::updateGameBehaviour, but the components updated here must not 
		be infuenced by the current game speed.
	*/
	void updateHUD( float elapsedTime );

	/**
		Return to normal game speed.
	*/
	void stopSlowMotion();
	
	/**
		Setup the HUD's components.
	*/
	void initializeHUD();

	/**
		Swithch the attack mode state to mEnumAttackMode::ATTACKMODE_CONTINUOS
	*/
	void switchAttackModeToContinuos();

	/**
		Swithch the attack mode state to mEnumAttackMode::ATTACKMODE_SINGLESHOT
	*/
	void switchAttackModeToSingleShot();
	
	/**
		Pause the game and show the console.
	*/
	void enterPause();
	
	/**
		Resume the game and hide the console.
	*/
	void exitPause();
	
	/**
		Show the help information into the console.
	*/
	void cmdHelp();

	/**
		Show/hide logger.
	*/
	void cmdLogger();

	/**
		Show/hide stats.
	*/
	void cmdFps();

	/**
		Show the key mapping.
	*/
	void cmdKeys();

	// ************************************************************************
	// GET/SET Methods
	// ************************************************************************

	float getGameSpeed() { return gameSpeed; }
	bool isGamePaused() { return gameSpeed == 0; }
	bool isGameSlowMotion() { return gameSpeed == mSlowMotionGameSpeed; }
	
	// ************************************************************************
	// Fields
	// ************************************************************************
protected:
	float SLOWMOTION_MAXTIME;

	/**
		Speed of the game.
	*/
	float gameSpeed;	

	/**
		Used for backsup the original game speed on slow motion.
	*/
	float lastGameSpeed;				

	/**
		Used for backsup the original game speed during the game pause.
	*/								
	float speedAfterPause;

	/**
		Manage the controller state.
	*/
	mEnumControllerStates mControllerState;
	
	/**
		Used to achieve a dynamic mouse speed factor.
	*/
	float mMouseSpeedFactor;

	/**
		Increasing the zoom, the mouse speed decrease.
		This field backs up the mMouseSpeedFactor during the zoom.
	*/
	float mOldMouseSpeedFactor;

	/**
		Time between now and the slow motion end.
	*/
	float mSlowMotionRemainingTime;

	/**
		Game speed during slow motion.
	*/
	float mSlowMotionGameSpeed;
	
	/**
		Mouse coordinates, in radiants.
	*/
	Radian mRotX, mRotY;
	
	/**
		String typed by the user into the game console.
	*/
	string mUserCommand;

	/**
		Continuos or single shot attack.
	*/
	mEnumAttackModes mAttackMode;
	
	/**
		During the single shot attack this var carries out the attack charge level.
	*/	
	float mAttackChargeLevel;

	/**
		The value of the zoom. By default is fixed to 0.5.
	*/
	float mZoomValue;
	
	/**
		Flag use to show/hide the right logger panel.
	*/
	bool mShowLogger;

	/**
		Flag use to show/hide the fps and stats panel.
	*/
	bool mShowFps;
	
	/**
		Use to reduce the updates of the HUD.
	*/
	bool mUpdateCounter;
};
