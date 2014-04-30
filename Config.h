/*
 * config.h
 *
 *  Created on: 2014/03/19
 *      Author: boku
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define PTM_RATIO 128			// 128ピクセルで1メートルの変換


#define TAG_BORDERLINE 28				// 画面枠のタグ
#define TAG_SEABED 29					// 海底のタグ
#define TAG_PLAYER_UNIT 30 				// 自機のタグ
#define TAG_SUBMARINE_UNIT 31 			// 敵機のタグ
#define TAG_DESTROYER_UNIT 32 			// 敵機のタグ
#define TAG_MISSILE 33 					// 自機のミサイルのタグ
#define TAG_MISSILE_ENEMY 34			// 敵機のミサイルのタグ
#define TAG_COLLISION_PLAYER 35			// 追突タグ
#define TAG_COLLISION_SUBMARINE 36		// 敵潜水艦の衝突
#define TAG_COLLISION_DESTROYER 37		// 敵駆逐艦の衝突
#define TAG_REMOVE_MISSILE 38			// ミサイル消失タグ
#define TAG_CALL_SCROLL 200				// スクロール開始タグ
#endif /* CONFIG_H_ */
