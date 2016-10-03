#pragma once

class InputOutputFileProcessor;
class DynamicSearchTreeNode;
class HeuristicComp;
//
// The Dynamic Search Tree Will Expand 
// and get Built based on the type of search algorithm 
// we are dealing with. Heuristic attributes will be given
// to each node, if we are dealing with an informed search
//

enum SearchAlgorithm
{
  AStarSearch = 0,
  BreadthFirstSearch = 1,
  DepthFirstSearch   = 2, 
  BestFirstSearch    = 3,

}; SearchAlgorithm;

enum Heuristic
{
  ManhattanDistance     = 0,
  MisplacedTiles        = 1,
  MinMisplacedManhattan = 2,
  Null                  = 3

}; Heuristic;


class DynamicSearchTreeComp { // add an object computation at the constructor

  private:
  
    std::vector<int> aInput, aGoalState;
    std::vector<DynamicSearchTreeNode*> aOpenStack;
  
    enum SearchAlgorithm            aSearchType;
    enum Heuristic                  aHeuristicType;

    DynamicSearchTreeNode*          aRoot;
    InputOutputFileProcessor*       aIOProcessor;
    HeuristicComp*                  aHeuristicComp;
  
    int aInternalError, aCurrentTreeDepth;
    bool aIsGoalState;

    std::string aOutputFilePathName; 
  
  public:
  
    DynamicSearchTreeComp(enum SearchAlgorithm pSearchType, enum Heuristic pHeuristicType, std::string pOutputFileName);
    ~DynamicSearchTreeComp();
  
    int mGenerateDynamicTreeAndSearch();
    int mInsertBranches(DynamicSearchTreeNode* pCurrentNode);
  
    bool isEmpty() const { return aRoot == NULL; }
    void mGenerateRoot(std::vector<int>);
  
    void mHeuristicCostHandler(DynamicSearchTreeNode* pCurrentNode);
    void mAttemptMoveThreadHandler(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildrenNodes, char pMove);
    void mAttemptToMove(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildNode, char pMove);
    int  mConfirmMoves(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pFirstChild, DynamicSearchTreeNode* pSecondChild, DynamicSearchTreeNode* pThirdChild, DynamicSearchTreeNode* pFourthChild);

};