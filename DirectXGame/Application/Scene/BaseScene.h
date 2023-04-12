#pragma once

//シーン基盤
class BaseScene
{
public:
	virtual~BaseScene() = default;
	//初期化
	virtual void Initialize() = 0;
	//更新
	virtual void Update() = 0;
	//描画
	virtual void Draw() = 0;
	//終了処理
	virtual void Finalize() = 0;

};
