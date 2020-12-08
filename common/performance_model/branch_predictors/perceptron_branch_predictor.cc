#include "simulator.h"
#include "perceptron_branch_predictor.h"
#include <iterator>
#include <numeric>

Perceptron::Perceptron (UInt32 size)
: m_weights (size, 0)
{}

PerceptronBranchPredictor::PerceptronBranchPredictor (String name, core_id_t core_id, UInt32 history,
                                                      UInt32 size, UInt32 bits, UInt32 theta)
: BranchPredictor (name, core_id), m_history (size), m_size (size), m_bits (bits), m_theta (theta),
  m_perceptrons (size, Perceptron (history + 1)), m_global_history (history, -1)
{
}

PerceptronBranchPredictor::~PerceptronBranchPredictor ()
{
}

bool
PerceptronBranchPredictor::predict (IntPtr ip, IntPtr target)
{
  UInt32 index = ip % m_size;
  Perceptron &p = m_perceptrons[index];
  int weight = std::inner_product (p.m_weights.begin () + 1, p.m_weights.end(), m_global_history.begin (), p.m_weights[0]);
  m_perceptron_state.emplace_back (p, weight);
  if (m_perceptron_state.size () > 100)
    m_perceptron_state.pop_front ();

  return (weight >= 0);
}

void
PerceptronBranchPredictor::update (bool predicted, bool actual, IntPtr ip, IntPtr target)
{
  updateCounters (predicted, actual);
  Perceptron &p = m_perceptron_state.rbegin ()->first;
  int weight = m_perceptron_state.rbegin ()->second;
  if ((weight <= m_theta || !actual) && (weight >= -m_theta || actual))
    {
      p.m_weights[0] = normalized_add (p.m_weights[0], 2 * int (actual) - 1);
      auto weight_iter = p.m_weights.begin () + 1;
      auto history_iter = m_global_history.begin ();
      while (weight_iter != p.m_weights.end () && history_iter != m_global_history.end ())
        {
          *weight_iter = normalized_add (*weight_iter, *history_iter);
          ++weight_iter;
          ++history_iter;
        }
    }
  m_global_history.push_back (2 * int (actual) - 1);
  m_global_history.pop_front ();
}

int
PerceptronBranchPredictor::normalized_add (int a, int b) const
{
  int tmp = a + b;
  if (tmp > (1 << (m_bits - 1)) - 1)
    return (1 << (m_bits - 1)) - 1;
  else if (tmp < -(1 << (m_bits - 1)))
    return -(1 << (m_bits - 1));
  else
    return tmp;
}
