#include "GameScene.h"
#include "config.h"
#include "AppMacros.h"
#include "GamePhysicsContactListener.h"
#include "SimpleAudioEngine.h"
#include "Animation.h"



using namespace CocosDenshion;
USING_NS_CC;

// コンストラクタ
GameScene::GameScene(){}
//:enemyUnit_num(2),
// player_VIT(2),
// submarine_VIT(202),
// destroyer_VIT(202),
// score_and_Maxplace(0.3),
// buttons_sum(11),
// lifepoint(3){
//	scoreText = new CCArray();
//	srand((unsigned int)time(NULL));
//}

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
	this->setTouchEnabled(true);				  							// タッチ可能にする
	this->setTouchMode(kCCTouchesAllAtOnce);								// マルチタップイベントを受け付ける

	b2Vec2 gravity;										    				// 重力の設定値を格納するための変数
	gravity.Set(0.0, -10.0);												// 重力を設定

	world = new b2World(gravity);											// 重力を持った世界を生成

	mGamePhysicsContactListener = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するための変数を用意
	world->SetContactListener(mGamePhysicsContactListener);				// 衝突判定処理を追加

	createBackground();
	createControllerPanel();
	createKey();

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

	b2Body* seabedBody = world->CreateBody(&seabedBodyDef);					// 重力世界に海底構造体を登録

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

//コントローラ下地を作成
void GameScene::createControllerPanel() {
	CCSize ccs = getWindowSize();
	//コントローラ部を作成
	CCSprite* pControl = CCSprite::create("control.png");					//control.pngをCCSprite型にし、pControlで初期化
	pControl->setPosition(ccp(ccs.width / 2, ccs.height / 8));				//座標のセット
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

	int arrowkey = TAG_ARROWKEY;												//マクロTAG_ARROWKEYでint型変数を初期化
	//先ほどの変数が0より大きい間後置デクリメント
	while(arrowkey-- > 0) {
		CCSprite* pKey = CCSprite::create("tri.png");							//tri.pngをCCSprite型で生成
		switch(arrowkey) {
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
	pMater2->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight));		//座標のセット
	this->addChild(pMater2, kZOrder_Label, kTag_Gear2);							//配置順kZOrder_Labelで実装

	CCSprite* pMater1 = CCSprite::create("Meter1.png");							//Meter1.pngをCCSprite型で生成
	pMater1->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight - pMater2->getContentSize().height ));		//座標のセット
	this->addChild(pMater1, kZOrder_Label, kTag_Gear1);							//配置順kZOrder_Labelで実装

	CCSprite* pMater3 = CCSprite::create("Meter2.png");							//Meter3.pngをCCSprite型で生成
	pMater3->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight + pMater2->getContentSize().height ));		//座標のセット
	this->addChild(pMater3, kZOrder_Label, kTag_Gear3);							//配置順kZOrder_Labelで実装

	CCSprite* pMater4 = CCSprite::create("Meter3.png");							//Meter3.pngをCCSprite型で生成
	pMater4->setPosition(ccp(getWindowSize().width / 8 * 5, stopHeight + (pMater2->getContentSize().height) * 2 ));		//座標のセット
	this->addChild(pMater4, kZOrder_Label, kTag_Gear4);							//配置順kZOrder_Labelで実装

}

//ミサイルボタン表示の定義
void GameScene::missileButton() {
	CCSprite* mater1 = 	(CCSprite*)getChildByTag(kTag_Gear1);						//オブジェクトmater1ボタンを取得
	float mater1Height = mater1->getPositionY();									//stopのy座標を取得

	/*----- missile 上キーの実装 -----*/
	CCSprite* pMissileUp = CCSprite::create("missile_up.png");						//missile_up.pngをCCSprite型で生成
	pMissileUp->setPosition(ccp(getWindowSize().width / 8 * 7, mater1Height * 3));	//座標のセット
	this->addChild(pMissileUp, kZOrder_Label, kTag_Shoot_Vertical);					//配置順kZOrder_Labelで実装

	/*----- missile 左キーの実装 -----*/
	CCSprite* pMissileLeft = CCSprite::create("missile_left.png");					//missile_left.pngをCCSprite型で生成
	pMissileLeft->setPosition(ccp(getWindowSize().width / 8 * 7, mater1Height));	//座標のセット
	this->addChild(pMissileLeft, kZOrder_Label, kTag_Shoot_Horizontal);				//配置順kZOrder_Labelで実装

}

//スピードスイッチ表示の定義
void GameScene::speedSwitch() {
	CCSprite* mater1 = 	(CCSprite*)getChildByTag(kTag_Gear1);					//オブジェクトギア1オブジェクトを取得
	float mater1HeightSize = mater1->getContentSize().height;					//ギア1の高さ取得
	CCSize ccs = mater1->getPosition();											//ギア1の座標を取得

	//速度メーター スイッチ
	CCSprite* pSwitch = CCSprite::create("switch.png");							//switch.pngファイルを取得
	pSwitch->setPosition(ccp(ccs.width, ccs.height - (mater1HeightSize / 20 * 7)));	//座標をセット
	this->addChild(pSwitch, kZOrder_Label, kTag_Switch);						//配置順kZOrder_Labelで実装
}

// タッチ開始時のイベント
void GameScene::touchesBegan(CCSet* touches, CCEvent* pEvent ) {
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {	// タッチ位置を順に探索
		CCTouch* touch  = (CCTouch *)(*it);									// 取得した値をノードと照合するためccTouch型に変換
		bool touch_judge = 0;											// 返り値として渡すためのタッチ判定フラグ
		tag_no = this->kTag_Key_Up;								// 各種ボタンの先頭のタグを取得
		CCPoint loc = touch->getLocation();							// タッチ位置を取得
		// 各ボタンのタッチ判定を繰り返し
		for (CCNode* i; tag_no - this->kTag_Key_Up < buttons_sum; tag_no++) {
			i = this->getChildByTag(tag_no);							// 各種ハンドルオブジェクトでiを初期化し、タップ可能にする
			touch_judge = i->boundingBox().containsPoint(loc);			// タグの座標がタッチされたかの判定を行う
			m_touchFlag[tag_no] = touch_judge;
			m_touchAt[tag_no] = touch->getLocation();
		}
	}
}
// スワイプしている途中に呼ばれる
void GameScene::touchesMoved(CCSet* touches, CCEvent* pEvent ) {

	//タッチしたボタンの数だけループ
	for (CCSetIterator it = touches->begin(); it != touches->end(); ++it) {
		CCTouch *touch = (CCTouch *)(*it);					// タッチボタンの取得
		tag_no = this->getTag();								// タグを取得

		//もし取得したタグとスイッチタグが同値であるならば以下ブロック
		if(tag_no == kTag_Switch) {							//タッチボタンがスイッチであれば以下ブロック
			CCPoint loc = touch->getLocation();										// タッチ座標の取得

			float threshold = CCDirector::sharedDirector()->getWinSize().width * 0.005f;	// しきい値の取得
			CCSprite* pSwitch = (CCSprite*)this->getChildByTag(kTag_Switch);		// スイッチのオブジェクトを取得

			pSwitch->setPosition(ccp(loc.x, pSwitch->getPositionY()));				//スイッチをタップ位置に追従

			m_touchAt[tag_no]  = touch->getLocation();									// タッチ位置を更新
			// 以下、必要な情報を取得し、update内で処理してやると良い
		}
	}
}

// ゲーム開始時のカウントダウン
void GameScene::showCountdown() {

	this->scheduleOnce(schedule_selector(GameScene::playCountdownSound), 0.5);	//playCountdownSound関数を0.5秒後に一度だけ呼び出す

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

// カウントダウンの音を取得する
void GameScene::playCountdownSound() {
	//SimpleAudioEngineクラスのsharedEngine関数の中のplayEffect関数にmp3(テスト用)をセット
	SimpleAudioEngine::sharedEngine()->playEffect("countdown.mp3");
}
// 駆逐艦AI
void GameScene::destroyerAI() {
	CCNode* position_of_destroyer = this->getChildByTag(kTag_EnemyDestroyer);	// ネコ画像のオブジェクトを生成
	CCPoint destroyer_loc = position_of_destroyer->getPosition();				// ネコの現在位置を取得
	//	if(!(rand() % lifepoint / 3)) {
	//		createMissile(destroyer_loc);
	//	} else if(!rand() % 3) {
	//		CCMoveBy* action = CCMoveBy::create(80, ccp(getViewSize().width, 0));
	//		// 向きを反転
	//	}
}
// ゲームスタートの処理を行う
void GameScene::startGame() {

	//敵：潜水艦、駆逐艦のaiを呼び出す
	//this->schedule(schedule_selector(GameScene::destroyerAI));
	//this->schedule(schedule_selector(GameScene::submarineAI));
}

// ウィンドウサイズをゲットする
CCSize GameScene::getWindowSize() {
	return CCDirector::sharedDirector()->getWinSize();					// ウィンドウサイズを取得
}
// 背景サイズをゲットする
CCSize GameScene::getViewSize() {
	return CCEGLView::sharedOpenGLView()->getFrameSize();				// シーンのサイズを取得
}
