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
// * * * type definition, define here all types used for Hex assignment * * * //

enum class HBPlayerElem{
    EMPTY = 0,
    BLUE,
    RED,
};

//we're not interested in cost (we can assume it to be constant, and set to 1
class edgeType {
public:
    int nodeA;
    int nodeB;
};

//type to hold graph data, i.e. distance within nodes
typedef std::vector< std::vector<HBPlayerElem> > matrix;

//-------------------------------------------------------------------------------------------------
// * * * constants - collect here all const values and identifiers * * * //
constexpr int Gr_MinBoardSize = 7;
constexpr int Gr_MaxBoardSize = 11;

//-------------------------------------------------------------------------------------------------
// * * * classes definition * * * //

//class to store graph
class HexBoard {
public:
    HexBoard(int paramSize);
    ~HexBoard() = default;
private:
    int size = 7;
    matrix graphMatrix;
    std::vector<edgeType> playerGraph[2];
public:
    void ResetBoard(void);
    int GetSize() { return this->size; }
    void PrintValues(bool printParenthesis = true);
    string SymbolToDisplay(HBPlayerElem lElement);

    void Resize(int newSize);
    void SetElement(int i, int j, HBPlayerElem edgeValue);
    bool PlayerMove(HBPlayerElem player, int i, int j);

    void ResetPlayerGraph(HBPlayerElem player);
    int GetNodeId(int nodaA_x, int nodeA_y);
    void AddToPlayerGraph(HBPlayerElem player, int nodeA, int nodeB);
};



//-------------------------------------------------------------------------------------------------
// * * * program main * * * //
//-------------------------------------------------------------------------------------------------
int main() {
    //constructor will load data from file

    
    HexBoard myBoard(11);
    char myChoice;
    HBPlayerElem myPlayer;
    int tempi, tempj;

    do {
        cout << "Enter choice: ('q' to exit)";
        cin >> myChoice;

        if (myChoice == 'q') {
            break;
        }
        if (myChoice == 'b') {
            myPlayer = HBPlayerElem::BLUE;
        }
        if (myChoice == 'r') {
            myPlayer = HBPlayerElem::RED;
        }
        cout << endl;
        cout << "Enter i:";
        cin >> tempi;
        cout << "Enter j:";
        cin >> tempj;

        myBoard.PlayerMove(myPlayer, tempi, tempj);
        myBoard.PrintValues();

    } while (1);
    myBoard.PrintValues();

}

//-------------------------------------------------------------------------------------------------
// * * * member functions * * * //
//HexBoard class - Member functions
HexBoard::HexBoard(int paramSize) {
    if (paramSize < Gr_MinBoardSize) {
        paramSize = Gr_MinBoardSize;
    }
    if (paramSize > Gr_MaxBoardSize) {
        paramSize = Gr_MaxBoardSize;
    }

    this->size = paramSize;

    //always an n X n matrix
    graphMatrix.resize(paramSize);
    for (int iter = 0; iter < paramSize; iter++) {
        graphMatrix[iter].resize(paramSize);
    }

    ResetBoard();
}

void HexBoard::ResetBoard(void) {
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            graphMatrix[outerIter][iter] = HBPlayerElem::EMPTY;
        }
    }
    ResetPlayerGraph(HBPlayerElem::BLUE);
    ResetPlayerGraph(HBPlayerElem::RED);
}

void HexBoard::PrintValues(bool showParenthesis) {
    string offset = "  ";
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        cout << endl;
        //plot ". - . - . - "
        for (int lOff = 0; lOff < outerIter; lOff++) {
            cout << offset;
        }
        cout << " ";
        cout << SymbolToDisplay(graphMatrix[outerIter][0]);
        for (int iter = 1; iter < graphMatrix.size(); iter++) {
            cout << " - " << SymbolToDisplay(graphMatrix[outerIter][iter]);;
        }
        cout << endl;
        cout << offset;
        //plot " \  /  \  / ..."
        for (int lOff = 0; lOff < outerIter; lOff++) {
            cout << offset;
        }
        if (outerIter < (graphMatrix.size() - 1)) {
            cout << "\\ ";
            for (int iter = 1; iter < graphMatrix.size(); iter++) {
                cout << "/ \\ ";
            }
        }
    }
}

string HexBoard::SymbolToDisplay(HBPlayerElem lElement) {
    if (lElement == HBPlayerElem::BLUE)
        return "X";
    if (lElement == HBPlayerElem::RED)
        return "O";
    return ".";
}

void HexBoard::SetElement(int i, int j, HBPlayerElem edgeValue) {
    graphMatrix[i][j] = edgeValue;
}

bool HexBoard::PlayerMove(HBPlayerElem player, int i, int j) {
    if (graphMatrix[i][j] != HBPlayerElem::EMPTY) {
        return false; //BAD MOVE!
    }

    SetElement(i, j, player);

    //verify if same color on the left
    if (i > 0) {
        if (graphMatrix[i-1][j] == player) {
            AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i - 1, j));
        }
    }

    //add relevant edges to player's graph!
}


void HexBoard::ResetPlayerGraph(HBPlayerElem player) {
    if (player == HBPlayerElem::BLUE) {
        playerGraph[0].resize(0);
    }
    if (player == HBPlayerElem::RED) {
        playerGraph[1].resize(0);
    }
}

int HexBoard::GetNodeId(int nodaA_x, int nodeA_y) {
    return (nodaA_x + (this->size) * nodeA_y);
}

void HexBoard::AddToPlayerGraph(HBPlayerElem player, int nodeA, int nodeB) {
    edgeType lEdge;

    lEdge.nodeA = nodeA;
    lEdge.nodeB = nodeB;

    if (player == HBPlayerElem::BLUE) {
        playerGraph[0].push_back(lEdge);
    }
    if (player == HBPlayerElem::RED) {
        playerGraph[1].push_back(lEdge);
    }
}