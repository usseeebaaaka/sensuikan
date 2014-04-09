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
 enemyUnit_num(2),
 player_VIT(2),
 submarine_VIT(202),
 destroyer_VIT(202),
 score_and_Maxplace(0.3),
 dealofScrollSpead(0.2),
 buttons_sum(11),
 lifepoint(3){
	scoreText = new CCArray();
	srand((unsigned int)time(NULL));
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

	b2Vec2 gravity;										    				// 重力の設定値を格納するための変数
	gravity.Set(0.0, 0.0);												// 重力を設定

	world = new b2World(gravity);											// 重力を持った世界を生成

	mGamePhysicsContactListener = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するための変数を用意
	world->SetContactListener(mGamePhysicsContactListener);				// 衝突判定処理を追加

	createBackground();
	// 自機を生成
	createUnit(player_VIT, kTag_PlayerUnit, submarine_VIT, playerUnit);
	// 敵駆逐艦を生成
	createUnit(destroyer_VIT % 100, kTag_EnemyDestroyer, destroyer_VIT, enemyDestroyer);
	createScore();
	createLifeCounter();
	createControllerPanel();
	createKey();

	// ミサイルの準備
	missileBatchNode = CCSpriteBatchNode::create("missile1.png");			// ミサイルの画像をセット
	this->addChild(missileBatchNode);									// ミサイル群をシーンに追加
	// カウントダウン開始
	this->scheduleOnce(schedule_selector(GameScene::showCountdown), 1);

	return true;
}


// 背景および海底を生成
void GameScene::createBackground() {
	CCSprite* pBgUnder = CCSprite::create("morning.png");					// 背景画像を取得し、オブジェクトに格納
	pBgUnder->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));	// 背景画像の中心を画面の中心にセット
	this->addChild(pBgUnder, kZOrder_Background, kTag_Background);			// 背景画像をシーンに登録

	CCNode* pSeabed = CCNode::create();										// 海底を実体を持つオブジェクトとして生成
	pSeabed->setTag(kTag_Seabed);											// 海底をタグと関連付ける
	this->addChild(pSeabed);												// 海底をシーンに登録

	b2BodyDef seabedBodyDef;												// 物理構造を持つ海底構造体を生成
	float seabedBottom = pBgUnder->getContentSize().height / 3;				// 海底とコントローラ部の境界座標を設定
	seabedBodyDef.position.Set(0, seabedBottom);							// 海底の位置をセット
	seabedBodyDef.userData = pSeabed;										// 海底オブジェクトのデータを構造体に登録

	b2Body* seabedBody = world->CreateBody(&seabedBodyDef);				// 重力世界に海底構造体を登録

	float seabedHeight = pBgUnder->getContentSize().height / 10;			// 海底構造体の高さを設定

	// コントローラ部とプレイ部を分けるための変数
	b2EdgeShape seabedBox;
	// 2つの部位の境界線を設定
	seabedBox.Set(b2Vec2(0, (seabedBottom + seabedHeight) / PTM_RATIO),
			b2Vec2(getWindowSize().width / PTM_RATIO, (seabedBottom + seabedHeight) / PTM_RATIO));
	// 指定した境界線と密度を海底オブジェクトに登録
	seabedBody->CreateFixture(&seabedBox, 0);

	CCNode* pBorderline = CCNode::create();									// 境界線を実体を持つオブジェクトとして生成
	pBorderline->setTag(kTag_Borderline);									// 境界線をタグと関連付ける
	this->addChild(pBorderline);											// 境界線をシーンに登録

	b2BodyDef borderlineBodyDef;											// 物理構造を持つ境界線構造体を生成
	borderlineBodyDef.position.Set(0, seabedBottom);						// 境界線の位置をセット
	borderlineBodyDef.userData = pBorderline;								// 境界線オブジェクトのデータを構造体に登録

	b2Body* borderlineBody = world->CreateBody(&borderlineBodyDef);			// 重力世界に境界線構造体を登録

	// 敵の移動範囲を制限するための変数
	b2EdgeShape borderlineBox;
	// 画面の左端より奥に行けなくなるように設定
	borderlineBox.Set(b2Vec2(0, seabedBottom / PTM_RATIO),
			b2Vec2( 0, getWindowSize().height));
	// 指定した境界線と密度を海底オブジェクトに登録
	borderlineBody->CreateFixture(&borderlineBox, 0);
	// 画面の右端より奥に行けなくなるように設定
	borderlineBox.Set(b2Vec2(getWindowSize().width, seabedBottom / PTM_RATIO),
			b2Vec2( getWindowSize().width, getWindowSize().height));
	// 指定した境界線と密度を海底オブジェクトに登録
	borderlineBody->CreateFixture(&borderlineBox, 0);
}


// ユニットを生成する
void GameScene::createUnit(int hp, int kTag, int vit, b2Body* body) {
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
				bgSize.height * 0.35 - (bgSize.height - getWindowSize().height) / 2));
	} else {																		// 潜水艦でない場合
		pUnit->initWithFile("stage1.png");											// 駆逐艦画像を取得し、オブジェクトに格納
		pUnit->setPosition(ccp(bgSize.width * 0.5,									// 任意の位置にオブジェクトをセット
				bgSize.height / 8 * 7 - (bgSize.height - getWindowSize().height) / 2));
	}
	pUnit = createPhysicsBody(kTag_StaticBody, pUnit, body, kTag_Polygon);			// オブジェクトに物理構造を持たせる
	this->addChild(pUnit, kZOrder_Unit, kTag);										// タグとオブジェクトを関連づける
}
// 物理構造を持ったユニットノードを作成
PhysicsSprite* GameScene::createPhysicsBody(int kTag, PhysicsSprite* pNode, b2Body* body, int shape) {
	b2BodyDef physicsBodyDef;														// 物理構造を持ったユニット変数
	if (kTag == kTag_StaticBody) {													// 静的構造が選択された場合
		physicsBodyDef.type = b2_staticBody;										// オブジェクトは静的になる
	} else if (kTag == kTag_DynamicBody) {											// 動的構造が選択された場合
		physicsBodyDef.type = b2_dynamicBody;										// オブジェクトは動的になる
	} else {																		// どちらでもない場合
		physicsBodyDef.type = b2_kinematicBody;										// オブジェクトは運動学的になる
	}

	float width = pNode->getPositionX();
	float height = pNode->getPositionY();
	physicsBodyDef.position.Set(pNode->getPositionX() / PTM_RATIO,					// 物理オブジェクトの位置をセット
			pNode->getPositionY() / PTM_RATIO);
	physicsBodyDef.userData = pNode;												// ノードをデータに登録
	body = world->CreateBody(&physicsBodyDef);										// 物理世界に登録
	b2FixtureDef physicsFixturedef;													// 物理形状を決める変数

	b2PolygonShape PolygonShape;
	b2CircleShape CircleShape;
	if (!shape) {																	// shapeが0の場合
		PolygonShape.SetAsBox(pNode->getContentSize().width * 0.8 / 2 / PTM_RATIO,
				pNode->getContentSize().height * 0.8 / 1 / PTM_RATIO);														// 角形の範囲を設定
	} else {																		// shapeが0でない場合
		CircleShape.m_radius = pNode->getContentSize().width * 0.4 / PTM_RATIO;	// 円形の範囲を設定
	}

	!shape ? physicsFixturedef.shape = &PolygonShape :
			physicsFixturedef.shape = &CircleShape;
	physicsFixturedef.density = 1;													// オブジェクトの密度を設定
	physicsFixturedef.friction = 0.3;												// オブジェクトの摩擦を設定

	body->CreateFixture(&physicsFixturedef);										// 構造体に情報を登録
	pNode->setPhysicsBody(body);													// ノードに構造体を登録
	return pNode;																	// 作成したノードを返却
}


// 残機カウンターを生成
void GameScene::createLifeCounter() {
	if(((CCSprite*)getChildByTag(kTag_LifeCounter))) {
		((CCSprite*)getChildByTag(kTag_LifeCounter))->removeFromParentAndCleanup(true);
	}
	// 画像ファイルをバッチノード化
	CCSpriteBatchNode* pLifeCounterBatchNode = CCSpriteBatchNode::CCSpriteBatchNode::create("hp.png");
	this->addChild(pLifeCounterBatchNode, kZOrder_Label, kTag_LifeCounter);				// タグとノードを関連づけ
	// ライフの数だけ繰り返し
	for (int i = 0; i < lifepoint; i++) {
		// バッチノードから画像を取得してオブジェクト化
		CCSprite* pLifeCounter = CCSprite::createWithTexture(pLifeCounterBatchNode->getTexture());
		// 任意の位置に画像をセット
		pLifeCounter->setPosition(ccp(getWindowSize().width / 10 * (9.5 - i * 0.8),		// スコアノードの位置を設定
				getWindowSize().height / 15 * 13));
		pLifeCounterBatchNode ->addChild(pLifeCounter);		// オブジェクト情報をバッチノードにセット
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
	stopButton();	//ストップボタン生成
	arrowKey();		//十字キー生成
	speedMater();	//速度メータ生成
	missileButton();		//ミサイル発射ボタン生成
	speedSwitch();	//メータスイッチ生成
}

//ストップボタン表示の定義
void GameScene::stopButton() {
	/*----- STOPボタンの実装 -----*/
	CCSprite* pStop = CCSprite::create("stop.png");												//stop.pngをCCSprite型で生成
	pStop->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8));				//座標のセット
	this->addChild(pStop, kZOrder_Label, kTag_Key_Center);										//配置順kZOrder_Labelで実装
}

//十字キー表示の定義
void GameScene::arrowKey() {
	CCSprite* stopSprite = 	(CCSprite*)getChildByTag(kTag_Key_Center);			//オブジェクトstopボタンを取得
	float stopSize = stopSprite->getContentSize().height;						//stopのy座標を取得

	//先ほどの変数が0より大きい間後置デクリメント
	while(arrow_key-- > 0) {
		CCSprite* pKey = CCSprite::create("tri.png");							//tri.pngをCCSprite型で生成
		switch(arrow_key) {
		//上キーの実装
		case 3 : pKey->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8 + stopSize));	//座標のセット
		this->addChild(pKey, kZOrder_Label, kTag_Key_Up);					//配置順kZOrder_Lavelで実装
		break;
		//下キーの実装
		case 2 : pKey->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8 - stopSize));	//座標のセット
		pKey->setRotation(180);													//180度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Down);						//配置順kZOrder_Lavelで実装
		break;
		//左キーの実装
		case 1 : pKey->setPosition(ccp(getWindowSize().width / 4 - stopSize, getWindowSize().height / 8));	//座標のセット
		pKey->setRotation(270);													//270度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Left);						//配置順kZOrder_Lavelで実装
		break;
		//右キーの実装
		case 0 : pKey->setPosition(ccp(getWindowSize().width / 4 + stopSize, getWindowSize().height / 8));	//座標のセット
		pKey->setRotation(90);													//90度回転
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
	pMater2->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight - stopSprite->getContentSize().height / 2));		//座標のセット
	this->addChild(pMater2, kZOrder_Label, kTag_Gear2);							//配置順kZOrder_Labelで実装

	CCSprite* pMater3 = CCSprite::create("Meter2.png");							//Meter3.pngをCCSprite型で生成
	pMater3->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight + stopSprite->getContentSize().height / 2));		//座標のセット
	this->addChild(pMater3, kZOrder_Label, kTag_Gear3);							//配置順kZOrder_Labelで実装

	CCSprite* pMater1 = CCSprite::create("Meter1.png");							//Meter1.pngをCCSprite型で生成
	pMater1->setPosition(ccp(getWindowSize().width / 8 * 5, pMater2->getPositionY() - pMater2->getContentSize().height ));		//座標のセット
	this->addChild(pMater1, kZOrder_Label, kTag_Gear1);							//配置順kZOrder_Labelで実装

	CCSprite* pMater4 = CCSprite::create("Meter3.png");							//Meter3.pngをCCSprite型で生成
	pMater4->setPosition(ccp(getWindowSize().width / 8 * 5, pMater3->getPositionY() + pMater3->getContentSize().height ));		//座標のセット
	this->addChild(pMater4, kZOrder_Label, kTag_Gear4);							//配置順kZOrder_Labelで実装

}

//ミサイルボタン表示の定義
void GameScene::missileButton() {
	CCSprite* Key_Down = 	(CCSprite*)getChildByTag(kTag_Key_Down);				//オブジェクト下キーボタンを取得
	float key_downHeight = Key_Down->getPositionY();								//materのy座標を取得

	/*----- missile 上キーの実装 -----*/
	CCSprite* pMissileUp = CCSprite::create("missile_up.png");						//missile_up.pngをCCSprite型で生成
	pMissileUp->setPosition(ccp(getWindowSize().width / 8 * 7, key_downHeight * 5));	//座標のセット
	this->addChild(pMissileUp, kZOrder_Label, kTag_Shoot_Vertical);					//配置順kZOrder_Labelで実装

	/*----- missile 左キーの実装 -----*/
	CCSprite* pMissileLeft = CCSprite::create("missile_left.png");					//missile_left.pngをCCSprite型で生成
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

	this->setTouchEnabled(true);				  							// タッチ可能にする
	this->setTouchMode(kCCTouchesAllAtOnce);								// マルチタップイベントを受け付ける

	//敵：潜水艦、駆逐艦のaiを呼び出す
	//this->schedule(schedule_selector(GameScene::destroyerAI));
	//this->schedule(schedule_selector(GameScene::submarineAI));
	// 毎フレームupdate( )関数を呼び出すように設定する
	scheduleUpdate();
}


// 自機撃沈関数
void GameScene::defeatPlayer () {

	this->lifepoint--;									// 残機を減らす
	if (lifepoint == 0) {								// 残機がなくなった場合
		finishGame();									// ゲームオーバー
	}
	createLifeCounter();
	//	// 自機を生成
	//	createUnit(player_VIT, kTag_PlayerUnit, submarine_VIT, playerUnit);
}

// オブジェクトを除去する
void GameScene::removeObject(CCNode* pObject, void* body) {
	pObject->removeFromParentAndCleanup(true);			// シーンからオブジェクトを削除する
	world->DestroyBody((b2Body*)body);					// 重力世界からオブジェクトを削除する
}

// 終了時clearもしくはgameoverの画像を表示
void GameScene::finishGame() {

	const char* fileName = lifepoint == 0 ? "gameover.png" : "clear.png";	// lifepointが0であればgameover.pngを、違うならclear.pngで初期化

	// 「Game Over」を表示する
	CCSprite* gameOver = CCSprite::create(fileName);
	gameOver->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));
	gameOver->setScale(0);
	gameOver->runAction(Animation::gameOverAnimation(this, callfunc_selector(GameScene::moveToNextScene)));
	this->addChild(gameOver, kZOrder_Countdown, kTag_Animation);
}

void GameScene::moveToNextScene() {
	// update( )関数の呼び出しを停止する
	unscheduleUpdate();

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

	// world内の全オブジェクトをループする
	for (b2Body* b = world->GetBodyList(); b; b = b->GetNext()) {
		if (!b->GetUserData()) {
			continue;													// オブジェクトが見つからない場合は次のループへ
		}
		CCNode* object = (CCNode*)b->GetUserData();						// オブジェクトを取得
		int objectTag = object->getTag();								// オブジェクトのタグを取得
		if (objectTag == kTag_Call_Scroll) {
			// 機体タグだった場合
		} else if (objectTag >= kTag_PlayerUnit && objectTag <= kTag_Missile) {
			//			if(objectTag ==kTag_EnemySubmarine || objectTag == kTag_EnemyDestroyer ) {
			//				setScoreNumber();
			//			}
			// 被弾したユニットを判別
			PhysicsSprite* damagedUnit = (PhysicsSprite*)this->getChildByTag(objectTag);

			// 被弾したユニットのHPを減らす
			damagedUnit->setHp(damagedUnit->getHp() - 1);
			if (damagedUnit->getHp()) {									// HPが0になっていない場合
				Animation::hitAnimation(kTag_HitAnimation);				// 被弾アニメーションを取得
			} else {													// 0になった場合
				Animation::hitAnimation(kTag_DefeatAnimation);			// 撃沈アニメーションを取得
				if (objectTag == kTag_PlayerUnit) {						// 自機が撃沈した場合
					//					defeatPlayer();										// 自機撃沈関数を呼び出す
					createLifeCounter();								// 残機を再表示
				}else {													// 敵機を撃沈した場合
					//					removeObject(object, (void*)b);						// 撃沈したオブジェクトを削除
					if (objectTag != kTag_Missile) {
						enemyUnit_num--;									// 敵機の数を減らす
						if(!enemyUnit_num) {								// 敵機がなくなった場合
							//							finishGame();									// ゲームクリア
							break;											// 繰り返しから抜ける
						}
					}
				}
			}
		} else if (objectTag == kTag_Collision) {						// 機体同士もしくはプレイヤーが海底に衝突した場合
			//			defeatPlayer();												// 自機が撃沈される
		}
	}
	CCNode* position_of_destroyer = this->getChildByTag(kTag_EnemyDestroyer);	// 駆逐艦画像のオブジェクトを生成
	if (position_of_destroyer) {
		CCPoint destroyer_loc = position_of_destroyer->getPosition();			// 潜水艦の現在位置を取得
	}
	CCNode* position_of_submarine = this->getChildByTag(kTag_EnemySubmarine);	// 潜水艦画像のオブジェクトを生成
	if (position_of_submarine) {
		CCPoint submarine_loc = position_of_submarine->getPosition();			// 潜水艦の現在位置を取得
	}
	if (!(rand() % 30)) {
		destroyerAI();														// ランダムで駆逐艦のAIを呼び出す
	}
	if (!(rand() % 30)) {
		//		submarineAI();														// ランダムで潜水艦のAiを呼び出す
	}
}


// スコア部を生成
void GameScene::createScore() {
	// スコア部分の生成
	scoreText = CCArray::create();														// スコアを入れるための配列を生成
	float j = 0;																		// スコア間隔調整のための変数
	CCLabelTTF* tmp;																	// スコアテキスト格納用変数
	// 任意の桁数だけ繰り返し
	for (int i = score_and_Maxplace * 10; i % 10; i-=1, j += 0.05) {
		i % 10 != 1 ?																	// 配列の最後の要素であるか？
				tmp = CCLabelTTF::create("0", "", NUMBER_FONT_SIZE) :					// でなければ数字表示なし
				tmp = CCLabelTTF::create("0", "", NUMBER_FONT_SIZE);					// であれば数字表示あり
		tmp->setPosition(ccp(getWindowSize().width * (0.1 + j),
				getWindowSize().height * 0.9));											// 座標をセット
		tmp->setColor(ccBLACK);															// フォントカラーをセット
		scoreText->addObject(tmp);														// 配列の末尾にオブジェクトをセット
		this->addChild((CCLabelTTF*)scoreText->lastObject(), kZOrder_Label);			// スコアノードに情報を登録
	}
}

void GameScene::setScoreNumber() {

	int showScore;											// 表示するスコア
	int maxScore = 1;										// 表示可能な最大スコア
	// 繰返し文により、表示可能最大値を算出
	for(int i = score_and_Maxplace * 10; i % 10; i-=1) {
		maxScore *= 10;
	}

	if (score_and_Maxplace > maxScore) {
		showScore = maxScore - 1;
	} else {
		showScore = score_and_Maxplace;
	}

	// 任意の桁数だけ繰り返し
	for (int i = score_and_Maxplace * 10; i % 10; i--) {

		CCString* tmp = (CCString*)scoreText->objectAtIndex(i % 10);	// 配列のインデックスを指定して、その要素を取り出す
		if (showScore * 10 / maxScore == 0) {
			i * 10 != 1 ?
					tmp = CCString::create(" "):
					tmp = CCString::create("%d");
		} else {
			tmp = CCString::create("%d");
		}
		scoreText->addObject(tmp);					// 配列の末尾にオブジェクトをセット
	}
}

// 駆逐艦AI
void GameScene::destroyerAI() {
	//		PhysicsSprite* destroyer = (PhysicsSprite*) this->getChildByTag(kTag_EnemyDestroyer);	// 駆逐艦画像のオブジェクトを生成
	//		CCPoint destroyer_loc = (CCPoint)destroyer->getPosition();			// 駆逐艦の現在位置を取得
	//		b2Vec2 pos = enemyDestroyer->GetPosition();
	//		if(!(rand() %  5)) {							// ランダムでミサイルを発射
	//			createMissile(pos);							// ミサイルを発射
	//		} else if(!(rand() % 2)) {									// ランダムで移動
	//			destroyer->setPositionX(destroyer->getPositionX() + 1);
	//			this->addChild(destroyer, kZOrder_Unit, kTag_EnemyDestroyer);
	//			// 向きを反転
	//		}
}

// ミサイル作成
void GameScene::createMissile(CCPoint point, int kTag) {
	PhysicsSprite* pMissile = new PhysicsSprite(1);										// 物理構造を持った画像オブジェクトを生成
	pMissile->autorelease();
	pMissile->initWithTexture(missileBatchNode->getTexture());						// りんごを指定位置にセット
	pMissile->setPosition(point);													// りんごを指定位置にセット
	this->addChild(pMissile, kZOrder_Missile, kTag_Missile);
	b2Body* missileBody;
	pMissile = createPhysicsBody(kTag_DynamicBody, pMissile, missileBody, kTag_Polygon);		// オブジェクトに物理構造を持たせる
}



/* ***********************
 *
 * 		タッチ処理
 *
 * ***********************/

// タッチ開始時のイベント
void GameScene::ccTouchesBegan(CCSet* touches, CCEvent* pEvent ) {
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {	// タッチ位置を順に探索
		CCTouch* touch  = (CCTouch *)(*it);									// 取得した値をノードと照合するためccTouch型に変換
		bool touch_judge = 0;											// 返り値として渡すためのタッチ判定フラグ
		tag_no = this->kTag_Key_Up;										// 各種ボタンの先頭のタグを取得
		CCPoint loc = touch->getLocation();								// タッチ位置を取得

		// 各ボタンのタッチ判定を繰り返し
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			i = this->getChildByTag(tag_no);							// 各種ハンドルオブジェクトでiを初期化し、タップ可能にする
			if(tag_no == kTag_Key_Up && true == i->boundingBox().containsPoint(loc)) {
				defeatPlayer();										// 自機撃沈関数を呼び出す
				createUnit(player_VIT, kTag_PlayerUnit, submarine_VIT, playerUnit);
			}

			/* tag_noがミサイル発射上ボタンもしくは左ボタンであり、
			 * かつそのオブジェクトの座標をタップしていれば以下のブロック
			 */
			if((tag_no == kTag_Shoot_Vertical || tag_no == kTag_Shoot_Horizontal)
					&& true == i->boundingBox().containsPoint(loc)) {
				CCPoint pPlayerUnit = ((CCSprite*)getChildByTag(kTag_PlayerUnit))->getPosition();		//PlayerUnitのスプライトを取得しその座標で初期化
				createMissile(pPlayerUnit, tag_no);	//自機の座標とタップした発射ボタンを引数にし、createMissile関数を呼び出す
			}
			touch_judge = i->boundingBox().containsPoint(loc);			// タグの座標がタッチされたかの判定を行う
			m_touchFlag[tag_no] = touch_judge;							// tag_no番目の配列にタッチしているかしていないかを代入
			m_touchAt[tag_no] = touch->getLocation();					// 位置を保存
		}
	}
}

// スワイプしている途中に呼ばれる
void GameScene::ccTouchesMoved(CCSet* touches, CCEvent* pEvent ) {
	//	CCSprite* Mater4 = 	(CCSprite*)getChildByTag(kTag_Gear4);			//オブジェクトGear4を取得
	float pMater4 = ((CCSprite*)getChildByTag(kTag_Gear4))->getPositionY();								//Mater4のy座標を取得

	//タッチしたボタンの数だけループ
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {
		CCTouch *touch = (CCTouch *)(*it);						// タッチボタンの取得
		tag_no = this->kTag_Key_Up;								// 一番小さい数値が入っているkTag_Key_Upをtag_noに代入

		// tag_noとkTag_Key_Upの差がボタンの合計数未満の間、tag_noを後置インクリメントしながらループ
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			m_touchAt[tag_no]  = touch->getLocation();								// タッチ位置を更新
			CCPoint loc = m_touchAt[tag_no];										// タッチ座標の取得
			CCSprite* pSwitch = (CCSprite*)this->getChildByTag(kTag_Switch);		// オブジェクトkTag_Switchを取得しCCSprite*型変数を初期化

			//もしtag_noとkTag_Switchが同値であり、かつスイッチオブジェクトをタップしていて、かつタッチ座標がGear4のy座標未満であれば以下ブロック
			if( (tag_no == kTag_Switch && pSwitch->boundingBox().containsPoint(loc) ) && loc.y < pMater4) {
				pSwitch->setPosition(ccp(pSwitch->getPositionX(), loc.y));			// スイッチをx座標は同じ座標、y座標はタッチされた座標にセット
				break;																// ブレイク
			}
		}
	}
}

// タッチ終了時のイベント
void GameScene::ccTouchesEnded(CCSet* touches, CCEvent* pEvent ) {
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {
		CCTouch* touch  = (CCTouch *)(*it);
		bool touch_judge = 0;											// 返り値として渡すためのタッチ判定フラグ
		tag_no = this->kTag_Key_Up;										// 各種ボタンの先頭のタグを取得
		CCPoint loc = touch->getLocation();								// タッチ位置を取得
		// 各ボタンのタッチ判定を繰り返し
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			i = this->getChildByTag(tag_no);							// 各種ハンドルオブジェクトでiを初期化し、タップ可能にする
			touch_judge = i->boundingBox().containsPoint(loc);			// タグの座標がタッチされたかの判定を行う

			m_touchFlag[tag_no] = !(touch_judge);
		}
	}
}


/* ***********************
 *
 * 		その他
 *
 *  ***********************/

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

//// カウントダウンの音を取得する
//void GameScene::playCountdownSound() {
//	//SimpleAudioEngineクラスのsharedEngine関数の中のplayEffect関数にmp3(テスト用)をセット
//	SimpleAudioEngine::sharedEngine()->playEffect("countdown.mp3");
//}


