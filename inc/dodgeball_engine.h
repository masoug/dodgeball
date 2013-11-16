/*
 * The Dogdeball class is responsible for handling the main Irrlicht and Bullet integration. It contains the Bullet world and
 * the Irrlicht devices (and fellow pointers) that manage the entire system. This class also manages the building of the scene
 * and simulation stepping. In short, this class glues the entrie game together :)
 *
 * This class will become a statemachine that handles the game states: (INIT, IN_GAME, FATAL_ERROR, etc...)
 */

#ifndef DODGEBALL_H_
#define DODGEBALL_H_

#include <vector>
#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>

#include "dynamic_object.h"
#include "state_machine.h"
#include "networking.h"

/* Port to operate on */
#define DODGEBALL_NET_PORT 4538

irr::core::vector3df RodriguesRotate(
    irr::core::vector3df input, irr::core::vector3df axis, double rad);

template<typename Type> Type* fromRigidbody(btRigidBody *body, std::vector<Type*> input);

class DodgeballEngine : public irr::IEventReceiver, public StateMachineBase {
    public:
        DodgeballEngine(
            unsigned int width, unsigned int height, bool serverMode);
        ~DodgeballEngine();

        void setupScene();
        void buildCourt();
        void clearScene();
        bool setupNetwork(std::string server);
        bool registerUser(std::string username);
        void run();
        void updatePhysics(double timestep);
        void updateHUD();

        /* Event handling */
        virtual bool OnEvent(const irr::SEvent& event);

    private:
        /* Administrative stuff */
        bool m_quit, m_serverMode;
        bool m_keyStates[irr::KEY_KEY_CODES_COUNT];
        void handleKeyEvents();

        /* network systems */
        NetBase     *m_netEngine = NULL;
        void handleFieldEvents();

        /* window dimensions */
        unsigned int    m_windowWidth;
        unsigned int    m_windowHeight;

        /* Scene elements */
        WallNode    *m_floor = NULL;
        WallNode    *m_nWall = NULL;
        WallNode    *m_sWall = NULL;
        WallNode    *m_eWall = NULL;
        WallNode    *m_wWall = NULL;
        WallNode    *m_ceiling = NULL;

        /* Irrlicht stuph... */
        irr::IrrlichtDevice             *m_device       = NULL;
        irr::video::IVideoDriver        *m_driver       = NULL;
        irr::scene::ISceneManager       *m_scenemgr     = NULL;
        irr::scene::ICameraSceneNode    *m_camera       = NULL;
        irr::ITimer                     *m_timer        = NULL;
        irr::gui::ICursorControl        *m_cursorCtrl   = NULL;
        irr::gui::IGUIEnvironment       *m_guiEnv       = NULL;

        /* GUI elements */
        irr::gui::IGUIWindow        *m_guiConnectWindow     = NULL;
        irr::gui::IGUIButton        *m_guiConnectButton     = NULL;
        irr::gui::IGUIEditBox       *m_guiServerAddrField   = NULL;
        irr::gui::IGUIStaticText    *m_guiStatusText        = NULL;

        /* Bullet stuph... */
        btBroadphaseInterface               *m_broadphase       = NULL;
        btDefaultCollisionConfiguration     *m_collisionConfig  = NULL;
        btCollisionDispatcher               *m_dispatcher       = NULL;
        btSequentialImpulseConstraintSolver *m_solver           = NULL;
        btDiscreteDynamicsWorld             *m_dynamicsWorld    = NULL;
        
        /* Players */
        void loadPlayers();
        AvatarNode                  *m_thisPlayer = NULL;
        std::vector<AvatarNode*>     m_players;

        /* Scene elements */
        bool checkCollisions(btRigidBody *bodyA, btRigidBody *bodyB);
        DodgeballNode* addDodgeball(btVector3 pos, unsigned int ballID);
        void fireDodgeball();
        void trackCamera(int x, int y);
        void handleCollisions();
        std::vector<DodgeballNode*>        m_dodgeballs;
};

#endif
