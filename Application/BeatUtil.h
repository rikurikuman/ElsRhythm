#pragma once
#include "Util.h"
#include <map>
#include <list>

struct Meter {
	int32_t beatAmount = 4; //1���߂ɓ��锏�̐�
	int32_t beatLength = 4; //1���̒���
};

struct Beat {
	int32_t measure = 0; //���ߔԍ�
	int32_t beat = 0; //����
	int32_t LPB = 4; //1���̒���

	bool operator==(const Beat& a) const {
		return measure == a.measure && (float)beat / LPB == (float)a.beat / a.LPB;
	}

	bool operator<(const Beat& a) const {
		if (this->measure < a.measure) {
			return true;
		}
		if (this->measure == a.measure
			&& (float)this->beat / this->LPB < (float)a.beat / a.LPB) {
			return true;
		}

		return false;
	}

	bool operator<=(const Beat& a) const {
		if (this->measure < a.measure) {
			return true;
		}
		if (this->measure == a.measure
			&& (float)this->beat / this->LPB <= (float)a.beat / a.LPB) {
			return true;
		}

		return false;
	}
};

class MusicDesc {
public:
	float baseBPM;
	std::map<Beat, float> bpmChange;
	Meter baseMeter;
	std::map<Beat, Meter> meterChange;
	std::list<Beat> changeList;
	std::map<Beat, float> cacheMiliSec;

	Beat metronomeBeat;

	MusicDesc()
		: baseBPM(120), bpmChange(std::map<Beat, float>{}), baseMeter({ 4, 4 }), meterChange(std::map<Beat, Meter>{}) {}

	MusicDesc(float baseBPM, std::map<Beat, float> bpmChange, Meter baseMeter, std::map<Beat, Meter> meterChange)
		: baseBPM(baseBPM), bpmChange(bpmChange), baseMeter(baseMeter), meterChange(meterChange) {
		Init();
	}

	void Init();

	//�w��n�_��BPM�𓾂�
	float GetBPM(const Beat& beat = { 0, 0, 1 });

	//�w��n�_�̔��q�𓾂�
	Meter GetMeter(const Beat& beat = { 0, 0, 1 });

	//�S�����̒����𓾂�
	float GetWholeNoteLength(const Beat& beat = {0, 0, 1});

	//1���߂̒����𓾂�
	float GetMeasureLength(const int32_t& measure = 0);

	//��������~���b�ɕϊ�����
	float ConvertBeatToMiliSeconds(const Beat& beat);

	//�~���b���甏���ɕϊ�����
	Beat ConvertMiliSecondsToBeat(const float& miliSec);

	//���g���m�[���@�\������������
	void InitMetronome(const float& miliSec);

	//���g���m�[�����Đ�����
	void PlayMetronome(const float& miliSec);
};