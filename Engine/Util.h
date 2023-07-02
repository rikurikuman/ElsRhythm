#pragma once
#include <string>
#include <vector>
#include <list>
#include <stdexcept>
#include <random>
#include <chrono>

namespace Util {
	extern bool debugBool;
    extern int32_t debugInt;
	constexpr float PI = 3.1415926535897932384626f;

    const int32_t WIN_WIDTH = 1280;
    const int32_t WIN_HEIGHT = 720;

    extern std::chrono::high_resolution_clock::time_point memTimePoint;
    extern double memElapsedTime;

	//Degree����Radian�֕ϊ�����
	float AngleToRadian(float angle);

	//Radian����Degree�֕ϊ�����
	float RadianToAngle(float radian);

    /*
    printf�݂�����
    �t�H�[�}�b�g�w��q���܂ޕ�����(X = %d�݂����Ȃ�)��
    �������D���Ȃ���������
    ���̌��ʂ̕������Ԃ�
    ��:
    StringFormat("X = %d, Y = %d", 100, 500); -> "X = 100, Y = 500"
*/
    template<typename ... Args>
    std::string StringFormat(const std::string& format, Args&& ... args) {
        /* �t�H�[�}�b�g��̕��������Z�o */
        int32_t str_len = std::snprintf(nullptr, 0, format.c_str(), std::forward<Args>(args) ...);

        /* �t�H�[�}�b�g���s */
        if (str_len < 0)
        {
            throw std::runtime_error("String Formatting Error");
        }

        /* �o�b�t�@�T�C�Y���Z�o(������ + null�����T�C�Y) */
        size_t bufferSize = str_len + sizeof(char);

        /* �o�b�t�@�T�C�Y���������m�� */
        std::vector<char> buffer;
        buffer.resize(bufferSize);

        /* ������̃t�H�[�}�b�g */
        std::snprintf(&buffer[0], bufferSize, format.c_str(), args ...);

        /* �������std::string�^�ɕϊ����ďo�� */
        return std::string(&buffer[0], &buffer[0] + str_len);
    }

    /*
        Java�ł���instanceof�Ƃقڈꏏ�I
        instanceof<int>(a)�݂����ȏ������łǂ���
    */
    template <typename check, typename NANIYATU>
    inline bool instanceof(const NANIYATU& a)
    {
        return dynamic_cast<const check*>(&a) != 0;
    }

    /*
        std::string��split���Ȃ��̂�
        ���O��split�ł�����
    */
    std::vector<std::string> StringSplit(std::string s, std::string separator);

    //�w�肵��int��min����max�̊ԂɎ��܂��Ă���΂��̂܂܁A
    //���܂��Ă��Ȃ���Ύ��߂ĕԂ�
    int32_t Clamp(int32_t i, int32_t min, int32_t max);

    //�w�肵��float��min����max�̊ԂɎ��܂��Ă���΂��̂܂܁A
    //���܂��Ă��Ȃ���Ύ��߂ĕԂ�
    float Clamp(float f, float min, float max);

    //�w�肵��double��min����max�̊ԂɎ��܂��Ă���΂��̂܂܁A
    //���܂��Ă��Ȃ���Ύ��߂ĕԂ�
    double Clamp(double d, double min, double max);

    //std::vector��search���܂܂�Ă��邩���ׂ�
    template <class T>
    bool Contains(std::vector<T> vector, T search) {
        for (auto itr = vector.begin(); itr != vector.end(); itr++) {
            if (*itr == search) {
                return true;
            }
        }
        return false;
    }

    //std::list��search���܂܂�Ă��邩���ׂ�
    template <class T>
    bool Contains(std::list<T> list, T search) {
        auto itr = std::find(list.begin(), list.end(), search);
        return itr != list.end();
    }

    //std::vector��search������Ȃ炻�̃C���f�b�N�X�𓾂�
    //�Ȃ��Ȃ�-1
    template <class T>
    int32_t IndexOf(std::vector<T> vector, T search) {
        auto itr = std::find(vector.begin(), vector.end(), search);
        if (itr == vector.end()) {
            return -1;
        }
        return static_cast<int32_t>(std::distance(vector.begin(), itr));
    }

    //string��wstring�ɕϊ�����iMultiByteToWideChar)
    std::wstring ConvertStringToWString(std::string str);

    //��������(��l���z)
    int32_t GetRand(int32_t min, int32_t max);

    //��������(��l���z)
    float GetRand(float min, float max);

    //��������(��l���z)
    double GetRand(double min, double max);

    /// <summary>
    /// a~b�͈͓̔��ŁA�ǂ̊�����p���ʒu���Ă��邩��Ԃ�
    /// e.x. a=2,b=6,p=4 �Ȃ� return 0.5f;
    /// </summary>
    /// <returns>����</returns>
    float GetRatio(float a, float b, float p);

    //�������Ԍv���J�n
    void CalcElapsedTimeStart();
    //�������Ԍv���o��
    void CalcElapsedTimeEnd(std::string name, bool consecutive = true);

    //�f�o�b�O�o��(�f�o�b�O�r���h�ł̂ݏ���)
    void DebugLog(std::string log);

    //�f�o�b�O�o��(debugBool�������Ă��鎞����)
    void DebugLogC(std::string log);
}
