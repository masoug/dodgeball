/*
 * Implements the DodgeballEngine class.
 */

#include <iostream>
#include <math.h>

#include "dodgeball_engine.h"

#define FREE(ptr) if (ptr) { delete ptr; }

irr::core::vector3df RodriguesRotate(
    irr::core::vector3df input, irr::core::vector3df axis, double rad) {
    /* rotate input vector about the axis */
    irr::core::vector3df vrot = input * cos(rad);
    vrot += (axis.crossProduct(input)) * sin(rad);
    vrot += axis * (axis.dotProduct(input)) * (1.0 - cos(rad));
    return vrot;
}

DodgeballEngine::DodgeballEngine(unsigned int width, unsigned int height) :
    m_windowWidth(width), m_windowHeight(height)
{
    /* Initialize Irrlicht! */
    /* TODO: if the prototype is greenlit, then add more options to the 
     * device. It helps add flexibility :) */
    m_device = irr::createDevice(
        irr::video::EDT_OPENGL,
        irr::core::dimension2d<irr::u32>(m_windowWidth, m_windowHeight));
    if (!m_device) {
        /* TODO: Set state to FATAL so that other guys can
         * recognize the error. For now, just return. */
        std::cerr << "Failed to initialize Irrlicht device!" << std::endl;
        return;
    }

    /* Initialize the rest of Irrlicht */
    m_driver = m_device->getVideoDriver();
    m_scenemgr = m_device->getSceneManager();
    /* TODO: Sometime in the future make a better FPS camera */
    m_camera = m_scenemgr->addCameraSceneNodeFPS(0, 100, 0.001);
    m_timer = m_device->getTimer();

    //m_device->getCursorControl()->setVisible(false);
    m_device->setWindowCaption(L"Dodgeball - PROTOTYPE");

    /* Initialize Bullet! */
    m_broadphase = new btDbvtBroadphase();
    m_collisionConfig = new btDefaultCollisionConfiguration();
    m_dispatcher = new btCollisionDispatcher(m_collisionConfig);
    m_solver = new btSequentialImpulseConstraintSolver();
    m_dynamicsWorld = new btDiscreteDynamicsWorld(
        m_dispatcher, m_broadphase, m_solver, m_collisionConfig);
    m_dynamicsWorld->setGravity(btVector3(0, -9.81, 0));

}

DodgeballNode* DodgeballEngine::addDodgeball(btVector3 pos) {
    DodgeballNode *ball = new DodgeballNode(m_device, m_dynamicsWorld, pos);
    m_dodgeballs.push_back(ball);
    return ball;
}

void DodgeballEngine::fireDodgeball() {
    /* get camera transform */
    irr::core::vector3df pos = m_camera->getAbsolutePosition();
    irr::core::vector3df target = m_camera->getTarget();
    
    /* get impulse vector */
    irr::core::vector3df imp = target - pos;
    imp = imp.normalize() * 7.0;

    /* create ball and throw it */
    DodgeballNode *ball = addDodgeball(btVector3(pos.X, pos.Y, pos.Z));
    ball->throwBall(btVector3(imp.X, imp.Y, imp.Z));
}

void DodgeballEngine::handleCollisions() {
    /* Handle them contact manifolds */
    int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold *contactManifold = 
            m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btRigidBody *bodyA = (btRigidBody*)(contactManifold->getBody0());
        btRigidBody *bodyB = (btRigidBody*)(contactManifold->getBody1());

        DodgeballNode *ball = getDodgeball(bodyA);
        if (ball && (bodyB == m_floor->getRigidBody()))
            ball->onCollision();
        else {
            ball = getDodgeball(bodyB);
            if (ball && (bodyA == m_floor->getRigidBody()))
                ball->onCollision();
        }
    }
}

DodgeballNode* DodgeballEngine::getDodgeball(btRigidBody *body) const {
    for (unsigned int i = 0; i < m_dodgeballs.size(); i++) {
        if (body == m_dodgeballs[i]->getRigidBody()) {
            return m_dodgeballs[i];
        }
    }
    return NULL;
}

void DodgeballEngine::setupScene() {
    /* Shift the camera back */
    m_camera->setPosition(irr::core::vector3df(0.0, 1.711, 4.0));
    m_camera->bindTargetAndRotation(true);
    m_camera->setTarget(irr::core::vector3df(0.0, 0.0, 0.0));
    m_camera->setUpVector(irr::core::vector3df(0.0, 1.0, 0.0));
    
    /* Build the court */
    buildCourt();

    /* setup input systems */
    m_device->setEventReceiver(this);
}

void DodgeballEngine::buildCourt() {
    /* Add walls */
    m_floor = new WallNode(
        m_device, m_dynamicsWorld,
        btVector3(0.0, 0.0, 0.0), irr::core::vector3df(10.0, 0.01, 10.0));
    m_nWall = new WallNode(
        m_device, m_dynamicsWorld,
        btVector3(0.0, 2.5, -5.0), irr::core::vector3df(10.0, 5.0, 0.01));
    m_sWall = new WallNode(
        m_device, m_dynamicsWorld,
        btVector3(0.0, 2.5, 5.0), irr::core::vector3df(10.0, 5.0, 0.01));
    m_eWall = new WallNode(
        m_device, m_dynamicsWorld,
        btVector3(5.0, 2.5, 0.0), irr::core::vector3df(0.01, 5.0, 10.0));
    m_wWall = new WallNode(
        m_device, m_dynamicsWorld,
        btVector3(-5.0, 2.5, 0.0), irr::core::vector3df(0.01, 5.0, 10.0));

}

void DodgeballEngine::clearScene() {
    std::cout << "Clearing scene..." << std::endl;
    for (unsigned int i = 0; i < m_dodgeballs.size(); i++)
        FREE(m_dodgeballs[i])
    
    std::cout << "Freeing environmental elements." << std::endl;
    FREE(m_floor)
    FREE(m_nWall)
    FREE(m_sWall)
    FREE(m_eWall)
    FREE(m_wWall)
    FREE(m_ceiling)
    std::cout << "Done clearing scene." << std::endl;
}

void DodgeballEngine::run() {
    double prevTime = m_timer->getTime();

    /* TODO: Maybe consider putting this loop at the top main()-level ? */
    /* doing so would enforce the "statefulness" of the engine? Also a
     * better coding practice? */
    while(m_device->run() && m_driver) {
        if (m_device->isWindowActive()) {
            /* update physics stuff */
            updatePhysics((m_timer->getTime()-prevTime) * 0.001);
            prevTime = m_timer->getTime();

            /* then deal with irrlicht */
            m_driver->beginScene(true, true, irr::video::SColor(255, 0, 0, 255));
            m_scenemgr->drawAll();
            m_driver->endScene();
        } else {
            m_device->yield();
        }
    }
}

void DodgeballEngine::updatePhysics(double timestep) {
    /* update physics */
    m_dynamicsWorld->stepSimulation(timestep, 60);
    
    /* apply to models */
    for (unsigned int i = 0; i < m_dodgeballs.size(); i++)
        m_dodgeballs[i]->applyTransform();

    /* Handle the collisions */
    handleCollisions();
}

bool DodgeballEngine::OnEvent(const irr::SEvent& event) {
    /* handle events */
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        switch(event.MouseInput.Event)
        {
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
            fireDodgeball();
            break;
        case irr::EMIE_MOUSE_MOVED:
            trackCamera(event.MouseInput.X, event.MouseInput.Y);
            break;
        default:
            // We won't use the wheel
            break;
        }
        return true;
    }
    return false;
}

void DodgeballEngine::trackCamera(int x, int y) {
    /* rotate camera... */
    int centerx = x - (m_windowWidth/2);
    int centery = y - (m_windowHeight/2);
    double xproportion = centerx / (m_windowWidth/2.0);
    double yproportion = centery / (m_windowHeight/2.0);
    irr::core::vector3df forward(
        -sin(xproportion*M_PI/2.0), 0.0, -cos(xproportion*M_PI/2.0));
    forward = forward.normalize();
    irr::core::vector3df right = forward.crossProduct(m_camera->getUpVector());
    irr::core::vector3df target = RodriguesRotate(
        forward, right.normalize(), -yproportion*M_PI/2.0);
    m_camera->setTarget(m_camera->getPosition() + target);
}

DodgeballEngine::~DodgeballEngine() {
    std::cout << "Stopping engine..." << std::endl;
    /* Bye bye Bullet! */
    FREE(m_dynamicsWorld)
    FREE(m_solver)
    FREE(m_dispatcher)
    FREE(m_collisionConfig)
    FREE(m_broadphase)

    /* Bye bye irrlicht */
    m_device->drop();
}

