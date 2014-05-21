/*
 * TitleScene.cpp
 *
 *  Created on: 2014/05/21
 *      Author: T.I
 */

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
	CCSprite* pBgUnder = CCSprite::create("title.png");				// 背景画像を取得し、ノードに格納
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
	CCSprite* pStartButton = CCSprite::create("start.png");			// スタートボタン画像を取得し、ノードに格納
	pStartButton->setPosition(ccp(getWindowSize().width / 2,
			getWindowSize().height / 2));							// スタートボタンの中心を画面の中心にセット
	this->addChild(pStartButton, kZOrder_Decoration, kTag_StartButton);	// スタートボタンをシーンに登録
	CCRepeatForever* rf
	= CCRepeatForever::create(CCBlink::create(0.3f, 2));
	rf->setTag(kTag_StartButton);
	pStartButton->runAction(rf);
	pStartButton->stopActionByTag(kTag_StartButton);
	pStartButton->setVisible(true);
}

// ウィンドウサイズを返却する
CCSize TitleScene::getWindowSize() {
	return CCDirector::sharedDirector()->getWinSize();					// ウィンドウサイズを取得
}
