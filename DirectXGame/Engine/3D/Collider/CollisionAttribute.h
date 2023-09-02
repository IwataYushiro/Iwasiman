#pragma once
///<summary>
///当たり判定属性
///</summary>
const unsigned short COLLISION_ATTR_LANDSHAPE = 0b1 << 0; //地形
const unsigned short COLLISION_ATTR_PLAYERS= 0b1 << 1;//味方陣営
const unsigned short COLLISION_ATTR_ENEMYS = 0b1 << 2;//敵陣営
const unsigned short COLLISION_ATTR_GOAL = 0b1 << 3;//ゴール
const unsigned short COLLISION_ATTR_ITEM = 0b1 << 4;//アイテム
const unsigned short COLLISION_ATTR_GIMMICK = 0b1 << 5;//仕掛け


//サブ属性
const unsigned short SUBCOLLISION_ATTR_NONE = (0b1 << 15) + 1;//サブ属性なし
//弾
const unsigned short SUBCOLLISION_ATTR_BULLET = (0b1 << 15) + 2;//弾

//アイテム属性
const unsigned short SUBCOLLISION_ATTR_ITEM_JUMP = (0b1 << 15) + 11;//ジャンプアイテム
const unsigned short SUBCOLLISION_ATTR_ITEM_HEAL = (0b1 << 15) + 12;//回復アイテム
const unsigned short SUBCOLLISION_ATTR_ITEM_ATTACK = (0b1 << 15) + 13;//攻撃力強化アイテム


//仕掛けの種類
const unsigned short SUBCOLLISION_ATTR_GIMMICK_SPIKE = (0b1 << 15) + 100;//トゲ
const unsigned short SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE = (0b1 << 15) + 101;//乗ると落ちる球(降りても落下しつづける)
const unsigned short SUBCOLLISION_ATTR_GIMMICK_FALLSPHERE_RETURN = (0b1 << 15) + 102;//乗ると落ちる球(降りると元の位置に戻る)
const unsigned short SUBCOLLISION_ATTR_GIMMICK_UPSPHERE = (0b1 << 15) + 103;//乗ると上がる球(降りても上昇し続ける)
const unsigned short SUBCOLLISION_ATTR_GIMMICK_UPSPHERE_RETURN = (0b1 << 15) + 104;//乗ると上がる球(降りると元の位置に戻る)


