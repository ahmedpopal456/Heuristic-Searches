#pragma once

#include <vector>

class DynamicSearchTreeNode; 

class InputOutputFileProcessor
{
  public:

    InputOutputFileProcessor();
    ~InputOutputFileProcessor();

    int mReadAndAssessInput(std::vector<int>& pInputState, std::vector<int>& pGoalState, std::string pFilePath);
    void mOutputFileGenerator(DynamicSearchTreeNode* pNode);
};
