#pragma once
#include <d3d12.h>
#include <string>
#include <list>
#include <wrl.h>
#include <memory>
#include <map>
#include <mutex>
#include "Color.h"
#include <Rect.h>
#include <Vector2.h>

typedef std::string TextureHandle;

class Texture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> mResource; //�e�N�X�`���̃��\�[�X
	D3D12_CPU_DESCRIPTOR_HANDLE mCpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(CPU��)
	D3D12_GPU_DESCRIPTOR_HANDLE mGpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(GPU��)
	uint32_t mHeapIndex = UINT32_MAX;
	std::string mFilePath; //�t�@�C���ւ̃p�X

	Texture() : mState(D3D12_RESOURCE_STATE_COMMON) {};
	Texture(D3D12_RESOURCE_STATES firstState) : mState(firstState) {};

	/// <summary>
	/// �e�N�X�`���̃��\�[�X�X�e�[�g��ύX���܂�
	/// ���̊֐��ȊO���烊�\�[�X�X�e�[�g��ύX����Ɨǂ��Ȃ����Ƃ��N���܂�
	/// </summary>
	/// <param name="state">�ύX�惊�\�[�X�X�e�[�g</param>
	void ChangeResourceState(D3D12_RESOURCE_STATES state);

	// �e�N�X�`���̌��݂̃��\�[�X�X�e�[�g���擾���܂�
	D3D12_RESOURCE_STATES GetResourceState() const {
		return mState;
	}

	/// <summary>
	/// �e�N�X�`�����R�s�[���܂�
	/// </summary>
	/// <param name="dest">�R�s�[��e�N�X�`��</param>
	/// <param name="srcRect">�R�s�[���̈�</param>
	/// <param name="destPos">�R�s�[��ʒu</param>
	void Copy(Texture* dest, RRect srcRect, Vector2 destPos = {0, 0});

private:
	D3D12_RESOURCE_STATES mState;
};

class TextureManager
{
public:
	//TextureManager���擾����
	static TextureManager* GetInstance() {
		static TextureManager instance;
		return &instance;
	}

	static Texture GetEmptyTexture();
	static Texture GetHogeHogeTexture();

	/// <summary>
	/// �w��F�œh��Ԃ��ꂽ�e�N�X�`�������
	/// </summary>
	/// <param name="color">�h��Ԃ��F</param>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="handle">�K�v�Ȃ�A�C�Ӄn���h�����w��</param>
	/// <returns>���ꂽ�e�N�X�`���̃n���h��</returns>
	static TextureHandle Create(const Color color, const size_t width, const uint32_t height, const std::string handle = "");

	/// <summary>
	/// �F���̔z��ɂ���ăe�N�X�`�������
	/// </summary>
	/// <param name="pSource">Color�z��</param>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="filepath">����΁A�t�@�C���p�X</param>
	/// <param name="handle">�K�v�Ȃ�A�C�Ӄn���h�����w��</param>
	/// <returns>���ꂽ�e�N�X�`���̃n���h��</returns>
	static TextureHandle Create(const Color* pSource, const size_t width, const uint32_t height, const std::string filepath = "", const std::string handle = "");

	/// <summary>
	/// �t�@�C������e�N�X�`����ǂݍ���œo�^����
	/// </summary>
	/// <param name="filepath">�t�@�C���ւ̃p�X</param>
	/// <param name="handle">�K�v�Ȃ�A�C�Ӄn���h�����w��</param>
	/// <returns>�ǂݍ��񂾃e�N�X�`���̃n���h��</returns>
	static TextureHandle Load(const std::string filepath, const std::string handle = "");

	/// <summary>
	/// ����������e�N�X�`����ǂݍ���œo�^����
	/// </summary>
	/// <param name="pSource">���̃������ւ̃|�C���^</param>
	/// <param name="size">�T�C�Y</param>
	/// <param name="filepath">����΁A�t�@�C���ւ̃p�X</param>
	/// <param name="handle">�K�v�Ȃ�A�C�Ӄn���h�����w��</param>
	/// <returns>�ǂݍ��񂾃e�N�X�`���̃n���h��</returns>
	static TextureHandle Load(const void* pSource, const size_t size, const std::string filepath = "", const std::string handle = "");

	/// <summary>
	/// �o�^�ς݂̃e�N�X�`�����擾����
	/// </summary>
	/// <param name="handle">�擾����n���h��</param>
	/// <returns></returns>
	static Texture& Get(const TextureHandle& handle);

	/// <summary>
	/// �e�N�X�`����o�^����
	/// </summary>
	/// <param name="texture">�o�^����e�N�X�`��</param>
	/// <param name="handle">�o�^��̃e�N�X�`���n���h��</param>
	/// <returns>�o�^��̃e�N�X�`���n���h��</returns>
	static TextureHandle Register(Texture texture, TextureHandle handle = "");

	/// <summary>
	/// �o�^�ς݂̃e�N�X�`���𑦍��ɔj������
	/// </summary>
	/// <param name="handle">�j������e�N�X�`���̃n���h��</param>
	static void UnRegister(const TextureHandle& handle);

	/// <summary>
	/// �o�^�ς݂̃e�N�X�`�������̃t���[���I�����ɔj������
	/// ���̃t���[���̕`��Ɏg�����ǂ���ȍ~����Ȃ��ꍇ�͂�����
	/// </summary>
	/// <param name="handle">�j������e�N�X�`���̃n���h��</param>
	static void UnRegisterAtEndFrame(const TextureHandle& handle);

	//�t���[���I�����̏������s��(�Ă�)
	static void EndFrameProcess();

	//�ǂݍ��񂾃e�N�X�`����S�đ����ɔj������
	static void UnRegisterAll();

	ID3D12DescriptorHeap* GetSRVHeap() {
		return mSrvHeap.Get();
	}

private:
	TextureManager() {
		Init();
	};
	~TextureManager() = default;
	TextureManager(const TextureManager&) {};
	TextureManager& operator=(const TextureManager&) { return *this; }

	void Init();
	
	TextureHandle CreateInternal(const Color color, const size_t width, const uint32_t height, const std::string handle = "");
	TextureHandle CreateInternal(const Color* pSource, const size_t width, const uint32_t height, const std::string filepath = "", const std::string handle = "");
	TextureHandle LoadInternal(const std::string filepath, const std::string handle = "");
	TextureHandle LoadInternal(const void* pSource, const size_t size, const std::string filepath, const std::string handle = "");
	Texture& GetInternal(const TextureHandle& handle);
	TextureHandle RegisterInternal(Texture texture, TextureHandle handle = "");
	void UnRegisterInternal(const TextureHandle& handle);
	void UnRegisterAtEndFrameInternal(const TextureHandle& handle);
	void EndFrameProcessInternal();

	std::recursive_mutex mMutex;
	static const uint32_t NUM_SRV_DESCRIPTORS = 2048; //�f�X�N���v�^�q�[�v�̐�
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> mSrvHeap; //�e�N�X�`���pSRV�f�X�N���v�^�q�[�v
	std::map<TextureHandle, Texture> mTextureMap;
	std::list<TextureHandle> mUnregisterScheduledList; //UnRegisterAtEndFrame�\�胊�X�g
};