#pragma once
#include <chrono>

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	Easing.h

	*	�C�[�W���O�֌W Easing(float start, float end, float time);

	*/
	class Easing
	{
	public:
		float start;
		float end;
		float maxtime;
	public:
		//�R���X�g���N�^
		Easing();
		//�J�n�n�_�A�I���n�_�A�������Ԏw��ŃR���X�g���N�^
		Easing(const float start, const float end, const float time);

		float num_X;//�C�[�W���O�����l��Ԃ��p

	public://�֐�
		//�C�[�W���O�����֐�(start��end�𔽓]���邩)
		void Standby(bool reverse);

		//�C�[�W���O�֐�
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
		//���Ԍv��
		std::chrono::steady_clock::time_point startCount_;	//�J�n����
		std::chrono::steady_clock::time_point nowCount_;		//���ݎ���
		std::chrono::microseconds elapsedCount_;				//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

		//�C�[�W���O�v���p�e�B
		float timeNow_;					//����
		float startPos_;				//�J�n�ʒu
		float differencePos_;			//�J�n�ʒu-�I���ʒu�̍�
		float totalTime_;				//���v����

		//���]
		bool isReverse_ = false;
	private://�萔
		//�~����
		const float PI = 3.141592f;

	private:
		//bounce_easing�v�Z�p
		float bounceCalculation(float x);

		//�C�[�W���O�̔z��p
		enum EasingSetNumM
		{
			//0�`9��10�܂őΉ�
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
		//�C�[�W���O�Z�b�g
		void SetEasing(const float s, const float e, const float t);
		//�C�[�W���O�f�[�^�̓ǂݍ���
		static void LoadEasingData(const std::string& fileName, Easing& ease, const uint32_t easeArrayNum = 0);
		//�C�[�W���O�̃R�}���h����l�̃Z�b�g
		static void SetEasingCommand(const char* findWord, const uint32_t num, const uint32_t easeArrayNum, Easing& ease, std::istream& stream, std::string& word);
		//SET�̕��������߂�
		static const char* SetNumStr(const uint32_t easeArrayNum);
	};
}
