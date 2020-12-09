#ifndef PERCEPTRON_BRANCH_PREDICTOR_H
#define PERCEPTRON_BRANCH_PREDICTOR_H

#include "branch_predictor.h"
#include <iterator>
#include <vector>

class PerceptronBranchPredictor : public BranchPredictor
{
public:
  // perceptron data structure
  typedef std::vector<int> perceptron; // just a vector of integers

  PerceptronBranchPredictor (String, core_id_t, int, int, int, int);
  ~PerceptronBranchPredictor ();

  bool predict (IntPtr, IntPtr);
  void update (bool, bool, IntPtr, IntPtr);

private:
  int m_history;                           // history length for the global history shift register
  int m_size;                              // number of perceptrons
  int m_theta;                             // threshold for training
  std::vector<perceptron> m_perceptrons;   // table of perceptrons
  int m_output;                            // the output of selected perceptron
  const int m_max_weight, m_min_weight;    // the maximum and minimum weight for each perceptron
  unsigned long long int m_global_history; // real global history - updated when the predictor is updated
  int normalized_add (int, int) const;
  inline std::vector<int>::iterator hashed_perceptron (IntPtr, IntPtr);
};

#endif // perceptron_branch_predictor.h
