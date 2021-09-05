/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/

#pragma once

#include <vector>
#include <queue>

class Grid
{
public:
  class Node
  {
  public:
    Node(int xLength, int zLength, int index);
    ~Node() {}
    int GetParent() const;
    void SetParent(int parent);
    bool IsWall() const;
    void SetWall(bool wall);
    bool IsInQueue() const;
    void Enqueue(bool inqueue);
    int GetHeuristic() const;
    void SetHeuristic(int goal_x, int goal_y);
    bool IsPath() const;
    void SetPath(bool path);
    int GetDistance() const;
    void SetDistance(int distance);
    int GetIndex() const;
    std::vector<int> GetNeighbors() const;
    void resetNode();
    bool operator< (Node const& other) const;
    int xPos() const;
    int zPos() const;
    

  private:
    int index_;
    bool wall_;
    int x_;
    int z_;
    int parent_;
    bool inQueue_;
    int heuristic_;
    bool path_;
    int distance_;
    std::vector<int> Neighbors;
  };

  class Node2
  {

  };

  Grid(float xLength, float zLength, float wallUnit);
  ~Grid();
  void ResetGrid();
  void SetWallLocation(std::vector<float> topLeft, std::vector<float> bottomRight);
  void ResetNodes();
  int getNodeIndex(std::vector<float> Pos);
  int getNodeIndex(float x, float z);
  std::vector<float> getPosition(int index);
  void SetHeuristicsAll(int goalIndex);
  Node* getNode(int index);
  bool Difference(int index1, int index2);
  //std::vector<int> getPosition(int index);

private:
  int xLength;
  int zLength;
  int numNodes;
  float xLengthWorld;
  float zLengthWorld;
  int wallUnit_;
  std::vector<Node*> nodes;
  void SetWallLocation(float x1, float z1, float x2, float z2);
  int scale = 4;

  //friend bool operator< (const Grid::Node* n1, const Grid::Node* n2);
};

//bool compareNodes (const Grid::Node* n1, const Grid::Node* n2)
//{
//  return n1->GetDistance() + n1->GetHeuristic() < n2->GetDistance() + n2->GetHeuristic();
//}


