#pragma once

//�V�[�����
class BaseScene
{
public:
	virtual~BaseScene() = default;
	//������
	virtual void Initialize() = 0;
	//�X�V
	virtual void Update() = 0;
	//�`��
	virtual void Draw() = 0;
	//�I������
	virtual void Finalize() = 0;

};
