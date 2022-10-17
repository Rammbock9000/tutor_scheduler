//
// Created by nfiege on 8/4/22.
//

#ifndef TUTOR_SCHEDULER_UTILITY_H
#define TUTOR_SCHEDULER_UTILITY_H

#include <iostream>
#include <string>

enum day_t {
	Monday,
	Tuesday,
	Wednesday,
	Thursday,
	Friday
};

std::ostream& operator<<(std::ostream & os, const day_t & day);

class Utility {
public:
	static std::string dayToString(const day_t &day);
	static day_t stringToDay(const std::string &day);
};


#endif //TUTOR_SCHEDULER_UTILITY_H
