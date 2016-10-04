#pragma once

#include <vector>
#include <iostream>

//
// This class basically consists of a node, which has a state, heuristical attributes
// a valid check (to see if node is valid based on possible moves), a parent 
// and 4 possible children 
//

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