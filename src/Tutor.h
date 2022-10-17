//
// Created by nfiege on 8/3/22.
//

#ifndef TUTOR_SCHEDULER_TUTOR_H
#define TUTOR_SCHEDULER_TUTOR_H

#include <string>
#include <map>
#include <iostream>
#include <Utility.h>

class Tutor {
public:
	Tutor();
	int maxNumGroups;
	std::string name;
	std::map<std::pair<day_t, int>, int> timeSlotPriorities;
	double prio;
};


#endif //TUTOR_SCHEDULER_TUTOR_H
