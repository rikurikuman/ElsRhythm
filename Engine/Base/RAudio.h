#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <map>
#include <string>
#include <memory>
#include <list>
#include <mutex>
#include "Util.h"

#pragma comment(lib,"xaudio2.lib")

typedef std::string AudioHandle;

struct ChunkHeader
{
	char id[4]; //チャンクID
	int32_t size;
};

struct RiffHeader
{
	ChunkHeader chunk;
	char type[4];
};

struct FormatChunk
{
	ChunkHeader chunk;
	WAVEFORMATEX fmt;
};

enum class AudioType {
	Wave, Other
};

struct AudioData {
	std::string filepath;
	AudioType type{};
};

struct WaveAudio : public AudioData
{
	WAVEFORMATEX wfex{};
	std::vector<BYTE> buffer;
	uint32_t bufferSize = 0;
};

class RVoiceCallBack : public IXAudio2VoiceCallback
{
public:
	HANDLE mStreamEndEventHandle;
	RVoiceCallBack() : mStreamEndEventHandle(CreateEvent(NULL, FALSE, FALSE, NULL)) {}
	~RVoiceCallBack() { CloseHandle(mStreamEndEventHandle); }

	//再生終了時イベント
	void OnStreamEnd() { SetEvent(mStreamEndEventHandle); }

	//未使用イベント
	void OnVoiceProcessingPassEnd() {}
	void OnVoiceProcessingPassStart(UINT32 SamplesRequired) {}
	void OnBufferEnd(void* pBufferContext) {}
	void OnBufferStart(void* pBufferContext) {}
	void OnLoopEnd(void* pBufferContext) {}
	void OnVoiceError(void* pBufferContext, HRESULT Error) {}
};

class RAudio
{
public:
	static RAudio* GetInstance() {
		static RAudio instance;
		return &instance;
	}

	static void Init() {
		GetInstance()->InitInternal();
	}

	static void Final() {
		GetInstance()->FinalInternal();
	}

	static AudioHandle Load(const std::string filepath, std::string handle = "");
	static void Play(AudioHandle handle, const float volume = 1.0f, const float pitch = 1.0f, const bool loop = false);
	static void Stop(AudioHandle handle);
	static bool IsPlaying(AudioHandle handle);

private:
	Microsoft::WRL::ComPtr<IXAudio2> mXAudio2;
	IXAudio2MasteringVoice* mMasteringVoice = nullptr;

	std::recursive_mutex mMutex;
	std::map<AudioHandle, std::shared_ptr<AudioData>> mAudioMap;

	struct PlayingInfo {
		AudioHandle handle;
		IXAudio2SourceVoice* pSource;
	};
	std::list<PlayingInfo> mPlayingList;

	RAudio() {};
	~RAudio() = default;
	RAudio(const RAudio&) {};
	RAudio& operator=(const RAudio&) { return *this; }

	void InitInternal();
	void FinalInternal();
};

