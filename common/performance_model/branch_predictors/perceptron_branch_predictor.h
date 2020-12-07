#ifndef PERCEPTRON_BRANCH_PREDICTOR_H
#define PERCEPTRON_BRANCH_PREDICTOR_H

#include "branch_predictor.h"

#include <vector>

class PerceptronBranchPredictor : public BranchPredictor
{
public:
   PerceptronBranchPredictor(String name, core_id_t core_id, UInt32 size);
   ~PerceptronBranchPredictor();

   bool predict(IntPtr ip, IntPtr target);
   void update(bool predicted, bool actual, IntPtr ip, IntPtr target);

private:
   std::vector<bool> m_bits;
};

#endif // perceptron_branch_predictor.h
