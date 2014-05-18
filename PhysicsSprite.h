#ifndef __PHYSICS_SPRITE_H__
#define __PHYSICS_SPRITE_H__

#include "cocos2d.h"
#include "box2d/Box2D.h"

USING_NS_CC;

class PhysicsSprite : public CCSprite {
protected:
	b2Body* m_pBody;
	int hp;
	bool isEnemy;
public:
	PhysicsSprite();
	PhysicsSprite(int hp);
	void setPhysicsBody(b2Body* body);
	b2Body* getPhysicsBody();
	virtual bool isDirty(void);
	virtual CCAffineTransform nodeToParentTransform(void);
	bool getIsEnemy() {
		return isEnemy;
	}
	void setIsEnemy(bool isEnemy) {
		this->isEnemy;
	}
	int getHp() const {
		return hp;
	}

	void setHp(int hp) {
		this->hp = hp;
	}
};

#endif // __PHYSICS_SPRITE_H__
