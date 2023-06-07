#include "RAudio.h"
#include <cassert>

using namespace std;

AudioHandle RAudio::Load(const std::string filepath, std::string handle)
{
	RAudio* instance = GetInstance();

	std::unique_lock<std::recursive_mutex> lock(GetInstance()->mutex);

	//ˆê‰ñ“Ç‚Ýž‚ñ‚¾‚±‚Æ‚ª‚ ‚éƒtƒ@ƒCƒ‹‚Í‚»‚Ì‚Ü‚Ü•Ô‚·
	auto itr = find_if(instance->audioMap.begin(), instance->audioMap.end(), [&](const std::pair<AudioHandle, shared_ptr<AudioData>>& p) {
		return p.second->filepath == filepath;
		});
	if (itr != instance->audioMap.end()) {
		return itr->first;
	}
	lock.unlock();

	std::ifstream file;
	file.open(filepath, std::ios_base::binary);

	if (file.fail()) {
		return "";
	}

	RiffHeader riff = {};
	file.read((char*)&riff, sizeof(riff));

	if (strncmp(riff.chunk.id, "RIFF", 4) != 0) {
		return "";
	}

	if (strncmp(riff.type, "WAVE", 4) != 0) {
		return "";
	}

	FormatChunk format = {};

	file.read((char*)&format, sizeof(ChunkHeader));
	if (strncmp(format.chunk.id, "fmt ", 4) != 0) {
		return "";
	}

	assert(format.chunk.size <= sizeof(format.fmt));
	file.read((char*)&format.fmt, format.chunk.size);

	ChunkHeader data = {};
	file.read((char*)&data, sizeof(data));

	while (!file.fail() && strncmp(data.id, "data", 4) != 0) {
		file.seekg(data.size, std::ios_base::cur);
		file.read((char*)&data, sizeof(data));
	}

	if (file.fail()) {
		return "";
	}

	char* pBuffer = new char[data.size];
	file.read(pBuffer, data.size);

	file.close();

	shared_ptr<WaveAudio> sound = make_shared<WaveAudio>();
	sound->filepath = filepath;
	sound->wfex = format.fmt;
	sound->pBuffer = reinterpret_cast<BYTE*>(pBuffer);
	sound->bufferSize = data.size;

	if (handle.empty()) {
		handle = "NoNameHandle_" + filepath;
	}

	lock.lock();
	GetInstance()->audioMap[handle] = sound;
	return handle;
}

void RAudio::Play(const AudioHandle handle, const float volume, const bool loop)
{
	RAudio* instance = GetInstance();
	HRESULT result;

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
	if (instance->audioMap.find(handle) == instance->audioMap.end()) {
		return;
	}

	shared_ptr<AudioData> data = instance->audioMap[handle];

	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);

		IXAudio2SourceVoice* pSourceVoice = nullptr;
		result = instance->xAudio2->CreateSourceVoice(&pSourceVoice, &waveData->wfex);
		assert(SUCCEEDED(result));

		XAUDIO2_BUFFER buf{};
		buf.pAudioData = waveData->pBuffer;
		buf.AudioBytes = waveData->bufferSize;
		buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		result = pSourceVoice->SubmitSourceBuffer(&buf);
		assert(SUCCEEDED(result));
		result = pSourceVoice->SetVolume(volume);
		assert(SUCCEEDED(result));
		result = pSourceVoice->Start();
		assert(SUCCEEDED(result));

		if (loop) {
			instance->playingList.push_back({ handle, pSourceVoice });
		}
	}
}

void RAudio::Stop(AudioHandle handle)
{
	RAudio* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mutex);
	for (auto itr = instance->playingList.begin(); itr != instance->playingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			info.pSource->Stop();
			info.pSource->DestroyVoice();
			itr = instance->playingList.erase(itr);
			continue;
		}
		itr++;
	}
}

void RAudio::InitInternal()
{
	HRESULT result;
	result = XAudio2Create(&xAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	result = xAudio2->CreateMasteringVoice(&master);
	assert(SUCCEEDED(result));
}

void RAudio::FinalInternal()
{
	master->DestroyVoice();
	xAudio2.Reset();
	audioMap.clear();
}
