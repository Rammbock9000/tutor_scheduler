//
// Created by nfiege on 8/3/22.
//

#include "TutorScheduler.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <cmath>


TutorScheduler::TutorScheduler(const std::list<std::string> &sw)
	: solver(sw), timeout(300), numThreads(1), numExercises(-1) {}

void TutorScheduler::solve() {
	// clear schedule container
	this->schedule.clear();

	// assignment variables
	std::map<std::tuple<Tutor*, day_t, int>, ScaLP::Variable> vars;
	for (auto &exerciseTime : this->exerciseTimes) {
		for (auto &tutor : this->tutors) {
			std::stringstream varName;
			varName << tutor.first << "_" << std::get<0>(exerciseTime) << "_" << std::get<1>(exerciseTime);
			vars[{&tutor.second, std::get<0>(exerciseTime), std::get<1>(exerciseTime)}] = ScaLP::newBinaryVariable(varName.str());
		}
	}

	// constraint "each exercise gets assigned at most n tutors"
	for (auto &exerciseTime : this->exerciseTimes) {
		ScaLP::Term t;
		for (auto &tutor : this->tutors) {
			t += vars[{&tutor.second, std::get<0>(exerciseTime), std::get<1>(exerciseTime)}];
		}
		if (this->numExercises > 0) {
			this->solver.addConstraint(t <= std::get<2>(exerciseTime));
		}
		else {
			this->solver.addConstraint(t == std::get<2>(exerciseTime));
		}
	}

	// constraint "choose exactly n exercises"
	if (this->numExercises > 0) {
		ScaLP::Term t;
		for (auto &exerciseTime : this->exerciseTimes) {
			for (auto &tutor : this->tutors) {
				t += vars[{&tutor.second, std::get<0>(exerciseTime), std::get<1>(exerciseTime)}];
			}
		}
		this->solver.addConstraint(t == this->numExercises);
	}

	// constraint "each tutor gets assigned to at most n exercises"
	for (auto &tutor : this->tutors) {
		ScaLP::Term t;
		for (auto &exerciseTime : this->exerciseTimes) {
			t += vars[{&tutor.second, std::get<0>(exerciseTime), std::get<1>(exerciseTime)}];
		}
		this->solver.addConstraint(t <= tutor.second.maxNumGroups);
	}

	// objective: maximize priorities
	ScaLP::Term obj;
	for (auto &tutor : this->tutors) {
		for (auto &exerciseTime : this->exerciseTimes) {
			obj += (tutor.second.prio * tutor.second.timeSlotPriorities[{std::get<0>(exerciseTime), std::get<1>(exerciseTime)}] * vars[{&tutor.second, std::get<0>(exerciseTime), std::get<1>(exerciseTime)}]);
		}
	}
	this->solver.setObjective(ScaLP::maximize(obj));

	// solve
	std::cout << "Start solving with " << this->numThreads << " threads and timeout=" << this->timeout << "sec" << std::endl;
	this->solver.timeout = this->timeout;
	this->solver.threads = this->numThreads;
	auto stat = this->solver.solve();

	// get solution
	if (stat != ScaLP::status::OPTIMAL and stat != ScaLP::status::TIMEOUT_FEASIBLE and stat != ScaLP::status::FEASIBLE) {
		std::cout << "failed to find schedule (status " << ScaLP::showStatus(stat) << ")" << std::endl;
		return;
	}
	auto solverResultContainer = this->solver.getResult();
	auto objectiveValue = solverResultContainer.objectiveValue;
	auto results = solverResultContainer.values;
	std::cout << "successfully computed schedule with ScaLP status " << ScaLP::showStatus(stat)
	   << " and objective value " << objectiveValue << std::endl;
	for (auto &it : vars) {
		if (std::round(results.at(it.second)) == 0.0) continue;
		this->schedule[std::get<0>(it.first)->name].insert({std::get<1>(it.first), std::get<2>(it.first)});
	}
}

Tutor *TutorScheduler::getTutor(std::string &name) {
	try {
		return &this->tutors.at(name);
	}
	catch (std::out_of_range &) {
		std::cout << "Unknown tutor: " << name << std::endl;
		return nullptr;
	}
}

void TutorScheduler::addTutor(Tutor tutor) {
	this->tutors[tutor.name] = tutor;
}

void TutorScheduler::printSolution() {
	if (this->schedule.empty()) {
		std::cout << "No feasible tutor schedule found (yet)" << std::endl;
		return;
	}
	std::cout << "Tutor schedule:" << std::endl;
	for (auto &it : this->schedule) {
		std::cout << "  " << it.first << std::endl;
		for (auto &t : it.second) {
			std::cout << "    " << t.first << " at " << t.second << ":00" << std::endl;
		}
	}
}

void TutorScheduler::addExerciseTime(const day_t &day, const int &startTime, const int &numTutorsPerExercise) {
	this->exerciseTimes.insert({day, startTime, numTutorsPerExercise});
}

void TutorScheduler::defineProblemFromFile(const std::string &pathToFile) {
	std::ifstream file;
	file.open(pathToFile);
	if (!file.is_open()) {
		std::cout << "Failed to open file '" << pathToFile << "'" << std::endl;
		file.close();
		return;
	}
	std::string linebuffer;
	while (std::getline(file, linebuffer)) {
		// skip empty lines
		if (linebuffer.empty()) continue;
		// skip comments
		if (linebuffer[0] == '#') continue;
		// cut line into elements
		std::stringstream linestream(linebuffer);
		std::vector<std::string> lineElements;
		std::string element;
		while (std::getline(linestream, element, ' ')) {
			lineElements.emplace_back(element);
		}
		// skip invalid lines
		if (lineElements.size() < 2 or lineElements.size() > 5) continue;
		if ((lineElements[0] == "Threads" or lineElements[0] == "threads" or lineElements[0] == "THREADS") and lineElements.size() >= 2) {
			int threads;
			try {
				threads = std::stoi(lineElements[1]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[1]+"' to int (line "+linebuffer+")");
			}
			this->numThreads = threads;
		}
		if ((lineElements[0] == "Timeout" or lineElements[0] == "timeout" or lineElements[0] == "TIMEOUT") and lineElements.size() >= 2) {
			int t;
			try {
				t = std::stoi(lineElements[1]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[1]+"' to int (line "+linebuffer+")");
			}
			this->timeout = t;
		}
		if ((lineElements[0] == "Exercises" or lineElements[0] == "exercises" or lineElements[0] == "EXERCISES") and lineElements.size() >= 2) {
			int n;
			try {
				n = std::stoi(lineElements[1]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[1]+"' to int (line "+linebuffer+")");
			}
			this->numExercises = n;
		}
		if ((lineElements[0] == "Exercise" or lineElements[0] == "exercise" or lineElements[0] == "EXERCISE") and lineElements.size() >= 3) {
			// parse exercise
			//std::cout << "Found line with exercise definition: " << linebuffer << std::endl;
			int time;
			int numTutorsPerExercise = 1;
			try {
				time = std::stoi(lineElements[2]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[2]+"' to int (line "+linebuffer+")");
			}
			if (lineElements.size() > 3) {
				try {
					numTutorsPerExercise = std::stoi(lineElements[3]);
				}
				catch (std::invalid_argument&) {
					throw std::runtime_error("failed to convert '"+lineElements[3]+"' to int (line "+linebuffer+")");
				}
			}
			this->exerciseTimes.insert({Utility::stringToDay(lineElements[1]), time, numTutorsPerExercise});
		}
		if ((lineElements[0] == "Tutor" or lineElements[0] == "tutor" or lineElements[0] == "TUTOR") and lineElements.size() >= 4) {
			//std::cout << "Found line with tutor definition: " << linebuffer << std::endl;
			auto &tutor = this->tutors[lineElements[1]];
			tutor.name = lineElements[1];
			int maxNumGroups;
			try {
				maxNumGroups = std::stoi(lineElements[2]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[2]+"' to int (line "+linebuffer+")");
			}
			tutor.maxNumGroups = maxNumGroups;
			double prio;
			try {
				prio = std::stod(lineElements[3]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[3]+"' to float (line "+linebuffer+")");
			}
			tutor.prio = prio;
		}
		if ((lineElements[0] == "Preference" or lineElements[0] == "preference" or lineElements[0] == "PREFERENCE") and lineElements.size() >= 5) {
			// parse tutor
			//std::cout << "Found line with tutor preferences definition: " << linebuffer << std::endl;
			auto &tutor = this->tutors[lineElements[1]];
			if (tutor.name.empty()) tutor.name = lineElements[1];
			int time;
			try {
				time = std::stoi(lineElements[3]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[3]+"' to int (line "+linebuffer+")");
			}
			int prio;
			try {
				prio = std::stoi(lineElements[4]);
			}
			catch (std::invalid_argument&) {
				throw std::runtime_error("failed to convert '"+lineElements[4]+"' to int (line "+linebuffer+")");
			}
			tutor.timeSlotPriorities[{Utility::stringToDay(lineElements[2]), time}] = prio;
		}
	}
	file.close();
}
