#pragma once
#include "json.hpp"
#include <BeatUtil.h>
#include <Note.h>
#include <string>

class ChartFile
{
public:
	std::string path;

	std::string audiopath;
	float audioOffset = 0;

	MusicDesc music;

	std::map<Beat, float> scrollChange;

	std::list<Note> notes;

	ChartFile() {}
	ChartFile(std::string path) : path(path) {}

	bool Load();
	bool Save();
};

//JsonWrapper
using json = nlohmann::json;

//Vector3
void to_json(json& j, const Vector3& o);
void from_json(const json& j, Vector3& o);

//Beat
void to_json(json& j, const Beat& o);
void from_json(const json& j, Beat& o);

//Meter
void to_json(json& j, const Meter& o);
void from_json(const json& j, Meter& o);

//Note
void to_json(json& j, const Note& o);
void from_json(const json& j, Note& o);

//Note::ControlPoint
void to_json(json& j, const Note::ControlPoint& o);
void from_json(const json& j, Note::ControlPoint& o);