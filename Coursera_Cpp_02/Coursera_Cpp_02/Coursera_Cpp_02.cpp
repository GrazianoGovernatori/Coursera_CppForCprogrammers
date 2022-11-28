#include <iostream>
#include <limits>
#include <vector>


using namespace std;

//we define a type for the weights on edges;
//can be int, float, double
typedef int EdgesType;

//we use a conventional value to identify infinity
constexpr EdgesType Gr_Infinity = std::numeric_limits<EdgesType>::max();
constexpr int Gr_NoNode = std::numeric_limits<int>::max();

typedef std::vector< std::vector<EdgesType> > matrix;

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
   
};

//Dijkstra shortest-path algo class
class Dijspa {
public:
    Dijspa() {
        numberOfNodes = 0;
        int sourceNode = 0;
        int destNode = 0;
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

//test commit
int main() {
    Graph MyGraph(7);

    MyGraph.SetElement(0, 1, 1);
    MyGraph.SetElement(0, 2, 1);
    MyGraph.SetElement(3, 1, 2);
    MyGraph.SetElement(3, 2, 3);
    MyGraph.SetElement(2, 4, 2);
    MyGraph.SetElement(4, 5, 1);
    MyGraph.SetElement(3, 5, 10);
    MyGraph.SetElement(3, 6, 20);
    MyGraph.SetElement(5, 6,  5);


    MyGraph.PrintValues();
    cout << endl;

    Dijspa Algo( MyGraph, MyGraph.GetSize());
    int start, dest;
    start = 0;
    dest = 6;
    auto distance = Algo.CalculateSPA(start, dest);
    if (distance < Gr_Infinity) {
        cout << endl << "distance btwn nodes: " << distance;
    } else {
        cout << endl << "not possible to go from node " << start << " to node " << dest;
    }
}


//Graph class - Mamber functions

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
    std::vector<DijkstraListEl> openSet ;
    
    DijkstraListEl myEl(sourceNode, sourceNode, 0);
    closedSet.push_back(myEl);

    for (int iterator = 0; iterator < this->numberOfNodes; iterator++) {
        //being symmetric, we use line zero
        if (iterator != sourceNode) {
            DijkstraListEl myEl(iterator, Gr_NoNode, Gr_Infinity);
            openSet.push_back(myEl);
        }
    }

    _print_open_list(openSet);
    _print_closed_list(closedSet);

    int minDistance;
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



        cout << endl << "iteration: " << iterCount++ << endl;
        _print_open_list(openSet);
        _print_closed_list(closedSet);


    } while ((openSet.size() > 0) && (idxNodeToAdd != Gr_NoNode));

    for (auto target : closedSet) {
        if (target.nodeId == destNode) {
            return target.edgeCost;
        }
    }
    //in case destination node is not in closed set => return an infinite cost
    return Gr_Infinity;
}


//utility functions
void _print_open_list(std::vector<DijkstraListEl> & list) {
    cout << endl;
    cout << "open list" << endl;
    cout << "node\tcost" << endl;
    for (auto x : list)  {
        cout << x.nodeId << "\t" << x.previousNodeId << "\t" << x.edgeCost << endl;
    }
}
void _print_closed_list(std::vector<DijkstraListEl>& list) {
    cout << endl;
    cout << "closed list" << endl;
    cout << "node\tcost\tprev" << endl;
    for (auto x : list) {
        cout << x.nodeId << "\t" << x.edgeCost << "\t" << x.previousNodeId  << endl;
    }
}