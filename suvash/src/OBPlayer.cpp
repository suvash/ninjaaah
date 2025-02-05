#include "OBPlayer.h"

OBPlayer::OBPlayer(Ogre::SceneManager* mSceneMgr,
							 OgreBulletDynamics::DynamicsWorld *mWorld,
							 Ogre::Vector3 position,
							 Ogre::Vector3 speed,
							 int instanceNumber,
							 Ogre::String meshName)
{
	// create an ordinary, Ogre mesh with texture
	Ogre::Entity *entity = mSceneMgr->createEntity(
 			"Player" + Ogre::StringConverter::toString(instanceNumber),
 			meshName);			    
 	entity->setCastShadows(true);

	sizeB = Ogre::Vector3::ZERO;
	// we need the bounding box of the box to be able to set the size of the Bullet-box
 	Ogre::AxisAlignedBox boundingB = entity->getBoundingBox();
	sizeB = boundingB.getSize(); sizeB /= 2.0f; // only the half needed
 	sizeB *= 0.98f;	// Bullet margin is a bit bigger so we need a smaller size
 							// (Bullet 2.76 Physics SDK Manual page 18)

	//Set material
	//entity->setMaterialName("Examples/RustySteel");
	//Get the scenenode to attach the entity to
 	node = mSceneMgr->getRootSceneNode()->createChildSceneNode();
 	//node->attachObject(entity);
	//Scaling down the cube/mesh entity
 	node->scale(0.03f, 0.01f, 0.03f);
	//Scaling down the bullet entity
 	sizeB *= Ogre::Vector3(0.03f, 0.01f, 0.03f);

	// after that create the Bullet shape with the calculated size
	//new OgreBulletCollisions::CylinderCollisionShape()
	sceneBoxShape = new OgreBulletCollisions::BoxCollisionShape(sizeB);
	//sceneCylinderShape = new OgreBulletCollisions::CylinderCollisionShape(sizeB, Ogre::Vector3::UNIT_Y);
		//new OgreBulletCollisions::CylinderCollisionShape(sizeB, Ogre::Vector3::UNIT_Y);
	//new OgreBulletCollisions::BoxCollisionShape(sizeB);
 	// and the Bullet rigid body
 	defaultBody = new OgreBulletDynamics::RigidBody(
 			"defaultBoxRigid" + Ogre::StringConverter::toString(instanceNumber), 
 			mWorld);

	//Physical properties of the box
	defaultBody->setShape(	node,
 		sceneBoxShape,
 		0.0f,			// dynamic body restitution
 		0.8f,			// dynamic body friction
 		0.001f, 			// dynamic bodymass
 		position,		// starting position of the box
 		Ogre::Quaternion(1,0,1,0));// orientation of the box
	
	defaultBody->getBulletRigidBody()->setGravity(btVector3(0,-30,0));

	//Set the shooting speed 
	//defaultBody->setLinearVelocity(speed ); // shooting speed
}

OBPlayer::~OBPlayer(void)
{
	// Destruct variables;
}
