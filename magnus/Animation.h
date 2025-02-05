#ifndef __Animation_h_
#define __Animation_h_
 
#include "BaseApplication.h"
#include "AI_Extension.h"
#include <deque>
#include <vector>
#include <algorithm>

class Animation
{
public:
	Animation(void);
    Animation(std::vector<std::vector<int>>, 
		Ogre::SceneManager*, 
		Ogre::Camera*,
		int SFR = 60,
		int FFR = 30,
		int SFD = 20,
		int FFD = 5,
		int DFD = 100,
		float AIS = 0.8);
    ~Animation(void);

	// Functions
	void AnimationInit(Ogre::SceneManager* mSceneMgr, Ogre::Camera* mCamera);
	bool UpdateAnimation(const Ogre::FrameEvent &evt,Ogre::SceneManager*, Ogre::Camera*);

private:
	 // Functions
     bool NextLocation(Ogre::Camera*);
	 void updateFog(const Ogre::FrameEvent &evt, Ogre::SceneManager*, Ogre::Camera*);
	 void updateArrow(const Ogre::FrameEvent &evt, Ogre::SceneManager*, Ogre::Camera*);
	 bool updateNinja(const Ogre::FrameEvent &evt, Ogre::SceneManager*, Ogre::Camera*);
 
     
	 Ogre::Real mDistance;                  // The distance the object has left to travel
     Ogre::Vector3 mDirection;              // The direction the object is moving	
	 Ogre::Vector3 mDestination;            // The destination the object is moving towards
	 Ogre::Real mWalkSpeed;                 // The speed at which the object is moving
	 std::deque<Ogre::Vector3> mWalkList;   // The list of points we are walking to

 
     Ogre::AnimationState *mAnimationState; // The current animation state of the object
 
     Ogre::Entity *ninjaEntity;                 // The Entity we are animating
     Ogre::SceneNode *ninjaNode;                // The SceneNode that the Entity is attached to
	 Ogre::Entity *arrowEntity;
	 Ogre::SceneNode *arrowNode;
	 bool robotAlive;
	 bool robotDead;
	 float deathAnimationTime;

	 // Ninja
	 Ogre::Quaternion mOrientSrc;               // Initial orientation
	 Ogre::Quaternion mOrientDest;              // Destination orientation
	 Ogre::Real mRotProgress;                   // How far we've interpolated
	 Ogre::Real mRotFactor;                     // Interpolation step
	 bool mRotating;

	 // Arrow
	 Ogre::Vector3 cameraPos;
	 Ogre::Vector3 cameraDir;
	 Ogre::Vector3 ninjaPos;
	 Ogre::Vector3 arrowPos;
	 Ogre::Vector3 arrowOri;
	 Ogre::Vector3 arrowDir;
	 Ogre::Quaternion arrowQuat;
	 
	 // Other
	 float mapSizeX;
	 float mapSizeY;
 
 
};
 
#endif // #ifndef __Animation_h_