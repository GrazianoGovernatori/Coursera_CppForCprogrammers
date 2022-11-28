#include <iostream>
#include <limits>
#include <vector>
#include <cstdlib>
#include <ctime>

using namespace std;

//-------------------------------------------------------------------------------------------------
// * * * type definition, define here all types used for Dijkstra assignment * * * //
//we define a type for the weights on edges;
//can be int, float, double
//typedef int EdgesType;
typedef double EdgesType;

//type to hold graph data, i.e. distance within nodes
typedef std::vector< std::vector<EdgesType> > matrix;

//-------------------------------------------------------------------------------------------------
// * * * constants - collect here all const values and identifiers * * * //
//we use a conventional value to identify infinity
constexpr EdgesType Gr_Infinity = std::numeric_limits<EdgesType>::max();

//constant to identify "no node", for example when tracking previous node
constexpr int Gr_NoNode = std::numeric_limits<int>::max();

//constant for graph size (number of nodes)
constexpr int Gr_Size = 50;

//set to "true" to print intermediate calculation
constexpr bool Gr_print_steps = false;

//-------------------------------------------------------------------------------------------------
// * * * classes definition * * * //
//class to store an element in open / closed list
class DijkstraListEl {
public:
    int nodeId;
    int previousNodeId;
    EdgesType edgeCost;
    DijkstraListEl(int id, int prev, EdgesType cost) : nodeId(id), previousNodeId(prev), edgeCost(cost) {};
};

//class to store graph
class Graph {
public:
    Graph(int paramSize) {
        if (paramSize < 1) {
            paramSize = 1;
        }    
        this->size = paramSize;

        graphMatrix.resize(paramSize);
        for (int iter = 0; iter < paramSize; iter++) {
            graphMatrix[iter].resize(paramSize);
        }
        //initialize to 0 the costs from a node to itself, infinite the cost from a node to any other node
        for (int outerIter = 0; outerIter < this->size; outerIter++) {
            for (int innerIter = 0; innerIter < this->size; innerIter++) {
                if (outerIter == innerIter) {
                    graphMatrix[outerIter][innerIter] = 0;
                }
                else {
                    graphMatrix[outerIter][innerIter] = Gr_Infinity;
                }
            }
        }
    }
    ~Graph() = default;
private:
    int size = 1;
    matrix graphMatrix = { {0, 0}, {0, 0 } };
public:
    int GetSize() { return this->size; }
    void PrintValues();
    void Resize(int newSize);
    void SetElement(int nodeA, int nodeB, EdgesType edgeValue);
    EdgesType GetEdgeDistance(int nodeA, int nodeB) { return graphMatrix[nodeA][nodeB]; }
    void RandomPopulate(double grDensity, EdgesType minDistanceRange, EdgesType maxDistanceRange);
};

//Dijkstra shortest-path algo class
class Dijspa {
public:
    Dijspa() {
        numberOfNodes = 0;
        sourceNode = 0;
        destNode = 0;
    }
    Dijspa(Graph& graphData, int graphSize) {
        rGraph = graphData;
        numberOfNodes = graphSize;
    }

    EdgesType CalculateSPA(int sourceNode, int destNode);

private:
    Graph rGraph = {0};
    int numberOfNodes;
    int sourceNode;
    int destNode;
};

//utility functions
void _print_open_list(std::vector<DijkstraListEl>& list);
void _print_closed_list(std::vector<DijkstraListEl>& list);

//-------------------------------------------------------------------------------------------------
// * * * program main * * * //
//-------------------------------------------------------------------------------------------------
int main() {
    //Assignment - A
    //  initialize the graph
    Graph MyGraph(Gr_Size);

    //Assignment - B
    //  set graph randomly, with given density and range
    MyGraph.RandomPopulate(0.4, 1.0, 10.0); 

    MyGraph.PrintValues();
    cout << endl;

    //Assignment - C
    //  evaluate all possible distances, 
    // To get an average path length, compute the 49 paths, 1 to 2, 1 to 3, 1 to 4, …, 1 to 50.
    //we need 49 elements
    cout << endl;
    std::vector<EdgesType> distancesCaseAlpha(0);
    EdgesType localDistance;
    Dijspa Algo(MyGraph, MyGraph.GetSize());
    for (int iterator = 0; iterator < (MyGraph.GetSize() - 1); iterator++) {
        localDistance = Algo.CalculateSPA(0, (iterator + 1));
        if (localDistance < Gr_Infinity) {
            distancesCaseAlpha.push_back(localDistance);
            if (Gr_print_steps) {
                cout << localDistance << endl;
            }
        } else {
            if (Gr_print_steps) {
                cout << "INFINITY" << endl;
            }
        }
    }

    //Assignment - D
    //evaluate mean of distances
    if (distancesCaseAlpha.size() == 0) {
        cout << "Graph not connected!";
    } else {
        double avgPathDistance = 0;
        for (auto& x : distancesCaseAlpha) {
            avgPathDistance += (double)x;
        }
        avgPathDistance /= distancesCaseAlpha.size();
        cout << "Average distance in paths: " << avgPathDistance;
    }
    cout << endl;
}



//-------------------------------------------------------------------------------------------------
// * * * member functions * * * //
//Graph class - Member functions
void Graph::PrintValues() {
    for (int outerIter = 0; outerIter< graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            if (graphMatrix[outerIter][iter] == Gr_Infinity) {
                cout << "\t[" << outerIter << "][" << iter << "]: " << "-";
            } else {
                cout << "\t[" << outerIter << "][" << iter << "]: " << graphMatrix[outerIter][iter];
            }
        }
        cout << endl;
    }
}

void Graph::Resize(int newSize) {
    if (newSize < 1) {
        newSize = 1;
    }
    this->size = newSize;

    graphMatrix.resize(newSize);
    for (int iter = 0; iter < newSize; iter++) {
        graphMatrix[iter].resize(newSize);
    }
}

void Graph::SetElement(int nodeA, int nodeB, EdgesType edgeValue) {
    if ((nodeA < this->size) && (nodeB < this->size)) {
        graphMatrix[nodeA][nodeB] = edgeValue;
        graphMatrix[nodeB][nodeA] = edgeValue;
    }
}

void Graph::RandomPopulate(double grDensity, EdgesType minDistanceRange, EdgesType maxDistanceRange) {
    srand((unsigned)time(nullptr));
    EdgesType edgeValue;

    if (maxDistanceRange < minDistanceRange) {
        //should throw an exception...
        return;
    }
    if (minDistanceRange == 0) {
        //we avoid that 2 distinct nodes have distances zero
        //should throw an exception...
        return;
    }
    if ((minDistanceRange < 0) || (maxDistanceRange < 0)) {
        //only positive distances
        //should throw an exception...
        return;
    }

    for (int outerIter = 0; outerIter < this->size; outerIter++) {
        for (int innerIter = 0; innerIter < outerIter; innerIter++) {

            if (((float)rand() / RAND_MAX) < grDensity) {
                edgeValue = (((float)rand() / RAND_MAX) * (maxDistanceRange - minDistanceRange)) + minDistanceRange;
                SetElement(outerIter, innerIter, edgeValue);
            }
            else {
                SetElement(outerIter, innerIter, Gr_Infinity);
            }
        }
        //set distance to self: zero
        SetElement(outerIter, outerIter, 0);
    }
}

//Dijspa class - Mamber functions
EdgesType Dijspa::CalculateSPA(int sourceNode, int destNode) {
    if (this->numberOfNodes < 1) {
        return Gr_Infinity;
    }
    if ((sourceNode > this->numberOfNodes) || (destNode > this->numberOfNodes)) {
        return Gr_Infinity;
    }
    if (sourceNode == destNode) {
        return static_cast<EdgesType>(0);
    }

    std::vector<DijkstraListEl> closedSet;
    std::vector<DijkstraListEl> openSet;

    DijkstraListEl myEl(sourceNode, sourceNode, 0);
    closedSet.push_back(myEl);

    for (int iterator = 0; iterator < this->numberOfNodes; iterator++) {
        //being symmetric, we use line zero
        if (iterator != sourceNode) {
            DijkstraListEl myEl(iterator, Gr_NoNode, Gr_Infinity);
            openSet.push_back(myEl);
        }
    }

    if (Gr_print_steps) {
        _print_open_list(openSet);
        _print_closed_list(closedSet);
    }

    EdgesType minDistance;
    int idxNodeToAdd;
    EdgesType localDist;
    int iterCount = 0;
    do {
        
        for (int iterClosed = 0; iterClosed < closedSet.size(); iterClosed++) {
            for (int iterOpen = 0; iterOpen < openSet.size(); iterOpen++) {
                localDist = rGraph.GetEdgeDistance(closedSet[iterClosed].nodeId, openSet[iterOpen].nodeId);
                if (localDist < Gr_Infinity) {
                    if ((closedSet[iterClosed].edgeCost + localDist) < openSet[iterOpen].edgeCost) {
                        //update open set value!
                        openSet[iterOpen].edgeCost = closedSet[iterClosed].edgeCost + localDist;
                        openSet[iterOpen].previousNodeId = iterClosed;
                    }
                }
            }
        }

        idxNodeToAdd = Gr_NoNode;
        minDistance = Gr_Infinity;
        for (int iterOpen = 0; iterOpen < openSet.size(); iterOpen++) {
            if (openSet[iterOpen].edgeCost < minDistance) {
                idxNodeToAdd = iterOpen;
            }
        }

        if (idxNodeToAdd != Gr_NoNode) {
            //TODO: replace by cc
            DijkstraListEl myEl(openSet[idxNodeToAdd].nodeId , openSet[idxNodeToAdd].previousNodeId, openSet[idxNodeToAdd].edgeCost);
            closedSet.push_back(myEl);
            openSet.erase(openSet.begin() + idxNodeToAdd);
        }

        if (Gr_print_steps) {
            cout << endl << "iteration: " << iterCount++ << endl;
            _print_open_list(openSet);
            _print_closed_list(closedSet);
        }

    } while ((openSet.size() > 0) && (idxNodeToAdd != Gr_NoNode));

    for (auto &target : closedSet) {
        if (target.nodeId == destNode) {
            return target.edgeCost;
        }
    }
    //in case destination node is not in closed set => return an infinite cost
    return Gr_Infinity;
}

//-------------------------------------------------------------------------------------------------
// * * * utility functions * * * //
void _print_open_list(std::vector<DijkstraListEl> & list) {
    cout << endl;
    cout << "open list" << endl;
    cout << "node\tcost" << endl;
    for (auto x : list) {
        cout << x.nodeId << "\t" << x.previousNodeId << "\t" << x.edgeCost << endl;
    }
}

void _print_closed_list(std::vector<DijkstraListEl>& list) {
    cout << endl;
    cout << "closed list" << endl;
    cout << "node\tcost\tprev" << endl;
    for (auto x : list) {
        cout << x.nodeId << "\t" << x.edgeCost << "\t" << x.previousNodeId << endl;
    }
}
