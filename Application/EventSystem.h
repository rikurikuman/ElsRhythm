#pragma once
#include <string>
#include <vector>
#include <list>

struct Event {
	std::string name;
	int32_t paramInt = 0;
	float paramFloat = 0;
	bool paramBool = false;
	std::string paramString;
};

class EventSystem
{
public:
	static bool HasTriggeredEvent(std::string name);
	static std::vector<Event> GetTriggeredEvents(std::string name);

	static void TriggerEvent(Event event);
	static void Clear();

private:
	static EventSystem* GetInstance();
	std::list<Event> triggeredEvents;
};

