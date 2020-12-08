#ifndef PERCEPTRON_BRANCH_PREDICTOR_H
#define PERCEPTRON_BRANCH_PREDICTOR_H

#include "branch_predictor.h"

#include <deque>
#include <vector>
#include <utility>

// perceptron data structure
class Perceptron
{
public:
  Perceptron (UInt32 size);
  std::vector<int> m_weights; // a vector of weights
};

class PerceptronBranchPredictor : public BranchPredictor
{
public:
  PerceptronBranchPredictor (String name, core_id_t core_id, UInt32 history,
                             UInt32 size, UInt32 bits, UInt32 theta);
  ~PerceptronBranchPredictor ();

  bool predict (IntPtr ip, IntPtr target);
  void update (bool predicted, bool actual, IntPtr ip, IntPtr target);

private:
  UInt32 m_history; // history length for the global history shift register
  UInt32 m_size;    // number of perceptrons
  UInt32 m_bits;    // number of bits per weight
  UInt32 m_theta;   // threshold for training
  int normalized_add (int, int) const;
  std::vector<Perceptron> m_perceptrons; // table of perceptrons
  std::deque<int> m_global_history;      // real global history
  std::deque< std::pair<Perceptron&, int> > m_perceptron_state; // state for updating perceptron predictor
};

#endif // perceptron_branch_predictor.h
