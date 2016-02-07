#include "general.h"

ScenarioBaseImpl::ScenarioBaseImpl(void)
{
	
}
ScenarioBaseImpl::~ScenarioBaseImpl(void)
{
	
}

// Aggiorna la scena
void ScenarioBaseImpl::onUpdate(const float currentTime, const float elapsedTime)
{
		
}

// Imposta la scena
void ScenarioBaseImpl::onEnter()
{
	
	SceneManager *mSceneMgr = mLump.mSceneMgr;
	
	string skyboxName = mLump.getSceneConfigFile()->getSetting("skyboxName", "General");

	float skyboxDistance = StringConverter::parseReal(
		mLump.getSceneConfigFile()->getSetting("skyboxDistance", "General")
	);
	
	mSceneMgr->setSkyBox(true, skyboxName, skyboxDistance, true);
	
	bool fogEnabled = StringConverter::parseBool(
		mLump.getSceneConfigFile()->getSetting("enabled", "Fog")
	);

	if ( fogEnabled )
	{
		Vector3 fogColor = StringConverter::parseVector3(
			mLump.getSceneConfigFile()->getSetting("color", "Fog")
		);
		float fogDensity = StringConverter::parseReal(
			mLump.getSceneConfigFile()->getSetting("density", "Fog")
		);
		float fogStart = StringConverter::parseReal(
			mLump.getSceneConfigFile()->getSetting("start", "Fog")
		);
		float fogEnd = StringConverter::parseReal(
			mLump.getSceneConfigFile()->getSetting("end", "Fog")
		);
		mSceneMgr->setFog( FOG_LINEAR, 
			ColourValue(fogColor.x, fogColor.y, fogColor.z), 
			fogDensity, 
			fogStart, 
			fogEnd
		);
	
	}

	
	mSceneMgr->setAmbientLight( ColourValue( 0.8, 0.5, 0.5 ) );

	// Crea la luce pricipale (simula la luce del sole disegnato sulla skybox)
	Light* l = mSceneMgr->createLight( "MainLight" );
	l->setType( Light::LT_DIRECTIONAL );
	l->setPowerScale( Real( 200 ) );
	l->setDiffuseColour( ColourValue( 1, 1, 1 ) );
	l->setDirection( 1, 0, 0 );
	
	///////////////////////////////////////////////////
	// Loading sectors

	StringVector sectorNames = mLump.getSceneConfigFile()->getMultiSetting( "sector", "SectorsList" );
	StringVector::iterator sectorNames_it;
	
	int temp_CurrentSectorIndex = 0;
	int temp_i = 0;
	for (StringVector::iterator sectorNames_it = sectorNames.begin(); 
			sectorNames_it != sectorNames.end(); ++sectorNames_it)
	{
		// Temporarily is used the last sector
		String sectorClassName = (String)(*sectorNames_it);

		// Get the sector raidus
		float sectorRadius = StringConverter::parseInt(
			mLump.getSceneConfigFile()->getSetting("radius", sectorClassName) 
		);
		
		StringVector instanceList = 
			mLump.getSceneConfigFile()->getMultiSetting( sectorClassName + "_instance" , "SectorsList" );
		for (StringVector::iterator instanceList_it = instanceList.begin(); 
			instanceList_it != instanceList.end(); ++instanceList_it)
		{
			// Get the instance position
			Vector3 instancePosition = StringConverter::parseVector3( (String)(*instanceList_it) );
		
			ScenarioSectorBase* sector = new ScenarioSectorBase( sectorClassName, instancePosition, sectorRadius );

			mSectors.push_back( sector );
		}
	}
	
	///////////////////////////////////////////////////
	// Scene entities (valid for any sector)

	// Player ship
	mPlayerEntity = new EntityPlayerShip( "PlayerShip", Vector3(0, 0, 2000) );
	mSingleEntities.push_back( mPlayerEntity );
		
	// Player flock	
	//mDefencePlayerFlock = new FlockPlayer( "DefencePlayerFlock", Vector3(0, 0, 1000) );
	//mFlocks.push_back( mDefencePlayerFlock );

	mDefencePlayerFlock = new FlockPlayer( "DefencePlayerFlock", Vector3(0, 0, -1000) );
	mFlocks.push_back( mDefencePlayerFlock );	


	for (sectors_it = mSectors.begin(); 
			sectors_it != mSectors.end(); ++sectors_it)
	{
		ScenarioSectorBase* sector = (ScenarioSectorBase*)(*sectors_it);

		sector->createSectorScene();
	}


	///////////////// COMP TEMP 


	/*Ogre::CompositorPtr comp3 = Ogre::CompositorManager::getSingleton().create(
				"Motion Blur", Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME
			);
	{
		Ogre::CompositionTechnique *t = comp3->createTechnique();
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("scene");
			def->width = 0;
			def->height = 0;
			def->format = Ogre::PF_R8G8B8;
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("sum");
			def->width = 0;
			def->height = 0;
			def->format = Ogre::PF_R8G8B8;
		}
		{
			Ogre::CompositionTechnique::TextureDefinition *def = t->createTextureDefinition("temp");
			def->width = 0;
			def->height = 0;
			def->format = Ogre::PF_R8G8B8;
		}
		/// Render scene
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("scene");
		}
		/// Initialisation pass for sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_PREVIOUS);
			tp->setOutputName("sum");
			tp->setOnlyInitial(true);
		}
		/// Do the motion blur
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("temp");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Combine");
			pass->setInput(0, "scene");
			pass->setInput(1, "sum");
			}
		}
		/// Copy back sum texture
		{
			Ogre::CompositionTargetPass *tp = t->createTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			tp->setOutputName("sum");
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/Copyback");
			pass->setInput(0, "temp");
			}
		}
		/// Display result
		{
			Ogre::CompositionTargetPass *tp = t->getOutputTargetPass();
			tp->setInputMode(Ogre::CompositionTargetPass::IM_NONE);
			{ Ogre::CompositionPass *pass = tp->createPass();
			pass->setType(Ogre::CompositionPass::PT_RENDERQUAD);
			pass->setMaterialName("Ogre/Compositor/MotionBlur");
			pass->setInput(0, "sum");
			}
		}
	}
	Ogre::CompositorManager::getSingleton().addCompositor(
		mLump.mCamera->getViewport(), "Motion blur", -1 );
	Ogre::CompositorManager::getSingleton().setCompositorEnabled(
		mLump.mCamera->getViewport(), "Motion Blur", true );
	*/
	
}

// Resetta la scena
void ScenarioBaseImpl::onReset()
{

}

// Distrugge la scena
void ScenarioBaseImpl::onExit()
{

}

// Crea le entità
void ScenarioBaseImpl::createSimpleEntities()
{

}
