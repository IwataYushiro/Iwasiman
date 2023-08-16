#pragma once
///<summary>
///当たり判定属性
///</summary>
const unsigned short COLLISION_ATTR_LANDSHAPE = 0b1 << 0; //地形
const unsigned short COLLISION_ATTR_PLAYERS= 0b1 << 1;//味方陣営
const unsigned short COLLISION_ATTR_ENEMYS = 0b1 << 2;//敵陣営
const unsigned short COLLISION_ATTR_GOAL = 0b1 << 3;//ゴール
const unsigned short COLLISION_ATTR_ITEM = 0b1 << 4;//アイテム
