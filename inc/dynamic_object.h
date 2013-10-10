/* Base class for all objects that move around during the game. In other
 * words, this class glues irrlicht's node and Bullet's rigidbody into a
 * single class.
 * TODO: Remember to convert this to a state machine!
 */

#ifndef DYNAMIC_OBJECT_H_
#define DYNAMIC_OBJECT_H_

#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>

#include "state_machine.h"

class DynamicObject : public StateMachineBase {
    public:
        DynamicObject();
        virtual ~DynamicObject();

        virtual void applyTransform();
        virtual void onCollision();
        //irr::scene::ISceneNode* getSceneNode() const;
        btRigidBody* getRigidBody() const;

    protected:
        irr::scene::ISceneNode  *m_sceneNode;
        btRigidBody             *m_rigidBody;
        btMotionState           *m_motionState;
        btCollisionShape        *m_collisionShape;
};

class DodgeballNode : public DynamicObject {
    public:
        DodgeballNode(
            irr::IrrlichtDevice *device,
            btDiscreteDynamicsWorld *world,
            btVector3 initPos);
        virtual ~DodgeballNode();

        virtual void onCollision();

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

#endif
