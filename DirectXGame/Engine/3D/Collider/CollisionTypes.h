#pragma once

/*

*	CollisionTypes.h (cpp X)

*	衝突判定用関連の型定義

*/
enum CollisionShapeType
{
	SHAPE_UNKNOWN = -1,			//未指定

	COLLISIONSHAPE_SPHERE,		//球
	COLLISIONSHAPE_MESH,		//メッシュ
	COLLISIONSHAPE_AABB,		//AABB

};