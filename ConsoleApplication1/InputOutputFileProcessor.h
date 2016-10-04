#pragma once

#include <vector>

//
// Regular IO Processing class. Used to process input file, as well as to generate the output file
// for all states
//
class DynamicSearchTreeNode; 

class InputOutputFileProcessor
{
  public:

    InputOutputFileProcessor();
    ~InputOutputFileProcessor();

    int mReadAndAssessInput(std::vector<int>& pInputState, std::vector<int>& pGoalState, std::string pFilePath);
    void mOutputFileGenerator(DynamicSearchTreeNode* pNode, std::string pFileName);
};
