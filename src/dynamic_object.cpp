#include <iostream>
#include <algorithm>

#include "dynamic_object.h"

#define FREE(ptr) if(ptr) { delete ptr; }


DynamicObject::DynamicObject(unsigned int id) : 
    m_objectID(id)
{
    setState(INIT_STATE);
}

DynamicObject::~DynamicObject() {
    m_sceneNode->remove();
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

unsigned int DynamicObject::getID() const {
    return m_objectID;
}

void DynamicObject::nudge(double x, double y, double z, double scale) {
    /* nudges the object towards given position */
    /* TODO: if the impulse is too large, consider just changing the 
     * object's coordinates directly */
    btVector3 target(x, y, z);
    btVector3 current = m_rigidBody->getCenterOfMassPosition();
    btVector3 push = target-current;
    m_rigidBody->activate();
    m_rigidBody->applyCentralImpulse(push*scale);
}

btVector3 DynamicObject::getPosition() const {
    return m_rigidBody->getCenterOfMassPosition(); 
}

/*
 * DODGEBALL NODE
 */

DodgeballNode::DodgeballNode(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    btVector3 initPos, unsigned int ballID) :
    DynamicObject(ballID)
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

bool DodgeballNode::isActive() const {
    return getState() == DGDBL_ACTIVE;
}

/*
 * WALL NODE
 */

WallNode::WallNode(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    btVector3 initPos,
    irr::core::vector3df scale,
    bool invisible) :
    DynamicObject(0)
{
    if (!device) {
        setState(FATAL_STATE);
        return;
    }
    
    irr::scene::ISceneManager *smgr = device->getSceneManager();
    irr::video::IVideoDriver *driver = device->getVideoDriver();
    m_sceneNode = smgr->addCubeSceneNode(1.0);
    if (invisible)
        m_sceneNode->setScale(irr::core::vector3df(0.0, 0.0, 0.0));
    else
        m_sceneNode->setScale(scale);
    m_sceneNode->setMaterialFlag(irr::video::EMF_LIGHTING, false);
    m_sceneNode->setMaterialTexture(
        0, driver->getTexture("models/wall.jpg"));
    
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

/*
 * AVATAR NODE
 */

AvatarNode::AvatarNode(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    btVector3 initPos,
    TeamType team,
    std::string fileName, unsigned int playerID, unsigned int possession) :
    DynamicObject(playerID), m_possession(possession), m_targetVel(0.0, 0.0, 0.0)
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
    m_collisionShape = new btCapsuleShape(HUMAN_RADIUS, HUMAN_HEIGHT);

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

unsigned int AvatarNode::getTeamType() const {
    return m_teamType;
}

void AvatarNode::boop() {
    if (getState() == PLAYER_ACTIVE)
        std::cout << "(PLAYER): BOOP!" << std::endl;
    setState(PLAYER_OUT);
}

void AvatarNode::setTargetVelocity(double x, double y, double z) {
    m_targetVel = btVector3(x, y, z);
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
    applyControlLoop();

    m_rigidBody->activate();
}

void AvatarNode::applyControlLoop() {
    btVector3 currentVel = m_rigidBody->getLinearVelocity();
    btVector3 error = m_targetVel - currentVel;
    btVector3 output = 1000*error; // 1000 turned out to be a good gain value...
    m_rigidBody->applyCentralForce(output);
    //m_rigidBody->applyCentralForce(btVector3(0.0, 0.0, -1000.0));
    //std::cout
    //    << output.getX() << ", " 
    //    << output.getY() << ", "
    //    << output.getZ() << std::endl;
}

btVector3 AvatarNode::getTargetVel() const {
    return m_targetVel;
}

unsigned int AvatarNode::getPossession() const {
    return m_possession;
}

void AvatarNode::incPossession() {
    if (m_possession < 3)
        m_possession++;
}

void AvatarNode::decPossession() {
    if (m_possession > 0)
        m_possession--;
}

AvatarNode::~AvatarNode() {
    /* dtor */
    std::cout << "DROP PLAYA" << std::endl;
    m_animatedMesh->drop();
}

/*
 * CAMERA AVATAR
 */

CameraAvatar::CameraAvatar(
    irr::IrrlichtDevice *device,
    btDiscreteDynamicsWorld *world,
    irr::scene::ICameraSceneNode *camera,
    btVector3 initPos,
    TeamType team, unsigned int playerID, unsigned int possession) :
    AvatarNode(device, world, initPos, team, "NULL", playerID, possession)
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

    /* Check if the player is out of bounds!
     * if a player crosses the center line, then his/her team loses a point.
     */
    //if (m_teamType == RED) {
    //    /* Check if my z position is greater than zero. */
    //    if (pos.getZ() > 0.0)
    //        std::cout << "CROSSED CENTER LINE!!!" << std::endl;
    //} else if (m_teamType == BLUE) {
    //    /* check if my z position is less than 0.0 */
    //    if (pos.getZ() < 0.0)
    //        std::cout << "CROSSED CENTER LINE!!!" << std::endl;
    //}
    
    //Bullet deactivates bodies that have zero velocity
    m_rigidBody->activate();
}

CameraAvatar::~CameraAvatar() {
    /* destruct!!! */
}
