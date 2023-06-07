#pragma once
#include <string>
#include <vector>
#include "RenderOrder.h"

class IRenderStage
{
public:
	virtual ~IRenderStage() {};

	virtual std::string GetTypeIndentifier() = 0;

	virtual void Init() = 0;
	virtual void Render() = 0;

	void AllCall();

	bool enabled = true;
	RenderOrder defParamater;
	std::vector<RenderOrder> orders;
};