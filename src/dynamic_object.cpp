#include <iostream>
#include <algorithm>

#include "dynamic_object.h"

#define FREE(ptr) if (ptr) { delete ptr; }


DynamicObject::DynamicObject() {
    setState(INIT_STATE);
}

DynamicObject::~DynamicObject() {
    m_sceneNode->drop();
    FREE(m_rigidBody)
    FREE(m_motionState)
    FREE(m_collisionShape)
}

void DynamicObject::applyTransform() {
    /* Just ignore if sceneNode or rigidbody is null */
    if (!(m_sceneNode && m_rigidBody)) 
        return;

    /* Copy the transform from bullet to the irrlicht node */
    btVector3 pos = m_rigidBody->getCenterOfMassPosition();
    m_sceneNode->setPosition(irr::core::vector3df(
        (irr::f32)pos[0],
        (irr::f32)pos[1],
        (irr::f32)pos[2]));

    irr::core::vector3df euler;
    const btQuaternion &quat = m_rigidBody->getOrientation();
    irr::core::quaternion iquat(
        quat.getX(),
        quat.getY(),
        quat.getZ(),
        quat.getW());
    iquat.toEuler(euler);
    m_sceneNode->setRotation(euler * irr::core::RADTODEG);
}

btRigidBody* DynamicObject::getRigidBody() const {
    return m_rigidBody;
}

DodgeballNode::DodgeballNode(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    btVector3 initPos) :
    DynamicObject()
{
    if (!device) {
        setState(FATAL_STATE);
        return;
    }
    
    irr::scene::ISceneManager *smgr = device->getSceneManager();
    irr::video::IVideoDriver *driver = device->getVideoDriver();
    m_sceneNode = smgr->addSphereSceneNode(DODGEBALL_RADIUS);
    m_sceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    m_sceneNode->setMaterialTexture(
        0, driver->getTexture("models/dodgeball/dodgeball_texture.jpg"));
    
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initPos);

    m_motionState = new btDefaultMotionState(transform);
    m_collisionShape = new btSphereShape(DODGEBALL_RADIUS);

    btVector3 locInertia;
    m_collisionShape->calculateLocalInertia(DODGEBALL_MASS, locInertia);
    m_rigidBody = new btRigidBody(
        DODGEBALL_MASS, m_motionState, m_collisionShape, locInertia);
    m_rigidBody->setRestitution(0.6);
    m_rigidBody->setDamping(0.2, 0.2);

    world->addRigidBody(m_rigidBody);
}

DodgeballNode::~DodgeballNode() {
    /* dtor */
}

void DodgeballNode::hitFloor() {
    if (getState() == DGDBL_ACTIVE)
        std::cout << "(BALL): Hit floor, inactive!" << std::endl;
    setState(DGDBL_INACTIVE);
}

void DodgeballNode::hitPlayer() {
    if (getState() == DGDBL_ACTIVE)
        std::cout << "(BALL): Hit player, inactive!" << std::endl;
    setState(DGDBL_INACTIVE);
}

void DodgeballNode::throwBall(btVector3 impulse) {
    m_rigidBody->applyCentralImpulse(impulse);
    setState(DGDBL_ACTIVE);
}

WallNode::WallNode(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    btVector3 initPos,
    irr::core::vector3df scale) :
    DynamicObject()
{
    if (!device) {
        setState(FATAL_STATE);
        return;
    }
    
    irr::scene::ISceneManager *smgr = device->getSceneManager();
    irr::video::IVideoDriver *driver = device->getVideoDriver();
    m_sceneNode = smgr->addCubeSceneNode(1.0);
    m_sceneNode->setScale(scale);
    m_sceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    m_sceneNode->setMaterialTexture(
        0, driver->getTexture("test_texture.jpg"));
    
    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initPos);

    m_motionState = new btDefaultMotionState(transform);
    m_collisionShape = new btBoxShape(btVector3(
        scale.X * 0.5, scale.Y * 0.5, scale.Z * 0.5));

    btVector3 locInertia;
    m_collisionShape->calculateLocalInertia(0.0, locInertia);
    m_rigidBody = new btRigidBody(
        0.0, m_motionState, m_collisionShape, locInertia);
    m_rigidBody->setRestitution(0.99);

    world->addRigidBody(m_rigidBody);
    applyTransform();
}

WallNode::~WallNode() {
    /* dtor */
}

AvatarNode::AvatarNode(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    btVector3 initPos,
    TeamType team,
    std::string fileName) :
    m_targetVel(0.0, 0.0, 0.0)
{
    m_teamType = team;

    if (!device) {
        setState(FATAL_STATE);
        return;
    }
    
    irr::scene::ISceneManager *smgr = device->getSceneManager();

    /* Load the model file. */
    if (fileName != "NULL") {
        m_animatedMesh = smgr->getMesh(fileName.c_str());
        m_sceneNode = smgr->addAnimatedMeshSceneNode(m_animatedMesh);
        m_sceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    }

    btTransform transform;
    transform.setIdentity();
    //initPos.setY(initPos.y()+(ext.Y/2.0));
    transform.setOrigin(initPos);
    
    m_motionState = new btDefaultMotionState(transform);
    if (m_sceneNode != NULL) {
        /* Setting scale; this is tricky... */
        irr::core::vector3df ext = m_sceneNode->getTransformedBoundingBox().getExtent();
        //std::cout << ext.X << ", " << ext.Y << ", " << ext.Z << std::endl;
        //m_sceneNode->setScale(irr::core::vector3df(
        //    (2.0*HUMAN_RADIUS)/ext.X,
        //    HUMAN_HEIGHT/ext.Y,
        //    (2.0*HUMAN_RADIUS)/ext.Z));

        double radius = std::max(ext.X, ext.Z);
        //m_collisionShape = new btCapsuleShape(radius, ext.Y);
        m_collisionShape = new btCapsuleShape(radius, ext.Y-(radius/2.0));
    } else {
        m_collisionShape = new btCapsuleShape(HUMAN_RADIUS, HUMAN_HEIGHT);
    }

    /* Generate the rigidbody and local inertia... */
    btVector3 locInertia;
    m_collisionShape->calculateLocalInertia(80.0, locInertia); // 80kg - avg human mass
    btRigidBody::btRigidBodyConstructionInfo constructInfo(
        80.0, m_motionState, m_collisionShape, locInertia);
    constructInfo.m_friction = 0.001;
    m_rigidBody = new btRigidBody(
        constructInfo);
        //80.0, m_motionState, m_collisionShape, locInertia);
    m_rigidBody->setRestitution(0.75);
    m_rigidBody->setAngularFactor(btVector3(0.0, 0.0, 0.0)); // no tumbling

    world->addRigidBody(m_rigidBody);
    applyTransform();
    
    /* Simple hack to get things working for now...
     * There will need to be a way to determine which side
     * who is on so the mirroring works correctly.
     *
     * TODO: A way to make the player "face" the origin?
     */
    if (m_teamType == RED && m_sceneNode)
        m_sceneNode->setRotation(
            irr::core::vector3df(0.0, 180.0, 0.0));
    
    setState(PLAYER_ACTIVE);
}

void AvatarNode::boop() {
    if (getState() == PLAYER_ACTIVE)
        std::cout << "(PLAYER): BOOP!" << std::endl;
    setState(PLAYER_OUT);
}

void AvatarNode::setTargetVelocity(btVector3 vel) {
    m_targetVel = vel;
}

void AvatarNode::setLateral(double lat) {
    m_targetVel.setX(lat);
}

void AvatarNode::setForward(double forward) {
    m_targetVel.setZ(forward);
}

void AvatarNode::stop() {
    m_targetVel.setZero();
}

void AvatarNode::applyTransform() {
    /* Modified transforms... */
    
    /* Just ignore if sceneNode or rigidbody is null */
    if (!(m_sceneNode && m_rigidBody)) 
        return;

    /* Copy the transform from bullet to the irrlicht node */
    /* we dont' care about rotation, there won't be any tumbling */
    btVector3 pos = m_rigidBody->getCenterOfMassPosition();
    m_sceneNode->setPosition(irr::core::vector3df(
        (irr::f32)pos[0],
        (irr::f32)pos[1],
        (irr::f32)pos[2]));

    /* Apply control loop to the model */
    //applyControlLoop();
}

void AvatarNode::applyControlLoop() {
    btVector3 currentVel = m_rigidBody->getLinearVelocity();
    btVector3 error = m_targetVel - currentVel;
    btVector3 output = 1000*error;
    m_rigidBody->applyCentralForce(output);
    //m_rigidBody->applyCentralForce(btVector3(0.0, 0.0, -1000.0));
    std::cout
        << output.getX() << ", " 
        << output.getY() << ", "
        << output.getZ() << std::endl;
}

AvatarNode::~AvatarNode() {
    /* dtor */
    std::cout << "DROP PLAYA" << std::endl;
    m_animatedMesh->drop();
}

CameraAvatar::CameraAvatar(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    irr::scene::ICameraSceneNode *camera,
    btVector3 initPos,
    TeamType team) :
    AvatarNode(device, world, initPos, team, "NULL")
{
    m_sceneNode = camera;
    
    /* Simple hack to get things working for now...
     * There will need to be a way to determine which side
     * who is on so the mirroring works correctly.
     *
     * TODO: A way to make the player "face" the origin?
     */
    if (m_teamType == RED && m_sceneNode)
        m_sceneNode->setRotation(
            irr::core::vector3df(0.0, 180.0, 0.0));
}

void CameraAvatar::applyTransform() {
    /* Modified transforms... */
    /* Copy the transform from bullet to the irrlicht node */
    /* we dont' care about rotation, there won't be any tumbling */
    btVector3 pos = m_rigidBody->getCenterOfMassPosition();
    m_sceneNode->setPosition(irr::core::vector3df(
        (irr::f32)pos[0],
        (irr::f32)pos[1]+(HUMAN_HEIGHT/2.0),
        (irr::f32)pos[2]));

    /* Apply control loop */
    applyControlLoop();

    m_rigidBody->activate();
}

CameraAvatar::~CameraAvatar() {
    /* destruct!!! */
}
