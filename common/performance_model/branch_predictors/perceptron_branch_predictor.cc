#include "simulator.h"
#include "perceptron_branch_predictor.h"

PerceptronBranchPredictor::PerceptronBranchPredictor(String name, core_id_t core_id, UInt32 size)
   : BranchPredictor(name, core_id)
   , m_bits(size)
{
}

PerceptronBranchPredictor::~PerceptronBranchPredictor()
{
}

bool PerceptronBranchPredictor::predict(IntPtr ip, IntPtr target)
{
   UInt32 index = ip % m_bits.size();
   return m_bits[index];
}

void PerceptronBranchPredictor::update(bool predicted, bool actual, IntPtr ip, IntPtr target)
{
   updateCounters(predicted, actual);
   UInt32 index = ip % m_bits.size();
   m_bits[index] = actual;
}
