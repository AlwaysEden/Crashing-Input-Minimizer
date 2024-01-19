# CMIN(Crashing Input minimizer)

## Background
Fixing bugs or debugging is the most time-consuming activities in  software  development  lifecycle  since  it  is  difficult  to mechanize  reasoning  for  identifying  the  locations  of  bugs  in  a program when crashes occur. To alleviate this challenge, many automated approaches that assist developers to diagnose buggy programs and test inputs, including test input generation, crash reproduction, test minimization and fault localization.

## Delta Debugging
"Delta debugging" is a test input minimization technique that takes a target program together with a crashing test input, and then automatically derives a part of the test input that still make the program crash. Reducing test input is helpful for developers because a smaller input covers less code regions, thus the scope to review the source code to find the root cause of the problem (i.e., buggy code elements) is reduced.
The delta algorithm which is implemented is not popular binray algorithm. Since it is infeasible to explore all input subsequences, the algorithm explores only a part of them in a greedy-search manner.

## How to Execute
To execute this program, you can put ./cimin -i [crash input file] -m "[Error string]" -o [file to store output] [target program].
Also, You can check how to execute the program in Makefile.
