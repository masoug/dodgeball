#include <iostream>

#include "dynamic_object.h"

#define FREE(ptr) if (ptr) { delete ptr; }

/* All units metric, mass in kg */
#define DODGEBALL_RADIUS    0.1524
#define DODGEBALL_MASS      0.680389

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
    if (!(m_sceneNode && m_rigidBody)) {
        /* TODO: switch to an error state! */
        return;
    }

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

void DynamicObject::onCollision() {
    /* Handle collisions */
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

void DodgeballNode::onCollision() {
    if (getState() == DGDBL_ACTIVE)
        std::cout << "COLLISION!!!" << std::endl;
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
    irr::core::vector3df scale,
    AvatarType type)
{
    m_avatarType = type;

    if (!device) {
        setState(FATAL_STATE);
        return;
    }

    irr::scene::ISceneManager *smgr = device->getSceneManager();
    irr::video::IVideoDriver *driver = device->getVideoDriver();

    /* Load the model file. */
    loadModel();

    btTransform transform;
    transform.setIdentity();
    transform.setOrigin(initPos);

    m_motionState = new btDefaultMotionState(transform);
    /* I think the collision shape should be cylinder? */
    /* Generate the rigidbody and localinertial... */
    btVector3 locInertia;
    m_collisionShape->calculateLocalInertia(0.0, locInertia);
    m_rigidBody = new btRigidBody(
        0.0, m_motionState, m_collisionShape, locInertia);
    m_rigidBody->setRestitution(0.75);

    world->addRigidBody(m_rigidBody);
    applyTransform();
}

void AvatarNode::loadModel() {
    /* CASE STATEMENTS!!! */
    switch (m_avatarType) {
        case HOTDOG:
            break;
        case ICE_CREAM:
            break;
        case BANANA:
            break;
        case PHIL:
            break;
        case BLACKHAT:
            break;
        case SNOWMAN:
            break;
        default:
            break;
    }
}

AvatarNode::~AvatarNode() {
    /* dtor */
}

