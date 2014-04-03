#ifndef __PHYSICS_SPRITE_H__
#define __PHYSICS_SPRITE_H__

#include "cocos2d.h"
#include "box2d/Box2D.h"

USING_NS_CC;

class PhysicsSprite : public CCSprite {
protected:
    b2Body* m_pBody;

public:
    PhysicsSprite();
    void setPhysicsBody(b2Body* body);

    virtual bool isDirty(void);
    virtual CCAffineTransform nodeToParentTransform(void);
};

#endif // __PHYSICS_SPRITE_H__
