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

	// ミサイルと海底が衝突した場合
	} else if ((tagA == TAG_MISSILE && tagB == TAG_SEABED) || (tagA == TAG_SEABED && tagB == TAG_MISSILE)) {
		// ミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);

    // ミサイルと画面端が衝突した場合
	} else if ((tagA == TAG_MISSILE && tagB == TAG_BORDERLINE) || (tagA == TAG_BORDERLINE && tagB == TAG_MISSILE)){
		// ミサイルが消える
        tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
        		: actorB->setTag(TAG_REMOVE_MISSILE);

    // 自機とミサイルが衝突した場合
	} else if((tagA == TAG_MISSILE && tagB == TAG_PLAYER_UNIT) || (tagA == TAG_PLAYER_UNIT && tagB == TAG_MISSILE))  {
		// 自機のライフが1減る
        tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_PLAYER)
        		: actorB->setTag(TAG_COLLISION_PLAYER);

    // 駆逐艦とミサイル、またはミサイルと駆逐艦が衝突した場合
	}else if ((tagA == TAG_DESTROYER_UNIT && tagB == TAG_MISSILE) || (tagA == TAG_MISSILE && tagB == TAG_DESTROYER_UNIT)) {
		// ミサイルを消してスコアを加算
		tagA == TAG_DESTROYER_UNIT ? actorA->setTag(TAG_COLLISION_DESTROYER)
				: actorB->setTag(TAG_COLLISION_DESTROYER);

	// 潜水艦とミサイル、またはミサイルと潜水艦が衝突した場合
	} else if ((tagA == TAG_SUBMARINE_UNIT && tagB == TAG_MISSILE) || (tagA == TAG_MISSILE && tagB == TAG_SUBMARINE_UNIT)) {
		// ミサイルを消してスコアを加算
        tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_SUBMARINE)
        		: actorB->setTag(TAG_COLLISION_SUBMARINE);
	}
}
