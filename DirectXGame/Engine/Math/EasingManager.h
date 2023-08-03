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
	void Standby() { startCount = std::chrono::steady_clock::now(); }

	//�C�[�W���O�֐�
	// sine
	float ease_in_sine(float time, float startpos, float differencepos, float totaltime);
	float ease_out_sine(float time, float startpos, float differencepos, float totaltime);
	float ease_in_out_sine(float time, float startpos, float differencepos, float totaltime);

	// quad
	
	// cubic
	float ease_in_cubic(float time, float startpos, float differencepos, float totaltime);
	float ease_out_cubic(float time, float startpos, float differencepos, float totaltime);
	float ease_in_out_cubic(float time, float startpos, float differencepos, float totaltime);
	
	// cric
	float ease_in_out_cric(float time, float startpos, float differencepos, float totaltime);

public:
	//���Ԍv��
	std::chrono::steady_clock::time_point startCount;	//�J�n����
	std::chrono::steady_clock::time_point nowCount;		//���ݎ���
	std::chrono::microseconds elapsedCount;				//�o�ߎ��� �o�ߎ���=���ݎ���-�J�n����

	//�C�[�W���O�v���p�e�B
	float t;					//����
	float b;					//�J�n�ʒu
	float c;					//�J�n�ʒu-�I���ʒu�̍�
	float d;					//���v����

public://�萔
	const float PI = 3.141592f;

};

