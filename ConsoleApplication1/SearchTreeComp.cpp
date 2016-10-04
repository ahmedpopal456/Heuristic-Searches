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

//
// Creating the Dynamic Tree for search and expansion. We also generate the 
// input state, as well as the root in the constructor
//
DynamicSearchTreeComp::DynamicSearchTreeComp(enum SearchAlgorithm pSearchType, enum Heuristic pHeuristicType, std::string pOutputFileName)
{
  aInternalError = 0;
  aCurrentTreeDepth = 0;
  aIsGoalState = false;
  aRoot = nullptr;

  aOutputFilePathName = pOutputFileName; 
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

//
// General Search Algorithm. Whenever we recursively enter the while loop, the 
// node sorted at the lists' 0th index is deleted from the open stack, and is used as 
// the current node. This node is then used to generate its children, after which the 
// whole open list will be sorted again based on heuristics/search algorithm
//

int DynamicSearchTreeComp::mGeneralSearch() {  

  if (aInternalError == 0)
  {
    DynamicSearchTreeNode* aCurrentNode;

    if (aRoot == nullptr) return -1; 

    while (aOpenStack.size() > 0) 
    {
      aCurrentNode = aOpenStack.front(); 
      aOpenStack.erase(aOpenStack.begin());

      aInternalError = mAddChildrenAndSortStack(aCurrentNode);

      if (aCurrentNode->aNodeHeuristics.aDepthCost > aCurrentTreeDepth) 
      {
        aCurrentTreeDepth = aCurrentNode->aNodeHeuristics.aDepthCost;
        cout << " Tree Depth : " << aCurrentTreeDepth << "\n";
      }

      if (aIsGoalState || (aInternalError == -1))
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

//
// Function used to generate the root of the tree, before we head out into expanding the
// state space
//

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

  mHeuristicCostHandler(aRootNode);
  std::fill(aRootNode->aChildren.begin(), aRootNode->aChildren.end(), nullptr);
  aRootNode->aParentNode = nullptr;

  if (aRoot == nullptr)
  {
    aRoot = aRootNode;
		aOpenStack.push_back(aRoot);
	}
}

//
// The following function are used to create four new possible nodes for the current node we are analyzing.
// A handler lauches four different threads to execute a function that will check if moving in that
// direction is possible for the current state the puzzle is in. Depending on the results, we will 
// delete dangling children, and keep the possible ones. Their costs will then be calculated, and 
// the open list will be sorted again 
//

int DynamicSearchTreeComp::mAddChildrenAndSortStack(DynamicSearchTreeNode* pCurrentNode)
{
  int lError = 0; 

  DynamicSearchTreeNode* lFirstChild = new DynamicSearchTreeNode();
  DynamicSearchTreeNode* lSecondChild = new DynamicSearchTreeNode();
  DynamicSearchTreeNode* lThirdChild = new DynamicSearchTreeNode();
  DynamicSearchTreeNode* lFourthChild = new DynamicSearchTreeNode();

  std::thread lMoveUp   (&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lFirstChild,  'u');
  std::thread lMoveDown (&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lSecondChild, 'd');
  std::thread lMoveRight(&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lThirdChild,  'r');
  std::thread lMoveLeft (&DynamicSearchTreeComp::mAttemptMoveThreadHandler, this, pCurrentNode, lFourthChild, 'l');

  lMoveUp.join(); lMoveDown.join(); lMoveRight.join(); lMoveLeft.join(); 

  lError = mConfirmMoves(pCurrentNode, lFirstChild, lSecondChild, lThirdChild, lFourthChild);
  lError = aHeuristicComp->mSortOpenListForSearchType(aOpenStack,aSearchType); // look at this later
  
  return lError; 
}

void DynamicSearchTreeComp::mAttemptMoveThreadHandler(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildNode, char pMove)
{
  auto lAttempt = std::bind(&DynamicSearchTreeComp::mAttemptToMove, this, pCurrentNode, pChildNode, pMove);
  async(std::launch::async, lAttempt);
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
        cout << "\n" << " Goal State was Found! Check Output File!!" << "\n";
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[0], aOutputFilePathName);
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
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[1], aOutputFilePathName);
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
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[2], aOutputFilePathName);
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
        aIOProcessor->mOutputFileGenerator(pCurrentNode->aChildren[3], aOutputFilePathName);
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

  int lIndex, lTemp;
  bool lIsBacktracking = false;

  for (int i = 0; i<(signed)pCurrentNode->aCurrentState.size(); i++)
  {
    if (pCurrentNode->aCurrentState[i] == 0)
    {
      lIndex = i;
    }
    pChildNode->aCurrentState.push_back(pCurrentNode->aCurrentState[i]);
  }
  pChildNode->aNodeHeuristics.aIndex = lIndex;

  if (pCurrentNode->aNodeHeuristics.aDepthCost > 2)  // To assure that no backtracking is done
  {
    if (lIndex == pCurrentNode->aParentNode->aNodeHeuristics.aIndex)
    {
      lIsBacktracking = true;
    }
  }


  switch (pMove)
  {

  case 'r':
    if ((lIndex % 3 != 2) && (!lIsBacktracking))
    {
      lTemp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex + 1];
      pChildNode->aCurrentState[lIndex + 1] = lTemp;
      pChildNode->aIsNodeValid = true;
    }
    break;

  case 'u':

    if ((lIndex > 2) && (!lIsBacktracking))
    {
      lTemp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex - 3];
      pChildNode->aCurrentState[lIndex - 3] = lTemp;
      pChildNode->aIsNodeValid = true;
    }
    break;

  case 'd':
    if ((lIndex < 6) && (!lIsBacktracking)) {
      lTemp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex + 3];
      pChildNode->aCurrentState[lIndex + 3] = lTemp;
      pChildNode->aIsNodeValid = true;
    }
    break;

  case 'l':
    if ((lIndex % 3 != 0) && (!lIsBacktracking)) {
      lTemp = pChildNode->aCurrentState[lIndex];
      pChildNode->aCurrentState[lIndex] = pChildNode->aCurrentState[lIndex - 1];
      pChildNode->aCurrentState[lIndex - 1] = lTemp;
      pChildNode->aIsNodeValid = true;
    }
    break;
  }
    pChildNode->aNodeHeuristics.aDepthCost = pCurrentNode->aNodeHeuristics.aDepthCost + 1;
    std::fill(pChildNode->aChildren.begin(), pChildNode->aChildren.end(), nullptr);
    pChildNode->aParentNode = pCurrentNode;

    aInternalError = mHeuristicCostHandler(pChildNode);
}
//
//

//
// This function handles everything that pertains to heuristic calculations, depending on the
// search type and the heuristic at use
//
int DynamicSearchTreeComp::mHeuristicCostHandler(DynamicSearchTreeNode* pCurrentNode)
{

  if (aHeuristicType == Heuristic::MisplacedTiles)
  {
    if (aSearchType == SearchAlgorithm::AStarSearch)
      pCurrentNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeMisplacedTilesCost(pCurrentNode, aGoalState) + pCurrentNode->aNodeHeuristics.aDepthCost; // Cost: g(n) + h(n), where h(n) is the misplaced tiles algorithm
    else if (aSearchType == SearchAlgorithm::BestFirstSearch)
      pCurrentNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeMisplacedTilesCost(pCurrentNode, aGoalState);// Cost:  h(n), where h(n) is the misplaced tiles algorithm
  }

  else if (aHeuristicType == Heuristic::ManhattanDistance)
  {
    if (aSearchType == SearchAlgorithm::AStarSearch)
      pCurrentNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeManhattanDistanceCost(pCurrentNode, aGoalState) + pCurrentNode->aNodeHeuristics.aDepthCost; // "" ...
    else if (aSearchType == SearchAlgorithm::BestFirstSearch)
      pCurrentNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeManhattanDistanceCost(pCurrentNode, aGoalState);
  }

  else if (aHeuristicType == Heuristic::SumOfPermutationInv)
  {
    if (aSearchType == SearchAlgorithm::AStarSearch)
      pCurrentNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeSumOfPermutationInversionCost(pCurrentNode, aGoalState) + pCurrentNode->aNodeHeuristics.aDepthCost; // "" ...
    else if (aSearchType == SearchAlgorithm::BestFirstSearch)
      pCurrentNode->aNodeHeuristics.aCost = aHeuristicComp->mComputeSumOfPermutationInversionCost(pCurrentNode, aGoalState);
  }

  else if (aHeuristicType == Heuristic::MinMisplacedManhattan)
  {
    if (aSearchType == SearchAlgorithm::AStarSearch)
    {
      bool lMinIsManhattan = (aHeuristicComp->mComputeManhattanDistanceCost(pCurrentNode, aGoalState) < aHeuristicComp->mComputeMisplacedTilesCost(pCurrentNode, aGoalState) + pCurrentNode->aNodeHeuristics.aDepthCost);
      pCurrentNode->aNodeHeuristics.aCost = (lMinIsManhattan ? aHeuristicComp->mComputeManhattanDistanceCost(pCurrentNode, aGoalState) : aHeuristicComp->mComputeMisplacedTilesCost(pCurrentNode, aGoalState)) + pCurrentNode->aNodeHeuristics.aDepthCost;
    }
    else if (aSearchType == SearchAlgorithm::BestFirstSearch)
    {
      bool lMinIsManhattan = (aHeuristicComp->mComputeManhattanDistanceCost(pCurrentNode, aGoalState) < aHeuristicComp->mComputeMisplacedTilesCost(pCurrentNode, aGoalState));
      pCurrentNode->aNodeHeuristics.aCost = (lMinIsManhattan ? aHeuristicComp->mComputeManhattanDistanceCost(pCurrentNode, aGoalState) : aHeuristicComp->mComputeMisplacedTilesCost(pCurrentNode, aGoalState));
    }
  }

  else if ((aSearchType == SearchAlgorithm::BreadthFirstSearch || aSearchType == SearchAlgorithm::DepthFirstSearch) && aHeuristicType == Heuristic::Null)
  {
    pCurrentNode->aNodeHeuristics.aCost = pCurrentNode->aNodeHeuristics.aDepthCost;  // Cost: g(n) only (smallest distance == breadth first search)
  }
  else
  {
    return -1;
  }

  return 0;
}

int main() 
{
	clock_t start, finish, total;
	start = clock();

  DynamicSearchTreeComp lZeroSearch(SearchAlgorithm::AStarSearch, Heuristic::SumOfPermutationInv, "astar_permutation_inv.txt");
  DynamicSearchTreeComp lFirstSearch(SearchAlgorithm::BestFirstSearch, Heuristic::SumOfPermutationInv, "bestfirst_permutation_inv.txt");

  start = clock();
  cout << " Dynamic AStar Search with Sum of Permutation Heuristic is Starting ..." << "\n\n";
  lZeroSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Dynamic BestFirst Search with Sum of Permutation Heuristic is Starting ..." << "\n\n";
  lFirstSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";
  // We create searches for all possibilities 

  /*
  DynamicSearchTreeComp lZeroSearch(SearchAlgorithm::AStarSearch, Heuristic::MinMisplacedManhattan, "astar_min.txt");
  DynamicSearchTreeComp lFirstSearch(SearchAlgorithm::AStarSearch, Heuristic::MisplacedTiles, "astar_misplaced.txt");
  DynamicSearchTreeComp lSecondSearch(SearchAlgorithm::AStarSearch, Heuristic::ManhattanDistance, "astar_manhattan");

  DynamicSearchTreeComp lThirdSearch(SearchAlgorithm::BestFirstSearch, Heuristic::MinMisplacedManhattan, "bestfirst_min.txt");
  DynamicSearchTreeComp lFourthSearch(SearchAlgorithm::BestFirstSearch, Heuristic::MisplacedTiles, "bestfirst_misplaced.txt");
  DynamicSearchTreeComp lFifthSearch(SearchAlgorithm::BestFirstSearch, Heuristic::ManhattanDistance, "bestfirst_manhattan");

  DynamicSearchTreeComp lSixthSearch(SearchAlgorithm::BreadthFirstSearch, Heuristic::Null, "bfs.txt");
  DynamicSearchTreeComp lSeventhSearch(SearchAlgorithm::DepthFirstSearch, Heuristic::Null, "dfs.txt");

  start = clock();
  cout << " Dynamic AStar Search with Min Heuristic is Starting ..." << "\n\n";
  lZeroSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Dynamic AStar Search with MisplacedTiles Heuristic is Starting ..." << "\n\n";
  lFirstSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Dynamic AStar Search with Manhattan Dis. Heuristic is Starting ..." << "\n\n";
  lSecondSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Best First Search with Min Heuristic is Starting ..." << "\n\n";
  lThirdSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Best First Search with Manhattan Dis. Heuristic is Starting ..." << "\n\n";
  lFifthSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Best First Search with MisplacedTiles Heuristic is Starting ..." << "\n\n";
  lFourthSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Dynamic BFS is Starting ..." << "\n\n";
  lSixthSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";

  start = clock();
  cout << " Dynamic DFS is Starting ..." << "\n\n";
  lSeventhSearch.mGeneralSearch();
  finish = clock();
  cout << "Time: " << (finish - start) << " msecs" << "\n\n";
  */


}