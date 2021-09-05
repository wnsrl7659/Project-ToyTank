/******************************************************************************
  * Author : Ray An
  * Class  : GAM 300
  * Brief  :

  (c) DigiPen Institute of Technology 2019
******************************************************************************/
#include "pch.h"
#include "Grid.h"

///////////////////////////////////////////////////////////////////////////////
// Grid Constructor/Destructor
///////////////////////////////////////////////////////////////////////////////
Grid::Grid(float x, float z, float wallUnit) : xLengthWorld(x), zLengthWorld(z), wallUnit_(wallUnit)
{
  // X - Horizontal
  // Z - Vertical
  xLength = x / wallUnit;
  zLength = z / wallUnit;

  numNodes = xLength * zLength;
  for (int i = 0; i < numNodes; ++i)
  {
    Node* node = new Node(xLength, zLength, i);
    nodes.push_back(node);
  }
}

Grid::~Grid()
{
  for (int i = 0; i < numNodes; ++i)
  {
    delete nodes[i];
  }
}

///////////////////////////////////////////////////////////////////////////////
// Reset when the wall locations are changed
///////////////////////////////////////////////////////////////////////////////
void Grid::ResetGrid()
{
  for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    (*it)->SetWall(false);
  }
}

///////////////////////////////////////////////////////////////////////////////
// If the node has walls, set the flag
//
// Input: position of the wall
///////////////////////////////////////////////////////////////////////////////
void Grid::SetWallLocation(float x1, float z1, float x2, float z2)
{
  // horizontal - vertical
  bool orientation = true;
  // left - right
  bool point1 = true;
  // top - bottom
  bool top = true;
  if (z1 > z2)
  {
    point1 = false;
  }
  if ((point1 && x1 < x2) || (!point1 && x2 < x1))
  {
    top = false;
  }

  if (x1 - x2 == wallUnit_ || x2 - x1 == wallUnit_)
  {
    orientation == false;
  }

  int length = 0;
  int startX = 0;
  int startZ = 0;

  if (point1 && top)
  {
    //startZ = z1 / wallUnit_;
    //startX = x1 / wallUnit_ - 1;
    // 0.5 = wallUnit_ / 2
    startZ = z1 - 0.5f;
    startX = x1 + 0.5f;
  }
  else if (point1 && !top)
  {
    //startZ = z1 / wallUnit_;
    //startX = x1 / wallUnit_;
    startZ = z1 - 0.5f;
    startX = x1 + 0.5f;
  }
  else if (!point1 && top)
  {
    //startZ = z2 / wallUnit_;
    //startX = x2 / wallUnit_ - 1;
    startZ = z2 - 0.5f;
    startX = x2 + 0.5f;
  }
  else
  {
    //startZ = z2 / wallUnit_;
    //startX = x2 / wallUnit_;
    startZ = z2 + 0.5f;
    startX = x2 + 0.5f;
  }

  if (orientation)
  {
    if (point1)
    {
      length = (x2 - x1) / wallUnit_;
    }
    else
    {
      length = (x1 - x2) / wallUnit_;
    }
    for (int i = 0; i < length; ++i)
    {
      nodes[startZ * xLength + startX + i]->SetWall(true);
    }

  }
  else
  {
    if ((point1 && top) || (!point1 && !top))
    {
      length = (z1 - z2) / wallUnit_;
    }
    else
    {
      length = (z2 - z1) / wallUnit_;
    }
    for (int i = 0; i < length; ++i)
    {
      nodes[(startZ + i) * xLength + startX]->SetWall(true);
    }
  }

}

void Grid::SetWallLocation(std::vector<float> topLeft, std::vector<float> bottomRight)
{
  SetWallLocation(topLeft[0] + xLengthWorld / 2, topLeft[2] + zLengthWorld / 2, bottomRight[0] + xLengthWorld / 2, bottomRight[2] + zLengthWorld / 2);
}


///////////////////////////////////////////////////////////////////////////////
// Get nodes by index
//
// Input:: the index of the node
///////////////////////////////////////////////////////////////////////////////
Grid::Node* Grid::getNode(int index)
{
  return nodes[index];
}

///////////////////////////////////////////////////////////////////////////////
// Get the world position of a node
//
// Input:: the index of the node
///////////////////////////////////////////////////////////////////////////////
std::vector<float> Grid::getPosition(int index)
{
  int x = index % xLength;
  int z = index / xLength;

  std::vector<float> pos;
  pos.push_back(x * wallUnit_ - xLengthWorld / 2);
  pos.push_back(z * wallUnit_ - zLengthWorld / 2);

  return pos;

}

bool Grid::Difference(int index1, int index2)
{
  int x1 = index1 % xLength;
  int z1 = index1 / xLength;
  int x2 = index2 % xLength;
  int z2 = index2 / xLength;

  int xdiff = x1 - x2;
  int zdiff = z1 - z2;
  if (xdiff < 0)
  {
    xdiff = -xdiff;
  }
  if (zdiff < 0)
  {
    zdiff = -zdiff;
  }

  return xdiff + zdiff < 50;
}

///////////////////////////////////////////////////////////////////////////////
// Reset each node's distance and heuristic estimate
///////////////////////////////////////////////////////////////////////////////
void Grid::ResetNodes()
{
  for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    (*it)->resetNode();
  }
}

///////////////////////////////////////////////////////////////////////////////
// Get the index of the node at (x, z)
//
// Input: position of the node in x-z pair
///////////////////////////////////////////////////////////////////////////////
int Grid::getNodeIndex(float x, float z)
{
  int xGrid = (x + xLengthWorld / 2) / wallUnit_;
  int zGrid = (z + zLengthWorld / 2) / wallUnit_;
  //return nodes[xGrid * zLength + zGrid]->GetIndex();
  return xGrid + xLength * zGrid;
}

int Grid::getNodeIndex(std::vector<float> Pos)
{
  return getNodeIndex(Pos[0], Pos[2]);
}

///////////////////////////////////////////////////////////////////////////////
// Calculate h for each node
//
// Input: position of the goal
///////////////////////////////////////////////////////////////////////////////
void Grid::SetHeuristicsAll(int goalIndex)
{
  int goalx = goalIndex % xLength;
  int goalz = goalIndex / xLength;
  for (std::vector<Node*>::iterator it = nodes.begin(); it != nodes.end(); ++it)
  {
    (*it)->SetHeuristic(goalx, goalz);
  }
}


///////////////////////////////////////////////////////////////////////////////
// Node Constructor/Destructor
///////////////////////////////////////////////////////////////////////////////
Grid::Node::Node(int xLength, int zLength, int index) : index_(index)
{
  parent_ = -1;
  inQueue_ = false;
  heuristic_ = 1000000;
  path_ = false;
  distance_ = 1000000;
  wall_ = false;

  x_ = index % xLength;
  z_ = index / xLength;

  if (x_ > 0)
  {
    Neighbors.push_back((x_ - 1) + xLength * (z_));
  }
  if (x_ < xLength - 1)
  {
    Neighbors.push_back((x_ + 1) + xLength * (z_));
  }
  if (z_ > 0)
  {
    Neighbors.push_back((x_)+xLength * (z_ - 1));
  }
  if (z_ < zLength - 1)
  {
    Neighbors.push_back((x_)+xLength * (z_ + 1));
  }

}

///////////////////////////////////////////////////////////////////////////////
// Node Getters/Setters
///////////////////////////////////////////////////////////////////////////////
int Grid::Node::GetParent() const
{
  return parent_;
}

void Grid::Node::SetParent(int parent)
{
  parent_ = parent;
}

bool Grid::Node::IsWall() const
{
  return wall_;
}

void Grid::Node::SetWall(bool wall)
{
  wall_ = wall;
}

int Grid::Node::GetDistance() const
{
  return distance_;
}

void Grid::Node::SetDistance(int distance)
{
  distance_ = distance;
}

bool Grid::Node::IsInQueue() const
{
  return inQueue_;
}

void Grid::Node::Enqueue(bool inqueue)
{
  inQueue_ = inqueue;
}

bool Grid::Node::IsPath() const
{
  return path_;
}

void Grid::Node::SetPath(bool path)
{
  path_ = path;
}

int Grid::Node::GetIndex() const
{
  return index_;
}

std::vector<int> Grid::Node::GetNeighbors() const
{
  return Neighbors;
}

int Grid::Node::GetHeuristic() const
{
  return heuristic_;
}

///////////////////////////////////////////////////////////////////////////////
// Calculate heuristic - Manhattan distance
//
// Input: position of the goal
///////////////////////////////////////////////////////////////////////////////
void Grid::Node::SetHeuristic(int goal_x, int goal_z)
{
  int heuristic = goal_x - x_;
  if (goal_x - x_ < 0)
  {
    heuristic = x_ - goal_x;
  }
  if (goal_z - z_ < 0)
  {
    heuristic += (z_ - goal_z);
  }
  else
  {
    heuristic += (goal_z - z_);
  }
  heuristic_ = heuristic;
}

///////////////////////////////////////////////////////////////////////////////
// Get the world position of a node
//
// Input:: the index of the node
///////////////////////////////////////////////////////////////////////////////
int Grid::Node::xPos() const
{
  return x_;
}

int Grid::Node::zPos() const
{
  return z_;
}

///////////////////////////////////////////////////////////////////////////////
// Reset each node's distance and heuristic estimate
///////////////////////////////////////////////////////////////////////////////
void Grid::Node::resetNode()
{
  inQueue_ = false;
  heuristic_ = 1000000;
  path_ = false;
  distance_ = 1000000;
  parent_ = -1;
}

///////////////////////////////////////////////////////////////////////////////
// Compare the estimate of two nodes
// Used for priority queue
///////////////////////////////////////////////////////////////////////////////
bool Grid::Node::operator< (Node const& other) const
{
  return distance_ + heuristic_ < other.distance_ + other.heuristic_;
}
