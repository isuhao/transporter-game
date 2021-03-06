#include "transporter.h"

bit Surface::init(str name)
{
	if(isEntityInited)
	{
		return true;
	}
	
	//Physics entity initializations
	{
		hkpRigidBodyCinfo surfaceInfo;
		physicsSurfaceShape = new PhysicsFlatLand(100);
		surfaceInfo.m_shape = (hkpShape*)physicsSurfaceShape->createMoppShape();
		surfaceInfo.m_position.set(0.0f,0.0f,0.0f);
		surfaceInfo.m_motionType = hkpMotion::MOTION_FIXED;
		surfaceInfo.m_friction = 0.65f;
		surfaceInfo.m_restitution = 0.005;

		physicsEntity = new hkpRigidBody(surfaceInfo);
		game->gameScene.physicsWorld.getWorld()->lock();
		game->gameScene.physicsWorld.getWorld()->addEntity(physicsEntity);
		game->gameScene.physicsWorld.getWorld()->unlock();

		physicsEntity->removeReference();
		surfaceInfo.m_shape->removeReference();		
	}

	game->visualSystem.lockThread();
	{
		Ogre::Plane surfacePlane;
		surfacePlane.normal = Ogre::Vector3::UNIT_Y;
		surfacePlane.d = 0;
		Ogre::MeshManager::getSingleton().createPlane(name+"Mesh",
			Ogre::ResourceGroupManager::DEFAULT_RESOURCE_GROUP_NAME,
			surfacePlane,
			4000, 4000, 150, 150, true, 1, 300, 300, Ogre::Vector3::UNIT_Z);		
		Ogre::SceneNode* surfaceNode = game->visualSystem.getSceneMgr()->getRootSceneNode()->createChildSceneNode();
		visualEntity  = game->visualSystem.getSceneMgr()->createEntity( name, name+"Mesh" );
		visualEntity->setMaterialName("Ocean2_HLSL");	
		visualEntity->setCastShadows(false);	
		visualEntity->getMesh()->buildTangentVectors();
		//visualEntity->setVisible(false);
		surfaceNode->attachObject(visualEntity);
		surfaceNode->setPosition(0.0f,-120.741,0.0f);		
	}
	game->visualSystem.unlockThread();

	isEntityInited = true;
	return true;
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧

Surface::Surface( Game* game )
:GameEntity(game)
{
	physicsSurfaceShape = NULL;
}

//覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧覧

Surface::~Surface()
{
	if(physicsSurfaceShape)
	{
		delete physicsSurfaceShape;
		physicsSurfaceShape = NULL;
	}
}