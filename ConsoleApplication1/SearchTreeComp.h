#pragma once

class InputOutputFileProcessor;
class DynamicSearchTreeNode;
class HeuristicComp;

//
// The Dynamic Search Tree Will Expand and get Built based on the type of search algorithm
// we are dealing with. Heuristic attributes will be given to each node, if we are dealing
// with an informed search
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
  SumOfPermutationInv   = 3,
  Null                  = 4

}; Heuristic;


class DynamicSearchTreeComp { 

  private:
  
    std::vector<int> aInput, aGoalState;             // Input and Goal States
    std::vector<DynamicSearchTreeNode*> aOpenStack;  // The Open List
  
    enum SearchAlgorithm            aSearchType;     // Search Type and Heuristic Type 
    enum Heuristic                  aHeuristicType;

    DynamicSearchTreeNode*          aRoot;
    InputOutputFileProcessor*       aIOProcessor;
    HeuristicComp*                  aHeuristicComp;
  
    int aInternalError, aCurrentTreeDepth;
    bool aIsGoalState;

    std::string aOutputFilePathName;                 // Path to save output file in 
  
  public:
  
    DynamicSearchTreeComp(enum SearchAlgorithm pSearchType, enum Heuristic pHeuristicType, std::string pOutputFileName);
    ~DynamicSearchTreeComp();
  
    int mGeneralSearch();                            

    int  mAddChildrenAndSortStack(DynamicSearchTreeNode* pCurrentNode);
    void mGenerateRoot(std::vector<int>);

    int  mHeuristicCostHandler(DynamicSearchTreeNode* pCurrentNode);

    void mAttemptMoveThreadHandler(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildrenNodes, char pMove);
    void mAttemptToMove(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pChildNode, char pMove);
    int  mConfirmMoves(DynamicSearchTreeNode* pCurrentNode, DynamicSearchTreeNode* pFirstChild, DynamicSearchTreeNode* pSecondChild, DynamicSearchTreeNode* pThirdChild, DynamicSearchTreeNode* pFourthChild);

};