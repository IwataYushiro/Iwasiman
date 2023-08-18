#pragma once
//敵基盤クラス
class BaseEnemy
{
public:
	virtual ~BaseEnemy() = default;

	virtual bool Initialize() = 0;

	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
	//終了処理
	virtual void Finalize() = 0;

};