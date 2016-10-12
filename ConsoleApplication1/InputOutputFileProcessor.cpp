#include "stdafx.h"
#include "InputOutputFileProcessor.h"
#include "Node.h"
#include <sstream>
#include <fstream>
#include <iostream>


using namespace std;


InputOutputFileProcessor::InputOutputFileProcessor() {}
InputOutputFileProcessor::~InputOutputFileProcessor() {}

//  
// The two following functions are self-explanatory
// 

int InputOutputFileProcessor::mReadAndAssessInput(std::vector<int>& pInputState, std::vector<int>& pGoalState, std::string pFilePath)
{
  int       	lItemnum = 0;
  ifstream		lFile;
  string			lLine, lItem;

  try
  {
    lFile.open(pFilePath);
  }
  catch (...)
  {
    cout << " Could Not Open Input File" << "\n";
    return -1; 
  }


  while (getline(lFile,lLine))
  {
    istringstream linestream(lLine);

    while (getline(linestream, lItem, ' ')) 
    {
      lItemnum++;

      if (lItemnum <= 9) 
      {
        pInputState.push_back(atoi(lItem.c_str()));
      }
      else
      {
        pGoalState.push_back(atoi(lItem.c_str()));
      }
    }
  }

  lFile.close();  
  return 0;
}

void InputOutputFileProcessor::mOutputFileGenerator(DynamicSearchTreeNode* pNode, std::string pFileName)
{

  try
  {
    ofstream fout(pFileName);
    std::vector<DynamicSearchTreeNode*> lTemp;

    while (pNode != nullptr)
    {
      lTemp.push_back(pNode);
      pNode = pNode->aParentNode;
    }
    fout << "Initial State" << endl;

    for (int j = lTemp.size() - 1; j >= 0; j--)
    {
      for (int i = 0; i < (signed)lTemp[j]->aCurrentState.size(); i++)
      {
        if (lTemp[j]->aCurrentState[i] == 0)
          fout << "B ";
        else
          fout << lTemp[j]->aCurrentState[i] << " ";
        if (i % 3 == 2) fout << endl;
      }
      fout << endl;
    }
    fout << "Goal State" << endl;
    fout.close();
  }
  catch (...)
  {
    cout << "Output file could not be generated. Please check filepath name provided";
  }
}