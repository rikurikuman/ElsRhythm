#include "BackGroundSelector.h"
#include <BGSimple.h>

void BackGroundSelector::Select(std::string name)
{
    BackGroundSelector* instance = GetInstance();

    if (name == "Simple") {
        instance->bg = std::make_unique<BGSimple>();
    }
    else {
        instance->bg = std::make_unique<IBackGround>();
    }
}

void BackGroundSelector::Update()
{
    GetInstance()->bg->Update();
}

void BackGroundSelector::Draw()
{
    GetInstance()->bg->Draw();
}

BackGroundSelector* BackGroundSelector::GetInstance()
{
    static BackGroundSelector instance;
    return &instance;
}
