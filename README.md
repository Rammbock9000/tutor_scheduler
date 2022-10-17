You need a working installation of ScaLP (https://digidev.digi.e-technik.uni-kassel.de/scalp/)
In the following, we assume that it is installed in /opt/scalp

Build the project with
  1) "cmake -DCMAKE_PREFIX_PATH=/opt/scalp ."
  2) "make"

Follow the example in file "Exercises_and_Tutors.txt" to set up the solver (lines starting with '#' are comments)

Execute the program with
  ./tutor_scheduler Exercises_and_Tutors.txt

Scheduling results are written to the console