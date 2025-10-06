#pragma once
#include <chrono>

class TimeManager
{
public:
	TimeManager();

	float CalculateDeltaTime();

private:
	float deltaTime;


};