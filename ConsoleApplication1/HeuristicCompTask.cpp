#include "stdafx.h"
#include "HeuristicCompTask.h"
#include "Node.h"
#include "SearchTreeComp.h"

int HeuristicComp::mSortOpenStackForSearchType(std::vector<DynamicSearchTreeNode*>& pSortedStack, enum SearchAlgorithm pSearchType)
{
  DynamicSearchTreeNode* lTemp;

  try
  {
    for (int i = 0; i < (signed)pSortedStack.size() - 1; i++)
    {
      for (int j = 0; j < (signed)pSortedStack.size() - 1; j++)
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

int HeuristicComp::mComputeMisplacedTilesCost(DynamicSearchTreeNode* pNode, std::vector<int>& pGoalState)
{
  int lComputedCost = 0;

  for (size_t i = 0; i< pNode->aCurrentState.size(); i++)
  {
    if(pNode->aCurrentState[i] != 0 && pNode->aCurrentState[i] != pGoalState[i]) // Misplaced Tile Cost does not include the blank space as an additional cost
    {
      lComputedCost++;
    }
  }

  return lComputedCost;
}

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
