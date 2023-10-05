#include "BackGroundSelector.h"
#include <BGSimple.h>

void BackGroundSelector::Select(std::string name)
{
    if (name == "Simple") {
        bg = std::make_unique<BGSimple>();
    }
    else {
        bg = std::make_unique<IBackGround>();
    }
}

void BackGroundSelector::Update()
{
    bg->Update();
}

void BackGroundSelector::Draw()
{
    bg->Draw();
}