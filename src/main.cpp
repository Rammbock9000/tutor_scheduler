//
// Created by nfiege on 8/3/22.
//

#include <iostream>
#include <TutorScheduler.h>

int main(int argc, char *argv[]) {
	// create scheduler
	TutorScheduler scheduler({"Gurobi"});

	// read problem from file
	if (argc != 2) {
		std::cout << "Expected exactly ONE argument (path to txt file containing the scheduling problem) but got " << argc-1 << std::endl;
		return 0;
	}
	scheduler.defineProblemFromFile(std::string(argv[1]));

	// solve
	scheduler.solve();

	// print solution
	scheduler.printSolution();
	return 0;
}