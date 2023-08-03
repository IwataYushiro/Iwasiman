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
	timeNow = 0.0f;					//����
	startpos = start;				//�J�n�ʒu
	differencepos = end - start;			//�J�n�ʒu-�I���ʒu�̍�
	totaltime = maxtime;				//���v����
	isReverse = false;
}

EasingManager::EasingManager(float start, float end, float time)
{
	startCount = std::chrono::steady_clock::now();	//�J�n����
	nowCount = std::chrono::steady_clock::now();		//���ݎ���

	this->start = start;
	this->end = end;
	this->maxtime = time;

	//�C�[�W���O�v���p�e�B
	timeNow = 0.0f;					//����
	totaltime = time;				//���v����
	startpos = start;				//�J�n�ʒu
	differencepos = end - start;	//�J�n�ʒu-�I���ʒu�̍�
	isReverse = false;
}

void EasingManager::Standby(bool reverse)
{
	startCount = std::chrono::steady_clock::now();
	isReverse = reverse;
	if (isReverse)
	{
		startpos = end;				//�J�n�ʒu
		differencepos = start - end;			//�J�n�ʒu-�I���ʒu�̍�
	}
	else
	{
		startpos = start;				//�J�n�ʒu
		differencepos = end - start;			//�J�n�ʒu-�I���ʒu�̍�
	}
}


float EasingManager::ease_in_sine()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - cosf((x * PI) / 2.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_sine()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = sinf((x * PI) / 2.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_sine()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = -(cosf(PI * x) - 1.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_quad()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_quad()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - (1.0f - x) * (1.0f - x);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_quad()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 2.0f * x * x : 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_cubic()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_cubic()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - powf(1.0f - x, 3.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_cubic()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_quart()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_quart()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - powf(1.0f - x, 4.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_quart()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 8.0f * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_quint()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x * x * x * x * x;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_quint()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - powf(1.0f - x, 5.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_quint()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_expo()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : powf(2.0f, 10.0f * x - 10.0f);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_expo()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x);

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_expo()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ? powf(2.0f, 20.0f * x - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_circ()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = 1.0f - sqrtf(1.0f - powf(x, 2.0f));

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_out_circ()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = sqrtf(1.0f - powf(x - 1.0f, 2.0f));

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}

float EasingManager::ease_in_out_circ()
{
	//���ݎ��Ԃ��擾����
	nowCount = std::chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount = std::chrono::duration_cast<std::chrono::microseconds>
		(nowCount - startCount);

	timeNow = std::chrono::duration_cast<std::chrono::microseconds>
		(elapsedCount).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	float x = min(timeNow / totaltime, 1.0f);

	float v = x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f :
		(sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	float ret = differencepos * v + startpos;

	num_X = ret;

	return num_X;
}
