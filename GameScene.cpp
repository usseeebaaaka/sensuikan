#include "GameScene.h"
#include "Config.h"
#include "AppMacros.h"

USING_NS_CC;
// コンストラクタ
GameScene::GameScene()
:score_and_Maxplace(0.3),
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

	sleep(20);
	this->setTouchEnabled(true);				  							// タッチ可能にする
	this->setTouchMode(kCCTouchesAllAtOnce);								// マルチタップイベントを受け付ける

	b2Vec2 gravity;										    				// 重力の設定値を格納するための変数
	gravity.Set(0.0, -10.0);												// 重力を設定

	world = new b2World(gravity);											// 重力を持った世界を生成

	mGamePhysicsContactListener = new GamePhysicsContactListener(this);		// 衝突判定結果を格納するための変数を用意
	world->SetContactListener(mGamePhysicsContactListener);					// 衝突判定処理を追加

	createBackground();														// 背景および海底を生成
	createScore();															// スコアラベルを生成
	createLifeCounter();													// 残機ラベルを生成

	return true;
}
// 背景および海底を生成
void GameScene::createBackground() {
	CCSprite* pBgUnder = CCSprite::create("morning.png");					// 背景画像を取得し、オブジェクトに格納
	pBgUnder->setPosition(ccp(getWindowSize().width / 2, getWindowSize().height / 2));	// 背景画像の中心を画面の中心にセット
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
// スコア部を生成
void GameScene::createScore() {
	// スコア部分の生成
	scoreText = CCArray::create();														// スコアを入れるための配列を生成
	float j = 0;																		// スコア間隔調整のための変数
	CCLabelTTF* tmp;																	// スコアテキスト格納用変数
	// 任意の桁数だけ繰り返し
	for (int i = score_and_Maxplace * 10; i % 10; i--, j = j + 0.05) {
		i * 10 != 1 ?																	// 配列の最後の要素であるか？
				tmp = CCLabelTTF::create(" ", "", NUMBER_FONT_SIZE) :					// でなければ数字表示なし
				tmp = CCLabelTTF::create("0", "", NUMBER_FONT_SIZE);					// であれば数字表示あり
		tmp->setPosition(ccp(getWindowSize().width * (0.1 + j),
				getWindowSize().height * 0.9));											// 座標をセット
		tmp->setColor(ccBLACK);															// フォントカラーをセット
		scoreText->addObject(tmp);														// 配列の末尾にオブジェクトをセット
		this->addChild((CCLabelTTF*)scoreText->lastObject(), kZOrder_Label);			// スコアノードに情報を登録
	}
}
// 残機カウンターを生成
void GameScene::createLifeCounter() {
	// ooooo
	// 画像ファイルをバッチノード化
	CCSpriteBatchNode* pLifeCounterBatchNode = CCSpriteBatchNode::CCSpriteBatchNode::create("hit1.png");
	this->addChild(pLifeCounterBatchNode, kZOrder_Label, kTag_LifeCounter);			// タグとノードを関連づけ
	// ライフの数だけ繰り返し
	for (int i = 0; i < lifepoint; i++) {
		// バッチノードから画像を取得してオブジェクト化
		CCSprite* pLifeCounter = CCSprite::createWithTexture(pLifeCounterBatchNode->getTexture());
		// 任意の位置に画像をセット
		pLifeCounter->setPosition(ccp(getWindowSize().width - pLifeCounter->getContentSize().width / 2 - 0.225 * i,		// スコアノードの位置を設定
				getWindowSize().width - pLifeCounter->getContentSize().width / 2));
		pLifeCounterBatchNode ->addChild(pLifeCounter);		// オブジェクト情報をバッチノードにセット
	}
}
// ウィンドウサイズをゲットする
CCSize GameScene::getWindowSize() {
	return CCDirector::sharedDirector()->getWinSize();					// ウィンドウサイズを取得
}
// 背景サイズをゲットする
CCSize GameScene::getViewSize() {
	return CCEGLView::sharedOpenGLView()->getFrameSize();				// シーンのサイズを取得
}
