#ifndef __PHYSICS_SPRITE_H__
#define __PHYSICS_SPRITE_H__

#include "cocos2d.h"
#include "box2d/Box2D.h"

USING_NS_CC;

class PhysicsSprite : public CCSprite {
protected:
    b2Body* m_pBody;
    int hp;
//    int kTag;
public:
    PhysicsSprite();
    PhysicsSprite(int hp);
    void setPhysicsBody(b2Body* body);
//   void setTag(int tag);
//    int getTag();
    b2Body* getPhysicsBody();
    virtual bool isDirty(void);
    virtual CCAffineTransform nodeToParentTransform(void);

	int getHp() const {
		return hp;
	}

	void setHp(int hp) {
		this->hp = hp;
	}
};

#endif // __PHYSICS_SPRITE_H__
