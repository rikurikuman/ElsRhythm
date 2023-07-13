#include "Util.h"
#include <windows.h>

using namespace std;

bool Util::debugBool = false;
int32_t Util::debugInt = 0;

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

	//��؂蕶���̒�����0�Ȃ��؂�킯���Ȃ��̂ł��̂܂ܕԂ�
	if (separator.length() == 0) {
		result.push_back(s);
		return result;
	}

	auto offset = string::size_type(0);
	while (1) {
		//��؂蕶�����o������ʒu���擾
		auto find = s.find(separator, offset);

		//������̏I�_�܂ōs������
		if (find == string::npos) {
			//�c��S�������Ă����܂�
			result.push_back(s.substr(offset));
			break;
		}

		//�����łȂ��Ȃ��؂��ĉ�����
		result.push_back(s.substr(offset, find - offset));

		//�����ς݂̏��܂ŃC�e���[�^��i�߂�
		offset = find + separator.length();
	}

	return result;
}

int32_t Util::Clamp(int32_t i, int32_t min, int32_t max) {
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
	//�K�v��wchar_t�z�񒷂𓾂�
	int32_t _arraySize = MultiByteToWideChar(CP_ACP, 0, str.c_str()
		, -1, (wchar_t*)NULL, 0);

	//�z���p�ӂ���
	std::vector<wchar_t> wArray;
	wArray.resize(_arraySize);

	//�ϊ�����wchar_t�̔z��ɂԂ�����
	MultiByteToWideChar(CP_ACP, 0, str.c_str(), -1, &wArray[0], _arraySize);

	//wstring�ɂ���
	std::wstring wStr(&wArray[0], &wArray.back());

	//�����܂�
	return wStr;
}

int32_t Util::GetRand(int32_t min, int32_t max)
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
	if (a > b) {
		float temp = b;
		b = a;
		a = temp;
	}

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
