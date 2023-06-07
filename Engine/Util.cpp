#include "Util.h"
#include <windows.h>

using namespace std;

bool Util::debugBool = false;
int Util::debugInt = 0;

std::chrono::high_resolution_clock::time_point Util::memTimePoint;
double Util::memElapsedTime = 0;

float Util::AngleToRadian(float angle)
{
	return angle * (PI / 180);
}

float Util::RadianToAngle(float radian)
{
	return radian * (180 / PI);
}

vector<string> Util::StringSplit(string s, string separator)
{
	vector<string> result;

	//区切り文字の長さが0なら区切るわけがないのでそのまま返す
	if (separator.length() == 0) {
		result.push_back(s);
		return result;
	}

	auto offset = string::size_type(0);
	while (1) {
		//区切り文字が出現する位置を取得
		auto find = s.find(separator, offset);

		//文字列の終点まで行ったら
		if (find == string::npos) {
			//残り全部加えておしまい
			result.push_back(s.substr(offset));
			break;
		}

		//そうでないなら区切って加える
		result.push_back(s.substr(offset, find - offset));

		//処理済みの所までイテレータを進める
		offset = find + separator.length();
	}

	return result;
}

int Util::Clamp(int i, int min, int max) {
	if (i < min) { return min; }
	if (i > max) { return max; }
	return i;
}

float Util::Clamp(float f, float min, float max) {
	if (f < min) { return min; }
	if (f > max) { return max; }
	return f;
}

double Util::Clamp(double d, double min, double max) {
	if (d < min) { return min; }
	if (d > max) { return max; }
	return d;
}

wstring Util::ConvertStringToWString(string str) {
	//必要なwchar_t配列長を得る
	int _arraySize = MultiByteToWideChar(CP_ACP, 0, str.c_str()
		, -1, (wchar_t*)NULL, 0);

	//配列を用意する
	wchar_t* wArray = new wchar_t[_arraySize];

	//変換してwchar_tの配列にぶち込む
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, wArray
		, _arraySize);

	//wstringにする
	std::wstring wStr(wArray, wArray + _arraySize - 1);

	//配列は罪深いことにnewしてしまったのでdeleteする
	delete[] wArray;

	//おしまい
	return wStr;
}

int Util::GetRand(int min, int max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_int_distribution<> distr(min, max);
	return distr(eng);
}

float Util::GetRand(float min, float max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<float> distr(min, max);
	return distr(eng);
}

double Util::GetRand(double min, double max)
{
	std::random_device rd;
	std::default_random_engine eng(rd());
	std::uniform_real_distribution<double> distr(min, max);
	return distr(eng);
}

float Util::GetRatio(float a, float b, float p)
{
	float size = b - a;
	if (size == 0) {
		return 1.0f;
	}
	return (p - a) / size;
}

void Util::CalcElapsedTimeStart() {
	memTimePoint = std::chrono::high_resolution_clock::now();
	memElapsedTime = 0;
}

void Util::CalcElapsedTimeEnd(std::string name, bool consecutive) {
	auto nowTimePoint = std::chrono::high_resolution_clock::now();
	double elapsedMicro = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(nowTimePoint - memTimePoint).count());
	if (consecutive) elapsedMicro -= memElapsedTime;
	double elapsedMili = elapsedMicro / 1000;
	
	OutputDebugStringA((name + ": " + StringFormat("%.3f", elapsedMili) + "ms / " + StringFormat("%.0f", elapsedMicro) + "us\n").c_str());
	memElapsedTime += elapsedMicro;
}

void Util::DebugLog(std::string log) {
#ifdef _DEBUG
	OutputDebugStringA((log + "\n").c_str());
#endif
}

void Util::DebugLogC(std::string log)
{
	if (debugBool) {
		OutputDebugStringA((log + "\n").c_str());
	}
}
