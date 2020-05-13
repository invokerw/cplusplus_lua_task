#include <iostream>
#include <string>


#include "task.h"
#include "event.h"

using namespace WEIFEI;

int main()
{
	Task *t = new Task();
	bool b = t->Init("./scripts/test.lua", "");
	if(!b) std::cout << "Init Flase 1" << std::endl;
	TaskManager::GetInstance()->Register(t);
	while(true)
	{
		TaskManager::GetInstance()->Tick();
	}
	return 0;
}
