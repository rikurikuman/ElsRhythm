#include "RAudio.h"
#include <cassert>

using namespace std;

AudioHandle RAudio::Load(const std::string filepath, std::string handle)
{
	RAudio* instance = GetInstance();

	std::unique_lock<std::recursive_mutex> lock(GetInstance()->mMutex);

	//ˆê‰ñ“Ç‚Ýž‚ñ‚¾‚±‚Æ‚ª‚ ‚éƒtƒ@ƒCƒ‹‚Í‚»‚Ì‚Ü‚Ü•Ô‚·
	auto itr = find_if(instance->mAudioMap.begin(), instance->mAudioMap.end(), [&](const std::pair<AudioHandle, shared_ptr<AudioData>>& p) {
		return p.second->filepath == filepath;
		});
	if (itr != instance->mAudioMap.end()) {
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

	shared_ptr<WaveAudio> sound = make_shared<WaveAudio>();
	sound->filepath = filepath;
	sound->wfex = format.fmt;
	sound->bufferSize = data.size;

	sound->buffer.resize(data.size);
	file.read(reinterpret_cast<char*>(&sound->buffer[0]), data.size);

	file.close();

	if (handle.empty()) {
		handle = "NoNameHandle_" + filepath;
	}

	lock.lock();
	GetInstance()->mAudioMap[handle] = sound;
	return handle;
}

void RAudio::Play(const AudioHandle handle, const float volume, const float pitch, const bool loop)
{
	RAudio* instance = GetInstance();
	HRESULT result;

	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	if (instance->mAudioMap.find(handle) == instance->mAudioMap.end()) {
		return;
	}

	shared_ptr<AudioData> data = instance->mAudioMap[handle];

	if (data->type == AudioType::Wave) {
		shared_ptr<WaveAudio> waveData = static_pointer_cast<WaveAudio>(data);

		IXAudio2SourceVoice* pSourceVoice = nullptr;
		result = instance->mXAudio2->CreateSourceVoice(&pSourceVoice, &waveData->wfex);
		assert(SUCCEEDED(result));

		XAUDIO2_BUFFER buf{};
		buf.pAudioData = &waveData->buffer[0];
		buf.AudioBytes = waveData->bufferSize;
		buf.LoopCount = loop ? XAUDIO2_LOOP_INFINITE : 0;
		buf.Flags = XAUDIO2_END_OF_STREAM;

		result = pSourceVoice->SubmitSourceBuffer(&buf);
		assert(SUCCEEDED(result));
		result = pSourceVoice->SetVolume(volume);
		assert(SUCCEEDED(result));
		result = pSourceVoice->SetFrequencyRatio(pitch);
		assert(SUCCEEDED(result));
		result = pSourceVoice->Start();
		assert(SUCCEEDED(result));

		instance->mPlayingList.push_back({ handle, pSourceVoice });
	}
}

void RAudio::Stop(AudioHandle handle)
{
	RAudio* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			info.pSource->Stop();
			info.pSource->DestroyVoice();
			itr = instance->mPlayingList.erase(itr);
			continue;
		}
		itr++;
	}
}

bool RAudio::IsPlaying(AudioHandle handle)
{
	RAudio* instance = GetInstance();
	std::lock_guard<std::recursive_mutex> lock(GetInstance()->mMutex);
	for (auto itr = instance->mPlayingList.begin(); itr != instance->mPlayingList.end();) {
		PlayingInfo info = *itr;
		if (info.handle == handle) {
			
		}
	}
	return false;
}

void RAudio::InitInternal()
{
	HRESULT result;
	result = XAudio2Create(&mXAudio2, 0, XAUDIO2_DEFAULT_PROCESSOR);
	assert(SUCCEEDED(result));

	result = mXAudio2->CreateMasteringVoice(&mMasteringVoice);
	assert(SUCCEEDED(result));
}

void RAudio::FinalInternal()
{
	mMasteringVoice->DestroyVoice();
	mXAudio2.Reset();
	mAudioMap.clear();
}
