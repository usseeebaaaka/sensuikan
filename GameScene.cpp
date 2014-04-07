#include "GameScene.h"
#include "config.h"
#include "AppMacros.h"
#include "GamePhysicsContactListener.h"
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
 buttons_sum(11),
 lifepoint(3){
	scoreText = new CCArray();
	srand((unsigned int)time(NULL));
}

CCScene* GameScene::scene() {
	CCScene* scene = CCScene::create();			// 背景の雛型となるオブジェクトを生�E
	GameScene* layer = GameScene::create();		// 背景に処琁E��重�Eるため�Eオブジェクトを生�E
	scene->addChild(layer);						// 雛型にレイヤーを埋め込む

	return scene;								// 作�Eした雛型を返却
}

// ゲーム冁E�E部品を生�Eする関数
bool GameScene::init() {
	if (!CCLayer::init()) {
		return false;														// シーンオブジェクト�E生�Eに失敗したらfalseを返す
	}


	// 
	//	this->setTouchEnabled(true);				  							// タチE��可能にする
	//	this->setTouchMode(kCCTouchesAllAtOnce);								// マルチタチE�Eイベントを受け付けめE	// 修正冁E�� ⇁E	this->setTouchEnabled(true);
	this->setTouchMode(kCCTouchesAllAtOnce);
	// ←ここまで
	b2Vec2 gravity;										    				// 重力の設定値を格納するため�E変数
	gravity.Set(0.0, -10.0);												// 重力を設宁E
	world = new b2World(gravity);											// 重力を持った世界を生戁E
	mGamePhysicsContactListener = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するため�E変数を用愁E	world->SetContactListener(mGamePhysicsContactListener);				// 衝突判定�E琁E��追加

	createBackground();
	// 自機を生�E
	createUnit(player_VIT, kTag_PlayerUnit, submarine_VIT, playerUnit);
	// 敵駁E��艦を生戁E	createUnit(destroyer_VIT % 100, kTag_EnemyDestroyer, destroyer_VIT, enemyDestroyer);
	createScore();
	createLifeCounter();
	createControllerPanel();
	createKey();

	// カウントダウン開姁E	this->scheduleOnce(schedule_selector(GameScene::showCountdown), 1);

	return true;
}


// 背景および海底を生�E
void GameScene::createBackground() {
	CCSprite* pBgUnder = CCSprite::create("morning.png");					// 背景画像を取得し、オブジェクトに格紁E	pBgUnder->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));	// 背景画像�E中忁E��画面の中忁E��セチE��
	this->addChild(pBgUnder, kZOrder_Background, kTag_Background);			// 背景画像をシーンに登録

	CCNode* pSeabed = CCNode::create();										// 海底を実体を持つオブジェクトとして生�E
	pSeabed->setTag(kTag_Seabed);											// 海底をタグと関連付けめE	this->addChild(pSeabed);												// 海底をシーンに登録

	b2BodyDef seabedBodyDef;												// 物琁E��造を持つ海底構造体を生�E
	float seabedBottom = pBgUnder->getContentSize().height / 3;				// 海底とコントローラ部の墁E��座標を設宁E	seabedBodyDef.position.Set(0, seabedBottom);							// 海底�E位置をセチE��
	seabedBodyDef.userData = pSeabed;										// 海底オブジェクト�EチE�Eタを構造体に登録

	b2Body* seabedBody = world->CreateBody(&seabedBodyDef);				// 重力世界に海底構造体を登録

	float seabedHeight = pBgUnder->getContentSize().height / 10;			// 海底構造体�E高さを設宁E
	// コントローラ部とプレイ部を�Eけるための変数
	b2EdgeShape seabedBox;
	// 2つの部位�E墁E��線を設宁E	seabedBox.Set(b2Vec2(0, (seabedBottom + seabedHeight) / PTM_RATIO),
			b2Vec2(getWindowSize().width / PTM_RATIO, (seabedBottom + seabedHeight) / PTM_RATIO));
	// 持E��した墁E��線と寁E��を海底オブジェクトに登録
	seabedBody->CreateFixture(&seabedBox, 0);

	CCNode* pBorderline = CCNode::create();									// 墁E��線を実体を持つオブジェクトとして生�E
	pBorderline->setTag(kTag_Borderline);									// 墁E��線をタグと関連付けめE	this->addChild(pBorderline);											// 墁E��線をシーンに登録

	b2BodyDef borderlineBodyDef;											// 物琁E��造を持つ墁E��線構造体を生�E
	borderlineBodyDef.position.Set(0, seabedBottom);						// 墁E��線�E位置をセチE��
	borderlineBodyDef.userData = pBorderline;								// 墁E��線オブジェクト�EチE�Eタを構造体に登録

	b2Body* borderlineBody = world->CreateBody(&borderlineBodyDef);			// 重力世界に墁E��線構造体を登録

	// 敵の移動篁E��を制限するため�E変数
	b2EdgeShape borderlineBox;
	// 画面の左端より奥に行けなくなるよぁE��設宁E	borderlineBox.Set(b2Vec2(0, seabedBottom / PTM_RATIO),
			b2Vec2( 0, getWindowSize().height));
	// 持E��した墁E��線と寁E��を海底オブジェクトに登録
	borderlineBody->CreateFixture(&borderlineBox, 0);
	// 画面の右端より奥に行けなくなるよぁE��設宁E	borderlineBox.Set(b2Vec2(getWindowSize().width, seabedBottom / PTM_RATIO),
			b2Vec2( getWindowSize().width, getWindowSize().height));
	// 持E��した墁E��線と寁E��を海底オブジェクトに登録
	borderlineBody->CreateFixture(&borderlineBox, 0);
}


// 物琁E��造を持ったユニットノードを作�E
PhysicsSprite* GameScene::createPhysicsBody(int kTag, PhysicsSprite* pNode, b2Body* body, int shape) {
	b2BodyDef physicsBodyDef;														// 物琁E��造を持ったユニット変数
	if (kTag == kTag_StaticBody) {													// 静的構造が選択された場吁E		physicsBodyDef.type = b2_staticBody;										// オブジェクト�E静的になめE	} else if (kTag == kTag_DynamicBody) {											// 動的構造が選択された場吁E		physicsBodyDef.type = b2_dynamicBody;										// オブジェクト�E動的になめE	} else {																		// どちらでもなぁE��吁E		physicsBodyDef.type = b2_kinematicBody;										// オブジェクト�E運動学皁E��なめE	}

	float width = pNode->getPositionX();
	float height = pNode->getPositionY();
	physicsBodyDef.position.Set(pNode->getPositionX() / PTM_RATIO,					// 物琁E��ブジェクト�E位置をセチE��
			pNode->getPositionY() / PTM_RATIO);
	physicsBodyDef.userData = pNode;												// ノ�EドをチE�Eタに登録
	body = world->CreateBody(&physicsBodyDef);										// 物琁E��界に登録
	b2FixtureDef physicsFixturedef;													// 物琁E��状を決める変数

	b2PolygonShape PolygonShape;
	b2CircleShape CircleShape;
	if (!shape) {																	// shapeぁEの場吁E		PolygonShape.SetAsBox(pNode->getContentSize().width * 0.8 / 2 / PTM_RATIO,
				pNode->getContentSize().height * 0.8 / 1 / PTM_RATIO);														// 角形の篁E��を設宁E	} else {																		// shapeぁEでなぁE��吁E		CircleShape.m_radius = pNode->getContentSize().width * 0.4 / PTM_RATIO;	// 冁E��の篁E��を設宁E	}

	!shape ? physicsFixturedef.shape = &PolygonShape :
			 physicsFixturedef.shape = &CircleShape;
	physicsFixturedef.density = 1;													// オブジェクト�E寁E��を設宁E	physicsFixturedef.friction = 0.3;												// オブジェクト�E摩擦を設宁E
	body->CreateFixture(&physicsFixturedef);										// 構造体に惁E��を登録
	pNode->setPhysicsBody(body);													// ノ�Eドに構造体を登録
	return pNode;																	// 作�Eしたノ�Eドを返却
}


// ユニットを生�Eする
void GameScene::createUnit(int hp, int kTag, int vit, b2Body* body) {
	CCSize bgSize = getChildByTag(kTag_Background)->getContentSize();				// 背景サイズを取征E	PhysicsSprite* pUnit = new PhysicsSprite(hp);									// 物琁E��造を持った画像オブジェクトを生�E
	pUnit->autorelease();															// メモリ領域を開放
	if (vit == submarine_VIT) {														// 潜水艦の場吁E		if (kTag == kTag_PlayerUnit) {												// プレイヤーユニット�E場吁E			pUnit->initWithFile("player.png");
			pUnit->setPosition(ccp(bgSize.width * 0.8,								// 任意�E位置にオブジェクトをセチE��
					bgSize.height * 0.35 - (bgSize.height - getWindowSize().height) / 2));
		} else {																	// プレイヤーの潜水艦でなぁE��吁E			pUnit->initWithFile("stage2.png");
			pUnit->setPosition(ccp(bgSize.width * 0.2,								// 任意�E位置にオブジェクトをセチE��
					bgSize.height * 0.35 - (bgSize.height - getWindowSize().height) / 2));
		}
	} else {																		// 潜水艦でなぁE��吁E		pUnit->initWithFile("stage1.png");										// 駁E��艦画像を取得し、オブジェクトに格紁E		pUnit->setPosition(ccp(bgSize.width * 0.5,									// 任意�E位置にオブジェクトをセチE��
				bgSize.height / 8 * 7 - (bgSize.height - getWindowSize().height) / 2));
	}
	pUnit = createPhysicsBody(kTag_DynamicBody, pUnit, body, kTag_Polygon);		// オブジェクトに物琁E��造を持たせめE	this->addChild(pUnit, kZOrder_Unit, kTag);										// タグとオブジェクトを関連づける
}

// スコア部を生戁Evoid GameScene::createScore() {
	// スコア部刁E�E生�E
	scoreText = CCArray::create();														// スコアを�Eれるための配�Eを生戁E	float j = 0;																		// スコア間隔調整のための変数
	CCLabelTTF* tmp;																	// スコアチE��スト格納用変数
	// 任意�E桁数だけ繰り返し
	for (int i = score_and_Maxplace * 10; i % 10; i--, j = j + 0.05) {
		i * 10 != 1 ?																	// 配�Eの最後�E要素であるか！E				tmp = CCLabelTTF::create("1", "", NUMBER_FONT_SIZE) :					// でなければ数字表示なぁE				tmp = CCLabelTTF::create("0", "", NUMBER_FONT_SIZE);					// であれば数字表示あり
		tmp->setPosition(ccp(getWindowSize().width * (0.1 + j),
				getWindowSize().height * 0.9));											// 座標をセチE��
		tmp->setColor(ccBLACK);															// フォントカラーをセチE��
		scoreText->addObject(tmp);														// 配�Eの末尾にオブジェクトをセチE��
		this->addChild((CCLabelTTF*)scoreText->lastObject(), kZOrder_Label);			// スコアノ�Eドに惁E��を登録
	}
}

// 残機カウンターを生戁Evoid GameScene::createLifeCounter() {
	// 画像ファイルをバチE��ノ�Eド化
	CCSpriteBatchNode* pLifeCounterBatchNode = CCSpriteBatchNode::CCSpriteBatchNode::create("hp.png");
	this->addChild(pLifeCounterBatchNode, kZOrder_Label, kTag_LifeCounter);			// タグとノ�Eドを関連づぁE	// ライフ�E数だけ繰り返し
	for (int i = 0; i < lifepoint; i++) {
		// バッチノードから画像を取得してオブジェクト化
		CCSprite* pLifeCounter = CCSprite::createWithTexture(pLifeCounterBatchNode->getTexture());
		// 任意�E位置に画像をセチE��
		pLifeCounter->setPosition(ccp(getWindowSize().width / 10 * 9 - (i-1)*5,		// スコアノ�Eド�E位置を設宁E				getWindowSize().height / 15 * 13));
		pLifeCounterBatchNode ->addChild(pLifeCounter);		// オブジェクト情報をバチE��ノ�EドにセチE��
	}
}

//コントローラ下地を作�E
void GameScene::createControllerPanel() {
	CCSize ccs = getWindowSize();
	//コントローラ部を作�E
	CCSprite* pControl = CCSprite::create("control.png");					//control.pngをCCSprite型にし、pControlで初期匁E	pControl->setPosition(ccp(ccs.width / 2, ccs.height / 8));				//座標�EセチE��
	this->addChild(pControl, kZOrder_Controller_Base);						//配置頁EZOrder_Controller_Baseで実裁E}

//コントローラ部全ての画像を生�E
void GameScene::createKey() {
	stopButton();	//ストップ�Eタン生�E
	arrowKey();		//十字キー生�E
	speedMater();	//速度メータ生�E
	missileButton();		//ミサイル発封E�Eタン生�E
	speedSwitch();	//メータスイチE��生�E
	testSprite();	//チE��ト用　最後消す
}

//ストップ�Eタン表示の定義
void GameScene::stopButton() {
	/*----- STOPボタンの実裁E-----*/
	CCSprite* pStop = CCSprite::create("stop.png");												//stop.pngをCCSprite型で生�E
	pStop->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8));				//座標�EセチE��
	this->addChild(pStop, kZOrder_Label, kTag_Key_Center);										//配置頁EZOrder_Labelで実裁E}

//十字キー表示の定義
void GameScene::arrowKey() {
	CCSprite* stopSprite = 	(CCSprite*)getChildByTag(kTag_Key_Center);			//オブジェクチEtopボタンを取征E	float stopSize = stopSprite->getContentSize().height;						//stopのy座標を取征E
	//先ほどの変数ぁEより大きい間後置チE��リメンチE	while(arrow_key-- > 0) {
		CCSprite* pKey = CCSprite::create("tri.png");							//tri.pngをCCSprite型で生�E
		switch(arrow_key) {
		//上キーの実裁E		case 3 : pKey->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8 + stopSize));	//座標�EセチE��
		this->addChild(pKey, kZOrder_Label, kTag_Key_Up);					//配置頁EZOrder_Lavelで実裁E		break;
		//下キーの実裁E		case 2 : pKey->setPosition(ccp(getWindowSize().width / 4, getWindowSize().height / 8 - stopSize));	//座標�EセチE��
		pKey->setRotation(180);													//180度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Down);						//配置頁EZOrder_Lavelで実裁E		break;
		//左キーの実裁E		case 1 : pKey->setPosition(ccp(getWindowSize().width / 4 - stopSize, getWindowSize().height / 8));	//座標�EセチE��
		pKey->setRotation(270);													//270度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Left);						//配置頁EZOrder_Lavelで実裁E		break;
		//右キーの実裁E		case 0 : pKey->setPosition(ccp(getWindowSize().width / 4 + stopSize, getWindowSize().height / 8));	//座標�EセチE��
		pKey->setRotation(90);													//90度回転
		this->addChild(pKey, kZOrder_Label, kTag_Key_Right);					//配置頁EZOrder_Lavelで実裁E		break;
		}

	}
}

//スピ�Eドメータ表示の定義
void GameScene::speedMater() {

	CCSprite* stopSprite = 	(CCSprite*)getChildByTag(kTag_Key_Center);			//オブジェクチEtopボタンを取征E	float stopHeight = stopSprite->getPositionY();								//stopのy座標を取征E
	CCSprite* pMater2 = CCSprite::create("Meter2.png");							//Meter2.pngをCCSprite型で生�E
	pMater2->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight));		//座標�EセチE��
	this->addChild(pMater2, kZOrder_Label, kTag_Gear2);							//配置頁EZOrder_Labelで実裁E
	CCSprite* pMater1 = CCSprite::create("Meter1.png");							//Meter1.pngをCCSprite型で生�E
	pMater1->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight - pMater2->getContentSize().height ));		//座標�EセチE��
	this->addChild(pMater1, kZOrder_Label, kTag_Gear1);							//配置頁EZOrder_Labelで実裁E
	CCSprite* pMater3 = CCSprite::create("Meter2.png");							//Meter3.pngをCCSprite型で生�E
	pMater3->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight + pMater2->getContentSize().height ));		//座標�EセチE��
	this->addChild(pMater3, kZOrder_Label, kTag_Gear3);							//配置頁EZOrder_Labelで実裁E
	CCSprite* pMater4 = CCSprite::create("Meter3.png");							//Meter3.pngをCCSprite型で生�E
	pMater4->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight + (pMater2->getContentSize().height) * 2 ));		//座標�EセチE��
	this->addChild(pMater4, kZOrder_Label, kTag_Gear4);							//配置頁EZOrder_Labelで実裁E
}

//ミサイルボタン表示の定義
void GameScene::missileButton() {
	CCSprite* mater1 = 	(CCSprite*)getChildByTag(kTag_Gear1);						//オブジェクチEater1ボタンを取征E	float mater1Height = mater1->getPositionY();									//stopのy座標を取征E
	/*----- missile 上キーの実裁E-----*/
	CCSprite* pMissileUp = CCSprite::create("missile_up.png");						//missile_up.pngをCCSprite型で生�E
	pMissileUp->setPosition(ccp(getWindowSize().width / 8 * 7, mater1Height * 3));	//座標�EセチE��
	this->addChild(pMissileUp, kZOrder_Label, kTag_Shoot_Vertical);					//配置頁EZOrder_Labelで実裁E
	/*----- missile 左キーの実裁E-----*/
	CCSprite* pMissileLeft = CCSprite::create("missile_left.png");					//missile_left.pngをCCSprite型で生�E
	pMissileLeft->setPosition(ccp(getWindowSize().width / 8 * 7, mater1Height));	//座標�EセチE��
	this->addChild(pMissileLeft, kZOrder_Label, kTag_Shoot_Horizontal);				//配置頁EZOrder_Labelで実裁E
}

//スピ�EドスイチE��表示の定義
void GameScene::speedSwitch() {
	CCSprite* mater1 = 	(CCSprite*)getChildByTag(kTag_Gear1);					//オブジェクトギア1オブジェクトを取征E	float mater1HeightSize = mater1->getContentSize().height;					//ギア1の高さ取征E	CCSize ccs = mater1->getPosition();											//ギア1の座標を取征E
	//速度メーター スイチE��
	CCSprite* pSwitch = CCSprite::create("switch.png");							//switch.pngファイルを取征E	pSwitch->setPosition(ccp(ccs.width, ccs.height - (mater1HeightSize / 20 * 7)));	//座標をセチE��
	this->addChild(pSwitch, kZOrder_Label, kTag_Switch);						//配置頁EZOrder_Labelで実裁E}

// ゲーム開始時のカウントダウン
void GameScene::showCountdown() {

//	this->scheduleOnce(schedule_selector(GameScene::playCountdownSound), 0.5);	//playCountdownSound関数めE.5秒後に一度だけ呼び出ぁE
	// 、E」�E表示
	CCSprite* pNum3 = CCSprite::create("count3.png");									//count3.pngを取征E	pNum3->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));	//座標を中忁E��せっと
	pNum3->setScale(0);																	//何もなぁE��態から表示するから表示倍率0倍でセチE��
	pNum3->runAction(Animation::num3Animation());										//pNum3にnum3AnimationアニメーションをセチE��
	this->addChild(pNum3, kZOrder_Countdown);											//オブジェクチENum3の実裁E
	// 、E」�E表示
	CCSprite* pNum2 = CCSprite::create("count2.png");									//count2.pngを取征E	pNum2->setPosition(pNum3->getPosition());											//pNum3と同じ座標にセチE��
	pNum2->setScale(0);																	//pNum3同様倍率0倍でセチE��
	pNum2->runAction(Animation::num2Animation());										//pNum2にnum2AnimationアニメーションをセチE��
	this->addChild(pNum2, kZOrder_Countdown);											//オブジェクチENum2の実裁E
	// 、E」�E表示
	CCSprite* pNum1 = CCSprite::create("count1.png");									//count1.pngを取征E	pNum1->setPosition(pNum3->getPosition());											//pNum3と同じ座標にセチE��
	pNum1->setScale(0);																	//pNum3同様倍率0倍でセチE��
	pNum1->runAction(Animation::num1Animation());										//pNum1にnum1AnimationアニメーションをセチE��
	this->addChild(pNum1, kZOrder_Countdown);											//オブジェクチENum2の実裁E
	// 「GO」�E表示
	CCSprite* pGo = CCSprite::create("go.png");											//go.pngを取征E	pGo->setPosition(pNum3->getPosition());												//pNum3と同じ座標にセチE��
	pGo->setScale(0);																	//pNum3同様倍率0倍でセチE��
	/* pGoにgameStartAnimationアニメーションをセチE��
	 * アニメーション終亁E��に呼び出す関数GameSceneクラスのstartGame関数を第二引数に持たぁE	 */
	pGo->runAction(Animation::gameStartAnimation(this, callfunc_selector(GameScene::startGame)));
	this->addChild(pGo, kZOrder_Countdown);												//オブジェクチEGoの実裁E

	/* チE��ト用ヒットアニメーション
	 * GO表示の後に画面中忁E��爁E��
	 */
	CCSprite* pHit = (CCSprite*)this->getChildByTag(kTag_Test);
	pHit->runAction(Animation::hitAnimation(kTag_HitAnimation));
	this->addChild(pHit, kZOrder_Countdown);
}

//ストップ�Eタン表示の定義
void GameScene::testSprite() {
	/*----- STOPボタンの実裁E-----*/
	CCSprite* ptest = CCSprite::create("player.png");												//stop.pngをCCSprite型で生�E
	ptest->setPosition(ccp(getWindowSize().width * 0.5, getWindowSize().height * 0.5));				//座標�EセチE��
	this->addChild(ptest, kZOrder_Label, kTag_Test);										//配置頁EZOrder_Labelで実裁E}


//タチE��座標�E取得かめE
// タチE��開始時のイベンチE// タチE��開始時のイベンチEvoid GameScene::ccTouchesBegan(CCSet* touches, CCEvent* pEvent ) {
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {	// タチE��位置を頁E��探索
		CCTouch* touch  = (CCTouch *)(*it);									// 取得した値をノードと照合するためccTouch型に変換
		bool touch_judge = 0;											// 返り値として渡すため�EタチE��判定フラグ
		tag_no = this->kTag_Key_Up;								// 吁E��ボタンの先頭のタグを取征E		CCPoint loc = touch->getLocation();							// タチE��位置を取征E		// 吁E�EタンのタチE��判定を繰り返し
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			i = this->getChildByTag(tag_no);							// 吁E��ハンドルオブジェクトでiを�E期化し、タチE�E可能にする
			touch_judge = i->boundingBox().containsPoint(loc);			// タグの座標がタチE��されたかの判定を行う
			m_touchFlag[tag_no] = touch_judge;
			m_touchAt[tag_no] = touch->getLocation();
		}
	}
}

//// カウントダウンの音を取得すめE//void GameScene::playCountdownSound() {
//	//SimpleAudioEngineクラスのsharedEngine関数の中のplayEffect関数にmp3(チE��ト用)をセチE��
//	SimpleAudioEngine::sharedEngine()->playEffect("countdown.mp3");
//}

// ゲームスタート�E処琁E��行う
void GameScene::startGame() {


	//敵�E�潜水艦、E��E��艦のaiを呼び出ぁE	//this->schedule(schedule_selector(GameScene::destroyerAI));
	//this->schedule(schedule_selector(GameScene::submarineAI));
}

// ウィンドウサイズをゲチE��する
CCSize GameScene::getWindowSize() {
	return CCDirector::sharedDirector()->getWinSize();					// ウィンドウサイズを取征E}
// 背景サイズをゲチE��する
CCSize GameScene::getViewSize() {
	return CCEGLView::sharedOpenGLView()->getFrameSize();				// シーンのサイズを取征E}
