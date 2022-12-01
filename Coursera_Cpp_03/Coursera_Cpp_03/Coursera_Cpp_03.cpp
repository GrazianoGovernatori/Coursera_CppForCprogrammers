#include <iostream>
#include <iterator>
#include <fstream>
#include <limits>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>

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

//set a maximum number of nodes
constexpr int Gr_Max_Number_Of_Nodes = 100;
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
        ResetGraph();
    }
    ~Graph() = default;
private:
    int size = 1;
    matrix graphMatrix = { {0, 0}, {0, 0 } };
public:
    int GetSize() { return this->size; }
    void PrintValues(bool printParenthesis = true);
    void Resize(int newSize);
    void SetElement(int nodeA, int nodeB, EdgesType edgeValue);
    EdgesType GetEdgeDistance(int nodeA, int nodeB) { return graphMatrix[nodeA][nodeB]; }
    void RandomPopulate(double grDensity, EdgesType minDistanceRange, EdgesType maxDistanceRange);
    bool PopulateFromFile(string path);
    void ResetGraph();
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
    Graph rGraph = { 0 };
    int numberOfNodes;
    int sourceNode;
    int destNode;
};

//class to store each element, that will make up a list, resulting of MST algorithm
class EdgesElement {
public:
    EdgesElement(int a, int b, EdgesType c) : nodeA(a), nodeB(b), edgeCost(c) {}
    EdgesElement() {}
public:
    int nodeA;
    int nodeB;
    EdgesType edgeCost;
};

class JarnikPrimMST {
private:
    std::vector<EdgesElement> fullGraph = {};
    std::vector<EdgesElement> mstGraph = {};
    int numOfNodes;

public:
    JarnikPrimMST(string path);

    void ResetGraph() {
        fullGraph.resize(0);
    }

    void AddElement(EdgesElement el) {
        fullGraph.push_back(el);
    }

    EdgesType ComputeMST(int startingNode);

    void PrintGraphs();

    int GetGraphSize() { return numOfNodes; };
    void SetGraphSize(int newSize) { numOfNodes = newSize; };
};

//utility functions
void _print_open_list(std::vector<DijkstraListEl>& list);
void _print_closed_list(std::vector<DijkstraListEl>& list);

//-------------------------------------------------------------------------------------------------
// * * * program main * * * //
//-------------------------------------------------------------------------------------------------
int main() {
    //constructor will load data from file
    JarnikPrimMST MyJPMST("cplusplus4c_homeworks_Homework3_SampleTestData_mst_data.txt");

    //set node "0" as starting node (can be whatever)
    auto totalCost = MyJPMST.ComputeMST(0);
    MyJPMST.PrintGraphs();

    if (totalCost < Gr_Infinity) {
        cout << endl << "Total cost of MST: " << totalCost << endl;
    } else {
        cout << endl << "Error in computing MST!" << endl;
    }
}

//-------------------------------------------------------------------------------------------------
// * * * member functions * * * //
//Graph class - Member functions
void Graph::PrintValues(bool showParenthesis) {
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            if (showParenthesis) {
                if (graphMatrix[outerIter][iter] == Gr_Infinity) {
                    cout << "\t[" << outerIter << "][" << iter << "]: " << "-";
                }
                else {
                    cout << "\t[" << outerIter << "][" << iter << "]: " << graphMatrix[outerIter][iter];
                }
            }
            else {
                if (graphMatrix[outerIter][iter] == Gr_Infinity) {
                    cout << "\t" << "-";
                }
                else {
                    cout << "\t" << (int)graphMatrix[outerIter][iter];
                }
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

bool Graph::PopulateFromFile(string path) {
    //ifstream graphInputFile("C:\\Users\\ggovernatori\\source\\repos\\Coursera_Cpp_03\\Coursera_Cpp_03\\Debug\\cplusplus4c_homeworks_Homework3_SampleTestData_mst_data.txt"); //file in same directory as executable
    ifstream graphInputFile(path); //file in VS project directory 
    //"cplusplus4c_homeworks_Homework3_SampleTestData_mst_data.txt"
    istream_iterator<string> start(graphInputFile), end;
    vector<string> fileRawData(start, end);

    if (fileRawData.size() == 0) {
        return false; //error in file acquisition
    }
    int numberOfEdges = (fileRawData.size() - 1) / 3;

    int localNumberOfNodes;
    //load number of nodes - example on typical check on inputs
    try {
        localNumberOfNodes = std::stoi(fileRawData[0]);
    }
    catch (std::invalid_argument const& exc) {
        std::cout << "invalid_argument: " << exc.what() << '\n';
    }
    catch (std::out_of_range const& exc) {
        std::cout << "out_of_range: " << exc.what() << '\n';
    }

    if (localNumberOfNodes > Gr_Max_Number_Of_Nodes) {
        std::cout << "too many nodes!";
        return false;
    }

    //reset graph matrix to be all zeros on diagonal, infinite otherwise
    this->ResetGraph();

    int nodeA, nodeB;
    EdgesType edgeCost;
    //for readability, we split in lines
    for (int iter = 1; iter < (fileRawData.size() - 1); /*do not increment here...*/) {
        nodeA = std::stoi(fileRawData[iter]);
        iter++;
        nodeB = std::stoi(fileRawData[iter]);
        iter++;
        edgeCost = static_cast<EdgesType>(std::stoi(fileRawData[iter]));
        iter++;
        SetElement(nodeA, nodeB, edgeCost);
    }
    return false;
}

void Graph::ResetGraph() {
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

//Dijspa class - Member functions
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
            DijkstraListEl myEl(openSet[idxNodeToAdd].nodeId, openSet[idxNodeToAdd].previousNodeId, openSet[idxNodeToAdd].edgeCost);
            closedSet.push_back(myEl);
            openSet.erase(openSet.begin() + idxNodeToAdd);
        }

        if (Gr_print_steps) {
            cout << endl << "iteration: " << iterCount++ << endl;
            _print_open_list(openSet);
            _print_closed_list(closedSet);
        }

    } while ((openSet.size() > 0) && (idxNodeToAdd != Gr_NoNode));

    for (auto& target : closedSet) {
        if (target.nodeId == destNode) {
            return target.edgeCost;
        }
    }
    //in case destination node is not in closed set => return an infinite cost
    return Gr_Infinity;
}


EdgesType JarnikPrimMST::ComputeMST(int startingNode) {

    std::vector<int> nodesReached = {};

    mstGraph.resize(0);

    nodesReached.push_back(startingNode);

    EdgesType localCost = Gr_Infinity;
    EdgesElement localElem;

    do {
        localElem.edgeCost = Gr_Infinity;
        for (auto x : nodesReached) {
            for (auto& y : fullGraph) {
                if ((y.nodeA == x) || (y.nodeB == x)) { //if node contains a node of the temporary MST...
                    if ((std::find(nodesReached.begin(), nodesReached.end(), y.nodeA) == nodesReached.end()) ||   //if one of the 2 nodes are not reached by MST...
                        (std::find(nodesReached.begin(), nodesReached.end(), y.nodeB) == nodesReached.end()))
                    {
                        if (y.edgeCost < localElem.edgeCost) {
                            localElem = y;  //assign new edge as possible new element...
                        }
                    }
                }
            }
        }
        if (localElem.edgeCost != Gr_Infinity) {
            //we have another edge...
            if (std::find(nodesReached.begin(), nodesReached.end(), localElem.nodeA) == nodesReached.end()) {
                nodesReached.push_back(localElem.nodeA);
            }
            else {
                nodesReached.push_back(localElem.nodeB);
            }

            mstGraph.push_back(localElem);
        } else {
            return Gr_Infinity; //no MST found!
        }
    } while (nodesReached.size() < GetGraphSize());

    EdgesType totalMstCost = 0;
    for (auto& el : mstGraph) {
        totalMstCost += el.edgeCost;
    }
    return totalMstCost;
}

void JarnikPrimMST::PrintGraphs() {
    cout << endl << "Original Graph: " << endl;
    for (auto &el : fullGraph) {
        cout << "   " << el.nodeA << ", " << el.nodeB << ", " << el.edgeCost << endl;
    }
    cout << endl << "MST: " << endl;
    for (auto &el : mstGraph) {
        cout << "   " << el.nodeA << ", " << el.nodeB << ", " << el.edgeCost << endl;
    }
}


JarnikPrimMST::JarnikPrimMST(string path) {
    ifstream graphInputFile(path); //file in VS project directory 
    //"cplusplus4c_homeworks_Homework3_SampleTestData_mst_data.txt"
    istream_iterator<string> start(graphInputFile), end;
    vector<string> fileRawData(start, end);

    //we do not check for file consistency, only some try-catch to alert about errors
    int numberOfEdges = (fileRawData.size() - 1) / 3;

    int localNumberOfNodes;
    //load number of nodes - example on typical check on inputs
    try {
        localNumberOfNodes = std::stoi(fileRawData[0]);
    }
    catch (std::invalid_argument const& exc) {
        std::cout << "invalid_argument: " << exc.what() << '\n';
    }
    catch (std::out_of_range const& exc) {
        std::cout << "out_of_range: " << exc.what() << '\n';
    }

    if (localNumberOfNodes > Gr_Max_Number_Of_Nodes) {
        std::cout << "too many nodes!";
    }

    //reset graph matrix to be all zeros on diagonal, infinite otherwise
    this->ResetGraph();
    this->SetGraphSize(localNumberOfNodes);

    int nodeA, nodeB;
    EdgesType edgeCost;
    EdgesElement elToAdd;
    //for readability, we split in lines
    for (int iter = 1; iter < (fileRawData.size() - 1); /*do not increment here...*/) {
        elToAdd.nodeA = std::stoi(fileRawData[iter]);
        iter++;
        elToAdd.nodeB = std::stoi(fileRawData[iter]);
        iter++;
        elToAdd.edgeCost = static_cast<EdgesType>(std::stoi(fileRawData[iter]));
        iter++;
        AddElement(elToAdd);
    }
}


//-------------------------------------------------------------------------------------------------
// * * * utility functions * * * //
void _print_open_list(std::vector<DijkstraListEl>& list) {
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
