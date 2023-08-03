#include "EasingManager.h"
#include <cmath>

EasingManager::EasingManager()
{
	startCount = std::chrono::steady_clock::now();	//�J�n����
	nowCount = std::chrono::steady_clock::now();		//���ݎ���

	//�E���獶�̃T���v��
	this->start = (float)WinApp::GetInstance()->window_width;
	this->end = 0.0f;
	this->maxtime = 1.0f;

	//�C�[�W���O�v���p�e�B
	t = 0.0f;					//����
	b = start;				//�J�n�ʒu
	c = end - start;			//�J�n�ʒu-�I���ʒu�̍�
	d = maxtime;				//���v����

}

EasingManager::EasingManager(float start, float end, float time)
{
	startCount = std::chrono::steady_clock::now();	//�J�n����
	nowCount = std::chrono::steady_clock::now();		//���ݎ���

	this->start = start;
	this->end = end;
	this->maxtime = time;

	//�C�[�W���O�v���p�e�B
	t = 0.0f;					//����
	b = start;				//�J�n�ʒu
	c = end - start;			//�J�n�ʒu-�I���ʒu�̍�
	d = time;				//���v����

}


float EasingManager::ease_in_cubic(float time, float startpos, float differencepos, float totaltime)
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	t = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(time / totaltime, 1.0f);

	float v = x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_cric(float time, float startpos, float differencepos, float totaltime)
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	t = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(time / totaltime, 1.0f);

	float v = x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f :
		(sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}
