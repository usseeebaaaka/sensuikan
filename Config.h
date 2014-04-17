/*
 * config.h
 *
 *  Created on: 2014/03/19
 *      Author: boku
 */

#ifndef CONFIG_H_
#define CONFIG_H_

#define PTM_RATIO 128			// 128ピクセルで1メートルの変換


#define TAG_BORDERLINE 28			// 画面枠のタグ
#define TAG_SEABED 29				// 海底のタグ
#define TAG_PLAYER_UNIT 30 		// 自機のタグ
#define TAG_ENEMY_SUBMARINE 31 		// 敵機のタグ
#define TAG_ENEMY_DESTROYER 32 		// 敵機のタグ
#define TAG_MISSILE 33 			// ミサイルのタグ
#define TAG_COLLISION_PLAYER 34			// 自機どっかんタグ
#define TAG_COLLISION_SUBMARINE 35	// 潜水艦どっかんタグ
#define TAG_COLLISION_DESTROYER 36	// 駆逐艦どっかんタグ
#define TAG_REMOVE_MISSILE 37	// ミサイル消失タグ
#define TAG_CALL_SCROLL 200			// スクロール開始タグ
#endif /* CONFIG_H_ */
