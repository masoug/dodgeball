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

irr::core::vector3df RodriguesRotate(
    irr::core::vector3df input, irr::core::vector3df axis, double rad);

class DodgeballEngine : public irr::IEventReceiver {
    public:
        DodgeballEngine(unsigned int width, unsigned int height);
        ~DodgeballEngine();

        void setupScene();
        void buildCourt();
        void clearScene();
        void run();
        void updatePhysics(double timestep);

        /* Event handling */
        virtual bool OnEvent(const irr::SEvent& event);

    private:
        /* window dimensions */
        unsigned int    m_windowWidth;
        unsigned int    m_windowHeight;

        /* Scene elements */
        WallNode    *m_floor;
        WallNode    *m_nWall;
        WallNode    *m_sWall;
        WallNode    *m_eWall;
        WallNode    *m_wWall;
        WallNode    *m_ceiling;

        /* Irrlicht stuph... */
        irr::IrrlichtDevice             *m_device;
        irr::video::IVideoDriver        *m_driver;
        irr::scene::ISceneManager       *m_scenemgr;
        irr::scene::ICameraSceneNode    *m_camera;
        irr::ITimer                     *m_timer;

        /* Bullet stuph... */
        btBroadphaseInterface               *m_broadphase;
        btDefaultCollisionConfiguration     *m_collisionConfig;
        btCollisionDispatcher               *m_dispatcher;
        btSequentialImpulseConstraintSolver *m_solver;
        btDiscreteDynamicsWorld             *m_dynamicsWorld;

        /* Scene elements */
        DodgeballNode* addDodgeball(btVector3 pos);
        void fireDodgeball();
        void trackCamera(int x, int y);
        std::vector<DodgeballNode* >        m_dodgeballs;
};

#endif
