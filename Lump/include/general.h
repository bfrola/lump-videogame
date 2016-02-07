#pragma once

/** 
	\mainpage Lump game

	\image html lumplogo.png

	\author Bernardino Frola
	
	\section intro_sec Introduction
		Lump is a game developed as project for the university course 
		"Interactive Virtual Environments and Videogames" 
		("Ambienti Virtuali Interattivi e Videogiochi"), University of Salerno.

		\subsection contacts_sec Contacts

		\li The project's page is http://bernardinofrola.altervista.org/lump/
		\li E-mail contact of Bernardino Frola: besaro [AT] tin [DOT] it

	\section devstages_sec Development period
		The project started in december of 2007 and it closed in june of 2008.

	\section getstarted_sec Getting started

		This section treats the system requiremnt needed to start the Lump Game.

		\subsection sysreq_sec Hardware requirements

			The suggest harwdare to run Lump at a playeble frame rate is:

			\li P4 1.5 Ghz;
			\li 256Mb of RAM;
			\li Graphics card ATI Radeon x700/nVidia GeForce 7000 series with 64Mb of RAM;
			\li 100Mb of free space on the hard disk.

		\subsection softreq_sec Software requirements
		
			\li Installed Microsoft DirectX 9 or OpenGL APIs.


	\section overview_sec Overview

		In this section is treated the game interface and the game use cases.

		\subsection hud_sec HUD and scenario elements

			The HUD is showed in the figure below.

			\image html hud.png

			Other than the HUD, the figure highlights the ship, player flock and some elements of the scenario,
			such as planets and asteroids.
			
			\subsubsection flockinfo_sec Flock info

				Now, let we see in detail, the informations about the flocks, reported on the left side
				of the HUD, that are, three values: 

				\li Items: the number of flock items is decsribed in the form: 
				Injuried items / Active items / Max flock items. An item is injuried when its energy is lower
				than 1/3 of the max energy, while is active when its energy is greater than 0.
				\li Speed: The current speed of the flock, to be more precise, is the average of the current speed
				of each items.
				\li Energy: Average of the items energy, comparated to the initial energy, e. g.: 
				Current energy / Initial energy.

			\subsubsection shipinfo_sec Ship info

			The player-ship informations, on the right side of the screen, show three values:
			
			\li Energy.
			\li Speed.
			\li Radar#: the number of the neighbors enemies.

		\subsection gamepurpose_sec Game purpose

			The purpose of the game is to destroy all the flock neast, similar to red asteroids, and 
			defend his ship and his flock from the enemy-flock attacks.

		\subsection gaming_sec Gaming

			The world is diveded into a several number of sub-world, called sectors. Each world-sector has
			a certain number of resident enemy-flocks, and a certain number of scenario objects, such as planets or
			asteroids.

			Thus, the player moves, through his ship, alongs different sectors, but only a sector at time is 
			active, the nearest one to the player-ship.
			
			All the enemy-flocks of the active sector try to attack the player when he enters into the enemy-flock
			invisible range area.	

			The game is focused on the battle between flocks. The player has only one weapon available: its flock.
			
			There are two manner to use the flock as a defence/attack weapon: continuos, single-shot.
			During each of this attacks mode, the flock shows a "energy burst effect", that is, a lot 
			of lightning near the flock center.

			\subsubsection enemyflocks_sec Enemy-flocks

				There are tho type of enemy flocks:

				\li Green flocks: this flocks have more energy and are slower and less slick of the red flocks.
				\li Red flocks: this flocks are more slick of the green ones, because it tries to avoid the 
				player-flock attacks. More, this type of flock is more 

			\subsubsection continuosattack_sec Continuos attack/defence

				Simply, by using the left mouse button, the player move flock toward a given target.
				The target is placed in the direction of the camera. The distance of the target to the camera 
				position is fixed when the player do not aim any enemy-flock (figure below).

				\image html continuos1.png

				When the player aims a enemy-flock, then the distance of the the target is setted
				to the distance of the enemy-flock selected. In this mode the player-flock attacks the 
				selected enemy-flock.

				\image html continuos2.png

				This functions is useful to collapse the flock to a restricted area.

			\subsubsection sinshotattack_sec Singe-shot attack

				The single-shot attack allows the player to use the whole flock as rocket launchers's slug.
				The player eneble this attack pressing on the right mouse button.
				The single-shot attack have three phases: flock group, power charging and shot.
				The first one group the flock to the right-bottom of the camera.

				\image html singleshot1.png

				In the second phase, the player charge the lower bar, by pressing on the left mouse button
				(without releasing the right button).
				The third phase is activated when the player release the left mouse button 
				(the right button is still pressed). Now the flock move at very-high speed toward the 
				camera direction (the speed is up to 10 times the continuos attack speed). 

				\image html singleshot2.png

			\subsubsection itemsreact_sec Flock-items reactivation
				
				When the flock impacts any object, some its items can deactivate (item's energy goes to 0). 
				An item deactivated is steady. The player can to reactivate items, simply by moving the
				flock during the attack (continuos or single-shot) near them.
			
			\subsubsection energyrecovery_sec Ship energy recovery

				The player-ship when impact any object leak a lot of energy. To recover the lost energy,
				the player must to destroy the energy-asteroids. The energy-asteroids are blu asteroids,
				smaller than the normal (brown) asteroids placed into the scenario.
		
	\subsection mousekey_sec Mouse/keyboard mapping
		
		Movement/game
		  \li [Mouse x/y]: Camera
		  \li [W] [S]: Playership engine power (increase/decrease)
		  \li [A] [D] [X]: Playership direction (left/right/back)
		  \li [LSHIFT]: Start/end slow motion
		  \li [LCTRL]: Move camera with mouse
		  \li [SPACE]: Lock playership direction

		Flock
		  \li [Mouse Left-button]: Attack target/Collapse flock
		  \li [Mouse Right-button]: Camera zoom/One shot attack mode

		Others
		  \li [ESC]: Pause/resume game




	\section teciss_sec Technical issues
		
		\subsection ogre_sec Ogre rendering engine and development

			The Lump Game is based on the Ogre rendering engine, version 1.2.5.

			The game-application is written in C++, using the develop evironment Visual Studio 2005 Express Edition.
			
			The code is commented with Doxygen 1.5.6.

			The 3D model of the player-ship and flocks items are taken from the "Free-game-models-collection",
			while the asteroids are modelled ad-hoc.

		\subsection opensteer_sec OpenSteer(r) library 

			Some operation as:
			\li Flocking;
			\li Target pursuiting;
			\li Target avoiding;
			\li Collision avoiding.

			are developed through the facilities of the OpenSteer library. This library provide a set
			of low-level artificial intelligence behavioral function, called steering function.

		\subsection world_sec World subdivision - Level of detail (LOD)

			The figure below show the world-sector status:

			\image html sectorization.png

			The nearst sector to the player-ship is the Active sector. When the player ship enter into a
			sector range radius, then, the sector becomve Visible, otherwise, the sector is Hidden.
			
			Depending on the status of the sector a certain level of detail is adopted:
			\li Active: all the object are updated and visible.
			\li Visible: all the object are not updated, only the static asteroid and planets are visible.
			\li Hidden: all the object are not updated, only the planets are visible.

			All the hide/show operation are smoothed. This allow to reduce the impact of the
			vaiable level of detail.

		\subsection ps_sec Particle systems and pools

			Wakes, esplosion and sparks are particle systems. While wakes are not optimized because
			each object must to have a dedicated effect, esplosion and sparks are upper-bounded.
			
			\li Sparks: showed at any impact between two object.
			\li Energy explosion: showed when a flock slams an other object.
			\li Asteroid explosion: showed when an asteroid is destroyed.

			For each of the former effect, there is a dedicated pool.
			Each pool preallocate a certain number of object and deactivate them. When an impact occours,
			the next ready element of the pool is started.
			This classical optimization allow to use at most a number equal the pool size of 
			particle systems.

		\subsection neighbors_sec Neighborhood and collisions detection

			We use the OpenSteer's embededded proximity database to find the neighbors of any object.

			The following figure show how the collision are detected between flocks - flocks and
			flocks - other objects. 

			\image html flockcollisions.png	
			
			In this case, neighbors are: Enemy-flock1, Obstacle1, Obstacle2.

			With exaustive distance calculation over all neighbors,
			is decided whether or not there is a collision.

			To improve the collision detection performance the neighbors area have a variable radius.
			When the neighbors number go over a certain threshold, as illustrated into the
			next diagram, the radius is reduced. Vice versa, when too few neighbors are into the area, 
			the radius is augmented.
			
			\image html flockneighborhood.png	


		\subsection conf_sec Configuration files

			The Lump Game uses two configuration files:

			\li Lump.cfg: general parameter (game speed, camera type, mouse speed, particle systems pools, 
				scene config file name, ...), loggers size, ships and flocks configurations.
			\li LumpScene.cfg (default name): the scene configuration (fog, skybox, ...), 
				sectors (number, position, size, ...), asteroids distribuition, enemy-flocks locations.
			

	\section future_sec Future works and extensions

		Several key poits are to be added/improved to the game:

		\li Improve the mission goals;
		\li Augment the scenario complexity;
		\li Create other types of enemy-flocks, such as, scouts, defender, explorer, ...;
		\li Create the allies;
		\li Create the player base;
		\li Create compex asteroids (not spherical);
		\li Improve the 3D models;
		\li Improve the enemy AI;

 */


#include "Annotation.h"
#include "SteerLibrary.h"
#include "OpenSteerDemo.h"

// Ogre library
#include <Ogre.h>
#include <OgreErrorDialog.h>
//#include <OgreKeyEvent.h>
#include <OgreEventListeners.h>

// Namespaces
using namespace Ogre;
using namespace std;

#include "UtilityLogger.h"
#include "UtilityParticleSet.h"

/*
#include "OpenSteer/Proximity.h"
#include "OpenSteer/OpenSteerDemo.h"
#include "OpenSteer/Draw.h"

*/

// Scenarios 
#include "ScenarioBaseImpl.h"

// Entities
#include "EntityFlockItem.h"
#include "EntityPlayerShip.h"
#include "EntityAsteroid.h"
#include "EntityPlanet.h"

// Flocks
#include "FlockPlayer.h"
#include "FlockEnemySoldier.h"

#include "Lump.h"


extern LumpApp mLump;

// General purpose functions
Ogre::Vector3 newRandVector3( float range );