#pragma once

//ゲーム全体
class Framework
{
public://メンバ関数

	
	virtual ~Framework() = default;

	virtual void Initialize();

	virtual void Update();

	virtual void Draw() = 0;

	virtual void Finalize();

	virtual bool IsEndRequest() { return EndGame_; }

private:
	//ゲーム終了フラグ
	bool EndGame_ = false;
};