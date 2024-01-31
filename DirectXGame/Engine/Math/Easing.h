#pragma once
#include <chrono>

namespace IwasiEngine//IwasiEngineのネームスペース
{
	/*

	*	Easing.h

	*	イージング関係 Easing(float start, float end, float time);

	*/
	class Easing
	{
	public:
		float start;
		float end;
		float maxtime;
	public:
		//コンストラクタ
		Easing();
		//開始地点、終了地点、持続時間指定版コンストラクタ
		Easing(const float start, const float end, const float time);

		float num_X;//イージングした値を返す用

	public://関数
		//イージング準備関数(startとendを反転するか)
		void Standby(bool reverse);

		//イージング関数
		float ease_in_sine();		//in_sine
		float ease_out_sine();		//out_sine
		float ease_in_out_sine();	//in_out_sine

		float ease_in_quad();		//in_quad
		float ease_out_quad();		//out_quad
		float ease_in_out_quad();	//in_out_quad

		float ease_in_cubic();		//in_cubic
		float ease_out_cubic();		//out_cubic
		float ease_in_out_cubic();	//in_out_cubic

		float ease_in_quart();		//in_quart
		float ease_out_quart();		//out_quart
		float ease_in_out_quart();	//in_out_quart

		float ease_in_quint();		//in_quint
		float ease_out_quint();		//out_quint
		float ease_in_out_quint();	//in_out_quint

		float ease_in_expo();		//in_expo
		float ease_out_expo();		//out_expo
		float ease_in_out_expo();	//in_out_expo

		float ease_in_circ();		//in_circ
		float ease_out_circ();		//out_circ
		float ease_in_out_circ();	//in_out_circ

		float ease_in_back();		//in_back
		float ease_out_back();		//out_back
		float ease_in_out_back();	//in_out_back

		float ease_in_elastic();	//in_elastic
		float ease_out_elastic();	//out_elastic
		float ease_in_out_elastic();//in_out_elastic

		float ease_in_bounce();		//in_bounce
		float ease_out_bounce();	//out_bounce
		float ease_in_out_bounce();	//in_out_bounce

	private:
		//時間計測
		std::chrono::steady_clock::time_point startCount_;	//開始時間
		std::chrono::steady_clock::time_point nowCount_;		//現在時間
		std::chrono::microseconds elapsedCount_;				//経過時間 経過時間=現在時間-開始時間

		//イージングプロパティ
		float timeNow_;					//時間
		float startPos_;				//開始位置
		float differencePos_;			//開始位置-終了位置の差
		float totalTime_;				//合計時間

		//反転
		bool isReverse_ = false;
	private://定数
		//円周率
		const float PI = 3.141592f;

	private:
		//bounce_easing計算用
		float bounceCalculation(float x);

		//イージングの配列用
		enum EasingSetNumM
		{
			//0～9の10個まで対応
			ESN_0,
			ESN_1,
			ESN_2,
			ESN_3,
			ESN_4,
			ESN_5,
			ESN_6,
			ESN_7,
			ESN_8,
			ESN_9,
			ESN_MAX,
		};

	public:
		//イージングセット
		void SetEasing(const float s, const float e, const float t);
		//イージングデータの読み込み
		static void LoadEasingData(const std::string& fileName, Easing& ease, const uint32_t easeArrayNum = 0);
		//イージングのコマンドから値のセット
		static void SetEasingCommand(const char* findWord, const uint32_t num, const uint32_t easeArrayNum, Easing& ease, std::istream& stream, std::string& word);
		//SETの文字を決める
		static const char* SetNumStr(const uint32_t easeArrayNum);
	};
}
