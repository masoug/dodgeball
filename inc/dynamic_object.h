/* Base class for all objects that move around during the game. In other
 * words, this class glues irrlicht's node and Bullet's rigidbody into a
 * single class.
 * TODO: Remember to convert this to a state machine!
 */

#ifndef DYNAMIC_OBJECT_H_
#define DYNAMIC_OBJECT_H_

#include <string>

#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>

#include "state_machine.h"

/* All units metric, mass in kg */
#define DODGEBALL_RADIUS    0.1524
#define DODGEBALL_MASS      0.680389
#define HUMAN_RADIUS        0.2286
#define HUMAN_HEIGHT        1.711

class DynamicObject : public StateMachineBase {
    public:
        DynamicObject();
        virtual ~DynamicObject();

        virtual void applyTransform();
                //irr::scene::ISceneNode* getSceneNode() const;
        btRigidBody* getRigidBody() const;

    protected:
        irr::scene::ISceneNode  *m_sceneNode        = NULL;
        btRigidBody             *m_rigidBody        = NULL;
        btMotionState           *m_motionState      = NULL;
        btCollisionShape        *m_collisionShape   = NULL;
};

class DodgeballNode : public DynamicObject {
    public:
        DodgeballNode(
            irr::IrrlichtDevice *device,
            btDiscreteDynamicsWorld *world,
            btVector3 initPos);
        virtual ~DodgeballNode();

        virtual void hitFloor();
        virtual void hitPlayer();

        void throwBall(btVector3 impulse);
};

class WallNode : public DynamicObject {
    public:
        WallNode(
            irr::IrrlichtDevice *device,
            btDiscreteDynamicsWorld *world,
            btVector3 initPos,
            irr::core::vector3df scale);
        virtual ~WallNode();
};

const std::string PLAYER_MODEL_PATHS[] = {
    "models/players/hotdog/hotdog.b3d",
    "models/players/banana/banana.b3d",
    "models/players/blackhat/blackhat.b3d",
    "models/players/phil/phil.b3d",
    "models/players/popsicle/popsicle.b3d",
    "models/players/snowman/snowman.b3d"
};

class AvatarNode : public DynamicObject {
    public:
        enum TeamType {
            RED, BLUE
        };

        AvatarNode(
            irr::IrrlichtDevice *device,
            btDiscreteDynamicsWorld *world,
            btVector3 initPos,
            TeamType team,
            std::string fileName);
        virtual ~AvatarNode();
        
        virtual void boop(); // hit by ball
        void setTargetVelocity(btVector3 vel);
        void setLateral(double lat);
        void setForward(double forward);
        void stop();
        virtual void applyTransform();
        virtual void applyControlLoop();

    protected:
        irr::scene::IAnimatedMesh  *m_animatedMesh = NULL;
        TeamType                    m_teamType;
        btVector3                   m_targetVel;
};

class CameraAvatar : public AvatarNode {
    public:
        CameraAvatar(
            irr::IrrlichtDevice *device,
            btDiscreteDynamicsWorld *world,
            irr::scene::ICameraSceneNode *camera,
            btVector3 initPos,
            TeamType team);
        virtual ~CameraAvatar();
        virtual void applyTransform();
};

#endif
