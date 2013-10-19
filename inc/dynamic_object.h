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

    private:
        irr::scene::IAnimatedMesh  *m_animatedMesh = NULL;
        TeamType                    m_teamType;
};

#endif
