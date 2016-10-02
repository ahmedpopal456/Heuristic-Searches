#include "stdafx.h"
#include "HeuristicCompTask.h"
#include "Node.h"
#include "SearchTreeComp.h"

int HeuristicComp::mSortOpenStackForSearchType(std::vector<DynamicSearchTreeNode*>& pSortedStack)
{
  DynamicSearchTreeNode* lTemp;

  try
  {
    for (int i = 0; i < (signed)pSortedStack.size() - 1; i++)
    {
      for (int j = 0; j < (signed)pSortedStack.size() - 1; j++)
      {
        if (pSortedStack[j]->aNodeHeuristics.aCost > pSortedStack[j + 1]->aNodeHeuristics.aCost)
        {
          lTemp = pSortedStack[j];
          pSortedStack[j] = pSortedStack[j + 1];
          pSortedStack[j + 1] = lTemp;
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

  for (int i = 0; i<(signed)pNode->aCurrentState.size(); i++)
  {
    if(pNode->aCurrentState[i] != 0 && pNode->aCurrentState[i] != pGoalState[i]) // Misplaced Tile Cost does not include the blank space as an additional cost
    {
      lComputedCost++;
    }
  }

  return lComputedCost;
}