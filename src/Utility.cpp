//
// Created by nfiege on 8/4/22.
//

#include "Utility.h"
#include <algorithm>
#include <stdexcept>

std::ostream &operator<<(std::ostream &os, const day_t &day) {
	os << Utility::dayToString(day);
	return os;
}

std::string Utility::dayToString(const day_t &day) {
	switch (day) {
		case Monday:
			return "Monday";
		case Tuesday:
			return "Tuesday";
		case Wednesday:
			return "Wednesday";
		case Thursday:
			return "Thursday";
		default: // Friday
			return "Friday";
	}
}

day_t Utility::stringToDay(const std::string &day) {
	auto dayCpy = day;
	std::transform(dayCpy.begin(), dayCpy.end(), dayCpy.begin(), [](unsigned char c){return std::tolower(c);});
	if (dayCpy == "monday") return Monday;
	else if (dayCpy == "tuesday") return Tuesday;
	else if (dayCpy == "wednesday") return Wednesday;
	else if (dayCpy == "thursday") return Thursday;
	else if (dayCpy == "friday") return Friday;
	else throw std::runtime_error("unknown day: "+day);
}
