#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>

/*

*	Audio.h

*	����(�I�[�f�B�I)

*/
class Audio final
{
private://�G�C���A�X
	// Microsoft::WRL::���ȗ�
	template <class T> using ComPtr = Microsoft::WRL::ComPtr<T>;

public://�T�u�N���X(�J�v�Z����)
	//�`�����N�w�b�_
	struct ChunkHeader
	{
		char id[4];		//�`�����N�p��ID
		int32_t size;	//�`�����N�T�C�Y
	};

	//RIFF�w�b�_�`�����N
	struct RiffHeader
	{
		ChunkHeader chunk;	//"RIFF"
		char type[4];		//"WAVE"
	};

	//Format�`�����N
	struct FormatChunk
	{
		ChunkHeader chunk;	//"fmt "
		WAVEFORMATEX fmt;	//�g�`�t�H�[�}�b�g
	};

	//�T�E���h�f�[�^
	struct SoundData
	{
		//�g�`�t�H�[�}�b�g
		WAVEFORMATEX wfex;
		//�o�b�t�@�̐擪�A�h���X
		BYTE* pBuffer;
		//�o�b�t�@�̃T�C�Y
		unsigned int bufferSize;
	};
public://�V���O���g���C���X�^���X
	static Audio* GetInstance();

public://�����o�֐�
	
	//������
	void Initialize();

	//�T�E���h�ǂݍ���(�g�p����.wav)
	SoundData SoundLoadWave(const char* filename);
	
	//�����Đ�(Xaudio2�A�T�E���h�f�[�^�Atrue�����[�v�Đ�)
	void SoundPlayWave(IXAudio2* xAudio2, const SoundData& soundData, bool loop);
	
	//�e�퉹���f�[�^�̊J��(�T�E���h�f�[�^)
	void SoundUnLoad(SoundData* soundData);
	
	//�I������(xAudio2�̊J��->�e�퉹���f�[�^���J������悤�ɁI)
	void Finalize();

private://�����o�ϐ�

	ComPtr<IXAudio2> xaudio2;//Xaudio2

public://�A�N�Z�b�T�u����
	//xAudio2�Q�b�g
	IXAudio2* GetXAudio2() { return xaudio2.Get(); }
private:
	//�R���X�g���N�^�i�V���O���g���p�^�[���j
	Audio() = default;
	//�f�X�g���N�^�i�V���O���g���p�^�[���j
	~Audio() = default;
public:
	//�R�s�[�R���X�g���N�^�̖h�~�i�V���O���g���p�^�[���j
	Audio(const Audio& obj) = delete;
	// �R�s�[������Z�q���֎~�i�V���O���g���p�^�[���j
	Audio& operator=(const Audio& obj) = delete;
};
