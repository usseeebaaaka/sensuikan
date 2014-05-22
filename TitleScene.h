/*
 * TitleScene.h
 *
 *  Created on: 2014/05/21
 *      Author: T.I
 */

#ifndef TITLESCENE_H_
#define TITLESCENE_H_

#include "cocos2d.h"					// cocos2dのヘッダをインクルード
USING_NS_CC;
/*
 * クラス名:TitleScene
 * 概要    :タイトル画面を表示するクラス
 * 継承元  :cocos2d::CCLayer
 * 作成日  :2014.05.21
 * 作成者  :T.I
 */
class TitleScene :  public cocos2d::CCLayer {

	// 各種ノードをタグで管理
	enum kTag {
		kTag_Background,				// 背景
		kTag_StartButton,				// スタートボタン
		kTag_Menu,
	};

	// 画像の表示順序のタグ
	enum kZOrder {
		kZOrder_Background,				// 背景
		kZOrder_Decoration,				// 背景の上に載せる画像
	};

public:
//	TitleScene();										// デフォルトコンストラクタ
	static cocos2d::CCScene* scene();					// 画面の雛型を作成する関数
	virtual bool init();								// 初期化処理を行う関数
	void createBackground();							// 背景画像を生成する関数
	void createStartButton();							// スタートボタンを生成する関数
	void createMenu();									// メニューボタン生成手続き関数
//	void createInfoButton();							// インフォボタンを生成する関数
	void blinkSprite(CCSprite* sprite);					// スプライトを点滅させる関数
    virtual void menuStartCallback(CCObject* pSender);	// スタートボタンの動作を定義する関数
	CCSize getWindowSize();
	CREATE_FUNC(TitleScene);			//マクロ
};
#endif /* TITLESCENE_H_ */
