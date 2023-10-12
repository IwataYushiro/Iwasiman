#pragma once
#include <chrono>

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
	Easing(float start, float end, float time);

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
	std::chrono::steady_clock::time_point startCount;	//�J�n����
	std::chrono::steady_clock::time_point nowCount;		//���ݎ���
	std::chrono::microseconds elapsedCount;				//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�C�[�W���O�v���p�e�B
	float timeNow;					//����
	float startpos;					//�J�n�ʒu
	float differencepos;			//�J�n�ʒu-�I���ʒu�̍�
	float totaltime;				//���v����

	//���]
	bool isReverse = false;
private://�萔
	//�~����
	const float PI = 3.141592f;

private:
	//bounce_easing�v�Z�p
	float bounceCalculation(float x);

public:
	//�C�[�W���O�Z�b�g
	void SetEasing(float s, float e, float t);
};

