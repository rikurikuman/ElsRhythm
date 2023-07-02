#include "ChartFile.h"
#include <iostream>
#include <fstream>

using namespace std;

bool ChartFile::Load()
{
	ifstream ifs(path);
	if (ifs) {
		json j;

		try {
			ifs >> j;

			if (j["audiopath"].is_string()) {
				audiopath = j["audiopath"].get<string>();
			}
			
			if (!j["audiooffset"].is_null()) {
				audioOffset = j["audiooffset"].get<float>();
			}
			
			music = MusicDesc();

			if (!j["basebpm"].is_null()) {
				music.baseBPM = j["basebpm"].get<float>();
			}

			if (!j["basemeter"].is_null()) {
				music.baseMeter = j["basemeter"].get<Meter>();
			}

			json bpms = j["bpmchange"];
			if (!bpms.is_null()) {
				for (json::iterator itr = bpms.begin(); itr != bpms.end(); itr++) {
					json obj = *itr;
					
					if (!obj["beat"].is_null() && !obj["bpm"].is_null()) {
						music.bpmChange[obj["beat"].get<Beat>()] = obj["bpm"].get<float>();
					}
				}
			}

			json meters = j["meterchange"];
			if (!meters.is_null()) {
				for (json::iterator itr = meters.begin(); itr != meters.end(); itr++) {
					json obj = *itr;

					if (!obj["beat"].is_null() && !obj["meter"].is_null()) {
						music.meterChange[obj["beat"].get<Beat>()] = obj["meter"].get<Meter>();
					}
				}
			}

			music.Init();

			scrollChange.clear();
			json scrolls = j["scrollchange"];
			if (!scrolls.is_null()) {
				for (json::iterator itr = scrolls.begin(); itr != scrolls.end(); itr++) {
					json obj = *itr;

					if (!obj["beat"].is_null() && !obj["scroll"].is_null()) {
						scrollChange[obj["beat"].get<Beat>()] = obj["scroll"].get<float>();
					}
				}
			}

			notes.clear();
			json _notes = j["notes"];
			if (!_notes.is_null()) {
				for (json::iterator itr = _notes.begin(); itr != _notes.end(); itr++) {
					json obj = *itr;

					notes.push_back(obj.get<Note>());
				}
			}
		}
		catch (json::exception e) {
			Util::DebugLog(e.what());
			return false;
		}

		return true;
	}
	else {
		Util::DebugLog("ì«Ç›çûÇﬂÇ‹ÇπÇÒÇ≈ÇµÇΩ");
		return false;
	}

	return false;
}

bool ChartFile::Save() {

	json j;

	j["audiopath"] = audiopath;
	j["audiooffset"] = audioOffset;
	j["basebpm"] = music.baseBPM;
	j["basemeter"] = music.baseMeter;

	for (std::pair<Beat, float> c : music.bpmChange) {
		json obj;
		obj["beat"] = c.first;
		obj["bpm"] = c.second;
		j["bpmchange"].push_back(obj);
	}

	for (std::pair<Beat, Meter> c : music.meterChange) {
		json obj;
		obj["beat"] = c.first;
		obj["meter"] = c.second;
		j["meterchange"].push_back(obj);
	}

	for (std::pair<Beat, float> c : scrollChange) {
		json obj;
		obj["beat"] = c.first;
		obj["scroll"] = c.second;
		j["scrollchange"].push_back(obj);
	}

	for (Note& n : notes) {
		j["notes"].push_back(n);
	}

	ofstream ofs(path);
	if (ofs) {
		ofs << j.dump(4);
		return true;
	}
	else {
		return false;
	}
	return false;
}

void to_json(json& j, const Beat& o)
{
	j["measure"] = o.measure;
	j["beat"] = o.beat;
	j["lpb"] = o.LPB;
}

void from_json(const json& j, Beat& o)
{
	if (j.find("measure") != j.end()) {
		o.measure = j.at("measure").get<int32_t>();
	}
	if (j.find("beat") != j.end()) {
		o.beat = j.at("beat").get<int32_t>();
	}
	if (j.find("lpb") != j.end()) {
		o.LPB = j.at("lpb").get<int32_t>();
	}
}

void to_json(json& j, const Meter& o)
{
	j["amount"] = o.beatAmount;
	j["length"] = o.beatLength;
}

void from_json(const json& j, Meter& o)
{
	if (j.find("amount") != j.end()) {
		o.beatAmount = j.at("amount").get<int32_t>();
	}
	if (j.find("length") != j.end()) {
		o.beatLength = j.at("length").get<int32_t>();
	}
}

void to_json(json& j, const Note& o)
{
	j["beat"] = o.beat;
	j["lane"] = o.lane;
}

void from_json(const json& j, Note& o)
{
	if (j.find("beat") != j.end()) {
		o.beat = j.at("beat").get<Beat>();
	}
	if (j.find("lane") != j.end()) {
		o.lane = j.at("lane").get<int32_t>();
	}
}
