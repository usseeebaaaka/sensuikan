#ifndef __GAME_SCENE_H__
#define __GAME_SCENE_H__

#include "cocos2d.h"
#include "Box2D/Box2D.h"
#include "GamePhysicsContactListener.h"
#include "PhysicsSprite.h"

class GameScene : public cocos2d::CCLayer {
    b2World* world;
    GamePhysicsContactListener* mGamePhysicsContactListener;
    float score_and_Maxplace;												// 整数部は得点で小数部は得点の桁
    int arrow_key;
    int tag_no;
    int buttons_sum;
    int lifeup_score;														// 残機アップする得点
    int enemyUnit_num;														// 作成する潜水艦の数
    int player_VIT;															// 自機の耐久度
    int submarine_VIT;														// 潜水艦の耐久度
    int destroyer_VIT;														// 駆逐艦の耐久度
    int lifepoint;															// 残機
    float dealofScrollSpead;												// スクロールスピードの倍率
    CCArray* scoreText;														// スコアの各位のテキスト
    map<int, CCPoint> m_touchAt;											// タッチ座標
    map<int, bool> m_touchFlag;												// タッチフラグ
    map<int, PhysicsSprite*> unitData;										// ユニットのデータ群
    map<int, b2Body*> unitPhysicsData;
    map<int, b2Vec2> unitPhysicsPoint;										// ユニットの物理構造体群
    b2Body* playerUnit;														// 自機の実体
    b2Body* enemyDestroyer;													// 敵駆逐艦の実体
    b2Body* enemySubmarine;													// 敵潜水艦の実体
    CCSpriteBatchNode* missileBatchNode;									// ミサイル群
    CCSpriteBatchNode* hpBatchNode;											// hp群
    CCSpriteBatchNode* lifeBatchNode;										// 残機群
    // 各種オブジェクト等をタグで管理
    enum kTag {
    	kTag_Polygon,				// 角形物理構造
    	kTag_Circle,				// 円形物理構造
    	kTag_DefeatAnimation = 7,	// 撃沈アニメーションのタグ
    	kTag_HitAnimation = 14,		// 被弾アニメーションのタグ
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
        kTag_Key_Center,			// ストップボタン
        kTag_Switch,
        kTag_Shoot_Vertical,		// 垂直射撃ボタン
        kTag_Shoot_Horizontal,		// 水平射撃ボタン
        kTag_Gear1,					// 1番スピードが遅いギア
        kTag_Gear2,					// 2番目にスピードが遅いギア
        kTag_Gear3,					// 3番目にスピードが遅いギア
        kTag_Gear4,					// 4番目にスピードが遅いギア
        kTag_Animation,
        kTag_Call_Scroll = 200,					// スクロール開始フラグ
    };
    // 画像の表示順序のタグ
    enum kZOrder {
    	kZOrder_Background,			// 背景
    	kZOrder_Controller_Base,	// 操作部の下地
    	kZOrder_Label,				// 操作部やラベル
    	kZOrder_Seabed,				// 海底
    	kZOrder_Missile,			// ミサイル
    	kZOrder_Unit,				// 機体
    	kZOrder_Countdown			// カウントダウン等のアニメーション
    };

public:
    GameScene();															// デフォルトコンストラクタ
    // Here's a difference. Method 'init' in cocos2d-x returns bool, instead of returning 'id' in cocos2d-iphone
    virtual bool init();
    void initPhysics();														// 重力世界の初期化
    // there's no 'id' in cpp, so we recommend returning the class instance pointer
    static cocos2d::CCScene* scene();										//関数
    void createBackground();												// 背景および海底を生成
    void createUnit(int hp, int kTag, int vit);				// ユニットを生成
    virtual void update(float dt);											// 毎フレームごとに衝突判定をチェックする関数

    // 物理構造を持ったユニットノードを作成
    PhysicsSprite* createPhysicsBody(int bodyTag, int kTag, PhysicsSprite* pNode, int shape);
    void createScore();														// スコアを生成
    void createLifeCounter();														// 残りhpカウンターを生成
    void createControllerPanel();											// 操作部を生成
    void createKey();															//十字キーを生成
    void showCountdown();													// ゲーム開始時のカウントダウン
    void callScroll();														// スクロール開始
    void rotateUpAngle();														// 船首を上げる
    void rotateDownAngle();														// 船首を下げる
    void forwardUnit();															// 前進する
    void backUnit();															// 後退する
//    void defeatPlayer();													// プレイヤーが撃沈
//    void finishGame();
//    void removeObject(CCNode* pObject, void* body);							// オブジェクトを除去する
    float getdealofScrollSpead();											// スクロールスピードの倍率をゲットする
    CCSize getWindowSize();													// ウィンドウサイズをゲットする
    CCSize getViewSize();													// ビューサイズをゲットする
//    void moveToNextScene();
    void startGame();
    void setScoreNumber();
    virtual void ccTouchesBegan(CCSet* touches, CCEvent* pEvent );			 // タッチ開始時のイベント
    virtual void ccTouchesMoved(CCSet* touches, CCEvent* pEvent );			// スワイプしている途中に呼ばれる
    virtual void ccTouchesEnded(CCSet* touches, CCEvent* pEvent );			// タッチ終了時のイベント
    void destroyerAI();
    void submarineAI();
    void createMissile(b2Vec2 position);
    CREATE_FUNC(GameScene);													//マクロ

    //追加
    void arrowKey();
    void stopButton();
    void speedMater();
    void missileButton();
    void speedSwitch();
    void playCountdownSound();
#define PI 3.141592															// 円周率をマクロ定義
};

#endif // __GAME_SCENE_H__
