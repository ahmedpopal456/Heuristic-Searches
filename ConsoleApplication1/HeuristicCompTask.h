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

  //
  // All Five Implemented Heuristics 
  //
  int mComputeMisplacedTilesCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);                                          //   ADMISSIBLE
  int mComputeManhattanDistanceCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);                                       //   ADMISSIBLE
  int mComputeMinManhattanMisplacedTiles(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);                                  //   ADMISSIBLE
  int mComputeMaxManhattanSumOfPermutationInversionCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);                   //   ADMISSIBLE
  int mComputeOverstimatedHeuristic(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState);                                       // NON-ADMISSIBLE

  //
  // Function that sorts the open stack, based on the type of search algorithm
  //
  int mSortOpenListForSearchType(std::vector<DynamicSearchTreeNode*>& pOpenStack, int pLeft, int pRight, enum SearchAlgorithm pSearchType);  

};