#include "EventSystem.h"

bool EventSystem::HasTriggeredEvent(std::string name)
{
    for (Event& e : GetInstance()->triggeredEvents) {
        if (e.name == name) return true;
    }

    return false;
}

std::vector<Event> EventSystem::GetTriggeredEvents(std::string name)
{
    EventSystem* instance = GetInstance();

    std::vector<Event> vec;
    for (Event& e : instance->triggeredEvents) {
        if (e.name == name) vec.push_back(e);
    }

    return vec;
}

void EventSystem::TriggerEvent(Event event)
{
    GetInstance()->triggeredEvents.push_back(event);
}

void EventSystem::Clear()
{
    GetInstance()->triggeredEvents.clear();
}

EventSystem* EventSystem::GetInstance()
{
    static EventSystem instance;
    return &instance;
}
