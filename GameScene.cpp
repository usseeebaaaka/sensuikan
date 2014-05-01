#include "GameScene.h"
#include "config.h"
#include "AppMacros.h"
#include "SimpleAudioEngine.h"
#include "Animation.h"

using namespace CocosDenshion;
USING_NS_CC;


// コンストラクタ
GameScene::GameScene()
:arrow_key(4),
 reloadMissile(3),
 enemyUnit_num(2),
 player_VIT(2),
 submarine_VIT(1515),
 destroyer_VIT(1515),
 score_and_Maxplace(50.3),
 dealofScrollSpead(0.2),
 buttons_sum(11),
 playerUnit(NULL),
 lifepoint(3),
 defeatAnimation(8),
 reloadTime(0),
 timeCounter(1260),
 hitAnimation(14){
	scoreText = new CCArray();
	srand((unsigned int)time(NULL));
	unitPhysicsData[kTag_PlayerUnit] = playerUnit;
	unitPhysicsData[kTag_EnemyDestroyer] = enemyDestroyer;
	unitPhysicsData[kTag_EnemySubmarine] = enemySubmarine;
	unitPhysicsData[kTag_Missile] = missile;
}

CCScene* GameScene::scene() {
	CCScene* scene = CCScene::create();			// 背景の雛型となるオブジェクトを生成
	GameScene* layer = GameScene::create();		// 背景に処理を重ねるためのオブジェクトを生成
	scene->addChild(layer);						// 雛型にレイヤーを埋め込む
	return scene;								// 作成した雛型を返却
}

// ゲーム内の部品を生成する関数
bool GameScene::init() {
	if (!CCLayer::init()) {
		return false;														// シーンオブジェクトの生成に失敗したらfalseを返す
	}
	initPhysics();
	createControllerPanel();
	createBackground();
	createScore();
	// 自機を生成
	createUnit(player_VIT, kTag_PlayerUnit, submarine_VIT);
	// 敵駆逐艦を生成
	createUnit(destroyer_VIT % 100, kTag_EnemyDestroyer, destroyer_VIT);
	// 敵潜水艦を生成
	createUnit(submarine_VIT % 100, kTag_EnemySubmarine, submarine_VIT);
	createLifeCounter();
	createKey();
	createLife();
	createRetryButton();
	// ミサイルの準備
	missileBatchNode = CCSpriteBatchNode::create("Missile.png");			// ミサイルの画像をセット
	this->addChild(missileBatchNode);									// ミサイル群をシーンに追加
	bombBatchNode = CCSpriteBatchNode::create("bomb.png");					// 駆逐艦の魚雷画像をセット
	this->addChild(bombBatchNode);											// 魚雷画像群をシーンにセット
	enemyMissileBatchNode = CCSpriteBatchNode::create("submarineMissile.png");		// 敵潜水艦のミサイル画像をセット
	this->addChild(enemyMissileBatchNode);
	// カウントダウン開始
	this->scheduleOnce(schedule_selector(GameScene::showCountdown), 1);

	return true;
}

void GameScene::initPhysics() {
	b2Vec2 gravity;															// 重力の設定値を格納するための変数
	gravity.Set(0, -0.6);													// 重力を設定

	world = new b2World(gravity);											// 重力を持った世界を生成

	mGamePhysicsContactListener = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するための変数を用意
	world->SetContactListener(mGamePhysicsContactListener);				// 衝突判定処理を追加

	//	world2 = new b2World(gravity);											// 重力を持った世界を生成
	//
	//	mGamePhysicsContactListener2 = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するための変数を用意
	//	world2->SetContactListener(mGamePhysicsContactListener2);				// 衝突判定処理を追加
	//
	//	world3 = new b2World(gravity);											// 重力を持った世界を生成
	//
	//	mGamePhysicsContactListener3 = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するための変数を用意
	//	world3->SetContactListener(mGamePhysicsContactListener3);				// 衝突判定処理を追加

}
// 背景および海底を生成
void GameScene::createBackground() {
	/*----- バッググラウンド(背景作成) -----*/
	CCSprite* pBgUnder = CCSprite::create("background.png");	// 背景画像を取得し、オブジェクトに格納
	pBgUnder->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));	// 背景画像の中心を画面の中心にセット
	this->addChild(pBgUnder, kZOrder_Background, kTag_Background);	// 背景画像をシーンに登録

	/*----- 海とコントローラの境界線を作成 -----*/
	CCNode* pSeabed = CCNode::create();	// 海底を実体を持つオブジェクトとして生成
	pSeabed->setTag(kTag_Seabed);	// 海底をタグと関連付ける
	this->addChild(pSeabed);	// 海底をシーンに登録

	b2BodyDef seabedBodyDef;	// 物理構造を持つ海底構造体を生成
	float seabedBottom = pBgUnder->getContentSize().height / 3;	// 海底とコントローラ部の境界座標を設定
	seabedBodyDef.position.Set(0, 0);	// 海底の位置をセット
	seabedBodyDef.userData = pSeabed;	// 海底オブジェクトのデータを構造体に登録

	b2Body* seabedBody = world->CreateBody(&seabedBodyDef);	// 重力世界に海底構造体を登録
	float seabedHeight = pBgUnder->getContentSize().height / 4;	// コントローラ下地の高さを取得

	// コントローラ部とプレイ部を分けるための変数
	b2EdgeShape seabedBox;
	/* 2つの部位の境界線を設定
	 * 一つ目のb2Vec2で左辺のx座標とy座標を
	 * 二つ目のb2Vec2で右辺のx座標とy座標をセット
	 */
	seabedBox.Set(b2Vec2(0, seabedHeight / PTM_RATIO),
			b2Vec2(getWindowSize().width / PTM_RATIO, seabedHeight / PTM_RATIO));
	// 指定した境界線と密度を海底オブジェクトに登録
	seabedBody->CreateFixture(&seabedBox, 0);

	/*----- 左右画面外に行けないよう境界線を作成 -----*/
	CCNode* pBorderline = CCNode::create();	// 境界線を実体を持つオブジェクトとして生成
	pBorderline->setTag(kTag_Borderline);	// 境界線をタグと関連付ける
	this->addChild(pBorderline);	// 境界線をシーンに登録

	b2BodyDef borderlineBodyDef;	// 物理構造を持つ境界線構造体を生成
	borderlineBodyDef.position.Set(0, 0);	// 境界線の位置をセット
	borderlineBodyDef.userData = pBorderline;	// 境界線オブジェクトのデータを構造体に登録

	b2Body* borderlineBody = world->CreateBody(&borderlineBodyDef);	// 重力世界に境界線構造体を登録

	// 敵の移動範囲を制限するための変数
	b2EdgeShape borderlineBox;
	/*----- 画面の左端より奥に行けなくなるように設定 -----*/
	/* 一つ目のb2Vec2で左辺のx座標とy座標を
	 * 二つ目のb2Vec2で右辺のx座標とy座標をセット */
	borderlineBox.Set(b2Vec2(0, pBgUnder->getContentSize().height / PTM_RATIO),
			b2Vec2(0, 0));
	// 指定した境界線と密度を海底オブジェクトに登録
	borderlineBody->CreateFixture(&borderlineBox, seabedHeight);

	/*----- 画面の右端より奥に行けなくなるように設定 -----*/
	/* 一つ目のb2Vec2で左辺のx座標とy座標を
	 * 二つ目のb2Vec2で右辺のx座標とy座標をセット */
	borderlineBox.Set(b2Vec2(pBgUnder->getContentSize().width / PTM_RATIO, pBgUnder->getContentSize().height / PTM_RATIO),
			b2Vec2(pBgUnder->getContentSize().width / PTM_RATIO, 0));
	// 指定した境界線と密度を海底オブジェクトに登録
	borderlineBody->CreateFixture(&borderlineBox, seabedHeight);
}

// ユニットを生成する
void GameScene::createUnit(int hp, int kTag, int vit) {
	CCSize bgSize = getChildByTag(kTag_Background)->getContentSize();				// 背景サイズを取得
	PhysicsSprite* pUnit = new PhysicsSprite(hp);									// 物理構造を持った画像オブジェクトを生成
	pUnit->autorelease();															// メモリ領域を開放
	if (kTag == kTag_PlayerUnit) {												// プレイヤーユニットの場合
		pUnit->initWithFile("player.png");
		pUnit->setPosition(ccp(bgSize.width * 0.8,								// 任意の位置にオブジェクトをセット
				bgSize.height * 0.35 - (bgSize.height - getWindowSize().height) / 2));
	} else if (kTag == kTag_EnemySubmarine){																	// プレイヤーの潜水艦でない場合
		pUnit->initWithFile("stage2.png");
		pUnit->setPosition(ccp(bgSize.width * 0.2,								// 任意の位置にオブジェクトをセット
				bgSize.height * 0.6 /*- (bgSize.height - getWindowSize().height) / 4*/));
	} else {																		// 潜水艦でない場合
		pUnit->initWithFile("stage1.png");										// 駆逐艦画像を取得し、オブジェクトに格納
		pUnit->setPosition(ccp(bgSize.width * 0.5,									// 任意の位置にオブジェクトをセット
				bgSize.height * 3 / 4 ));
	}
	this->addChild(pUnit, kZOrder_Unit, kTag);										// タグとオブジェクトを関連づける
	pUnit = createPhysicsBody(kTag_StaticBody, kTag, pUnit, kTag_Polygon);		// オブジェクトに物理構造を持たせる
	unitData[kTag] = pUnit;												// ユニットのデータを配列に格納
	pUnit->retain();
	b2Vec2 a = unitPhysicsData[kTag]->GetPosition();
}

// 物理構造を持ったユニットノードを作成
PhysicsSprite* GameScene::createPhysicsBody(int bodyTag, int kTag, PhysicsSprite* pNode, int shape) {
	b2BodyDef physicsBodyDef;														// 物理構造を持ったユニット変数
	if (bodyTag == kTag_StaticBody) {													// 静的構造が選択された場合
		physicsBodyDef.type = b2_staticBody;										// オブジェクトは静的になる
	} else if (bodyTag == kTag_DynamicBody) {											// 動的構造が選択された場合
		physicsBodyDef.type = b2_dynamicBody;										// オブジェクトは動的になる
	} else {																		// どちらでもない場合
		physicsBodyDef.type = b2_kinematicBody;										// オブジェクトは運動学的になる
	}
	float width = pNode->getPositionX() / PTM_RATIO;
	float height = pNode->getPositionY() / PTM_RATIO;
	physicsBodyDef.position.Set(pNode->getPositionX() / PTM_RATIO,					// 物理オブジェクトの位置をセット
			pNode->getPositionY() / PTM_RATIO);
	physicsBodyDef.userData = pNode;												// ノードをデータに登録
	unitPhysicsData[kTag] = world->CreateBody(&physicsBodyDef);						// 物理世界に登録
	b2FixtureDef physicsFixturedef;													// 物理形状を決める変数

	b2PolygonShape PolygonShape;
	b2CircleShape CircleShape;
	if (!shape) {																	// shapeが0の場合
		PolygonShape.SetAsBox(pNode->getContentSize().width * 0.8 / 2 / PTM_RATIO,
				pNode->getContentSize().height * 0.8 / 2 / PTM_RATIO);														// 角形の範囲を設定
	} else {																		// shapeが0でない場合
		CircleShape.m_radius = pNode->getContentSize().width * 0.4 / PTM_RATIO;	// 円形の範囲を設定
	}

	!shape ? physicsFixturedef.shape = &PolygonShape :
			physicsFixturedef.shape = &CircleShape;
	physicsFixturedef.density = 10;													// オブジェクトの密度を設定
	physicsFixturedef.friction = 1;												// オブジェクトの摩擦を設定

	unitPhysicsData[kTag]->CreateFixture(&physicsFixturedef);										// 構造体に情報を登録
	pNode->setPhysicsBody(unitPhysicsData[kTag]);													// ノードに構造体を登録
	b2Vec2 a = unitPhysicsData[kTag]->GetPosition();
	return pNode;																	// 作成したノードを返却
}

// 残機を表示
void GameScene::createLife() {
	if(((CCSprite*)getChildByTag(kTag_Life))) {
		((CCSprite*)getChildByTag(kTag_Life))->removeFromParentAndCleanup(true);
	}
	// 画像ファイルをバッチノード化
	lifeBatchNode = CCSpriteBatchNode::CCSpriteBatchNode::create("life.png");
	this->addChild(lifeBatchNode, kZOrder_Label, kTag_Life);				// タグとノードを関連づけ
	// ライフの数だけ繰り返し
	for (int i = 0; i < lifepoint; i++) {
		// バッチノードから画像を取得してオブジェクト化
		CCSprite* pLife = CCSprite::createWithTexture(lifeBatchNode->getTexture());
		// 任意の位置に画像をセット
		pLife->setPosition(ccp(getWindowSize().width / 10 * (9.5 - i * 0.8),		// スコアノードの位置を設定
				getWindowSize().height / 15 * 14));
		lifeBatchNode ->addChild(pLife);		// オブジェクト情報をバッチノードにセット
	}
}

// 残りHPを生成
void GameScene::createLifeCounter() {
	// もし先に表示されているhp画像があれば削除をする
	if(((CCSprite*)getChildByTag(kTag_LifeCounter))) {
		((CCSprite*)getChildByTag(kTag_LifeCounter))->removeFromParentAndCleanup(true);
	}
	// hp.pngをバッチノードに追加
	hpBatchNode = CCSpriteBatchNode::CCSpriteBatchNode::create("hp.png");
	// タグとノードを関連づけ
	this->addChild(hpBatchNode, kZOrder_Label, kTag_LifeCounter);
	// playerUnitのhp分ループして繰り返す
	for (int i = 0; i < unitData[kTag_PlayerUnit]->getHp(); i++) {
		// バッチノードから画像を取得してオブジェクト化
		CCSprite* pLifeCounter = CCSprite::createWithTexture(hpBatchNode->getTexture());
		// 任意の位置に画像をセット
		pLifeCounter->setPosition(ccp(getWindowSize().width / 10 * (9.5 - i * 0.8),		// スコアノードの位置を設定
				getWindowSize().height / 15 * 13));
		hpBatchNode ->addChild(pLifeCounter);		// オブジェクト情報をバッチノードにセット
	}
}


//コントローラ下地を作成
void GameScene::createControllerPanel() {
	//コントローラ部を作成
	CCSprite* pControl = CCSprite::create("control.png");					//control.pngをCCSprite型にし、pControlで初期化
	pControl->setPosition(ccp(getWindowSize().width / 2, getWindowSize().height / 8));				//座標のセット
	this->addChild(pControl, kZOrder_Controller_Base);						//配置順kZOrder_Controller_Baseで実装
}

//コントローラ部全ての画像を生成
void GameScene::createKey() {
	stopButton();		//ストップボタン生成
	arrowKey();			//十字キー生成
	speedMater();		//速度メータ生成
	missileButton();	//ミサイル発射ボタン生成
	speedSwitch();		//メータスイッチ生成
}

//ストップボタン表示の定義
void GameScene::stopButton() {
	/*----- STOPボタンの実装 -----*/
	CCSprite* pStop = CCSprite::create("stop.png");												//stop.pngをCCSprite型で生成
	pStop->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8));				//座標のセット
	this->addChild(pStop, kZOrder_Seabed, kTag_Key_Center);										//配置順kZOrder_Labelで実装
}

//十字キー表示の定義
void GameScene::arrowKey() {
	CCSprite* stopSprite = 	(CCSprite*)getChildByTag(kTag_Key_Center);			//オブジェクトstopボタンを取得
	float stopSize = stopSprite->getContentSize().height;						//stopの高さを取得

	//先ほどの変数が0より大きい間後置デクリメント
	while(arrow_key-- > 0) {

		CCSprite* pKey = CCSprite::create("arrow.png");							//tri.pngをCCSprite型で生成
		switch(arrow_key) {
		//上キーの実装
		case 3 : pKey->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8 + stopSize * 0.9));	//座標のセット
		pKey->setRotation(315);
		this->addChild(pKey, kZOrder_Label, kTag_Key_Up);					//配置順kZOrder_Lavelで実装
		break;
		//下キーの実装
		case 2 : pKey->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8 - stopSize * 0.9));	//座標のセット
		pKey->setRotation(135);													//180度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Down);						//配置順kZOrder_Lavelで実装
		break;
		//左キーの実装
		case 1 : pKey->setPosition(ccp(getWindowSize().width / 4 - stopSize * 0.9, getWindowSize().height / 8));	//座標のセット
		pKey->setRotation(225);													//270度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Left);						//配置順kZOrder_Lavelで実装
		break;
		//右キーの実装
		case 0 : pKey->setPosition(ccp(getWindowSize().width / 4 + stopSize * 0.9, getWindowSize().height / 8));	//座標のセット
		pKey->setRotation(45);													//90度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Right);					//配置順kZOrder_Lavelで実装
		break;
		}

	}
}

//スピードメータ表示の定義
void GameScene::speedMater() {

	CCSprite* stopSprite = 	(CCSprite*)getChildByTag(kTag_Key_Center);			//オブジェクトstopボタンを取得
	float stopHeight = stopSprite->getPositionY();								//stopのy座標を取得

	CCSprite* pMater2 = CCSprite::create("Meter2.png");							//Meter2.pngをCCSprite型で生成
	pMater2->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight - pMater2->getContentSize().height / 2));		//座標のセット
	this->addChild(pMater2, kZOrder_Label, kTag_Gear2);							//配置順kZOrderds_Labelで実装
	float a = pMater2->getPositionY();									// メーターの座標値を取得

	CCSprite* pMater3 = CCSprite::create("Meter2.png");							//Meter3.pngをCCSprite型で生成
	pMater3->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight + pMater2->getContentSize().height / 2));		//座標のセット
	this->addChild(pMater3, kZOrder_Label, kTag_Gear3);							//配置順kZOrder_Labelで実装

	CCSprite* pMater1 = CCSprite::create("Meter1.png");							//Meter1.pngをCCSprite型で生成
	pMater1->setPosition(ccp(getWindowSize().width / 8 * 5, pMater2->getPositionY() - pMater2->getContentSize().height ));		//座標のセット
	this->addChild(pMater1, kZOrder_Label, kTag_Gear1);							//配置順kZOrder_Labelで実装

	CCSprite* pMater4 = CCSprite::create("Meter3.png");							//Meter3.pngをCCSprite型で生成
	pMater4->setPosition(ccp(getWindowSize().width / 8 * 5, pMater3->getPositionY() + pMater3->getContentSize().height ));		//座標のセット
	this->addChild(pMater4, kZOrder_Label, kTag_Gear4);							//配置順kZOrder_Labelで実装

	/*
	 * 自機の加減速に利用するため、各メーターのY座標を配列に格納
	 */
	for (int i = kTag_Gear1; i <= kTag_Gear4; i++) {
		meterPosition.push_back(this->getChildByTag(i)->getPositionY());
	}
}

//ミサイルボタン表示の定義
void GameScene::missileButton() {
	CCSprite* Key_Down = 	(CCSprite*)getChildByTag(kTag_Key_Down);				//オブジェクト下キーボタンを取得
	float key_downHeight = Key_Down->getPositionY();								//materのy座標を取得

	/*----- missile 上キーの実装 -----*/
	CCSprite* pMissileUp = CCSprite::create("MissileButton.png");						//missile_up.pngをCCSprite型で生成
	pMissileUp->setPosition(ccp(getWindowSize().width / 8 * 7, key_downHeight * 3.5));	//座標のセット
	this->addChild(pMissileUp, kZOrder_Label, kTag_Shoot_Vertical);					//配置順kZOrder_Labelで実装

	/*----- missile 左キーの実装 -----*/
	CCSprite* pMissileLeft = CCSprite::create("MissileButton.png");					//missile_left.pngをCCSprite型で生成
	pMissileLeft->setRotation(270);
	pMissileLeft->setPosition(ccp(getWindowSize().width / 8 * 7, key_downHeight));	//座標のセット
	this->addChild(pMissileLeft, kZOrder_Label, kTag_Shoot_Horizontal);				//配置順kZOrder_Labelで実装
}

//スピードスイッチ表示の定義
void GameScene::speedSwitch() {
	CCSprite* mater2 = 	(CCSprite*)getChildByTag(kTag_Gear2);				//オブジェクトギア1オブジェクトを取得
	CCPoint pMater2 = mater2->getPosition();								//ギア2の高さ取得
	float Mater2Size = mater2->getContentSize().height;						//ギア1の座標を取得

	//速度メーター スイッチ
	CCSprite* pSwitch = CCSprite::create("switch.png");							//switch.pngファイルを取得
	pSwitch->setPosition(ccp(pMater2.x, pMater2.y - Mater2Size / 10 * 9 ));		//座標をセット
	this->addChild(pSwitch, kZOrder_Countdown, kTag_Switch);					//配置順kZOrder_Labelで実装
}
// ストップボタン
bool GameScene::stopJudge() {
	return 0;
}
// ゲーム開始時のカウントダウン
void GameScene::showCountdown() {

	//	this->scheduleOnce(schedule_selector(GameScene::playCountdownSound), 0.5);	//playCountdownSound関数を0.5秒後に一度だけ呼び出す

	// 「3」の表示
	CCSprite* pNum3 = CCSprite::create("count3.png");									//count3.pngを取得
	pNum3->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));	//座標を中心にせっと
	pNum3->setScale(0);																	//何もない状態から表示するから表示倍率0倍でセット
	pNum3->runAction(Animation::num3Animation());										//pNum3にnum3Animationアニメーションをセット
	this->addChild(pNum3, kZOrder_Countdown);											//オブジェクトpNum3の実装

	// 「2」の表示
	CCSprite* pNum2 = CCSprite::create("count2.png");									//count2.pngを取得
	pNum2->setPosition(pNum3->getPosition());											//pNum3と同じ座標にセット
	pNum2->setScale(0);																	//pNum3同様倍率0倍でセット
	pNum2->runAction(Animation::num2Animation());										//pNum2にnum2Animationアニメーションをセット
	this->addChild(pNum2, kZOrder_Countdown);											//オブジェクトpNum2の実装

	// 「1」の表示
	CCSprite* pNum1 = CCSprite::create("count1.png");									//count1.pngを取得
	pNum1->setPosition(pNum3->getPosition());											//pNum3と同じ座標にセット
	pNum1->setScale(0);																	//pNum3同様倍率0倍でセット
	pNum1->runAction(Animation::num1Animation());										//pNum1にnum1Animationアニメーションをセット
	this->addChild(pNum1, kZOrder_Countdown);											//オブジェクトpNum2の実装

	// 「GO」の表示
	CCSprite* pGo = CCSprite::create("go.png");											//go.pngを取得
	pGo->setPosition(pNum3->getPosition());												//pNum3と同じ座標にセット
	pGo->setScale(0);																	//pNum3同様倍率0倍でセット
	/* pGoにgameStartAnimationアニメーションをセット
	 * アニメーション終了後に呼び出す関数GameSceneクラスのstartGame関数を第二引数に持たす
	 */
	pGo->runAction(Animation::gameStartAnimation(this, callfunc_selector(GameScene::startGame)));
	this->addChild(pGo, kZOrder_Countdown);												//オブジェクトpGoの実装
}
// ゲームスタートの処理を行う
void GameScene::startGame() {
	SimpleAudioEngine::sharedEngine()->playBackgroundMusic("battle.mp3", true);
	this->setTouchEnabled(true);				  							// タッチ可能にする
	this->setTouchMode(kCCTouchesAllAtOnce);								// マルチタップイベントを受け付ける
	// 毎フレームupdate( )関数を呼び出すように設定する
	scheduleUpdate();
}

void GameScene::hitUnit(PhysicsSprite* unit){
	this->scheduleOnce(schedule_selector(GameScene::explosionSound), 0);		// 0秒後に爆発エフェクト音を鳴らす
	unit->setHp(unit->getHp() - 1);	// hpを-1してセット
	CCSprite* bombAction = CCSprite::create("hit0.png");						// hit0.pngを取得
	bombAction->setPosition(unit->getPosition());								// playerのオブジェクト(潜水艦)と同じ座標にセット
	int anima = unit->getHp() != 0 ? hitAnimation : defeatAnimation;
	bombAction->runAction(Animation::hitAnimation(anima));						// 被弾時のアニメーションhitAnimationを呼び出す
	this->addChild(bombAction, kZOrder_Countdown);								// 爆発アニメーションの実装

	if (unit == unitData[kTag_PlayerUnit]) {
		if(unit->getHp() == 0) {												// hpがなくなった場合
			defeatPlayer();														// 残機を減らす処理へ
			// hpが0でなければ以下の処理
		} else {
			createLifeCounter();												// 自機life表示を再生性
		}
		// hp0になったユニットが敵潜水艦だったら以下
	}else if (unit == unitData[kTag_EnemySubmarine] && unit->getHp() == 0) {
		removeObject(unit, (void*)unitPhysicsData[kTag_EnemySubmarine]);		// 敵機を削除
		// hp0になったユニットが敵駆逐艦だったら以下
	}else if (unit == unitData[kTag_EnemyDestroyer] && unit->getHp() == 0) {
		removeObject(unit, (void*)unitPhysicsData[kTag_EnemyDestroyer]);
	}
}

// 自機撃沈関数
void GameScene::defeatPlayer () {
	this->lifepoint--;									// 残機を減らす
	if (lifepoint == 0) {								// 残機がなくなった場合
		finishGame();								// ゲームオーバー
	}else {
		removeObject(unitData[kTag_PlayerUnit], (void*)unitPhysicsData[kTag_PlayerUnit]);						// 撃沈したオブジェクトを削除
		// 自機を生成
		createUnit(player_VIT, kTag_PlayerUnit, submarine_VIT);
		createLife();								// 残機を再表示
		createLifeCounter();
	}
}

// オブジェクトを除去する
void GameScene::removeObject(CCNode* pObject, void* body = 0) {
	pObject->removeFromParentAndCleanup(true);			// シーンからオブジェクトを削除する
	if (body) {
		world->DestroyBody((b2Body*)body);					// 重力世界からオブジェクトを削除する
	}
}

// 終了時clearもしくはgameoverの画像を表示
void GameScene::finishGame() {
	// update( )関数の呼び出しを停止する
	unscheduleUpdate();
	const char* fileName = unitData[kTag_PlayerUnit]->getHp() == 0
			? "gameover.png" : "clear.png";	// lifepointが0であればgameover.pngを、違うならclear.pngで初期化

	// 「Game Over」を表示する
	CCSprite* gameOver = CCSprite::create(fileName);
	gameOver->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));
	gameOver->setScale(0);
	gameOver->runAction(Animation::gameOverAnimation(this, callfunc_selector(GameScene::moveToNextScene)));
	this->addChild(gameOver, kZOrder_Countdown, kTag_Animation);
}

void GameScene::moveToNextScene() {

	// 次の画面へ遷移する
	CCTransitionProgressRadialCCW* tran;
	tran = CCTransitionProgressRadialCCW::create(1, GameScene::scene());
	CCDirector::sharedDirector()->replaceScene(tran);
}
// 毎フレームごとに衝突判定をチェックする
void GameScene::update(float dt) {
	// 物理シミュレーションの正確さを決定するパラメーター
	int velocityIterations = 8;
	int positionIterations = 1;
	// worldを更新する
	world->Step(dt, velocityIterations, positionIterations);
	timeCounter > 0 ? timeCounter-- : timeCounter++;
	// world内の全オブジェクトをループする
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
		if (!b->GetUserData()) {
			continue;													// オブジェクトが見つからない場合は次のループへ
		}
		CCNode* object = (CCNode*)b->GetUserData();						// オブジェクトを取得
		if (!(object->getTag())) {
			// オブジェクトのタグを取得
			continue;
		}
		int objectTag;
		objectTag = object->getTag();
		if (objectTag == kTag_Call_Scroll) {
			// ミサイル消失タグだった場合
		} else if (objectTag == kTag_Remove_Missile) {
			// 0.1秒後に消えるアクションをセットする
			CCDelayTime* delay = CCDelayTime::create(0);
			CCCallFuncND* func = CCCallFuncND::create(this, callfuncND_selector(GameScene::removeObject), (void*)b);
			CCSequence* action = CCSequence::createWithTwoActions(delay, func);
			action->setTag(kTag_Remove_Missile);
			object->runAction(action);
		} else if (objectTag == kTag_CollisionPlayer) {						// 機体同士もしくはプレイヤーが海底に衝突した場合
			removeObject(object, (void*)b);								// ミサイルを消す
			//          PhysicsSprite* pObject = (PhysicsSprite*)object;
			//            if (pObject->getTag() /*!= kTag_PlayerUnit*/) {
			hitUnit(unitData[kTag_PlayerUnit]);												// 自機が撃沈される
		} else if (objectTag == kTag_PlayerUnit) {						// 機体同士もしくはプレイヤーが海底に衝突した場合
			CCNode* myUnit = this->getChildByTag(kTag_PlayerUnit);
			/*			if(myUnit->getPositionY() > getWindowSize().height * 3 / 4) {
				// 毎フレームrotateUpAngle関数を呼び出すように設定する
				this->unschedule(schedule_selector(GameScene::rotateUpAngle));	// 上キーから指が離れた場合は船首上げ関数の呼び出しをストップ
			} else if(myUnit->getPositionY() < getWindowSize().height / 4) {
				// 毎フレームrotateDownAngle関数を呼び出すように設定する
				this->unschedule(schedule_selector(GameScene::rotateDownAngle));
			} else */if(myUnit->getPositionX() < getWindowSize().width * 0.15 || myUnit->getPositionY() > getWindowSize().height * 0.7 || myUnit->getPositionY() < getWindowSize().height * 0.3) {
				// 毎フレームforwardUnit関数を呼び出すように設定
				this->unschedule(schedule_selector(GameScene::forwardUnit));
			} else if(myUnit->getPositionX() > getWindowSize().width * 0.85 || myUnit->getPositionY() > getWindowSize().height * 3 / 4 || myUnit->getPositionY() < getWindowSize().height / 4) {
				// 毎フレームbackUnit関数を呼び出すように設定する
				this->unschedule(schedule_selector(GameScene::backUnit));
			}
		} else if (objectTag == kTag_CollisionSubmarine) {						// 機体同士もしくはプレイヤーが海底に衝突した場合
			score_and_Maxplace += 5;
			setScoreNumber();
			removeObject(object, (void*)b);								// ミサイルを消す
			hitUnit(unitData[kTag_EnemySubmarine]);											// 自機が撃沈される
		} else if (objectTag == kTag_CollisionDestroyer) {						// 機体同士もしくはプレイヤーが海底に衝突した場合
			score_and_Maxplace += 5;
			setScoreNumber();
			removeObject(object, (void*)b);								// ミサイルを消す
			hitUnit(unitData[kTag_EnemyDestroyer]);											// 自機が撃沈される
		}
	}
	if (this->getChildByTag(kTag_EnemyDestroyer) && timeCounter > 629) {
		destroyerAI();
	} else if (this->getChildByTag(kTag_EnemyDestroyer) && timeCounter < 630) {
		destroyerAI2();
	} if (this->getChildByTag(kTag_EnemySubmarine) && (timeCounter > 1140 || (timeCounter > 1020 && timeCounter < 1080)|| (timeCounter > 660 && timeCounter < 780)|| (timeCounter > 300 && timeCounter < 420))) {
		submarineAI();
	} else if (this->getChildByTag(kTag_EnemySubmarine) && ((timeCounter > 840 && timeCounter < 960)|| (timeCounter > 480 && timeCounter < 660) || (timeCounter > 180 && timeCounter < 240) || (timeCounter > 0 && timeCounter < 120))) {
		submarineAI2();
	} else if (this->getChildByTag(kTag_EnemySubmarine) && (timeCounter > 1080 && timeCounter < 1140) || (timeCounter > 420 && timeCounter < 480) || (timeCounter > 240 && timeCounter < 300)) {
		submarineAI4();
	} else if (this->getChildByTag(kTag_EnemySubmarine) && (timeCounter > 960 && timeCounter < 1020) || (timeCounter > 780 && timeCounter < 840) || (timeCounter > 120 && timeCounter < 180)) {
		submarineAI3();
	}
	if (!(this->getChildByTag(kTag_EnemyDestroyer) || this->getChildByTag(kTag_EnemySubmarine))) {
		finishGame();
	}
	if (!timeCounter) {
		timeCounter += 1260;
	}
}
// スコア部を生成
void GameScene::createScore() {
	float j = 0;																		// スコア間隔調整のための変数
	CCLabelTTF* tmp;																	// スコアテキスト格納用変数
	// 任意の桁数だけ繰り返し
	for (int i = score_and_Maxplace * 10; i % 10; i-=1, j += 0.05) {
		i % 10 != 1 ?																	// 配列の最後の要素であるか？
				tmp = CCLabelTTF::create("5", "", NUMBER_FONT_SIZE):					// でなければ数字表示なし
				tmp = CCLabelTTF::create("0", "", NUMBER_FONT_SIZE);					// であれば数字表示あり
		tmp->setPosition(ccp(getWindowSize().width * (0.2 - j),
				getWindowSize().height * 0.9));											// 座標をセット
		tmp->setColor(ccBLACK);															// フォントカラーをセット

		scoreText->addObject(tmp);														// 配列の末尾にオブジェクトをセット
		this->addChild((CCLabelTTF*)scoreText->lastObject(), kZOrder_Label);			// スコアノードに情報を登録
	}
}
/* setScoreNumber関数
 * 返却型 : void
 *
 * createScoreで生成されたラベルを更新する関数
 * ポイントが関係する衝突があった際に呼び出される
 */
void GameScene::setScoreNumber() {
	int showScore;											// 表示するスコア
	int maxScore = 1;										// 表示可能な最大スコア
	// 繰返し文により、表示可能最大値を算出
	// score_and_Maxplaceは小数部が桁数、整数部がスコア
	// 小数部が0.3である為３回繰り返され、maxScoreは1000になる
	for(int i = score_and_Maxplace * 10; i % 10; i-=1) {
		maxScore *= 10;
	}

	// score_and_Maxplaceが1000より大きければ
	// 1000以上であれば最高スコアの999に
	// そうでない場合はその値の整数部をshowScoreに代入
	if (score_and_Maxplace > maxScore) {
		showScore = maxScore - 1;		// maxScore(1000)から1引かれた値をshowScoreに代入する
	} else {
		showScore = score_and_Maxplace;	// float型score_and_Maxplaceをint型showScoreに代入、暗黙的キャスト
	}

	/*----- 4/11 add植田 : 以下作り直し内容 -----*/
	// 各桁の数字を取得
	int hundredsPlace = showScore / 100;								// 100の位の値を取得
	int tenthsPlace = (showScore - hundredsPlace * 100) / 10;			// 10の位の値を取得
	int onesPlace = showScore -hundredsPlace * 100 - tenthsPlace * 10;	// 1の位の値を取得

	/* score_and_Maxplaceを10倍し整数化した値を10で割った余りが0(ヌル)になるまでループ
	 * ヌルでない間は下一桁をデクリメント
	 */
	for (int i = score_and_Maxplace * 10; i % 10; i--) {

		/* indexに該当した配列scoreTextのオブジェクトを取り出しCCLabelTTF*型にキャスト
		 * その値でCClabelTTF*型変数を初期化
		 */
		CCLabelTTF* tmp = (CCLabelTTF*)scoreText->objectAtIndex(i % 10 - 1);
		/* CCString型変数numを宣言
		 * このString変数をもってラベル(スコアの値)を更新する
		 */
		CCString* num;


		/* hundredsPlace ⇒ 100の位の値
		 * tenthsPlace   ⇒  10の位の値
		 * onesPlace     ⇒   1の位の値
		 */
		// ループ一巡目であれば以下のブロック
		if(i % 10 == 3) {
			// 100の位が0であれば半角スペースを出力
			if(hundredsPlace == 0) {
				num = CCString::create(" ");
				// 0以外であればhundredsPlaceの値を10進数で出力
			}else {
				num = CCString::createWithFormat("%d", hundredsPlace);
			}

			// ループ二巡目であれば以下のブロック
		}else if(i % 10 == 2) {
			// 100の位が0であれば以下のブロック
			if(hundredsPlace == 0) {
				// 10の位も0であれば半角スペースを出力
				if(tenthsPlace == 0) {
					num = CCString::create(" ");
					// 100の位は0であるが10の位は0以外である場合tenthsPlaceの値を出力
				}else {
					num = CCString::createWithFormat("%d", tenthsPlace);
				}
				// 100の位が0でない場合tenthsPlaceの値をそのまま出力
			}else {
				num = CCString::createWithFormat("%d", tenthsPlace);
			}

			// 三巡目であればonesPlaceの値を出力
		}else if(i % 10 == 1) {
			num = CCString::createWithFormat("%d", onesPlace);
		}

		// CCString* numのgetString()情報を引数にセットしtmpを実行
		tmp->setString(num->getCString());
	}
}
// 駆逐艦AI
void GameScene::destroyerAI() {
	PhysicsSprite* ab = unitData[kTag_EnemyDestroyer];
	b2Vec2 destroyerPosition = unitPhysicsData[kTag_EnemyDestroyer]->GetPosition();

	CCPoint destroyerPositions = unitData[kTag_EnemyDestroyer]->getPosition();
	if(!(rand() %  200)) {										// ランダムでミサイルを発射
		createMissile(destroyerPosition);							// ミサイルを発射
	} else if(destroyerPositions.x > getWindowSize().width / 4) {									// ランダムで移動
		float unitAngle = unitPhysicsData[kTag_EnemyDestroyer]->GetAngle();		// ユニットの現在角度を取得
		float angleBonusSpeed = unitAngle > 0 ? PI * (PI / 2 - unitAngle) : PI * (PI / 2 + unitAngle);	// 角度から速度を計算
		float forward = unitData[kTag_EnemyDestroyer]->getPositionX()  - 0.2 * angleBonusSpeed;		// ユニットの進むべきX座標を計算
		float up = unitData[kTag_EnemyDestroyer]->getPositionY() - 0.2 * PI * unitAngle;		// ユニットの進むべきY座標を計算
		unitData[kTag_EnemyDestroyer]->setPosition(ccp(forward, up));			// 画像の座標を設定
		// 物理オブジェクトの座標を設定
		unitPhysicsData[kTag_EnemyDestroyer]->SetTransform(b2Vec2(forward / PTM_RATIO, up / PTM_RATIO), unitPhysicsData[kTag_EnemyDestroyer]->GetAngle());
	}
}
// 駆逐艦AI
void GameScene::destroyerAI2() {
	PhysicsSprite* ab = unitData[kTag_EnemyDestroyer];
	b2Vec2 destroyerPosition = unitPhysicsData[kTag_EnemyDestroyer]->GetPosition();

	CCPoint destroyerPositions = unitData[kTag_EnemyDestroyer]->getPosition();
	if(!(rand() %  200)) {										// ランダムでミサイルを発射
		createMissile(destroyerPosition);							// ミサイルを発射
	}  else if(destroyerPositions.x < getWindowSize().width * 3 / 4) {									// ランダムで移動
		float unitAngle = unitPhysicsData[kTag_EnemyDestroyer]->GetAngle();		// ユニットの現在角度を取得
		float angleBonusSpeed = unitAngle > 0 ? PI * (PI / 2 - unitAngle) : PI * (PI / 2 + unitAngle);	// 角度から速度を計算
		float back = unitData[kTag_EnemyDestroyer]->getPositionX()  + 0.2 * angleBonusSpeed;		// ユニットの進むべきX座標を計算
		float up = unitData[kTag_EnemyDestroyer]->getPositionY() + 0.2 * PI * unitAngle;		// ユニットの進むべきY座標を計算
		unitData[kTag_EnemyDestroyer]->setPosition(ccp(back, up));			// 画像の座標を設定
		// 物理オブジェクトの座標を設定
		unitPhysicsData[kTag_EnemyDestroyer]->SetTransform(b2Vec2(back / PTM_RATIO, up / PTM_RATIO), unitPhysicsData[kTag_EnemyDestroyer]->GetAngle());
	}
}

// 潜水艦AIの作成
void GameScene::submarineAI() {
	PhysicsSprite* ab = unitData[kTag_EnemySubmarine];
	b2Vec2 destroyerPosition = unitPhysicsData[kTag_EnemySubmarine]->GetPosition();

	CCPoint destroyerPositions = unitData[kTag_EnemySubmarine]->getPosition();
	if(!(rand() %  200)) {										// ランダムでミサイルを発射
		createMissileSubmarine(destroyerPosition);							// ミサイルを発射
	}  else if(destroyerPositions.x < getWindowSize().width * 3 / 4) {									// ランダムで移動
		float unitAngle = unitPhysicsData[kTag_EnemySubmarine]->GetAngle();		// ユニットの現在角度を取得
		float angleBonusSpeed = unitAngle > 0 ? PI * (PI / 2 - unitAngle) : PI * (PI / 2 + unitAngle);	// 角度から速度を計算
		float back = unitData[kTag_EnemySubmarine]->getPositionX()  + 0.2 * angleBonusSpeed;		// ユニットの進むべきX座標を計算
		float up = unitData[kTag_EnemySubmarine]->getPositionY() + 0.2 * PI * unitAngle;		// ユニットの進むべきY座標を計算
		if (up > getWindowSize().height * 0.65 ) {
			up = getWindowSize().height * 0.65;
		}
		unitData[kTag_EnemySubmarine]->setPosition(ccp(back, up));			// 画像の座標を設定
		// 物理オブジェクトの座標を設定
		unitPhysicsData[kTag_EnemySubmarine]->SetTransform(b2Vec2(back / PTM_RATIO, up / PTM_RATIO), unitPhysicsData[kTag_EnemySubmarine]->GetAngle());
	}
}
// 潜水艦AIの作成
void GameScene::submarineAI2() {
	PhysicsSprite* ab = unitData[kTag_EnemySubmarine];
	b2Vec2 destroyerPosition = unitPhysicsData[kTag_EnemySubmarine]->GetPosition();

	CCPoint destroyerPositions = unitData[kTag_EnemySubmarine]->getPosition();
	if(!(rand() %  200)) {										// ランダムでミサイルを発射
		createMissileSubmarine(destroyerPosition);							// ミサイルを発射
	} else if(destroyerPositions.x > getWindowSize().width / 4) {									// ランダムで移動
		float unitAngle = unitPhysicsData[kTag_EnemySubmarine]->GetAngle();		// ユニットの現在角度を取得
		float angleBonusSpeed = unitAngle > 0 ? PI * (PI / 2 - unitAngle) : PI * (PI / 2 + unitAngle);	// 角度から速度を計算
		float forward = unitData[kTag_EnemySubmarine]->getPositionX()  - 0.2 * angleBonusSpeed;		// ユニットの進むべきX座標を計算
		float up = unitData[kTag_EnemySubmarine]->getPositionY() - 0.2 * PI * unitAngle;		// ユニットの進むべきY座標を計算
		if (up > getWindowSize().height * 0.65 ) {
			up = getWindowSize().height * 0.65;
		}
		unitData[kTag_EnemySubmarine]->setPosition(ccp(forward, up));			// 画像の座標を設定
		// 物理オブジェクトの座標を設定
		unitPhysicsData[kTag_EnemySubmarine]->SetTransform(b2Vec2(forward / PTM_RATIO, up / PTM_RATIO), unitPhysicsData[kTag_EnemySubmarine]->GetAngle());
	}
}
// 船首を上げる関数
void GameScene::submarineAI3() {
	float unitAngle = unitPhysicsData[kTag_EnemySubmarine]->GetAngle();		// 角度を変えるためにプレイヤーの潜水艦オブジェクトを呼びます
	if (unitAngle > -1 * PI / 4) {
		unitPhysicsData[kTag_EnemySubmarine]->SetTransform(unitPhysicsData[kTag_EnemySubmarine]->GetPosition(), unitAngle - 0.01);	// 船首を上げます
	}
}
// 船首を下げる関数
void GameScene::submarineAI4() {
	float unitAngle = unitPhysicsData[kTag_EnemySubmarine]->GetAngle();		// 角度を変えるためにプレイヤーの潜水艦オブジェクトを呼びます
	if (unitAngle < PI / 4) {
		unitPhysicsData[kTag_EnemySubmarine]->SetTransform(unitPhysicsData[kTag_EnemySubmarine]->GetPosition(), unitAngle + 0.01);	// 船首を下げます
	}
}
/*----- 駆逐艦のミサイル(やや右) -----*/
void GameScene::createMissile(b2Vec2 position) {									// を指定位置にセット
	PhysicsSprite* pBomb = new PhysicsSprite(1);									// 物理構造を持った画像オブジェクトを生成
	pBomb->autorelease();														// 使われなくなったら自動的に開放
	pBomb->initWithTexture(bombBatchNode->getTexture());						// を指定位置にセット
	pBomb->setPosition(ccp(position.x * PTM_RATIO, position.y * PTM_RATIO - PTM_RATIO * 0.4));	// ミサイルを指定位置にセット
	pBomb->setOpacity(200);																		// 透過設定(0…完全に透過、255…元の画像表示)
	bombBatchNode->addChild(pBomb, kZOrder_Missile, kTag_MissileEnemy);
	pBomb = createPhysicsBody(kTag_DynamicBody, kTag_MissileEnemy, pBomb, kTag_Polygon);		// オブジェクトに物理構造を持たせる
	b2Body* missileBody = pBomb->getPhysicsBody();
	missileBody->SetLinearVelocity(b2Vec2(0.2, 0));
}

/*----- 潜水艦のミサイル(かなり右) -----*/
void GameScene::createMissileSubmarine(b2Vec2 position) {
	PhysicsSprite* pMissileSubmarine = new PhysicsSprite(1);										// 物理構造を持った画像オブジェクトを生成
	pMissileSubmarine->autorelease();															// 使われなくなったら自動的に開放
	pMissileSubmarine->setRotation(90);															// を指定位置にセット
	pMissileSubmarine->initWithTexture(enemyMissileBatchNode->getTexture());							// を指定位置にセット
	pMissileSubmarine->setPosition(ccp(position.x * PTM_RATIO, position.y * PTM_RATIO - PTM_RATIO * 0.4));	// 画像の座標を指定													// ミサイルを指定位置にセット
	pMissileSubmarine->setOpacity(200);																		// 透過設定(0…完全に透過、255…元の画像表示)
	enemyMissileBatchNode->addChild(pMissileSubmarine, kZOrder_Missile, kTag_MissileEnemy);						// 以上の情報でミサイル画像を生成
	pMissileSubmarine = createPhysicsBody(kTag_DynamicBody, kTag_MissileEnemy, pMissileSubmarine, kTag_Polygon);	// オブジェクトに物理構造を持たせる
	b2Body* missileBody = pMissileSubmarine->getPhysicsBody();											// オブジェクトpMissileのデータメンバ
	position.Set((position.x * PTM_RATIO * 1.4 /*- PTM_RATIO * 0.4*/) / PTM_RATIO, (position.y * PTM_RATIO - PTM_RATIO * 0.2) / PTM_RATIO);			// 重力世界と座標をセット
	missileBody->SetTransform(position, PI/2);													// 重力世界上の座標と角度を持たせ回転
	missileBody->SetLinearVelocity(b2Vec2(0.8, 0.2));											// x座標y座標に圧力をかけてオブジェクト生成
}

/*----- 自機の左ミサイル発射ボタンに対応 -----*/
void GameScene::createMissileLeft(b2Vec2 position) {
	PhysicsSprite* pMissile = new PhysicsSprite(1);										// 物理構造を持った画像オブジェクトを生成
	pMissile->autorelease();															// 使われなくなったら自動的に開放
	pMissile->initWithTexture(missileBatchNode->getTexture());							// を指定位置にセット													// 表示上の画像を180度回転
	pMissile->setPosition(ccp(position.x * PTM_RATIO, position.y * PTM_RATIO - PTM_RATIO * 0.4));	// 画像の座標を指定
	pMissile->setOpacity(200);																		// 透過設定(0…完全に透過、255…元の画像表示)
	missileBatchNode->addChild(pMissile, kZOrder_Missile, kTag_Missile);							// 以上の情報でミサイル画像を生成
	pMissile = createPhysicsBody(kTag_DynamicBody, kTag_Missile, pMissile, kTag_Polygon);		// オブジェクトに物理構造を持たせる
	b2Body* missileBody = pMissile->getPhysicsBody();											// オブジェクトpMissileのデータメンバ
	position.Set(position.x, (position.y * PTM_RATIO - PTM_RATIO * 0.4) / PTM_RATIO);			// 重力世界と座標をセット
	missileBody->SetTransform(position, -PI/2);												// 重力世界上の座標と角度を持たせ回転
	missileBody->SetLinearVelocity(b2Vec2(-0.8, 0.2));										// x座標y座標に圧力をかける
}

/*----- ミサイル左上発射 -----*/
void GameScene::createMissileDiagonal(b2Vec2 position) {
	PhysicsSprite* pMissile = new PhysicsSprite(1);										// 物理構造を持った画像オブジェクトを生成
	pMissile->autorelease();															// 使われなくなったら自動的に開放
	pMissile->initWithTexture(missileBatchNode->getTexture());							// を指定位置にセット													// 表示上の画像を180度回転
	pMissile->setPosition(ccp(position.x * PTM_RATIO, position.y * PTM_RATIO + PTM_RATIO * 0.4));	// 画像の座標を指定(PTM_RATIOは重力世界と表示を重ねるため)
	pMissile->setOpacity(200);																		// 透過設定(0…完全に透過、255…元の画像表示)
	missileBatchNode->addChild(pMissile, kZOrder_Missile, kTag_Missile);							// 以上の情報でミサイル画像を生成
	pMissile = createPhysicsBody(kTag_DynamicBody, kTag_Missile, pMissile, kTag_Polygon);		// オブジェクトに物理構造を持たせる
	b2Body* missileBody = pMissile->getPhysicsBody();											// オブジェクトpMissileのデータメンバを取得
	position.Set(position.x, (position.y * PTM_RATIO + PTM_RATIO * 0.4) / PTM_RATIO);			// 重力世界の座標をセット
	missileBody->SetTransform(position, -PI/ 10 * 9);													// 重力世界上の座標と角度を持たせ回転
	missileBody->SetLinearVelocity(b2Vec2(-0.8, 0.8));										// x座標y座標に圧力をかける
}

// 船首を上げる関数
void GameScene::rotateUpAngle() {
	float unitAngle = unitPhysicsData[kTag_PlayerUnit]->GetAngle();		// 角度を変えるためにプレイヤーの潜水艦オブジェクトを呼びます
	if (unitAngle > -1 * PI / 4) {
		unitPhysicsData[kTag_PlayerUnit]->SetTransform(unitPhysicsData[kTag_PlayerUnit]->GetPosition(), unitAngle - 0.02);	// 船首を上げます
	}
}
// 船首を下げる関数
void GameScene::rotateDownAngle() {
	float unitAngle = unitPhysicsData[kTag_PlayerUnit]->GetAngle();		// 角度を変えるためにプレイヤーの潜水艦オブジェクトを呼びます
	if (unitAngle < PI / 4) {
		unitPhysicsData[kTag_PlayerUnit]->SetTransform(unitPhysicsData[kTag_PlayerUnit]->GetPosition(), unitAngle + 0.02);	// 船首を下げます
	}
}
// 前進する関数
void GameScene::forwardUnit() {
	float coefficientOfSpeed = this->coefficientOfSpeed();
	float unitAngle = unitPhysicsData[kTag_PlayerUnit]->GetAngle();		// ユニットの現在角度を取得
	float angleBonusSpeed = unitAngle > 0 ? PI * (PI / 2 - unitAngle) : PI * (PI / 2 + unitAngle);	// 角度から速度を計算
	float forward = unitData[kTag_PlayerUnit]->getPositionX()  - 0.2 * angleBonusSpeed * coefficientOfSpeed;		// ユニットの進むべきX座標を計算
	float up = unitData[kTag_PlayerUnit]->getPositionY() - 0.2 * PI * unitAngle * coefficientOfSpeed;		// ユニットの進むべきY座標を計算
	unitData[kTag_PlayerUnit]->setPosition(ccp(forward, up));			// 画像の座標を設定
	// 物理オブジェクトの座標を設定
	unitPhysicsData[kTag_PlayerUnit]->SetTransform(b2Vec2(forward / PTM_RATIO, up / PTM_RATIO), unitPhysicsData[kTag_PlayerUnit]->GetAngle());
}
// 後退する関数
void GameScene::backUnit() {
	float coefficientOfSpeed = this->coefficientOfSpeed();
	float unitAngle = unitPhysicsData[kTag_PlayerUnit]->GetAngle();		// ユニットの現在角度を取得
	float angleBonusSpeed = unitAngle > 0 ? PI * (PI / 2 - unitAngle) : PI * (PI / 2 + unitAngle);	// 角度から速度を計算
	float back = unitData[kTag_PlayerUnit]->getPositionX()  + 0.2 * angleBonusSpeed * coefficientOfSpeed;		// ユニットの進むべきX座標を計算
	float up = unitData[kTag_PlayerUnit]->getPositionY() + 0.2 * PI * unitAngle * coefficientOfSpeed;		// ユニットの進むべきY座標を計算
	unitData[kTag_PlayerUnit]->setPosition(ccp(back, up));			// 画像の座標を設定
	// 物理オブジェクトの座標を設定
	unitPhysicsData[kTag_PlayerUnit]->SetTransform(b2Vec2(back / PTM_RATIO, up / PTM_RATIO), unitPhysicsData[kTag_PlayerUnit]->GetAngle());
}
/* ***********************
 *
 * 		タッチ処理
 *
 * ***********************/

/* 関数名 : ccTouchesBegan
 * 返却型 : void(returnなし)
 *
 * 配置したオブジェクトにタップの判定があった時に呼び出される
 */
void GameScene::ccTouchesBegan(CCSet* touches, CCEvent* pEvent ) {

	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {	// タッチ位置を順に探索
		CCTouch* touch  = (CCTouch *)(*it);									// 取得した値をノードと照合するためccTouch型に変換
		bool touch_judge = 0;											// 返り値として渡すためのタッチ判定フラグ
		tag_no = this->kTag_Key_Up;										// 各種ボタンの先頭のタグを取得
		CCPoint loc = touch->getLocation();								// タッチ位置を取得

		// 各ボタンのタッチ判定を繰り返し
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			i = this->getChildByTag(tag_no);							// 各種ハンドルオブジェクトでiを初期化し、タップ可能にする

			/* tag_noがミサイル発射上ボタンもしくは左ボタンであり、
			 * かつそのオブジェクトの座標をタップしていれば以下のブロック
			 */
			if((tag_no == kTag_Shoot_Vertical || tag_no == kTag_Shoot_Horizontal)
					&& i->boundingBox().containsPoint(loc)) {
				b2Vec2 playerPosition = unitPhysicsData[kTag_PlayerUnit]->GetPosition();	//PlayerUnitのスプライトを取得しその座標で初期化
				if(tag_no == kTag_Shoot_Horizontal && reloadMissile) {
				reloadMissile--;
					createMissileLeft(playerPosition);	//自機の座標とタップした発射ボタンを引数にし、createMissile関数を呼び出す
				} else if (reloadMissile){
					reloadMissile--;
					createMissileDiagonal(playerPosition);
				}
				if (!reloadMissile) {
					this->schedule(schedule_selector(GameScene::missileTimer), 1.0 / 60.0 );
				}			// retryボタンをタップしたら以下処理
			} else if(tag_no == kTag_Retry && i->boundingBox().containsPoint(loc)) {
				GameScene::moveToNextScene();									// moveToNextSceneを呼び出しシーンの再生成
			// stopボタンをタップしたら以下処理
			} else if(tag_no == kTag_Key_Center) {
				this->unschedule(schedule_selector(GameScene::rotateUpAngle));	// 上キーから指が離れた場合は船首上げ関数の呼び出しをストップ
				this->unschedule(schedule_selector(GameScene::rotateDownAngle));
				this->unschedule(schedule_selector(GameScene::forwardUnit));
				this->unschedule(schedule_selector(GameScene::backUnit));
			}
			touch_judge = i->boundingBox().containsPoint(loc);			// タグの座標がタッチされたかの判定を行う
			if(touch_judge) {
				changeButton(tag_no, kTag_changeBegan);
			}
			m_touchFlag[tag_no] = touch_judge;							// tag_no番目の配列にタッチしているかしていないかを代入
			m_touchAt[tag_no] = touch->getLocation();					// 位置を保存
		}
	}
}

/* 関数名 : ccTouchesMoved関数
 * 返却型 : void
 *
 * タップ後スワイプをした際に呼び出される関数
 */
void GameScene::ccTouchesMoved(CCSet* touches, CCEvent* pEvent ) {
	//	CCSprite* Mater4 = 	(CCSprite*)getChildByTag(kTag_Gear4);			// オブジェクトGear4を取得
	float pMeter4 = getCCPoint(kTag_Gear4).y;								// Mater4のy座標を取得
	float pMeter1 = getCCPoint(kTag_Gear1).y;								// meter1のy座標を取得

	//タッチしたボタンの数だけループ
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {
		CCTouch *touch = (CCTouch *)(*it);						// タッチボタンの取得
		tag_no = this->kTag_Key_Up;								// 一番小さい数値が入っているkTag_Key_Upをtag_noに代入

		// tag_noとkTag_Key_Upの差がボタンの合計数未満の間、tag_noを後置インクリメントしながらループ
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			m_touchAt[tag_no]  = touch->getLocation();								// タッチ位置を更新
			CCPoint loc = m_touchAt[tag_no];										// タッチ座標の取得
			CCSprite* pSwitch = (CCSprite*)this->getChildByTag(kTag_Switch);		// オブジェクトkTag_Switchを取得しCCSprite*型変数を初期化

			if(kTag_Key_Up <= tag_no && tag_no <= kTag_Shoot_Horizontal) {
				CCPoint objPoint = getCCPoint(tag_no);					// タップされたオブジェの中心座標を取得
				CCSize objSize = getCCSprite(tag_no)->getContentSize();	// タップされたオブジェの幅と高さを取得
				float obj_left   = objPoint.x - objSize.width / 2;		// オブジェクトの左辺を取得
				float obj_right  = objPoint.x + objSize.width / 2;		//      〃     の右辺を取得
				float obj_bottom = objPoint.y - objSize.height / 2;		//      〃     の下辺を取得
				float obj_top    = objPoint.y + objSize.height / 2;		//      〃     の上辺を取得
				/* タップ時取得座標がオブジェクト外の座標を取得した時
				 * スプライトを非タップ時のスプライトに差し替える
				 */
				if((loc.x < obj_left || obj_right < loc.x)
						|| (loc.y < obj_bottom || obj_top < loc.y)) {
					changeButton(tag_no, kTag_changeEnded);
				}
			}
			//もしtag_noとkTag_Switchが同値であり、かつスイッチオブジェクトをタップしていて、かつタッチ座標がGear4のy座標未満であれば以下ブロック
			if( (tag_no == kTag_Switch && pSwitch->boundingBox().containsPoint(loc) ) && (pMeter1 < loc.y && loc.y < pMeter4)) {
				pSwitch->setPosition(ccp(pSwitch->getPositionX(), loc.y));			// スイッチをx座標は同じ座標、y座標はタッチされた座標にセット
				break;																// ブレイク
			}
		}
	}
}
// スピード倍率を返す関数
float GameScene::coefficientOfSpeed() {
	CCSprite* pSwitch = (CCSprite*)this->getChildByTag(kTag_Switch);				// スイッチのノードを取得
	float pSwitchPointY = pSwitch->getPositionY();
	float coefficientOfSpeed;
	float a = meterPosition[kTag_Gear4 - kTag_Gear1];
	float b = meterPosition[kTag_Gear3 - kTag_Gear1];
	float c = meterPosition[kTag_Gear2 - kTag_Gear1];
	if (pSwitchPointY <= meterPosition[0]) {
		coefficientOfSpeed = 7;
	} else if (pSwitchPointY >= meterPosition[1]) {
		coefficientOfSpeed = 5;
	} else if (pSwitchPointY >= meterPosition[2]) {
		coefficientOfSpeed = 3;
	} else {
		coefficientOfSpeed = 1;
	}
	return coefficientOfSpeed;
}

/* 関数名 : ccTouchesEnded関数
 * 返却型 : void
 *
 * 概要 : タップ中のオブジェクトから指を離した際に呼び出される関数
 */
void GameScene::ccTouchesEnded(CCSet* touches, CCEvent* pEvent ) {

	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {
		CCTouch* touch  = (CCTouch *)(*it);
		bool touch_judge = 0;											// 返り値として渡すためのタッチ判定フラグ
		tag_no = this->kTag_Key_Up;										// 各種ボタンの先頭のタグを取得
		CCPoint loc = touch->getLocation();								// タッチ位置を取得
		// 各ボタンのタッチ判定を繰り返し
		// ボタンの数(button_sum)分繰り返す
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			i = this->getChildByTag(tag_no);							// 各種ハンドルオブジェクトでiを初期化し、タップ可能にする
			touch_judge = i->boundingBox().containsPoint(loc);			// タグの座標がタッチされたかの判定を行う
			if(tag_no == kTag_Key_Up && i->boundingBox().containsPoint(loc)) {
				// 毎フレームrotateUpAngle関数を呼び出すように設定する
				this->schedule(schedule_selector(GameScene::rotateUpAngle), 1.0 / 60.0 );
			} else if(tag_no == kTag_Key_Down && i->boundingBox().containsPoint(loc)) {
				// 毎フレームrotateDownAngle関数を呼び出すように設定する
				this->schedule(schedule_selector(GameScene::rotateDownAngle), 1.0 / 60.0 );
			} else if(tag_no == kTag_Key_Left && i->boundingBox().containsPoint(loc)) {
				// 毎フレームforwardUnit関数を呼び出すように設定する
				this->schedule(schedule_selector(GameScene::forwardUnit), 1.0 / 60.0 );
			} else if(tag_no == kTag_Key_Right && i->boundingBox().containsPoint(loc)) {
				// 毎フレームbackUnit関数を呼び出すように設定する
				this->schedule(schedule_selector(GameScene::backUnit), 1.0 / 60.0 );
			}
			// もしtouch_judgeがtrue(離した判定)であれば以下ブロック
			if(touch_judge) {
				/* タグの番号と終わりを表す値ををもつ変数を引数に
				 * changeButton関数を呼び出す
				 */
				changeButton(tag_no, kTag_changeEnded);
			}
			m_touchFlag[tag_no] = !(touch_judge);		// 各判定を配列に代入していく
		}
	}
}

/* ***********************
 *
 * 	タップ時スプライト変更
 *
 * ***********************/

/* 関数名 : changeButton
 * 返却型 : void(何も返却しない)
 * 仮引数 : int型変数tag_no(tagの番号を受け取る), int型変数change(タップ開始か終了かを受け取る)
 *
 * 概要 : 十字キー、ストップボタン、ミサイル発射ボタンタップ時呼び出される
 * 		　画像を入れ替える
 */
void GameScene::changeButton(int tag_no, int change) {
	/* 仮引数の値が十字キーの上もしくは下もしくは左もしくは右のタグと
	 * 同じ値であった場合以下のブロック
	 */
	if(tag_no == kTag_Key_Up || tag_no == kTag_Key_Down ||
			tag_no == kTag_Key_Left || tag_no == kTag_Key_Right) {
		changeCrosskey(tag_no, change);									// tag_noを引数にし、changeCrosskey関数を呼び出す
		/* 仮引数の値がストップボタンのタグと同じ値であれば
		 * 以下のブロック
		 */
	}else if(tag_no == kTag_Key_Center) {
		changeStopButton(tag_no, change);								// tag_noを引数にし、changeStopButton関数を呼び出す
		/* 仮引数の値がミサイル発射上ボタンかもしくは左ボタンであれば
		 * 以下のブロック
		 */
	}else if(tag_no == kTag_Shoot_Vertical || tag_no == kTag_Shoot_Horizontal) {
		changeMissileButton(tag_no, change);							// tag_noを引数にし、changeMissileButton関数を呼び出す
	}
}



/* 関数名 : changeCrosskey
 * 返却型 : void
 * 仮引数 : int型変数tag_no(tagの番号を受け取る), int型変数change(タップ開始か終了かを受け取る)
 *
 * 概要 : 十字キータップ時画像の入れ替え
 */
void GameScene::changeCrosskey(int tag_no, int change) {
	const char* fileName = change == kTag_changeBegan ? "arrow_tap.png" : "arrow.png";
	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(fileName);
	CCSprite* pushButton = getCCSprite(tag_no);		// tag_noのスプライトをゲット
	pushButton->setPosition(getCCPoint(tag_no));
	pushButton->setRotation(getAngle(tag_no));
	pushButton->setTexture(pTexture);
}

/* 関数名 : changeStopButton
 * 返却型 : void
 * 仮引数 : int型変数tag_no(tagの番号を受け取る), int型変数change(タップ開始か終了かを受け取る)
 *
 * 概要 : ストップボタンタップ時画像の入れ替え
 */
void GameScene::changeStopButton(int tag_no, int change) {
	const char* fileName = change == kTag_changeBegan ? "stop_tap.png" : "stop.png";
	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(fileName);
	CCSprite* pushButton = getCCSprite(tag_no);		// tag_noのスプライトをゲット
	pushButton->setPosition(getCCPoint(tag_no));
	pushButton->setRotation(getAngle(tag_no));
	pushButton->setTexture(pTexture);
}
void GameScene::createRetryButton() {
	CCSprite* pRetry = CCSprite::create("retry.png");
	CCSize size = getCCSprite(kTag_Background)->getContentSize();
	pRetry->setPosition(ccp(size.width * (0.2 - 0.05), size.height / 20 * 19));	//座標をセット x座標はスコア2桁目と同じ座標
	this->addChild(pRetry, kZOrder_Label, kTag_Retry);							//配置順kZOrder_Labelで実装

}
/* 関数名 : changeMissleButton
 * 返却型 : void
 * 仮引数 : int型変数tag_no(tagの番号を受け取る), int型変数change(タップ開始か終了かを受け取る)
 *
 * 概要 : ミサイルタップ時画像の入れ替え
 */
void GameScene::changeMissileButton(int tag_no, int change) {
	const char* fileName = change == kTag_changeBegan ? "MissileButton_tap.png" : "MissileButton.png";
	CCTexture2D *pTexture = CCTextureCache::sharedTextureCache()->addImage(fileName);
	CCSprite* pushButton = getCCSprite(tag_no);		// tag_noのスプライトをゲット
	pushButton->setPosition(getCCPoint(tag_no));
	pushButton->setRotation(getAngle(tag_no));
	pushButton->setTexture(pTexture);
}

void GameScene::missileTimer() {
	reloadTime++;
	if (reloadTime == 50) {
		reloadTime = 0;
		reloadMissile += 3;
		this->unschedule(schedule_selector(GameScene::missileTimer));	// 上キーから指が離れた場合は船首上げ関数の呼び出しをストップ
	}
}
///* ***********************
// *
// * 		その他
// *
// *  ***********************/

// スクロール開始関数
void GameScene::callScroll() {
	float dealofScrollSpead = getdealofScrollSpead();
}

// スクロール倍率を与える
float GameScene::getdealofScrollSpead() {
	return dealofScrollSpead;
}
// ウィンドウサイズをゲットする
CCSize GameScene::getWindowSize() {
	return CCDirector::sharedDirector()->getWinSize();					// ウィンドウサイズを取得
}
// 背景サイズをゲットする
CCSize GameScene::getViewSize() {
	return CCEGLView::sharedOpenGLView()->getFrameSize();				// シーンのサイズを取得
}

// tag_noのオブジェクトのスプライトゲット
CCSprite* GameScene::getCCSprite(int tag_no) {
	return (CCSprite*)getChildByTag(tag_no);
}

// tag_noのオブジェクトの座標をゲット
CCPoint GameScene::getCCPoint(int tag_no) {
	return getChildByTag(tag_no)->getPosition();
}

// tag_noのオブジェクトの角度をゲット
float GameScene::getAngle(int tag_no) {
	return getChildByTag(tag_no)->getRotation();
}


//// カウントダウンの音を取得する
//void GameScene::playCountdownSound() {
//	//SimpleAudioEngineクラスのsharedEngine関数の中のplayEffect関数にmp3(テスト用)をセット
//	SimpleAudioEngine::sharedEngine()->playEffect("countdown.mp3");
//}

void GameScene::explosionSound() {
	//SimpleAudioEngineクラスのsharedEngine関数の中のplayEffect関数にmp3(テスト用)をセット
	SimpleAudioEngine::sharedEngine()->playEffect("explosion.mp3");
}
