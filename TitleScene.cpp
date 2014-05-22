/*
 * TitleScene.cpp
 *
 *  Created on: 2014/05/21
 *      Author: T.I
 */

#include "GameScene.h"
#include "TitleScene.h"					// TitleSceneのヘッダをインクルード
#include "cocos2d.h"
USING_NS_CC;

/*
 * 関数名:CCScene* scene()
 * 概要  :画面の雛型を作成
 * 引数  :なし
 * 戻り値:scene				= 作成した雛型
 * 作成日:2014.05.21
 * 作成者:T.I
 */
CCScene* TitleScene::scene(){
	CCScene* scene = CCScene::create();			// 背景の雛型となるオブジェクトを生成
	TitleScene* layer = TitleScene::create();		// 背景に処理を重ねるためのオブジェクトを生成
	scene->addChild(layer);						// 雛型にレイヤーを埋め込む
	return scene;								// 作成した雛型を返却
}

/*
 * 関数名:bool init()
 * 概要  :初期化処理を行う
 * 引数  :なし
 * 戻り値:bool				= 初期化の成否
 * 作成日:2014.05.21
 * 作成者:T.I
 */
bool TitleScene::init(){
	if(!CCLayer::init()){
		return false;							// シーンの初期化に失敗したらfalseを返す
	}

//	sleep(15);
	createBackground();							// 背景画像を生成
	createStartButton();						// スタートボタンを生成
//	createInfoButton();							// インフォボタンを生成

	return true;								// シーンの初期化に成功したらtrueを返す
}

/*
 * 関数名:void createBackground()
 * 概要  :背景画像を生成
 * 引数  :なし
 * 戻り値:なし
 * 作成日:2014.05.21
 * 作成者:T.I
 */
void TitleScene::createBackground(){
	CCSprite* pBgUnder = CCSprite::create("ssn_d2124_title.png");				// 背景画像を取得し、ノードに格納
	pBgUnder->setPosition(ccp(getWindowSize().width / 2,
			getWindowSize().height / 2));							// 背景画像の中心を画面の中心にセット
	this->addChild(pBgUnder, kZOrder_Background, kTag_Background);	// 背景画像をシーンに登録
}

/*
 * 関数名:void createStartButton()
 * 概要  :スタートボタンを生成
 * 引数  :なし
 * 戻り値:なし
 * 作成日:2014.05.21
 * 作成者:T.I
 */
void TitleScene::createStartButton(){
//	CCSprite* a = CCSprite::create("");
	CCMenuItemImage* pStartButton
	= CCMenuItemImage::create("button_start.png", "button_start_push.png",
			this, menu_selector(TitleScene::menuStartCallback));			// 	スタートボタンに画像と機能を実装
	pStartButton->setPosition(ccp(getWindowSize().width / 2,
			getWindowSize().height / 2));									// スタートボタンの中心を画面の中心にセット
	pStartButton->setTag(kTag_StartButton);									// スタートボタンにタグをセット
    CCMenu* pMenu = CCMenu::create(pStartButton,NULL);
    pMenu->setPosition(CCPointZero);										// 事前にセットしてあるのでここはゼロでよい
    pMenu->setTag(kTag_Menu);
    this->addChild(pMenu, kZOrder_Decoration, kTag_Menu);
}

/*
 * 関数名:void menuStartCallback()
 * 概要  :スタートボタンタップ後の動作を定義
 * 引数  :CCObject* pSender						= メニューセレクタを受け取る
 * 戻り値:なし
 * 作成日:2014.05.22
 * 作成者:T.I
 */
void TitleScene::menuStartCallback(CCObject* pSender){
    CCScene* scene = GameScene::scene();									// 遷移先のシーン関数を指定
    CCTransitionJumpZoom* tran = CCTransitionJumpZoom::create(1, scene);	// 遷移アニメーションを指定
    CCDirector::sharedDirector()->replaceScene(tran);						// 画面遷移を実行
}

// ウィンドウサイズを返却する
CCSize TitleScene::getWindowSize() {
	return CCDirector::sharedDirector()->getWinSize();					// ウィンドウサイズを取得
}
