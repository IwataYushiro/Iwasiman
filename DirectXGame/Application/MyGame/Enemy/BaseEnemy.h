#pragma once
//�G��ՃN���X
class BaseEnemy
{
public:
	virtual ~BaseEnemy() = default;

	virtual bool Initialize() = 0;

	//�X�V
	virtual void Update() = 0;
	//�`��
	virtual void Draw() = 0;
	//�I������
	virtual void Finalize() = 0;

};