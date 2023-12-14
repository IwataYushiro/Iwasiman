#pragma once

#include "Vector3.h"
#include "XYZ.h"

namespace IwasiEngine//IwasiEngine�̃l�[���X�y�[�X
{
	/*

	*	Matrix4.h

	*	Matrix4(4x4�s��)�\����

	*/
	struct Matrix4
	{
		float m[XYZW_Num][XYZW_Num];
	};
}
	//�P�ʍs������߂�
	IwasiEngine::Matrix4 identity();

	//�g��k���̐ݒ�
	IwasiEngine::Matrix4 scale(const IwasiEngine::Vector3& s);

	//��]
	IwasiEngine::Matrix4 rotateX(const float angle);//X��
	IwasiEngine::Matrix4 rotateY(const float angle);//Y��
	IwasiEngine::Matrix4 rotateZ(const float angle);//Z��

	//���s�ړ�
	IwasiEngine::Matrix4 translate(const IwasiEngine::Vector3& t);

	//���W�ϊ�
	IwasiEngine::Vector3 transform(const IwasiEngine::Vector3& v, const IwasiEngine::Matrix4& m);

	//������Z�q�I�[�o�[���[�h
	IwasiEngine::Matrix4& operator*=(IwasiEngine::Matrix4& m1, IwasiEngine::Matrix4& m2);

	//2�����Z�q�I�[�o�[���[�h
	const IwasiEngine::Matrix4 operator* (const IwasiEngine::Matrix4& m1, const IwasiEngine::Matrix4& m2);
	const IwasiEngine::Vector3 operator* (const IwasiEngine::Vector3& v, const IwasiEngine::Matrix4& m);
