#include <iostream>
#include <limits>
#include <vector>


using namespace std;

//we define a type for the weights on edges;
//can be int, float, double
typedef int EdgesType;

//we use a conventional value to identify infinity
constexpr EdgesType Gr_Infinity = std::numeric_limits<EdgesType>::max();

typedef std::vector< std::vector<EdgesType> > matrix;

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
    }
    ~Graph() = default;
private:
    int size = 1;
    matrix graphMatrix = { {0, 1}, {2, 3 } };
public:
    int GetSize() { return this->size; }
    void PrintValues();
    void Resize(int newSize);
    void SetElement(int nodeA, int nodeB, EdgesType edgeValue);
   
};




int main() {
    Graph MyGraph(4);

    MyGraph.PrintValues();
    cout << endl;
    MyGraph.Resize(8);
    MyGraph.PrintValues();
    cout << endl;

    MyGraph.SetElement(5, 4, 100);
    MyGraph.PrintValues();
    cout << endl;

}


//Graph class - Mamber functions

void Graph::PrintValues() {
    for (int outerIter = 0; outerIter< graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            cout << "\t[" << outerIter << "][" << iter << "]: " << graphMatrix[outerIter][iter];
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
