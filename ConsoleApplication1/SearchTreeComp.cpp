#include "stdafx.h"
#include <iostream>
#include <sstream>
#include <fstream>
#include <vector>
#include <string>
#include <math.h>
#include <algorithm>
#include <time.h>
#include <atomic>
#include <future>
#include "SearchTreeComp.h"
#include "Node.h"
#include "InputOutputFileProcessor.h"
#include "HeuristicCompTask.h"

using namespace std;

DynamicSearchTreeComp::DynamicSearchTreeComp(enum SearchAlgorithm pSearchType, enum Heuristic pHeuristicType)
{
  aInternalError = 0;
  aCurrentTreeDepth = 0;
  aIsGoalState = false;
  aRoot = nullptr;

  aSearchType = pSearchType;
  aHeuristicType = pHeuristicType;

  aIOProcessor = new InputOutputFileProcessor();
  aHeuristicComp = new HeuristicComp(); 

  aInternalError = aIOProcessor->mReadAndAssessInput(aInput, aGoalState, "in.txt");
  mGenerateRoot(aInput);
}

DynamicSearchTreeComp::~DynamicSearchTreeComp()
{
  delete aIOProcessor; 
  delete aHeuristicComp;
  delete aRoot; 
}

int DynamicSearchTreeComp::mGenerateDynamicTreeAndSearch() {

  if (aInternalError == 0)
  {
    DynamicSearchTreeNode* aCurrentNode;

    cout << " Dynamic Search and Tree Generation is Starting ..." << "\n\n";

    while (aOpenStack.size() > 0) 
    {
      aCurrentNode = aOpenStack.front(); aOpenStack.erase(aOpenStack.begin());
      aInternalError = mInsertBranches(aCurrentNode);

      if (aCurrentNode->aNodeHeuristics.aDepthCost > aCurrentTreeDepth) 
      {
        aCurrentTreeDepth = aCurrentNode->aNodeHeuristics.aDepthCost;
        cout << " Tree Depth : " << aCurrentTreeDepth << "\n";
      }

      if (aIsGoalState || aInternalError)
        break;

    }
    return 0; 
  }
  else
  {
    cout << "Procedure Aborted.";
    return -1; 
  }
}

void DynamicSearchTreeComp::mGenerateRoot(vector<int> state) {

  DynamicSearchTreeNode* aRootNode = new DynamicSearchTreeNode;
 
  aRootNode->aNodeHeuristics.aDepthCost = 1;
  for (int i = 0; i < (signed)state.size(); i++)
  {
    aRootNode->aCurrentState.push_back(state[i]);

    if (state[i] == 0)
    {
      aRootNode->aNodeHeuristics.aIndex = i;
    }
  }

  if (aSearchType == SearchAlgorithm::AStarSearch && aHeuristicType == Heuristic::MisplacedTiles)
  {
    aRootNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeMisplacedTilesCost(aRootNode, aGoalState) + aRootNode->aNodeHeuristics.aDepthCost;
  }
  else if (aSearchType == SearchAlgorithm::AStarSearch && aHeuristicType == Heuristic::ManhattanDistance)
  {
    aRootNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeManhattanDistanceCost(aRootNode, aGoalState) + aRootNode->aNodeHeuristics.aDepthCost;
  }
  else if (aSearchType == SearchAlgorithm::BreadthFirstSearch || aSearchType == SearchAlgorithm::DepthFirstSearch)
  {
    aRootNode->aNodeHeuristics.aCost = aRootNode->aNodeHeuristics.aDepthCost;
  }

  std::fill(aRootNode->aChildren.begin(), aRootNode->aChildren.end(), nullptr);
  aRootNode->aParentNode = nullptr;

	if (isEmpty()) 
  {
    aRoot = aRootNode;
		aOpenStack.push_back(aRoot);
	}
}

int DynamicSearchTreeComp::mInsertBranches(DynamicSearchTreeNode* pCurrentNode)
{
  int lError = 0; 

  DynamicSearchTreeNode* lFirstChild = new DynamicSearchTreeNode();
  DynamicSearchTreeNode* lSecondChild = new DynamicSearchTreeNode();
  DynamicSearchTreeNode* lThirdChild = new DynamicSearchTreeNode();
  DynamicSearchTreeNode* lFourthChild = new DynamicSearchTreeNode();

  std::thread lMoveUp(&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lFirstChild, 'u');
  std::thread lMoveDown(&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lSecondChild, 'd');
  std::thread lMoveRight(&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lThirdChild, 'r');
  std::thread lMoveLeft(&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lFourthChild, 'l');

  lMoveUp.join(); lMoveDown.join(); lMoveRight.join(); lMoveLeft.join(); 

  lError = mConfirmMoves(pCurrentNode, lFirstChild, lSecondChild, lThirdChild, lFourthChild);
  lError = aHeuristicComp->mSortOpenStackForSearchType(aOpenStack,aSearchType); // look at this later
  
  return lError; 
}

void DynamicSearchTreeComp::mAttemptMoveThreadHandler(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildNode, char pMove)
{
  auto lUpAttempt = std::bind(&DynamicSearchTreeComp::mAttemptToMove, this, pCurrentNode, pChildNode, pMove);
  async(std::launch::async, lUpAttempt);
}


int DynamicSearchTreeComp::mConfirmMoves(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pFirstChild,
                                         DynamicSearchTreeNode* pSecondChild, DynamicSearchTreeNode* pThirdChild, 
                                         DynamicSearchTreeNode* pFourthChild)
{
  int lError = 0; 

  bool lFirstIsPossible, lSecondIsPossible, lThirdIsPossible, lFourthIsPossible = false; 
  bool aIsGoalStateFound = false;

  lFirstIsPossible = (pCurrentNode->aChildren[0] == nullptr) && (pFirstChild->aIsNodeValid);
  lSecondIsPossible = (pCurrentNode->aChildren[1] == nullptr) && (pSecondChild->aIsNodeValid);
  lThirdIsPossible = (pCurrentNode->aChildren[2] == nullptr) && (pThirdChild->aIsNodeValid);
  lFourthIsPossible = (pCurrentNode->aChildren[3] == nullptr) && (pFourthChild->aIsNodeValid);

  try
  {
    if (lFirstIsPossible && !aIsGoalStateFound)
    {
      pCurrentNode->aChildren[0] = pFirstChild;  aOpenStack.push_back(pCurrentNode->aChildren[0]);
      aIsGoalState = true;

      for (int i = 0; i < (signed)aGoalState.size(); i++)
      {
        if (pCurrentNode->aChildren[0] != nullptr)
        {
          if (pCurrentNode->aChildren[0]->aCurrentState[i] != aGoalState[i])
            aIsGoalState = false; 
        }
      }
      if (aIsGoalState)
      {
        cout << "\n" << " Goal State was Found! Check Output File!!";
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[0]);
        aIsGoalStateFound = true;
      }
    }

    if (lSecondIsPossible && !aIsGoalStateFound)
    {
      pCurrentNode->aChildren[1] = pSecondChild; aOpenStack.push_back(pCurrentNode->aChildren[1]);
      aIsGoalState = true;

      for (int i = 0; i < (signed)aGoalState.size(); i++)
      {
        if (pCurrentNode->aChildren[1] != nullptr)
        {
          if (pCurrentNode->aChildren[1]->aCurrentState[i] != aGoalState[i])
            aIsGoalState = false;
        }
      }
      if (aIsGoalState)
      {
        cout << "\n" << " Goal State was Found! Check Output File!!";
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[1]);
        aIsGoalStateFound = true;
      }
    }

    if (lThirdIsPossible && !aIsGoalStateFound)
    {
      pCurrentNode->aChildren[2] = pThirdChild;  aOpenStack.push_back(pCurrentNode->aChildren[2]);
      aIsGoalState = true;

      for (int i = 0; i < (signed)aGoalState.size(); i++)
      {
        if (pCurrentNode->aChildren[2] != nullptr)
        {
          if (pCurrentNode->aChildren[2]->aCurrentState[i] != aGoalState[i])
            aIsGoalState = false;
        }
      }
      if (aIsGoalState)
      {
        cout << "\n" << " Goal State was Found! Check Output File!!";
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[2]);
        aIsGoalStateFound = true;
      }
    }

    if (lFourthIsPossible && !aIsGoalStateFound)
    {
      pCurrentNode->aChildren[3] = pFourthChild; aOpenStack.push_back(pCurrentNode->aChildren[3]);
      aIsGoalState = true;

      for (int i = 0; i < (signed)aGoalState.size(); i++)
      {
        if (pCurrentNode->aChildren[3] != nullptr)
        {
          if (pCurrentNode->aChildren[3]->aCurrentState[i] != aGoalState[i])
            aIsGoalState = false;
        }
      }
      if (aIsGoalState)
      {
        cout << "\n" << " Goal State was Found! Check Output File!!";
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[3]);
        aIsGoalStateFound = true;
      }
    }

    if (!lFirstIsPossible)  delete pFirstChild;
    if (!lSecondIsPossible) delete pSecondChild;
    if (!lThirdIsPossible)  delete pThirdChild;
    if (!lFourthIsPossible) delete pFourthChild;

    return 0; 
  }

  catch (...)
  {
    cout << " Procedure failed while trying to confirm all possible moves.";
    return -1; 
  }
}

void DynamicSearchTreeComp::mAttemptToMove(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildNode, char pMove) { // make all moves at once on threads

  int lIndex, temp;
  bool badMove = false;

  for (int i = 0; i<(signed)pCurrentNode->aCurrentState.size(); i++)
  {
    if (pCurrentNode->aCurrentState[i] == 0)
    {
      lIndex = i;
    }
    pChildNode->aCurrentState.push_back(pCurrentNode->aCurrentState[i]);
  }
  pChildNode->aNodeHeuristics.aIndex = lIndex;

  if (pCurrentNode->aNodeHeuristics.aDepthCost > 2)
  {
    if (lIndex == pCurrentNode->aParentNode->aNodeHeuristics.aIndex)
    {
      badMove = true;
    }
  }


  switch (pMove)
  {
  case 'u':

    if ((lIndex > 2) && (!badMove))
    {
      temp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex - 3];
      pChildNode->aCurrentState[lIndex - 3] = temp;
      pChildNode->aIsNodeValid = true;
    }
    break;

  case 'r':
    if ((lIndex % 3 != 2) && (!badMove))
    {
      temp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex + 1];
      pChildNode->aCurrentState[lIndex + 1] = temp;
      pChildNode->aIsNodeValid = true;
    }
    break;

  case 'd':
    if ((lIndex < 6) && (!badMove)) {
      temp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex + 3];
      pChildNode->aCurrentState[lIndex + 3] = temp;
      pChildNode->aIsNodeValid = true;
    }
    break;

  case 'l':
    if ((lIndex % 3 != 0) && (!badMove)) {
      temp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex - 1];
      pChildNode->aCurrentState[lIndex - 1] = temp;
      pChildNode->aIsNodeValid = true;
    }
    break;
  }
  pChildNode->aNodeHeuristics.aDepthCost = pCurrentNode->aNodeHeuristics.aDepthCost + 1;


    if (aSearchType == SearchAlgorithm::AStarSearch && aHeuristicType == Heuristic::MisplacedTiles)
    {
      pChildNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeMisplacedTilesCost(pChildNode, aGoalState) + pChildNode->aNodeHeuristics.aDepthCost; // Cost: g(n) + h(n), where h(n) is the misplaced tiles algorithm
    }
    else if (aSearchType == SearchAlgorithm::AStarSearch && aHeuristicType == Heuristic::ManhattanDistance)
    {
      pChildNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeManhattanDistanceCost(pChildNode, aGoalState) + pChildNode->aNodeHeuristics.aDepthCost;
    }
    else if (aSearchType == SearchAlgorithm::BreadthFirstSearch || aSearchType == SearchAlgorithm::DepthFirstSearch)
    {
      pChildNode->aNodeHeuristics.aCost = pChildNode->aNodeHeuristics.aDepthCost;  // Cost: g(n) only (smallest distance == breadth first search)
    }


    std::fill(pChildNode->aChildren.begin(), pChildNode->aChildren.end(), nullptr);
    pChildNode->aParentNode = pCurrentNode;
}


int main() 
{
	clock_t start, finish, total;
	start = clock();

  DynamicSearchTreeComp lFirstSearch(SearchAlgorithm::AStarSearch, Heuristic::MisplacedTiles);
  DynamicSearchTreeComp lSecondSearch(SearchAlgorithm::AStarSearch, Heuristic::ManhattanDistance);
  DynamicSearchTreeComp lThirdSearch(SearchAlgorithm::BreadthFirstSearch, Heuristic::Null);
  DynamicSearchTreeComp lFourthSearch(SearchAlgorithm::DepthFirstSearch, Heuristic::Null);

  start = clock();
  std::chrono::milliseconds timespan(1000);

  lFirstSearch.mGenerateDynamicTreeAndSearch();
  std::this_thread::sleep_for(timespan);

  lSecondSearch.mGenerateDynamicTreeAndSearch();
  std::this_thread::sleep_for(timespan);

  lThirdSearch.mGenerateDynamicTreeAndSearch();
  std::this_thread::sleep_for(timespan);

  lFourthSearch.mGenerateDynamicTreeAndSearch();
  std::this_thread::sleep_for(timespan);


	finish = clock();
	total = finish - start;
	cout << "time: " << total << " msecs" << endl;
}