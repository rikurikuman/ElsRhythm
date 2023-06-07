#pragma once
#include <xaudio2.h>
#include <fstream>
#include <wrl.h>
#include <map>
#include <string>
#include <memory>
#include <vector>
#include <mutex>
#include "Util.h"

#pragma comment(lib,"xaudio2.lib")

typedef std::string AudioHandle;

struct ChunkHeader
{
	char id[4]; //ƒ`ƒƒƒ“ƒNID
	int size;
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
	AudioType type;
};

struct WaveAudio : public AudioData
{
	WAVEFORMATEX wfex;
	BYTE* pBuffer = nullptr;
	unsigned int bufferSize;

	~WaveAudio() {
		if (pBuffer != nullptr) {
			OutputDebugStringA(Util::StringFormat("Delete %p\n", pBuffer).c_str());
			delete[] pBuffer;
		}
	}
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
	static void Play(AudioHandle handle, const float volume = 1.0f, const bool loop = false);
	static void Stop(AudioHandle handle);

private:
	Microsoft::WRL::ComPtr<IXAudio2> xAudio2;
	IXAudio2MasteringVoice* master = nullptr;

	std::recursive_mutex mutex;
	std::map<AudioHandle, std::shared_ptr<AudioData>> audioMap;

	struct PlayingInfo {
		AudioHandle handle;
		IXAudio2SourceVoice* pSource;
	};
	std::vector<PlayingInfo> playingList;

	RAudio() {};
	~RAudio() = default;
	RAudio(const RAudio& a) {};
	RAudio& operator=(const RAudio&) { return *this; }

	void InitInternal();
	void FinalInternal();
};

