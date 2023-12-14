#pragma once

/*

*	Vector2.h

*	x,yのベクトル2つ

*/
namespace IwasiEngine//IwasiEngineのネームスペース
{
	class Vector2
	{
	public:
		float x;//x成分
		float y;//y成分

	public:
		//コンストラクタ
		Vector2();								//零ベクトルとして生成
		Vector2(const float x, const float y);				//x成分、y成分を指定して生成

		//メンバ関数
		float length() const;					//ノルム(長さ)を求める
		Vector2& normalize();					//正規化する
		float dot(const Vector2& v) const;		//内積を求める
		float cross(const Vector2& v) const;	//外積を求める

		//単項演算子オーバーロード
		Vector2 operator+() const;
		Vector2 operator-() const;

		//代入演算子オーバーロード
		Vector2& operator+=(const Vector2& v);
		Vector2& operator-=(const Vector2& v);
		Vector2& operator*=(float s);
		Vector2& operator/=(float s);
	};
}
	//2項演算子オーバーロード
	//パターンの準備
	const IwasiEngine::Vector2 operator+(const IwasiEngine::Vector2& v1, const IwasiEngine::Vector2& v2);
	const IwasiEngine::Vector2 operator-(const IwasiEngine::Vector2& v1, const IwasiEngine::Vector2& v2);
	const IwasiEngine::Vector2 operator*(const IwasiEngine::Vector2& v, float s);
	const IwasiEngine::Vector2 operator*(float s, const IwasiEngine::Vector2& v);
	const IwasiEngine::Vector2 operator/(const IwasiEngine::Vector2& v, float s);
