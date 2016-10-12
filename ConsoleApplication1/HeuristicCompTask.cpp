#include "stdafx.h"
#include "HeuristicCompTask.h"
#include "Node.h"
#include "SearchTreeComp.h"

//
// This function uses the quick-sort algorithm to sort the list which consists of nodes that each have a cost associated to them 
// and that have previously not been visited. These nodes are sorted depending on the search algorithms
// and heuristics used... For the AStar and BestFirst searches, we look at sorting the stack in such a way
// that the node with the smallest cost comes first... for the BFS and DFS, we sort the node based on the 
// distance of each node from the root (BFS - smallest distance, DFS - greatest distance)
//
int HeuristicComp::mSortOpenListForSearchType(std::vector<DynamicSearchTreeNode*>& pSortedStack, int pLeft, int pRight, enum SearchAlgorithm pSearchType)
{
  DynamicSearchTreeNode* lTemp;

  try
  {
    int lLeft = pLeft, 
        lRight = pRight;

    int lPivot = pSortedStack.at((lLeft + lRight) / 2)->aNodeHeuristics.aCost;

    //
    // Partitioning Algorithm
    //

    while (lLeft <= lRight)
    {
      if (pSearchType == SearchAlgorithm::AStarSearch || pSearchType == SearchAlgorithm::BestFirstSearch || pSearchType == SearchAlgorithm::BreadthFirstSearch)
      {
        while (pSortedStack.at(lLeft)->aNodeHeuristics.aCost < lPivot)
        {
          lLeft++;
        }

        while (pSortedStack.at(lRight)->aNodeHeuristics.aCost > lPivot)
        {
          lRight--;
        }
      }
      else if (pSearchType == SearchAlgorithm::DepthFirstSearch)
      {
        while (pSortedStack.at(lLeft)->aNodeHeuristics.aCost > lPivot)
        {
          lLeft++;
        }

        while (pSortedStack.at(lRight)->aNodeHeuristics.aCost < lPivot)
        {
          lRight--;
        }
      }

      if (lLeft <= lRight) 
      {
        lTemp = pSortedStack.at(lLeft);
        pSortedStack.at(lLeft) = pSortedStack.at(lRight);
        pSortedStack.at(lRight) = lTemp;
        lLeft++;
        lRight--;
      }
    };
    //
    // Recursively Sort the List
    //
    if (pLeft < lRight)
      mSortOpenListForSearchType(pSortedStack, pLeft, lRight, pSearchType);

    if (pRight < lLeft)
      mSortOpenListForSearchType(pSortedStack, lLeft, pRight, pSearchType);

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

// Algorithm computes both Manhattan Distance and Misplaced Tiles cost and returns
// the one with the smallest cost, as per the guidelines
int HeuristicComp::mComputeMinManhattanMisplacedTiles(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedManhattanCost = 0, lComputedMisplacedTilesCost = 0;

  lComputedManhattanCost = mComputeManhattanDistanceCost(pNode, pGoalState);
  lComputedMisplacedTilesCost = mComputeMisplacedTilesCost(pNode, pGoalState);

  if (lComputedManhattanCost < lComputedMisplacedTilesCost)
    return lComputedManhattanCost;
  else
    return lComputedMisplacedTilesCost;
}

// This Heuristic calculates the Sum oF Permutation Inversion algorithm
// for each numbered tile, by counting how many tiles on its right should be on its left 
// as well as the Manhattan Distance. It returns the largest cost between both

int HeuristicComp::mComputeMaxManhattanSumOfPermutationInversionCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedSumOfPerCost = 0, lComputedManhattanDrive = 0;

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
    // on the right side of the current index, should actually be on its left
    // 
    
    if (lFinalIndex != 0 && lCurrentIndex != (static_cast<int>(pNode->aCurrentState.size() - 1)))
    {
      std::vector<int>   lLeftOfTileInGoalState(pGoalState.begin(), pGoalState.begin() + lFinalIndex);
      std::vector<int>   lRightOfTileInCurrentState(pNode->aCurrentState.begin() + lCurrentIndex + 1, pNode->aCurrentState.end());

      for (size_t l = 0; l < lRightOfTileInCurrentState.size(); l++)
      {
        for (size_t k = 0; k < lLeftOfTileInGoalState.size(); k++)
        {
          if (lLeftOfTileInGoalState[k] == lRightOfTileInCurrentState[l])
            lComputedSumOfPerCost += 1;
        }
      }
    }
    else
      continue; 
  }
   
  lComputedManhattanDrive = mComputeManhattanDistanceCost(pNode, pGoalState);

  if (lComputedSumOfPerCost > lComputedManhattanDrive)
    return lComputedSumOfPerCost;
  else
    return lComputedManhattanDrive;
}

//
// This heuristic is a NON-ADMISSIBLE one and is an overestimation of the manhattan distance where
// cost = alpha * h(n), where h(n) is the max of the manhattan distance/ sum of permutation inversions
// algorithm and alpha is a very large number ,ideally infinity, but in this case 5000
// 

int HeuristicComp::mComputeOverstimatedHeuristic(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedCost = 5000 * (mComputeMaxManhattanSumOfPermutationInversionCost(pNode, pGoalState));
  return  lComputedCost;
}