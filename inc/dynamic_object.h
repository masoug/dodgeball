/* Base class for all objects that move around during the game. In other
 * words, this class glues irrlicht's node and Bullet's rigidbody into a
 * single class.
 * TODO: Remember to convert this to a state machine!
 */

#ifndef DYNAMIC_OBJECT_H_
#define DYNAMIC_OBJECT_H_

#include <irrlicht.h>
#include <btBulletDynamicsCommon.h>

class DynamicObject {
    public:
        DynamicObject();
        virtual ~DynamicObject();

        void applyTransform();
        //irr::scene::ISceneNode* getSceneNode() const;
        //btRigidBody* getRigidBody() const;

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
