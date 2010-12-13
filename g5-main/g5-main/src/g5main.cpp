/*
-----------------------------------------------------------------------------
Filename:    g5main.cpp
-----------------------------------------------------------------------------


This source file is generated by the
   ___                   _              __    __ _                  _ 
  /___\__ _ _ __ ___    /_\  _ __  _ __/ / /\ \ (_)______ _ _ __ __| |
 //  // _` | '__/ _ \  //_\\| '_ \| '_ \ \/  \/ / |_  / _` | '__/ _` |
/ \_// (_| | | |  __/ /  _  \ |_) | |_) \  /\  /| |/ / (_| | | | (_| |
\___/ \__, |_|  \___| \_/ \_/ .__/| .__/ \/  \/ |_/___\__,_|_|  \__,_|
      |___/                 |_|   |_|                                 
      Ogre 1.7.x Application Wizard for VC10 (August 2010)
      http://code.google.com/p/ogreappwizards/
-----------------------------------------------------------------------------
*/
#include "g5main.h"
//-------------------------------------------------------------------------------------
g5main::g5main(void)
	: 
	mGuiActive(false),
	mainMenuActive(false),
	cameraFPVinUse(false)
{
	mCEGUI = new OgreCEGUI();
}
//-------------------------------------------------------------------------------------
g5main::~g5main(void)
{
	delete mCEGUI;
}
//-------------------------------------------------------------------------------------
void g5main::createScene(void)
{
	mGuiActive = true;
	mainMenuActive = true;
	mCEGUI->createScene();
}
//-------------------------------------------------------------------------------------
void g5main::clearScene(void)
{
	mSceneMgr->clearScene();
	mTrayMgr->hideAll();
	mTrayMgr->destroyAllWidgets();
	mRoot->removeFrameListener(this);
	if (mCEGUI->extensionSettings.aiSettings != 0) delete mAnimation;
	if (mCEGUI->extensionSettings.physSettings != 0)
	{
		delete mBulletWorld;
		delete player;
	}
	delete mTrayMgr;
}
//-------------------------------------------------------------------------------------
void g5main::createCamera(void)
{
	// Create the camera
	mCamera = mSceneMgr->createCamera("PlayerCam");

	// Position it at 500 in Z direction
	mCamera->setPosition(30, 50, 200);
	mCamera->lookAt(25, 0, 25);
	mCamera->setNearClipDistance(0.1);

	mCameraMan = new OgreBites::SdkCameraMan(mCamera);   // create a default camera controller

	mCameraFPV = mSceneMgr->createCamera("FPVCam");
	mCamera->lookAt(Ogre::Vector3(0,0,-300));
	mCameraFPV->setNearClipDistance(0.1);
	
}
//-------------------------------------------------------------------------------------
void g5main::createFrameListener(void)
{
	Ogre::LogManager::getSingletonPtr()->logMessage("*** Initializing OIS ***");
	OIS::ParamList pl;
	size_t windowHnd = 0;
	std::ostringstream windowHndStr;

	mWindow->getCustomAttribute("WINDOW", &windowHnd);
	windowHndStr << windowHnd;
	pl.insert(std::make_pair(std::string("WINDOW"), windowHndStr.str()));

	mInputManager = OIS::InputManager::createInputSystem( pl );

	mKeyboard = static_cast<OIS::Keyboard*>(mInputManager->createInputObject( OIS::OISKeyboard, true ));
	mMouse = static_cast<OIS::Mouse*>(mInputManager->createInputObject( OIS::OISMouse, true ));

	mMouse->setEventCallback(this);
	mKeyboard->setEventCallback(this);

	//Set initial mouse clipping size
	windowResized(mWindow);

	//Register as a Window listener
	Ogre::WindowEventUtilities::addWindowEventListener(mWindow, this);

	mRoot->addFrameListener(this);
}
//-------------------------------------------------------------------------------------
bool g5main::frameRenderingQueued(const Ogre::FrameEvent& evt)
{
	if(mWindow->isClosed()) return false;

	if(mCEGUI->mShutDown)
	{
		if (mainMenuActive) BaseApplication::createFrameListener();
		return false;
	}
	
	if(mCEGUI->mLaunch) launch();

	if(mGuiActive)
	{
		if(mCEGUI->showMainMenu)
		{
			g5main::clearScene();
			mCEGUI->ShowMainMenu();
			mCEGUI->showMainMenu = false;
			mGuiActive = true;
			mainMenuActive = true;
			g5main::createFrameListener();
		}

		if(!mainMenuActive && !(mCEGUI->ingameMenuVisible)) mGuiActive = false;

		//Need to capture/update each device
		mKeyboard->capture();
		mMouse->capture();

		return true;
	}
	
	// Camera Toggle
	if(cameraFPVinUse)
	{
		mKeyboard->capture();

		if(mKeyboard->isKeyDown(OIS::KC_A))
		{
			player->defaultBody->setLinearVelocity(mCameraFPV->getDerivedRight().normalisedCopy() * Ogre::Vector3(-10.0f,0,-10.0f));
		}

		if(mKeyboard->isKeyDown(OIS::KC_D))
		{
			player->defaultBody->setLinearVelocity(mCameraFPV->getDerivedRight().normalisedCopy() * Ogre::Vector3(10.0f,0,10.0f));
		}

		if(mKeyboard->isKeyDown(OIS::KC_W))
		{
			player->defaultBody->setLinearVelocity(mCameraFPV->getDerivedDirection().normalisedCopy() * Ogre::Vector3(10.0f,0,10.0f));
		}

		if(mKeyboard->isKeyDown(OIS::KC_S))
		{
			player->defaultBody->setLinearVelocity(mCameraFPV->getDerivedDirection().normalisedCopy() * Ogre::Vector3(-10.0f,0,-10.0f));
		}
	}

	//Need to capture/update each device
	mKeyboard->capture();
	mMouse->capture();

	// AI
	if (mCEGUI->extensionSettings.aiSettings != 0 && mGuiActive == false)
	{
		gameFinished = mAnimation->UpdateAnimation(evt, mSceneMgr, mCamera);
	}

	if (!gameFinished)
	{
		mCEGUI->ShowIngameMenu();
		mGuiActive = true;
		gameFinished = false;
	}

	mBulletWorld->mWorld->stepSimulation(evt.timeSinceLastFrame);

	return BaseApplication::frameRenderingQueued(evt);
}
//-------------------------------------------------------------------------------------
bool g5main::keyPressed( const OIS::KeyEvent &arg )
{
	if(mGuiActive)
	{
		mCEGUI->keyPressed(arg);
		return true;
	}
	if (mGuiActive == false && arg.key == OIS::KC_ESCAPE)
	{
		mCEGUI->ShowIngameMenu();
		mGuiActive = true;
		return true;
	}
	return BaseApplication::keyPressed(arg);
}
//-------------------------------------------------------------------------------------
bool g5main::keyReleased( const OIS::KeyEvent &arg )
{
	if(mGuiActive)
	{
		mCEGUI->keyReleased(arg);
		return true;
	}
	if (arg.key == OIS::KC_B){//&& mTimeUntilNextToggle <=0){

		if(cameraFPVinUse)
		{
		
		}
		else
		{
			// starting position of the box
			Ogre::Vector3 position = (mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);
			Ogre::Vector3 speed = mCamera->getDerivedDirection().normalisedCopy() * 9.0f;

			OBBox *box = new OBBox(mSceneMgr, mBulletWorld->mWorld, position, speed, mBulletWorld->mNumEntitiesInstanced, "cube.mesh");

			mBulletWorld->mNumEntitiesInstanced++;				
			//mTimeUntilNextToggle = 0.5;

			// push the created objects to the dequese
			mBulletWorld->mShapes.push_back(box->sceneBoxShape);
			mBulletWorld->mBodies.push_back(box->defaultBody);				
			//mTimeUntilNextToggle = 0.5;
		}

		return true;
	}
	
	if (arg.key == OIS::KC_C){

		//Toggle the camera ...
		if(cameraFPVinUse)
		{
			cameraFPVinUse = false;
			mWindow->getViewport(0)->setCamera(mCamera);
			mCamera->setPosition(mCameraPos);
			mCamera->setOrientation(mCameraOrt);
		}
		else
		{
			cameraFPVinUse = true;
			mWindow->getViewport(0)->setCamera(mCameraFPV);
			mCameraPos = mCamera->getPosition();
			mCameraOrt = mCamera->getOrientation();
		}

		return true;
	}

	
	return BaseApplication::keyReleased(arg);
}
//-------------------------------------------------------------------------------------
bool g5main::mouseMoved( const OIS::MouseEvent &arg )
{
	if(mGuiActive)
	{
		mCEGUI->mouseMoved(arg);
		return true;
	}
	if(cameraFPVinUse)
	{
		mCameraFPV->yaw(Ogre::Degree(-arg.state.X.rel * 0.25f));
		mCameraFPV->pitch(Ogre::Degree(-arg.state.Y.rel * 0.25f));
		return true;
	}
	//return true;
	return BaseApplication::mouseMoved(arg);
}
//-------------------------------------------------------------------------------------
bool g5main::mousePressed( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mGuiActive)
	{
		mCEGUI->mousePressed(arg,id);
		return true;
	}
	//return true;
	return BaseApplication::mousePressed(arg,id);
}
//-------------------------------------------------------------------------------------
bool g5main::mouseReleased( const OIS::MouseEvent &arg, OIS::MouseButtonID id )
{
	if(mGuiActive)
	{
		mCEGUI->mouseReleased(arg,id);
		return true;
	}
	//return true;
	return BaseApplication::mouseReleased(arg,id);
}
//-------------------------------------------------------------------------------------
bool g5main::launch()
{
	mCEGUI->mLaunch = false;
	mGuiActive = false;
	mainMenuActive = false;

	BaseApplication::createFrameListener();
	if (mCEGUI->extensionSettings.threeDSettingsActive == true)
	{
		mMapCreate = new MapCreate(mSceneMgr, mCEGUI->extensionSettings.threeDSettingsArenaSizeX, mCEGUI->extensionSettings.threeDSettingsArenaSizeY, 14, 14 , mCEGUI->extensionSettings.threeDsettingsMaxRoomSize, mCEGUI->extensionSettings.threeDsettingsDoorCnt, mCEGUI->extensionSettings.threeDsettingsFurnitureEn);
	}
	else mMapCreate = new MapCreate(mSceneMgr);

	// AI
	if (mCEGUI->extensionSettings.aiSettings != 0)
	mAnimation = new Animation(mMapCreate->map, mSceneMgr, mCamera,mCEGUI->extensionSettings.aiSettings);

	//Create the Physics world
	mBulletWorld = new BulletInitWorld(mSceneMgr,
									   mMapCreate->returnFloorNode(),
									   mMapCreate->returnWallNodeVec(),
									   mCEGUI->extensionSettings.threeDSettingsArenaSizeX,
								       mCEGUI->extensionSettings.threeDSettingsArenaSizeY,
									   mCEGUI->extensionSettings.threeDSettingsArenaSizeX);

	// starting position of the player
	Ogre::Vector3 position = Ogre::Vector3(10, 3, 10);//(mCamera->getDerivedPosition() + mCamera->getDerivedDirection().normalisedCopy() * 10);
	Ogre::Vector3 speed = Ogre::Vector3(0, 0, 0);//mCamera->getDerivedDirection().normalisedCopy() * 9.0f;


	player = new OBPlayer(mSceneMgr, mBulletWorld->mWorld, position, speed, mBulletWorld->mNumEntitiesInstanced, "ogrehead.mesh");
	player->defaultBody->disableDeactivation();

	//Add the body count and add to deque
	mBulletWorld->mNumEntitiesInstanced++;				
	// push the created objects to the deque
	mBulletWorld->mShapes.push_back(player->sceneBoxShape);
	mBulletWorld->mBodies.push_back(player->defaultBody);				
	//mTimeUntilNextToggle = 0.5;

	//Adding camera to the box node
	player->node->attachObject(mCameraFPV);

	delete mMapCreate;
	return true;
}
//-------------------------------------------------------------------------------------
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
        g5main app;
		//OgreBulletProg app;

        try {
            app.go();
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
