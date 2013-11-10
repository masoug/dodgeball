/*
 * Implements the DodgeballEngine class.
 */

#include <iostream>
#include <math.h>
#include <unistd.h>

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

DodgeballEngine::DodgeballEngine(
    unsigned int width, unsigned int height, bool serverMode) :
    m_quit(false), m_serverMode(serverMode),
    m_windowWidth(width), m_windowHeight(height)
{
    /* Initialize the keycodes */
    for (int i = 0; i < irr::KEY_KEY_CODES_COUNT; i++)
        m_keyStates[i] = false;

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
    m_guiEnv = m_device->getGUIEnvironment();
    
    /* TODO: Sometime in the future make a better FPS camera */
    m_camera = m_scenemgr->addCameraSceneNodeFPS(0, 100, 0.001);
    m_camera->setNearValue((irr::f32)0.001);
    m_camera->bindTargetAndRotation(true);
    m_camera->setPosition(irr::core::vector3df(0.0, 7.0, 0.0));
    m_camera->setTarget(irr::core::vector3df(0.0, 0.0, 0.0));
    m_camera->setUpVector(irr::core::vector3df(0.0, 1.0, 0.0));

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

    /* setup input systems */
    m_device->setEventReceiver(this);

    /* Setup network engine */
    if (serverMode)
        m_netEngine = new DodgeballServer(DODGEBALL_NET_PORT);
    else
        m_netEngine = new DodgeballClient();

    setState(INIT_STATE);
}

DodgeballNode* DodgeballEngine::addDodgeball(btVector3 pos) {
    DodgeballNode *ball = new DodgeballNode(m_device, m_dynamicsWorld, pos);
    m_dodgeballs.push_back(ball);
    return ball;
}

void DodgeballEngine::fireDodgeball() {
    if (m_serverMode)
        return;

    /* get camera transform */
    irr::core::vector3df pos = m_camera->getAbsolutePosition();
    irr::core::vector3df target = m_camera->getTarget();
    
    /* get impulse vector */
    irr::core::vector3df imp = target - pos;
    imp = imp.normalize();
    btVector3 ballPos(pos.X, pos.Y, pos.Z);
    ballPos += btVector3(imp.X, imp.Y, imp.Z) * (0.7);
    imp *= 8.0; // applying an impulse simulates throwing a ball 

    /* send ball spawn signal */
    ((DodgeballClient*)m_netEngine)->sendSpawnBall(
        ballPos.x(), ballPos.y(), ballPos.z(),
        imp.X, imp.Y, imp.Z);
    
    /* Create ball and throw it.
     * we add the ball at the target because we don't want it to collide with myself. */
    DodgeballNode *ball = addDodgeball(ballPos);
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

        if (!checkCollisions(bodyA, bodyB))
            checkCollisions(bodyB, bodyA);
    }
}

bool DodgeballEngine::checkCollisions(
    btRigidBody *bodyA, btRigidBody *bodyB)
{
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
                if (ball->getState() == DGDBL_ACTIVE)
                    player->boop();
                ball->hitPlayer();
                return true;
            }
        }
   }
   return false;
}

void DodgeballEngine::setupScene() {
    /* Build the court and add players */
    buildCourt();

    /* load players accordingly */
    if (m_serverMode) {
        /* server already has game state/player roster
         * thus it just proceeds to build the scene graph
         */
        std::cout << "Building game state..." << std::endl;
        /* build game state */
    } else {
        /* Client must wait for all other players to join and 
         * for the server to send the game state to everyone
         */
        std::cout << "\n\nWaiting for all other players to join\n"
            << "and for server to send initial game state." << std::endl;
        
        DodgeballClient *client = (DodgeballClient*)m_netEngine;
        while (!client->waitForInitialState())
            std::cerr << "Server derped out. Trying again..." << std::endl;
    }
    loadPlayers();
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

void DodgeballEngine::loadPlayers() {
    /* Create player */

    /* build scene graph based on game state */
    unsigned int myID = -1;
    if (!m_serverMode)
        myID = ((DodgeballClient*)m_netEngine)->getPlayerID();
    NetProtocol::GameState gstate = m_netEngine->getGameState();
    for (int i = 0; i < gstate.player_state_size(); i++) {
        NetProtocol::PlayerState player = gstate.player_state(i);
        
        if (!m_serverMode && player.id() == myID) {
            /* this is me! */
             m_thisPlayer = new CameraAvatar(
                m_device, m_dynamicsWorld, m_camera,
                btVector3(
                    player.position().x(),
                    player.position().y(),
                    player.position().z()),
                (AvatarNode::TeamType)player.team_type(),
                myID);
            m_players.push_back(m_thisPlayer);
        } else {
            /* normal player */
            m_players.push_back(new AvatarNode(
                m_device, m_dynamicsWorld,
                btVector3(
                    player.position().x(),
                    player.position().y(),
                    player.position().z()),
                (AvatarNode::TeamType)player.team_type(),
                PLAYER_MODEL_PATHS[player.avatar()], player.id()));
        }
    }
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

bool DodgeballEngine::setupNetwork(std::string server) {
    std::cout << "\n\nSETUP NETWORK\n\n" << std::endl;
    if (m_serverMode) {
        /* wait for user to start game */
        std::cout << "\n\n\nWaiting for you to start game:" << std::endl;
        std::string response;
        std::cin >> response;
        while (response != "start") { std::cin >> response; }
        ((DodgeballServer*)m_netEngine)->sendGameState();
        return true;
    } else {
        /* connect to server */
        DodgeballClient *client = (DodgeballClient*)m_netEngine;
        std::cout 
            << "Connecting to " << server 
            << " at port " << DODGEBALL_NET_PORT
            << std::endl;
        if (client->connect(server, DODGEBALL_NET_PORT))
            return client->waitForConnection(3000);
    }
    return false;
}

bool DodgeballEngine::registerUser(std::string username) {
    if (m_serverMode)
        return false;

    DodgeballClient *client = (DodgeballClient*)m_netEngine;
    client->playerRequest(username);
    return client->waitForConfirmation(3000);
}

void DodgeballEngine::run() {
    setState(DGBENG_RUNNING);
    double prevTime = m_timer->getTime();

    /* TODO: Maybe consider putting this loop at the top main()-level ? */
    /* doing so would enforce the "statefulness" of the engine? Also a
     * better coding practice? */
    while(m_device->run() && m_driver && !m_quit) {
        /* handle key events */
        handleKeyEvents();

        /* handle events from network */
        handleFieldEvents();

        /* update physics stuff */
        updatePhysics((m_timer->getTime()-prevTime) * 0.001);
        prevTime = m_timer->getTime();

        /* then deal with irrlicht */
        m_driver->beginScene(true, true, irr::video::SColor(255, 0, 0, 255));
        m_scenemgr->drawAll();
        updateHUD();
        m_driver->endScene();
    }
}

void DodgeballEngine::updatePhysics(double timestep) {
    /* update physics */
    m_dynamicsWorld->stepSimulation(timestep, 60);
    
    /* apply to models */
    for (unsigned int i = 0; i < m_dodgeballs.size(); i++)
        m_dodgeballs[i]->applyTransform();
    for (unsigned int i = 0; i < m_players.size(); i++)
        m_players[i]->applyTransform();

    /* Handle the collisions */
    handleCollisions();
}

void DodgeballEngine::updateHUD() {
    /* Render images and health etc... */
    //m_driver->enableMaterial2D();
    
    if (!m_serverMode) {
        /* Draw crosshairs... */
        irr::core::rect<irr::s32> windowSize = m_driver->getViewPort();
        m_driver->draw2DImage(
            m_driver->getTexture("models/crosshair.png"),
            irr::core::position2d<irr::s32>((windowSize.getWidth()/2)-64, (windowSize.getHeight()/2)-64),
            irr::core::rect<irr::s32>(0, 0, 128, 128), 0, irr::video::SColor(255, 255, 255, 255), true);
        
        /* Draw possessions */
        for (int ballsRemaining = 0; ballsRemaining < 3; ballsRemaining++)
            m_driver->draw2DImage(
                m_driver->getTexture("models/logo-blue.png"),
                irr::core::position2d<irr::s32>(70*ballsRemaining+20, 20),
                irr::core::rect<irr::s32>(0, 0, 64, 64), 0, irr::video::SColor(255, 255, 255, 255), true);
    }

    //m_driver->enableMaterial2D(false);
}

bool DodgeballEngine::OnEvent(const irr::SEvent& event) {
    /* handle events */
    if (event.EventType == irr::EET_MOUSE_INPUT_EVENT) {
        switch(event.MouseInput.Event)
        {
        case irr::EMIE_LMOUSE_PRESSED_DOWN:
                fireDodgeball();
                return true;
            break;
        case irr::EMIE_MOUSE_MOVED:
                trackCamera(event.MouseInput.X, event.MouseInput.Y);
                return true;
            break;
        default:
            return false;
            break;
        }
    } else if (event.EventType == irr::EET_KEY_INPUT_EVENT) {
        m_keyStates[event.KeyInput.Key] = event.KeyInput.PressedDown;
    } 
    return false;
}

void DodgeballEngine::handleKeyEvents() {
    /* handle them key events */
    if (!m_serverMode) {
        if (m_keyStates[irr::KEY_KEY_W]) 
            m_thisPlayer->setForward(-3.0);
        else if (m_keyStates[irr::KEY_KEY_S])
            m_thisPlayer->setForward(3.0);
        else
            m_thisPlayer->setForward(0.0);

        if (m_keyStates[irr::KEY_KEY_A])
            m_thisPlayer->setLateral(3.0);
        else if (m_keyStates[irr::KEY_KEY_D])
            m_thisPlayer->setLateral(-3.0);
        else
            m_thisPlayer->setLateral(0.0);

        /* send player events if there is a 
         * significant change in the user's 
         * motion */
        static btVector3 prevTargetVel;
        btVector3 currVel = m_thisPlayer->getTargetVel();
        if (currVel.distance(prevTargetVel) >= 0.5) {
            ((DodgeballClient*)m_netEngine)->sendPlayerEvent(
                m_thisPlayer->getPlayerID(),
                (double)currVel.x(), (double)currVel.y(), (double)currVel.z());
        }
        prevTargetVel = currVel;
    }

    if (m_keyStates[irr::KEY_KEY_Q] ||m_keyStates[irr::KEY_ESCAPE]) {
        m_quit = true;
        /* Stop network engine */
        m_netEngine->gracefulStop();
    }
}

void DodgeballEngine::handleFieldEvents() {
    /* go through all queued field events */
    while (m_netEngine->eventSize() > 0) {
        NetProtocol::FieldEvent *fieldEvent = 
            m_netEngine->releaseLatestFieldEvent();
        /* and apply event to physics engine */

        switch (fieldEvent->type()) {
            case NetProtocol::FieldEvent::SPAWN_BALL:
            {
                NetProtocol::SpawnBall *sball = fieldEvent->release_spawn_ball();
                /* fire dodgeball */
                DodgeballNode *ball = addDodgeball(
                    btVector3(
                        sball->position().x(),
                        sball->position().y(),
                        sball->position().z()
                    ));
                ball->throwBall(btVector3(
                        sball->impulse().x(),
                        sball->impulse().y(),
                        sball->impulse().z()
                    ));
                free(sball);
            }
                break;
            case NetProtocol::FieldEvent::PLAYER_EVENT:
            {
                NetProtocol::PlayerEvent *pevent = 
                    fieldEvent->release_player_event();
                /* Find the corresponding player and
                 * apply the event to it. */
                for (unsigned int i = 0; i < m_players.size(); i++) {
                    if (m_players[i]->getPlayerID() == pevent->id()) {
                        m_players[i]->setTargetVelocity(
                            pevent->targetvelocity().x(),
                            pevent->targetvelocity().y(),
                            pevent->targetvelocity().z());
                    }
                }
                free(pevent);
            }
            default:
                break;
        }
        /* delete field event because we own the the field
         * event.
         */
        free(fieldEvent);
    }
}

void DodgeballEngine::trackCamera(int x, int y) {
    if (m_serverMode)
        return;
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

