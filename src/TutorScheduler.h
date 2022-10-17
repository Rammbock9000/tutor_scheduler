//
// Created by nfiege on 8/3/22.
//

#ifndef TUTOR_SCHEDULER_TUTORSCHEDULER_H
#define TUTOR_SCHEDULER_TUTORSCHEDULER_H

#include <string>
#include <set>
#include <map>
#include <ScaLP/Solver.h>
#include <Tutor.h>

class TutorScheduler {
public:
	TutorScheduler(const std::list<std::string> &sw);
	void solve();
	Tutor* getTutor(std::string &name);
	void addTutor(Tutor tutor);
	void addExerciseTime(const day_t &day, const int &startTime, const int &numTutorsPerExercise);
	void printSolution();
	void defineProblemFromFile(const std::string &pathToFile);

private:
	int timeout;
	int numThreads;
	ScaLP::Solver solver;
	int numExercises;
	std::set<std::tuple<day_t, int, int>> exerciseTimes;
	std::map<std::string, Tutor> tutors;
	std::map<std::string, std::set<std::pair<day_t, int>>> schedule;
};


#endif //TUTOR_SCHEDULER_TUTORSCHEDULER_H
