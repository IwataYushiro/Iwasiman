#include "Easing.h"
#include <cmath>

using namespace std;

/*

*	Easing.cpp

*	�C�[�W���O�֌W Easing(float start, float end, float time);

*/

Easing::Easing()
{
	startCount_ = chrono::steady_clock::now();	//�J�n����
	nowCount_ = chrono::steady_clock::now();		//���ݎ���

	struct DefaultNum
	{
		const float start = 0.0f;
		const float end = 1.0f;
		const float maxTime = 1.0f;
	};
	DefaultNum defaultNum;
	//0����1�̃T���v��
	this->start = defaultNum.start;
	this->end = defaultNum.end;
	this->maxtime = defaultNum.maxTime;

	//�C�[�W���O�v���p�e�B
	num_X = 0.0f;
	timeNow_ = 0.0f;					//����
	startPos_ = start;				//�J�n�ʒu
	differencePos_ = end - start;			//�J�n�ʒu-�I���ʒu�̍�
	totalTime_ = maxtime;				//���v����
	isReverse_ = false;
}

Easing::Easing(float start, float end, float time)
{
	startCount_ = chrono::steady_clock::now();	//�J�n����
	nowCount_ = chrono::steady_clock::now();		//���ݎ���

	this->start = start;
	this->end = end;
	this->maxtime = time;

	//�C�[�W���O�v���p�e�B
	num_X = 0.0f;
	timeNow_ = 0.0f;					//����
	totalTime_ = time;				//���v����
	startPos_ = start;				//�J�n�ʒu
	differencePos_ = end - start;	//�J�n�ʒu-�I���ʒu�̍�
	isReverse_ = false;
}

void Easing::Standby(bool reverse)
{
	startCount_ = chrono::steady_clock::now();
	isReverse_ = reverse;
	num_X = 0.0f;
	if (isReverse_)
	{
		startPos_ = end;				//�J�n�ʒu
		differencePos_ = start - end;			//�J�n�ʒu-�I���ʒu�̍�
	}
	else
	{
		startPos_ = start;				//�J�n�ʒu
		differencePos_ = end - start;			//�J�n�ʒu-�I���ʒu�̍�
	}
}


float Easing::ease_in_sine()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - cosf((x * PI) / 2.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_sine()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = sinf((x * PI) / 2.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_sine()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = -(cosf(PI * x) - 1.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_quad()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_quad()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - (1.0f - x) * (1.0f - x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_quad()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 2.0f * x * x : 1.0f - powf(-2.0f * x + 2.0f, 2.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_cubic()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_cubic()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - powf(1.0f - x, 3.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_cubic()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 4.0f * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 3.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_quart()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_quart()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - powf(1.0f - x, 4.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_quart()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 8.0f * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 4.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_quint()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x * x * x * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_quint()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - powf(1.0f - x, 5.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_quint()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? 16.0f * x * x * x * x * x : 1.0f - powf(-2.0f * x + 2.0f, 5.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_expo()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : powf(2.0f, 10.0f * x - 10.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_expo()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 1.0f ? 1.0f : 1.0f - powf(2.0f, -10.0f * x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_expo()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ? powf(2.0f, 20.0f * x - 10.0f) / 2.0f
		: (2.0f - powf(2.0f, -20.0f * x + 10.0f)) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_circ()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - sqrtf(1.0f - powf(x, 2.0f));

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_circ()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = sqrtf(1.0f - powf(x - 1.0f, 2.0f));

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_circ()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f ? (1.0f - sqrtf(1.0f - powf(2.0f * x, 2.0f))) / 2.0f :
		(sqrtf(1.0f - powf(-2.0f * x + 2.0f, 2.0f)) + 1.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_back()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	const float v = c3 * x * x * x - c1 * x * x;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;;
}

float Easing::ease_out_back()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float c1 = 1.70158f;
	const float c3 = c1 + 1.0f;
	const float v = 1.0f + c3 * powf(x - 1.0f, 3.0f) + c1 * pow(x - 1.0f, 2.0f);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;;
}

float Easing::ease_in_out_back()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float c1 = 1.70158f;
	const float c2 = c1 * 1.525f;
	const float v = x < 0.5f
		? (powf(2.0f * x, 2.0f) * ((c2 + 1.0f) * 2.0f * x - c2)) / 2.0f
		: (powf(2.0f * x - 2.0f, 2.0f) * ((c2 + 1.0f) * (x * 2.0f - 2.0f) + c2) + 2.0f) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;;
}

float Easing::ease_in_elastic()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float c4 = (2.0f * PI) / 3.0f;

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f :
		-powf(2.0f, 10.0f * x - 10.0f) * sinf((x * 10.0f - 10.75f) * c4);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_elastic()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float c4 = (2.0f * PI) / 3.0f;

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f :
		powf(2.0f, -10.0f * x) * sinf((x * 10.0f - 0.75f) * c4) + 1.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_elastic()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float c5 = (2.0f * PI) / 4.5f;

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x == 0.0f ? 0.0f : x == 1.0f ? 1.0f : x < 0.5f ?
		-(powf(2.0f, 20.0f * x - 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f :
		(powf(2.0f, -20.0f * x + 10.0f) * sinf((20.0f * x - 11.125f) * c5)) / 2.0f + 1.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_bounce()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = 1.0f - bounceCalculation(1.0f - x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_out_bounce()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = bounceCalculation(x);

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::ease_in_out_bounce()
{
	//���ݎ��Ԃ��擾����
	nowCount_ = chrono::steady_clock::now();
	//�O��L�^����̌o�ߎ��Ԃ��擾����
	elapsedCount_ = chrono::duration_cast<chrono::microseconds>
		(nowCount_ - startCount_);

	timeNow_ = chrono::duration_cast<chrono::microseconds>
		(elapsedCount_).count() / 1'000'000.0f;//�}�C�N���b��b�ɒP�ʕϊ�

	const float x = min(timeNow_ / totalTime_, 1.0f);

	const float v = x < 0.5f
		? (1.0f - bounceCalculation(1.0f - 2.0f * x)) / 2.0f
		: (1.0f + bounceCalculation(2.0f * x - 1.0f)) / 2.0f;

	const float ret = differencePos_ * v + startPos_;

	num_X = ret;

	return num_X;
}

float Easing::bounceCalculation(float x)
{
	float v;

	const float n1 = 7.5625f;
	const float d1 = 2.75f;

	//�v�Z�p�^�[��
	const float cal1 = 1.0f;
	const float cal2 = 2.0f;
	const float cal3 = 2.5f;

	if (x < cal1 / d1)
	{
		const float calculation = n1 * x * x;
		v = calculation;
	}
	else if (x < cal2 / d1)
	{
		const float calculation = n1 * (x -= 1.5f / d1) * x + 0.75f;
		v = calculation;
	}
	else if (x < cal3 / d1)
	{
		const float calculation = n1 * (x -= 2.25f / d1) * x + 0.9375f;
		v = calculation;
	}
	else
	{
		const float calculation = n1 * (x -= 2.625f / d1) * x + 0.984375f;
		v = calculation;
	}
	return v;
}

void Easing::SetEasing(float s, float e, float t)
{

	this->start = s;
	this->end = e;
	this->maxtime = t;

	startCount_ = chrono::steady_clock::now();	//�J�n����
	nowCount_ = chrono::steady_clock::now();		//���ݎ���
	//�C�[�W���O�v���p�e�B
	num_X = 0.0f;
	timeNow_ = 0.0f;					//����
	totalTime_ = t;					//���v����
	startPos_ = start;				//�J�n�ʒu
	differencePos_ = end - start;	//�J�n�ʒu-�I���ʒu�̍�
	isReverse_ = false;

}
