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

	// データが2つ揃っていない場合はそのまま元の処理に戻る
	if (!actorA || !actorB){
		return;

	/* ***********************
	 *
	 * 自機ミサイルの衝突判定
	 *
	 * ***********************/

	// 自機ミサイルと自機もしくは自機と自機ミサイルの衝突
	}else if((tagA == TAG_MISSILE && tagB == TAG_PLAYER_UNIT)
			|| (tagA == TAG_PLAYER_UNIT && tagB == TAG_MISSILE)){
		//ミサイルタグにミサイル除去タグをセット
		tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
				: actorB->setTag(TAG_EXPLOSION_MISSILE);

	// 自機ミサイルと海底もしくは海底と自機ミサイルの衝突
	} else if ((tagA == TAG_MISSILE && tagB == TAG_SEABED)
			|| (tagA == TAG_SEABED && tagB == TAG_MISSILE)){
		// ミサイルタグにミサイル除去タグをセット
		tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
				: actorB->setTag(TAG_REMOVE_MISSILE);

	// 自機ミサイルと駆逐艦もしくは駆逐艦と自機ミサイルの衝突
	}else if ((tagA == TAG_MISSILE && tagB == TAG_DESTROYER_UNIT)
			|| (tagA == TAG_DESTROYER_UNIT && tagB == TAG_MISSILE)) {
		// ミサイルタグに駆逐艦衝突タグをセット
		tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_DESTROYER)
				: actorB->setTag(TAG_COLLISION_DESTROYER);

	// 自機ミサイルと敵潜水艦もしくは敵潜水艦と自機ミサイルの衝突
	} else if ((tagA == TAG_MISSILE && tagB == TAG_SUBMARINE_UNIT)
			|| (tagA == TAG_SUBMARINE_UNIT && tagB == TAG_MISSILE)) {
		// ミサイルタグに潜水艦衝突タグをセット
		tagA == TAG_MISSILE ? actorA->setTag(TAG_COLLISION_SUBMARINE)
				: actorB->setTag(TAG_COLLISION_SUBMARINE);

	/* ***********************
	 *
	 * 敵ミサイルの衝突判定
	 *
	 * ***********************/

	// 敵機ミサイルと海底もしくは海底と敵機ミサイルの衝突
	} else if ((tagA == TAG_MISSILE_ENEMY && tagB == TAG_SEABED)
			|| (tagA == TAG_SEABED && tagB == TAG_MISSILE_ENEMY)){
		// 敵ミサイルタグにミサイル除去タグをセット
		tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_REMOVE_MISSILE)
				: actorB->setTag(TAG_REMOVE_MISSILE);

	// 敵機ミサイルと自機ユニットもしくは自機ユニットと敵機ミサイルの衝突
	} else if((tagA== TAG_MISSILE_ENEMY && tagB == TAG_PLAYER_UNIT)
			|| (tagA == TAG_PLAYER_UNIT && tagB == TAG_MISSILE_ENEMY)) {
		// ミサイルタグに自機衝突タグをセット
		tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_COLLISION_PLAYER)
				: actorB->setTag(TAG_COLLISION_PLAYER);

	// 敵機ミサイルと敵潜水艦もしくは敵機潜水艦と敵機ミサイルの衝突
	} else if ((tagA == TAG_MISSILE_ENEMY && tagB == TAG_SUBMARINE_UNIT)
			|| (tagA == TAG_SUBMARINE_UNIT && tagB == TAG_MISSILE_ENEMY)){
		// 敵ミサイルタグにミサイル除去タグをセット
		tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_EXPLOSION_MISSILE)
				: actorB->setTag(TAG_EXPLOSION_MISSILE);


	/* ***********************
	 *
	 * 自ミサイルもしくは敵ミサイルと
	 * ボーダーラインの衝突判定
	 *
	 * ***********************/

	// 自機ミサイルとボーダーライン(画面端)もしくはボーダーラインと自機ミサイルの衝突
	} else if ((tagA == TAG_MISSILE && tagB == TAG_BORDERLINE)
			|| (tagA == TAG_BORDERLINE && tagB == TAG_MISSILE)){
		// 自ミサイルタグにミサイル除去タグをセット
		tagA == TAG_MISSILE ? actorA->setTag(TAG_REMOVE_MISSILE)
				: actorB->setTag(TAG_REMOVE_MISSILE);

	// 敵機ミサイルとボーダーラインもしくはボーダーラインと敵機ミサイルの衝突
	} else if ((tagA == TAG_MISSILE_ENEMY && tagB == TAG_BORDERLINE)
			|| (tagA == TAG_BORDERLINE && tagB == TAG_MISSILE_ENEMY)){
		// 敵ミサイルタグにミサイル除去タグをセット
		tagA == TAG_MISSILE_ENEMY ? actorA->setTag(TAG_REMOVE_MISSILE)
				: actorB->setTag(TAG_REMOVE_MISSILE);


	/* ***********************
	 *
	 * ミサイル同士の衝突判定
	 *
	 * ***********************/

	} else if (((tagA == TAG_MISSILE && tagB == TAG_MISSILE_ENEMY)			// 自ミサイルと敵ミサイル
			|| (tagA == TAG_MISSILE_ENEMY && tagB == TAG_MISSILE))			// 敵ミサイルと自ミサイル
			|| (tagA == TAG_MISSILE && tagB == TAG_MISSILE)					// 自ミサイルと自ミサイル
			|| (tagA == TAG_MISSILE_ENEMY && tagB == TAG_MISSILE_ENEMY)){	// 敵ミサイルと敵ミサイル
		// 両タグにミサイル除去タグをセット
		actorA->setTag(TAG_EXPLOSION_MISSILE);
		actorB->setTag(TAG_EXPLOSION_MISSILE);


	/* ***********************
	 *
	 * 機体同士の衝突判定
	 *
	 * ***********************/

	 } else if ((tagA == TAG_PLAYER_UNIT && tagB == TAG_SUBMARINE_UNIT)
			 || (tagA == TAG_SUBMARINE_UNIT && tagB == TAG_PLAYER_UNIT)){		// 自機と敵潜水艦
	 		// 敵ミサイルと敵ミサイル
	 		// 両タグにミサイル除去タグをセット
			tagA == TAG_PLAYER_UNIT ? actorA->setTag(TAG_DEFEAT_PLAYER)
					: actorB->setTag(TAG_DEFEAT_PLAYER);

	 			}

}
