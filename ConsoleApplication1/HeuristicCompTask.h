#pragma once

#include <vector>

//
// Class merely acts as a clearer way to show how heuristics are separate, yet can 
// play a vital role in how search can be optimized. It is mainly used to calculate 
// heuristic costs, as well as to sort the open list based on the search type
//

class InputOutputFileProcessor;
class DynamicSearchTreeNode;

class HeuristicComp {  

private:

public:

  HeuristicComp() {};
  ~HeuristicComp() {};

  int mComputeMisplacedTilesCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);
  int mComputeManhattanDistanceCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);
  int mComputeSumOfPermutationInversionCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);

  int mSortOpenListForSearchType(std::vector<DynamicSearchTreeNode*>& pOpenStack, enum SearchAlgorithm pSearchType);

};