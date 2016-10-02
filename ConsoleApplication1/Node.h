#pragma once

#include <vector>
#include <iostream>

class DynamicSearchTreeNode
{

public: 

  struct HeuristicAttributes
  {
    int aCost; 
    int aIndex;
    int aDepthCost;
  };

   HeuristicAttributes aNodeHeuristics;
   DynamicSearchTreeNode* aParentNode;

   std::vector <DynamicSearchTreeNode*> aChildren; 
   std::vector<int> aCurrentState;
   
   bool aIsNodeValid;
  
   DynamicSearchTreeNode()
   {
     //
     // Any Node can have a maximum of 4 children
     //

     aChildren.resize(4);  
     aIsNodeValid = false; 
   }
};