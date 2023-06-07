#pragma once
#include <d3d12.h>
#include <string>
#include <list>
#include <wrl.h>
#include <memory>
#include <map>
#include <mutex>
#include "Color.h"

typedef std::string TextureHandle;

class Texture
{
public:
	Microsoft::WRL::ComPtr<ID3D12Resource> resource; //�e�N�X�`���̃��\�[�X
	D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle = D3D12_CPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(CPU��)
	D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle = D3D12_GPU_DESCRIPTOR_HANDLE(); //SRV�̃n���h��(GPU��)
	UINT heapIndex = -1;
	std::string filePath; //�t�@�C���ւ̃p�X

	Texture() {};
	Texture(
		Microsoft::WRL::ComPtr<ID3D12Resource> resource,
		D3D12_CPU_DESCRIPTOR_HANDLE cpuHandle,
		D3D12_GPU_DESCRIPTOR_HANDLE gpuHandle,
		std::string filePath
	) : resource(resource), cpuHandle(cpuHandle), gpuHandle(gpuHandle), filePath(filePath) {};
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
	static TextureHandle Create(const Color color, const UINT64 width, const UINT height, const std::string handle = "");

	/// <summary>
	/// �F���̔z��ɂ���ăe�N�X�`�������
	/// </summary>
	/// <param name="pSource">Color�z��</param>
	/// <param name="width">����</param>
	/// <param name="height">�c��</param>
	/// <param name="filepath">����΁A�t�@�C���p�X</param>
	/// <param name="handle">�K�v�Ȃ�A�C�Ӄn���h�����w��</param>
	/// <returns>���ꂽ�e�N�X�`���̃n���h��</returns>
	static TextureHandle Create(const Color* pSource, const UINT64 width, const UINT height, const std::string filepath = "", const std::string handle = "");

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
		return this->srvHeap.Get();
	}

private:
	TextureManager() {
		Init();
	};
	~TextureManager() = default;
	TextureManager(const TextureManager& a) {};
	TextureManager& operator=(const TextureManager&) { return *this; }

	void Init();
	
	TextureHandle CreateInternal(const Color color, const UINT64 width, const UINT height, const std::string handle = "");
	TextureHandle CreateInternal(const Color* pSource, const UINT64 width, const UINT height, const std::string filepath = "", const std::string handle = "");
	TextureHandle LoadInternal(const std::string filepath, const std::string handle = "");
	TextureHandle LoadInternal(const void* pSource, const size_t size, const std::string filepath, const std::string handle = "");
	Texture& GetInternal(const TextureHandle& handle);
	TextureHandle RegisterInternal(Texture texture, TextureHandle handle = "");
	void UnRegisterInternal(const TextureHandle& handle);
	void UnRegisterAtEndFrameInternal(const TextureHandle& handle);
	void EndFrameProcessInternal();

	std::recursive_mutex mutex;
	static const UINT numSRVDescritors = 2048; //�f�X�N���v�^�q�[�v�̐�
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> srvHeap; //�e�N�X�`���pSRV�f�X�N���v�^�q�[�v
	std::map<TextureHandle, Texture> textureMap;
	std::list<TextureHandle> unregisterScheduledList; //UnRegisterAtEndFrame�\�胊�X�g
};