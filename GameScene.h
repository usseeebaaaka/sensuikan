#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GamePhysicsContactListener.h"

class GameScene : public cocos2d::CCLayer {
    GameScene();
    int lifepoint;															// 残機
    float score_and_Maxplace;												// 整数部は得点で小数部は得点の桁
    CCArray* scoreText;														// スコアの各位のテキスト
    b2World* world;
    GamePhysicsContactListener* mGamePhysicsContactListener;
    // 各種オブジェクト等をタグで管理
    enum kTag {
    	kTag_Polygon,				// 角形物理構造
    	kTag_Circle,				// 円形物理構造
    	kTag_HitAnimation = 9,		// 被弾アニメーションのタグ
    	kTag_DefeatAnimation = 12,	// 撃沈アニメーションのタグ
    	kTag_StaticBody,			// 静的なボディ
    	kTag_DynamicBody,			// 動的なボディ
    	kTag_KinematicBody,			// 運動学的ボディ
        kTag_Background,			// 背景
        kTag_Seabed,				// 海底
        kTag_Borderline,			// 画面限界
        kTag_Score,					// 得点
        kTag_LifeCounter,			// 残機
        kTag_PlayerUnit,			// 自機
        kTag_EnemySubmarine,		// 敵潜水艦
        kTag_EnemyDestroyer,		// 敵駆逐艦
        kTag_Missile,				// ミサイル
        kTag_Collision,				// 衝突
        kTag_Key_Up,				// 上キー
        kTag_Key_Down,				// 下キー
        kTag_Key_Left,				// 左キー
        kTag_Key_Right,				// 右キー
        kTag_Key_Center,
        kTag_Switch,
        kTag_Shoot_Vertical,		// 垂直射撃ボタン
        kTag_Shoot_Horizontal,		// 水平射撃ボタン
        kTag_Gear1,					// 1番スピードが遅いギア
        kTag_Gear2,					// 2番目にスピードが遅いギア
        kTag_Gear3,					// 3番目にスピードが遅いギア
        kTag_Gear4,					// 4番目にスピードが遅いギア
        kTag_Animation,
    };
    // 画像の表示順序のタグ
    enum kZOrder {
    	kZOrder_Background,			// 背景
    	kZOrder_Seabed,				// 海底
    	kZOrder_Missile,			// ミサイル
    	kZOrder_Unit,				// 機体
    	kZOrder_Controller_Base,	// 操作部の下地
    	kZOrder_Label,				// 操作部やラベル
    	kZOrder_Countdown			// カウントダウン等のアニメーション
    };
public:
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();
    void createBackground();														// 背景および海底を生成
    void createScore();														// スコアを生成
    void createLifeCounter();												// 残機カウンターを生成
//    void createKey();
//    void arrowKey();
//    void stopButton();
//    void speedMater();
//    void missileButton();
//    void speedSwitch();
//    void playCountdownSound();
    CCSize getWindowSize();													// ウィンドウサイズをゲットする
    CCSize getViewSize();													// ビューサイズをゲットする
    // implement the "static node()" method manually
    CREATE_FUNC(GameScene);
};

#endif // __GAME_SCENE_H__
