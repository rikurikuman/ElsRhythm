#pragma once
#include <Windows.h>
#include <mmsystem.h>

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

