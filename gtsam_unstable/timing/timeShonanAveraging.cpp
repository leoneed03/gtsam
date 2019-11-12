/* ----------------------------------------------------------------------------

 * GTSAM Copyright 2010-2019, Georgia Tech Research Corporation,
 * Atlanta, Georgia 30332-0415
 * All Rights Reserved
 * Authors: Frank Dellaert, et al. (see THANKS for the full author list)

 * See LICENSE for the license information

 * -------------------------------------------------------------------------- */

/**
 * @file   testShonanAveraging.cpp
 * @date   September 2019
 * @author Jing Wu
 * @author Frank Dellaert
 * @brief  Timing script for Shonan Averaging algorithm
 */

#include <gtsam_unstable/slam/ShonanAveraging.h>
#include <gtsam/base/timing.h>

#include <CppUnitLite/TestHarness.h>

#include <iostream>
#include <fstream>
#include <map>
#include <chrono>

using namespace std;
using namespace gtsam;

//string g2oFile = findExampleDataFile("toyExample.g2o");
// string g2oFile = "/Users/dellaert/git/SE-Sync/data/toy3D.g2o";
// string g2oFile = "/home/jingwu/catkin_workspace/gtsam/examples/Data/tinyGrid3D.g2o";

// save a single line of timing info to an output stream
void saveData(size_t p, double time, double costP, double cost3, std::ostream* os){
  *os << (int)p << "\t" << time << "\t" << costP << "\t" << cost3 << endl;
}

int main(int argc, char* argv[]) {
  // primitive argument parsing:
  if (argc > 3) {
    throw runtime_error("Usage: timeShonanAveraging  [g2oFile]");
  }

  string g2oFile;
  try {
    if (argc > 1)
      g2oFile = argv[argc - 1];
    else
//         g2oFile = "/home/jingwu/catkin_workspace/gtsam/examples/Data/toyExample.g2o";
        g2oFile = string("/home/jingwu/Desktop/CS8903/SESync/data/SE3/tinyGrid3D.g2o");

    } catch (const exception& e) {
      cerr << e.what() << '\n';
      exit(1);
    }

  // Create a csv output file
  ofstream csvFile("shonan_timing.csv");

  // Create Shonan averaging instance from the file.
  static const ShonanAveraging kShonan(g2oFile);

  // increase p value and try optimize using Shonan Algorithm. use chrono for timing
  const size_t N = 5;
  for (size_t p = 3; p < 15; p++){
    // cout << "*********************************************************" << endl;
    const Values initial = kShonan.initializeRandomlyAt(p);
    gttic_(optimize);
    chrono::steady_clock::time_point t1 = chrono::steady_clock::now();
    double sumCostP=0, sumCost3=0;
    for (size_t i = 0; i < N; i++){
      const Values result = kShonan.tryOptimizingAt(p, initial);
      sumCostP += kShonan.costAt(p, result);
      const Values SO3Values = kShonan.projectFrom(p, result);
      sumCost3 += kShonan.cost(SO3Values);
    }

    chrono::steady_clock::time_point t2 = chrono::steady_clock::now();
    chrono::duration<double> timeUsed = chrono::duration_cast<chrono::duration<double>>( t2-t1 ) / N;
    saveData(p, timeUsed.count(), sumCostP/N, sumCost3/N, &cout);
    saveData(p, timeUsed.count(), sumCostP/N, sumCost3/N, &csvFile);
    // tictoc_finishedIteration_();
    // tictoc_print_();
  }
  
  return 0;
}