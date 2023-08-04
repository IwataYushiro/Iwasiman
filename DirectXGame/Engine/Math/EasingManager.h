#pragma once
#include "WinApp.h"
#include <chrono>

class EasingManager
{
public:
	float start;
	float end;
	float maxtime;
public:
	EasingManager();
	EasingManager(float start, float end, float time);
	float num_X = 0.0f;//�Ԃ��p

public://�֐�
	void Standby(bool reverse);

	//�C�[�W���O�֐�
	// sine
	float ease_in_sine();
	float ease_out_sine();
	float ease_in_out_sine();
	// quad
	float ease_in_quad();
	float ease_out_quad();
	float ease_in_out_quad();
	// cubic
	float ease_in_cubic();
	float ease_out_cubic();
	float ease_in_out_cubic();
	// quart
	float ease_in_quart();
	float ease_out_quart();
	float ease_in_out_quart();
	// quint
	float ease_in_quint();
	float ease_out_quint();
	float ease_in_out_quint();
	// expo
	float ease_in_expo();
	float ease_out_expo();
	float ease_in_out_expo();
	// circ
	float ease_in_circ();
	float ease_out_circ();
	float ease_in_out_circ();
	// back
	float ease_in_back();
	float ease_out_back();
	float ease_in_out_back();
	// elastic
	float ease_in_elastic();
	float ease_out_elastic();
	float ease_in_out_elastic();
	// bounce
	float ease_in_bounce();
	float ease_out_bounce();
	float ease_in_out_bounce();

public:
	//���Ԍv��
	std::chrono::steady_clock::time_point startCount;	//�J�n����
	std::chrono::steady_clock::time_point nowCount;		//���ݎ���
	std::chrono::microseconds elapsedCount;				//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�C�[�W���O�v���p�e�B
	float timeNow;					//����
	float startpos;					//�J�n�ʒu
	float differencepos;					//�J�n�ʒu-�I���ʒu�̍�
	float totaltime;					//���v����

	bool isReverse = false;
public://�萔
	const float PI = 3.141592f;

};

