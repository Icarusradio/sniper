#include "simulator.h"
#include "perceptron_branch_predictor.h"

PerceptronBranchPredictor::PerceptronBranchPredictor (
  String name, core_id_t core_id, int history, int size, int bits, int theta)
: BranchPredictor(name, core_id),
  m_history (history),
  m_size (size),
  m_theta (theta),
  m_perceptrons (size, perceptron (history + 1, 0)),
  m_output (0),
  m_max_weight ((1 << (bits - 1)) - 1),
  m_min_weight (-(1 << (bits - 1))),
  m_global_history (0)
{}

PerceptronBranchPredictor::~PerceptronBranchPredictor ()
{}

bool
PerceptronBranchPredictor::predict (IntPtr ip, IntPtr target)
{
  int i;
  unsigned long long int mask;

  // Hash the address to get an index into the table of perceptrons
  auto w = hashed_perceptron (ip, target);
  
  // Initialize the output to the bias weight
  m_output = *w;

  // Advance the iterator
  ++w;

  // Find the (rest of the) dot product of the history register
  // and the perceptron weights. Note that, instead of actually
  // doing the expensive multiplies, we simply add a weight when the
  // corresponding branch in the history register is taken, or
  // subtract a weight when the branch is not taken. This also lets
  // us use binary instead of bipolar logic to represent the history
  // register
  for (mask = 1, i = 0; i < m_history; i++, mask <<= 1, w++)
    {
      if (m_global_history & mask)
        m_output += *w;
      else
        m_output += -*w;
    }

  return m_output >= 0;
}

void
PerceptronBranchPredictor::update (bool predicted, bool actual, IntPtr ip, IntPtr target)
{
  updateCounters (predicted, actual);
  int i;
  unsigned long long int mask;

  // If the output of the perceptron predictor is outside of
  // the range [-THETA,THETA] *and* the prediction was correct,
  // then we don't need to adjust the weights

  // By De Morgan's laws
  if ((m_output <= m_theta || !actual) && (m_output >= -m_theta || actual))
    {
      // w is a iterator to the bias weight
      auto w = hashed_perceptron (ip, target);

      // If the branch was taken, increment the bias weight,
      // else decrement it, with saturating arithmetic
      *w = normalized_add (*w, 2 * int (actual) - 1);

      // Now w points to the next weight
      ++w;

      // For each weight and corresponding bit in the history register...
      for (mask = 1, i = 0; i < m_history; i++, mask <<= 1, ++w)
        {
          // If the i'th bit in the history positively correlates
          // with this branch outcome, increment the corresponding 
          // weight, else decrement it, with saturating arithmetic

          // A common trick to conver to boolean => !!x is 1 iff x is not zero,
          // in this case history is positively correlated with branch outcome
          *w = normalized_add (*w, 2 * int (!!(m_global_history & mask) == actual) - 1);
        }
    }

  // Update the real global history shift register
  m_global_history <<= 1;
  m_global_history |= actual;
}

// Ensure that the addition does not exceed the limit
int
PerceptronBranchPredictor::normalized_add (int a, int b) const
{
  int tmp = a + b;
  if (tmp > m_max_weight)
    return m_max_weight;
  else if (tmp < m_min_weight)
    return m_min_weight;
  else
    return tmp;
}

// A simple hash function
inline std::vector<int>::iterator
PerceptronBranchPredictor::hashed_perceptron (IntPtr ip, IntPtr target)
{
  return (m_perceptrons.begin () + (ip % m_size))->begin ();
}
