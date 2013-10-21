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

template<typename Type> Type* fromRigidbody(
    btRigidBody *body,
    std::vector<Type*> input)
{
    for (unsigned int i = 0; i < input.size(); i++) {
        if (body == input[i]->getRigidBody()) {
            return input[i];
        }
    }
    return NULL;
}

DodgeballEngine::DodgeballEngine(unsigned int width, unsigned int height) :
    m_quit(false), m_windowWidth(width), m_windowHeight(height)
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
    m_cursorCtrl = m_device->getCursorControl();

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

    setState(INIT_STATE);
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

void DodgeballEngine::loadPlayers() {
    m_players.push_back(new AvatarNode(
        m_device, m_dynamicsWorld, 
        btVector3(2.5, 0.855, -2.5), AvatarNode::RED,
        "models/players/hotdog/hotdog.b3d"));
   //m_hotdog = new AvatarNode(
   //     m_device, m_dynamicsWorld, 
   //     btVector3(0.0, 0.35, -2.5), AvatarNode::HOTDOG);
   //m_banana = new AvatarNode(
   //     m_device, m_dynamicsWorld, 
   //     btVector3(-2.5, 0.35, -2.5), AvatarNode::BANANA);
}

void DodgeballEngine::handleCollisions() {
    /* Handle them contact manifolds */
    int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
    for (int i = 0; i < numManifolds; i++) {
        btPersistentManifold *contactManifold = 
            m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
        btRigidBody *bodyA = (btRigidBody*)(contactManifold->getBody0());
        btRigidBody *bodyB = (btRigidBody*)(contactManifold->getBody1());

        if (!checkCollisions(bodyA, bodyB))
            checkCollisions(bodyB, bodyA);

        /*
        DodgeballNode *ball = fromRigidbody<DodgeballNode>(bodyA, m_dodgeballs);
        if (ball) {
            if (bodyB == m_floor->getRigidBody())
                ball->hitFloor();
            else {
                AvatarNode *player = fromRigidbody<AvatarNode>(bodyB, m_players);
                if (player) {
                    player->boop();
                    ball->hitPlayer();
                }
            }
       } else {
            ball = fromRigidbody<DodgeballNode>(bodyB, m_dodgeballs);
            if (ball && (bodyA == m_floor->getRigidBody()))
                ball->onCollision();
        }*/
    }
}

bool DodgeballEngine::checkCollisions(btRigidBody *bodyA, btRigidBody *bodyB) {
    /* Check half cases for body a and body b */
    DodgeballNode *ball = fromRigidbody<DodgeballNode>(bodyA, m_dodgeballs);
    if (ball) {
        if (bodyB == m_floor->getRigidBody()) {
            ball->hitFloor();
            return true;
        } else {
            //std::cout << "CHECK PLAYER" << std::endl;
            AvatarNode *player = fromRigidbody<AvatarNode>(bodyB, m_players);
            if (player) {
                player->boop();
                ball->hitPlayer();
                return true;
            }
        }
   }
   return false;
}

void DodgeballEngine::setupScene() {
    /* Shift the camera back */
    m_camera->setPosition(irr::core::vector3df(0.0, 1.6, 4.0));
    m_camera->bindTargetAndRotation(true);
    m_camera->setTarget(irr::core::vector3df(0.0, 0.0, 0.0));
    m_camera->setUpVector(irr::core::vector3df(0.0, 1.0, 0.0));
    
    /* Build the court and add players */
    buildCourt();
    loadPlayers();

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
    setState(DGBENG_RUNNING);
    double prevTime = m_timer->getTime();

    /* TODO: Maybe consider putting this loop at the top main()-level ? */
    /* doing so would enforce the "statefulness" of the engine? Also a
     * better coding practice? */
    while(m_device->run() && m_driver && !m_quit) {
        if (m_device->isWindowActive()) {
            /* update physics stuff */
            updatePhysics((m_timer->getTime()-prevTime) * 0.001);
            prevTime = m_timer->getTime();

            /* then deal with irrlicht */
            m_driver->beginScene(true, true, irr::video::SColor(255, 0, 0, 255));
            m_scenemgr->drawAll();
            updateHUD();
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

void DodgeballEngine::updateHUD() {
    /* Render images and health etc... */
    //m_driver->enableMaterial2D();

    /* Draw crosshairs... */
    irr::core::rect<irr::s32> windowSize = m_driver->getViewPort();
    m_driver->draw2DImage(
        m_driver->getTexture("models/crosshair.png"),
        irr::core::position2d<irr::s32>((windowSize.getWidth()/2)-64, (windowSize.getHeight()/2)-64),
        irr::core::rect<irr::s32>(0, 0, 128, 128), 0, irr::video::SColor(255, 255, 255, 255), true);

    //m_driver->enableMaterial2D(false);
}

void DodgeballEngine::moveMe(double dx, double dy, double dz) {
    /* Move me!
     * We have to be careful here, since we exist in both Bullet's and Irrlicht's world,
     * we need to make sure that we move both the bullet rigidbody and the scenenode.
     */
    irr::core::vector3df camLoc = m_camera->getAbsolutePosition();
    camLoc.X += dx;
    camLoc.Y += dy;
    camLoc.Z += dz;
    m_camera->setPosition(camLoc);
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
    } else if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        if (event.KeyInput.PressedDown) {
            /* do the yucky case statements! */
            switch (event.KeyInput.Key) {
                case irr::KEY_KEY_W:
                    moveMe(0.0, 0.0, -0.5);
                    break;
                case irr::KEY_KEY_A:
                    moveMe(0.5, 0.0, 0.0);
                    break;
                case irr::KEY_KEY_S:
                    moveMe(0.0, 0.0, 0.5);
                    break;
                case irr::KEY_KEY_D:
                    moveMe(-0.5, 0.0, 0.0);
                    break;
                case irr::KEY_KEY_Q:
                    m_quit = true;
                    break;
                case irr::KEY_ESCAPE:
                    m_quit = true;
                    break;
                default:
                    break;
            }
        }
    }
    return false;
}

void DodgeballEngine::trackCamera(int x, int y) {
    /* rotate camera... */
    irr::core::rect<irr::s32> windowSize = m_driver->getViewPort();
    int centerx = x - (windowSize.getWidth()/2);
    int centery = y - (windowSize.getHeight()/2);
    double xproportion = centerx / (windowSize.getWidth()/2.0);
    double yproportion = centery / (windowSize.getHeight()/2.0);
    irr::core::vector3df forward(
        -sin(xproportion*M_PI/2.0), 0.0, -cos(xproportion*M_PI/2.0));
    forward = forward.normalize();
    irr::core::vector3df right = forward.crossProduct(m_camera->getUpVector());
    irr::core::vector3df target = RodriguesRotate(
        forward, right.normalize(), -yproportion*M_PI/2.0);
    m_camera->setTarget(m_camera->getPosition() + target);

    /* Prevent the user from mousing off the screen... */
    //irr::f32 newX = 1.0 * x / m_windowWidth;
    //irr::f32 newY = 1.0 * y / m_windowHeight;
    //if (newX >= 0.99)
    //    newX = 0.99;
    //else if (newX <= 0.01)
    //    newX = 0.01;
    //m_cursorCtrl->setPosition(newX, newY);
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

