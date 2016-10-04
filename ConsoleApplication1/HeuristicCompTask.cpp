#include "stdafx.h"
#include "HeuristicCompTask.h"
#include "Node.h"
#include "SearchTreeComp.h"

//
// This algorithm sorts the open list which consists of nodes that each have a cost associated to them 
// and that have previously not been visited. These nodes are sorted depending on the search algorithms
// and heuristics used... For the AStar and BestFirst searches, we look at sorting the stack in such a way
// that the node with the smallest cost comes first... for the BFS and DFS, we sort the node based on the 
// distance of each node from the root (BFS - smallest distance, DFS - greatest distance)
//

int HeuristicComp::mSortOpenListForSearchType(std::vector<DynamicSearchTreeNode*>& pSortedStack, enum SearchAlgorithm pSearchType)
{
  DynamicSearchTreeNode* lTemp;

  try
  {
    for (size_t i = 0; i < pSortedStack.size() - 1; i++)
    {
      for (size_t j = 0; j < pSortedStack.size() - 1; j++)
      {
        if (pSearchType == SearchAlgorithm::AStarSearch || pSearchType == SearchAlgorithm::BreadthFirstSearch || pSearchType == SearchAlgorithm::BestFirstSearch)
        {
          if (pSortedStack[j]->aNodeHeuristics.aCost > pSortedStack[j + 1]->aNodeHeuristics.aCost)
          {
            lTemp = pSortedStack[j];
            pSortedStack[j] = pSortedStack[j + 1];
            pSortedStack[j + 1] = lTemp;
          }
        }
        else if(pSearchType == SearchAlgorithm::DepthFirstSearch)
        {
          if (pSortedStack[j]->aNodeHeuristics.aCost < pSortedStack[j + 1]->aNodeHeuristics.aCost)
          {
            lTemp = pSortedStack[j];
            pSortedStack[j] = pSortedStack[j + 1];
            pSortedStack[j + 1] = lTemp;
          }
        }
      }
    }
    return 0; 
  }
  catch (...)
  {
    std::cout << "Error occured when trying to sort the Stack";
    return -1; 
  }
}

// Algorithm for Misplaced Tiles; Checks how many tiles from the current node are different
// to the goal state (not including the blank space)

int HeuristicComp::mComputeMisplacedTilesCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedCost = 0;

  for (size_t i = 0; i< pNode->aCurrentState.size(); i++)
  {
    if(pNode->aCurrentState[i] != 0 && pNode->aCurrentState[i] != pGoalState[i])
    {
      lComputedCost++;
    }
  }

  return lComputedCost;
}

// Algorithm computes Manhattan distance using the following formula: 
// Sum (from n = 0 to 8) = (|xi(s) - xi|) + (|yi(s) - yi|)

int HeuristicComp::mComputeManhattanDistanceCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedCost = 0;

  for (size_t i = 0; i < pNode->aCurrentState.size(); i++)
  {
    int lCurrentIndex = i, lFinalIndex = 0;

    int lTemp = pNode->aCurrentState.at(i);

    if (lTemp != 0)
    {
      for (size_t j = 0; j < pGoalState.size(); j++)
      {
        if (pGoalState.at(j) == lTemp)
        {
          lFinalIndex = j;
        }
      }

      div_t lCurrentTile, lFinalTile;
      lCurrentTile = div(lCurrentIndex, 3);
      lFinalTile = div(lFinalIndex, 3);

      lComputedCost += ((std::abs(lCurrentTile.quot - lFinalTile.quot)) + (std::abs(lCurrentTile.rem - lFinalTile.rem)));
    }
  }

  return lComputedCost;
}


// This Heuristic calculates the Sum of Permutation Inversions for each numbered tile, by counting
// how many tiles on its right should be on its left in the goal state.  

int HeuristicComp::mComputeSumOfPermutationInversionCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedCost = 0;

  for (size_t i = 0; i < pNode->aCurrentState.size(); i++)
  {
    int lCurrentIndex = i, lFinalIndex = 0;
    int lTemp = pNode->aCurrentState.at(i);
    
    //
    // Find the Indexes for same time in both states
    //
    if (lTemp != 0)
    {
      for (size_t j = 0; j < pGoalState.size(); j++)
      {
        if (pGoalState.at(j) == lTemp)
        {
          lFinalIndex = j;
        }
      }
    }
    else
      continue; 

    //
    // Check if there is something at the right side of the current state 
    // index, or at the left of the goal state index/ If so, then we see how many tiles
    //  on the right side of the current index, should actually be on its left
    // 
    
    if (lFinalIndex != 0 && lCurrentIndex != (static_cast<int>(pNode->aCurrentState.size() - 1)))
    {
      std::vector<int>   lLeftOfTileInGoalState(pGoalState.begin(), pGoalState.begin() + lFinalIndex);
      std::vector<int>   lRightOfTileInCurrentState(pNode->aCurrentState.begin() + lCurrentIndex +1 , pNode->aCurrentState.end());

      for (size_t l = 0; l < lRightOfTileInCurrentState.size(); l++)
      {
        for (size_t k = 0; k < lLeftOfTileInGoalState.size(); k++)
        {
          if(lLeftOfTileInGoalState[k] == lRightOfTileInCurrentState[l])
            lComputedCost += 1; 
        }
      }
    }
    else
      continue; 
  }
   
  return lComputedCost;
}