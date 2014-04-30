#include "GamePhysicsContactListener.h"
#include "Config.h"
#include "GameScene.h"

// 衝突判定を定義するためのコンストラクタ？
GamePhysicsContactListener::GamePhysicsContactListener(CCObject* target) {
	m_target = target;
}
// 物理構造を持つふたつのオブジェクトが衝突した時に呼ばれる関数
void GamePhysicsContactListener::BeginContact(b2Contact* contact) {
	// 衝突した2つのオブジェクトのデータを取得
	CCNode* actorA = (CCNode*)contact->GetFixtureA()->GetBody()->GetUserData();
	CCNode* actorB = (CCNode*)contact->GetFixtureB()->GetBody()->GetUserData();

	// 衝突した2つのオブジェクトに値するタグを取得
	int tagA = actorA->getTag();
	int tagB = actorB->getTag();

	if (!actorA || !actorB){										// データが2つ揃っていない場合はそのまま戻る
		return;
	} else if ((tagA == TAG_MISSILE && tagB == TAG_SEABED) || (tagA == TAG_SEABED && tagB == TAG_MISSILE)){
		// ミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);


	} else if ((tagA == TAG_MISSILE && tagB == TAG_BORDERLINE) || (tagA == TAG_BORDERLINE && tagB == TAG_MISSILE)){
		// ミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);


	} else if ((tagA == TAG_MISSILE_ENEMY && tagB == TAG_BORDERLINE) || (tagA == TAG_BORDERLINE && tagB == TAG_MISSILE_ENEMY)){
		// ミサイルが消える
        tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);


	} else if ((tagA == TAG_MISSILE_ENEMY && tagB == TAG_SEABED) || (tagA == TAG_SEABED && tagB == TAG_MISSILE_ENEMY)){
		// ミサイルが消える
        tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);


	} else if ((tagA == TAG_MISSILE_ENEMY && tagB == TAG_SUBMARINE_UNIT) || (tagA == TAG_SUBMARINE_UNIT && tagB == TAG_MISSILE_ENEMY)){
		// ミサイルが消える
        tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);


	} else if((tagA== TAG_MISSILE_ENEMY && tagB == TAG_PLAYER_UNIT) || (tagA == TAG_PLAYER_UNIT && tagB == TAG_MISSILE_ENEMY)) {
		// 自機に衝突したミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_PLAYER)
        		: actorB->setTag(TAG_COLLISION_PLAYER);


	}else if ((tagA == TAG_MISSILE && tagB == TAG_DESTROYER_UNIT) || (tagA == TAG_DESTROYER_UNIT && tagB == TAG_MISSILE)) {
		// 自機に衝突したミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_DESTROYER)
        		: actorB->setTag(TAG_COLLISION_DESTROYER);


	} else if ((tagA == TAG_MISSILE && tagB == TAG_SUBMARINE_UNIT) || (tagA == TAG_SUBMARINE_UNIT && tagB == TAG_MISSILE)) {
		// 敵潜水艦に衝突したミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_SUBMARINE)
        		: actorB->setTag(TAG_COLLISION_SUBMARINE);
	}
}
