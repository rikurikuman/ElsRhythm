#pragma once

//外部ヘッダ
#pragma warning(push, 0)
#include <Windows.h>
#include <mmsystem.h>
#pragma warning(pop)

namespace TimeManager
{
	extern float targetFPS;
	extern float fps;
	extern float deltaTime;
	extern float deltaMiliTime;
	extern float deltaFrame;
	extern LARGE_INTEGER timeFreq;
	extern LARGE_INTEGER timeStart;
	extern LARGE_INTEGER timeEnd;
	
	void Init();

	void Update();
};

