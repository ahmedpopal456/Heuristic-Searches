#pragma once

#include <vector>

class InputOutputFileProcessor;
class DynamicSearchTreeNode;

class HeuristicComp {  

private:

public:

  HeuristicComp() {};
  ~HeuristicComp() {};


  int mComputeMisplacedTilesCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);
  int mSortOpenStackForSearchType(std::vector<DynamicSearchTreeNode*>& pOpenStack);
};