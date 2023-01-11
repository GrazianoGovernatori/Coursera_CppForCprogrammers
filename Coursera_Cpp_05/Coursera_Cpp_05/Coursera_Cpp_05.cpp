#include <iostream>
#include <iterator>
#include <fstream>
#include <limits>
#include <algorithm>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <string>
#include <random>

using namespace std;

//-------------------------------------------------------------------------------------------------
// * * * type definition, define here all types used for Hex assignment * * * //

//we define a type for the weights on edges;
//can be int, float, double
//typedef int EdgesType;
typedef int EdgesType;

//-------------------------------------------------------------------------------------------------
// * * * constants - collect here all const values and identifiers * * * //
//we use a conventional value to identify infinity
constexpr EdgesType Gr_Infinity = std::numeric_limits<EdgesType>::max();

enum class HBPlayerElem {
    EMPTY = 0,
    BLUE,
    RED,
};

//type to hold graph data, i.e. distance within nodes
typedef std::vector< std::vector<HBPlayerElem> > matrix;

//type to create a node - chances of win pair
typedef std::pair<int, int> nodeMeritPair;
//-------------------------------------------------------------------------------------------------
// * * * constants - collect here all const values and identifiers * * * //
constexpr int Gr_MinBoardSize = 4;
constexpr int Gr_MaxBoardSize = 11;

//-------------------------------------------------------------------------------------------------
// * * * classes definition * * * //

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

//class to store graph
class HexBoard {
public:
    HexBoard(int paramSize);
    ~HexBoard() = default;
private:
    int size = 4;
    matrix graphMatrix;
    std::vector<EdgesElement> playerGraph[2];
    HBPlayerElem winner = HBPlayerElem::EMPTY;
    int remainingMoves;

    //AI-related
    std::vector<HBPlayerElem> remainingMovesSequence = {};
public:
    void ResetBoard(void);
    int GetSize() { return this->size; }
    void PrintValues(bool printParenthesis = true);
    string SymbolToDisplay(HBPlayerElem lElement);

    void Resize(int newSize);
    void SetElement(int i, int j, HBPlayerElem edgeValue);
    void SetElement(int node, HBPlayerElem edgeValue);
    bool PlayerMove(HBPlayerElem player, int i, int j);

    void ResetPlayerGraph(HBPlayerElem player);
    int GetNodeId(int nodaA_x, int nodeA_y);
    void AddToPlayerGraph(HBPlayerElem player, int nodeA, int nodeB);
    void PrintPlayerGraph(HBPlayerElem player);
    bool PlayerWin(HBPlayerElem player, std::vector<EdgesElement> graph);
    int GetNumOfNodes(void) { return size * size; }
    HBPlayerElem GetWinner(void) { return winner; }
    int GetRemainingMoves(void) { return remainingMoves; };

    //AI-related
    int ComputeBestMove(HBPlayerElem player);
    HBPlayerElem EvaluateWin(void);  //alternative method to compute winner
    void MarkNeighbors(std::vector< std::vector<int>>& matrix, int row, int col);

};


class JarnikPrimMST {
private:
    std::vector<EdgesElement>& fullGraph;
    int numOfNodes;

public:
    std::vector<EdgesElement> mstGraph = {};

public:
    JarnikPrimMST(std::vector<EdgesElement>& fullGraphRef, int numOfNodes);

    void ResetGraph() {
        //fullGraph.resize(0);
    }

    void AddElement(EdgesElement el) {
        //fullGraph.push_back(el);
    }

    EdgesType ComputeMST(int startingNode);

    void PrintGraphs();

    int GetGraphSize() { return numOfNodes; };
    void SetGraphSize(int newSize) { numOfNodes = newSize; };
};


//-------------------------------------------------------------------------------------------------
// * * * program main * * * //
//-------------------------------------------------------------------------------------------------
int main() {

    char myChoice;
    HBPlayerElem myPlayer;
    int tempi, tempj;

    do {
        cout << "Enter player color: [b|r]";
        cin >> myChoice;

        if (myChoice == 'b') {
            myPlayer = HBPlayerElem::BLUE;
            break;
        }
        if (myChoice == 'r') {
            myPlayer = HBPlayerElem::RED;
            break;
        }

    } while (1);

    int tmpSize;
    do {
        cout << "Enter board size (edge size):";
        cin >> tmpSize;

        if ((tmpSize >= Gr_MinBoardSize) && (tmpSize <= Gr_MaxBoardSize)) {
            break;
        }
        //if value is not valid, loop forever
    } while (1);
    HexBoard myBoard(tmpSize);

    HBPlayerElem currPlayer;
    cout << "BLUE move" << endl;
    currPlayer = HBPlayerElem::BLUE;
    srand(time(NULL));
    do {
        if (myPlayer == currPlayer) {
            cout << "Player move " << endl;
            cout << endl;
            cout << "Enter i: ";
            cin >> tempi;
            cout << endl;
            cout << "Enter j: ";
            cin >> tempj;
        }
        else {
            cout << "Computer move " << endl;

            //DEBUG, REMOVE!!!
            auto nodeNextMove = myBoard.ComputeBestMove(currPlayer);
            if (nodeNextMove != -1) {
                tempi = nodeNextMove / myBoard.GetSize();
                tempj = nodeNextMove % myBoard.GetSize();
            }
            else {
                cout << "WTF????????" << endl;
            }
        }

        if (myBoard.PlayerMove(currPlayer, tempi, tempj) == true) {
            //switch player
            if (currPlayer == HBPlayerElem::BLUE) {
                currPlayer = HBPlayerElem::RED;
            }
            else {
                currPlayer = HBPlayerElem::BLUE;
            }
            myBoard.PrintValues();

            if (myBoard.GetWinner() == HBPlayerElem::BLUE) {
                cout << endl << "BLUE WINS!" << endl;
                break;
            }
            else if (myBoard.GetWinner() == HBPlayerElem::RED) {
                cout << endl << "RED WINS!" << endl;
                break;
            }

        }
        else {
            if (myPlayer == currPlayer) {
                cout << "Wrong selection! Place already taken" << endl;
            }
        }

    } while (1);

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
    remainingMoves = this->size * this->size;
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
    cout << endl;
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

void HexBoard::SetElement(int node, HBPlayerElem edgeValue) {
    int i, j;
    
    i = node / (this->size);
    j = node % (this->size);
    graphMatrix[i][j] = edgeValue;
}

bool HexBoard::PlayerMove(HBPlayerElem player, int i, int j) {
    if (graphMatrix[i][j] != HBPlayerElem::EMPTY) {
        return false; //BAD MOVE!
    }

    SetElement(i, j, player);

    //verify if same color on the left
    if (j > 0) {
        if (graphMatrix[i][j - 1] == player) {
            AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i, j - 1));
        }
    }
    //verify if same color on the right
    if ((j + 1) < this->size) {
        if (graphMatrix[i][j + 1] == player) {
            AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i, j + 1));
        }
    }
    //verify if same color on top
    if (i > 0) {
        //top left...
        if (graphMatrix[i - 1][j] == player) {
            AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i - 1, j));
        }
        //top right
        if ((j + 1) < (this->size)) {
            if (graphMatrix[i - 1][j + 1] == player) {
                AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i - 1, j + 1));
            }
        }
    }
    //verify if same color on bottom
    if ((i + 1) < (this->size)) {
        //bottom right...
        if (graphMatrix[i + 1][j] == player) {
            AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i + 1, j));
        }
        //bottom left
        if (j > 0) {
            if (graphMatrix[i + 1][j - 1] == player) {
                AddToPlayerGraph(player, GetNodeId(i, j), GetNodeId(i + 1, j - 1));
            }
        }
    }

    std::vector<EdgesElement>* lGraphRef = nullptr;
    if (player == HBPlayerElem::BLUE) {
        lGraphRef = &(playerGraph[0]);
    }
    else {
        lGraphRef = &(playerGraph[1]);
    }
    JarnikPrimMST  localMst(*lGraphRef, (this->size) * (this->size));
    localMst.ComputeMST(GetNodeId(i, j));


    if (PlayerWin(player, localMst.mstGraph)) {
        winner = player;
    }

    PrintPlayerGraph(player);

    return true;
}


void HexBoard::ResetPlayerGraph(HBPlayerElem player) {
    if (player == HBPlayerElem::BLUE) {
        playerGraph[0].resize(0);
    }
    if (player == HBPlayerElem::RED) {
        playerGraph[1].resize(0);
    }
}

int HexBoard::GetNodeId(int nodeA_x, int nodeA_y) {
    return (nodeA_y + (this->size) * nodeA_x);
}

void HexBoard::AddToPlayerGraph(HBPlayerElem player, int nodeA, int nodeB) {

    EdgesElement localElement;
    localElement.nodeA = nodeA;
    localElement.nodeB = nodeB;
    localElement.edgeCost = 1; //constant cost

    if (player == HBPlayerElem::BLUE) {
        playerGraph[0].push_back(localElement);
    }
    if (player == HBPlayerElem::RED) {
        playerGraph[1].push_back(localElement);
    }
}

void HexBoard::PrintPlayerGraph(HBPlayerElem player) {
    cout << endl;
    if (player == HBPlayerElem::BLUE) {
        for (auto& i : playerGraph[0])
            cout << i.nodeA << " " << i.nodeB << endl;
    }
    if (player == HBPlayerElem::RED) {
        for (auto& i : playerGraph[1])
            cout << i.nodeA << " " << i.nodeB << endl;
    }
}

bool HexBoard::PlayerWin(HBPlayerElem player, std::vector<EdgesElement> graph) {
    bool graphContainsStart = false;
    bool graphContainsEnd = false;

    if (player == HBPlayerElem::RED) {
        for (auto& el : graph) {
            if ((el.nodeA < this->size) || (el.nodeB < this->size)) { //if first ROW
                //contains START!
                graphContainsStart = true;
            }
            if ((el.nodeA >= (GetNumOfNodes() - this->size)) || (el.nodeB >= (GetNumOfNodes() - this->size))) {  //if last row
                //contains END!
                graphContainsEnd = true;
            }
        }
    }
    if (player == HBPlayerElem::BLUE) {
        for (auto& el : graph) {
            if (((el.nodeA % this->size) == 0) || (el.nodeB % this->size) == 0) { //if first COLUMN
                //contains START!
                graphContainsStart = true;
            }
            if (((el.nodeA % this->size) == (this->size - 1)) || (el.nodeB % this->size) == (this->size - 1)) { //if last COLUMN
                //contains END!
                graphContainsEnd = true;
            }
        }
    }

    return graphContainsStart && graphContainsEnd;
}

int HexBoard::ComputeBestMove(HBPlayerElem player) {
    std::vector<int> freeNodesList = {};
    std::vector<nodeMeritPair> freeNodeMerit;

    //init vectors used for computation - used also to restore back hex matrix
    int tmpNode = 0;
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            if (graphMatrix[outerIter][iter] == HBPlayerElem::EMPTY) {
                tmpNode = GetNodeId(outerIter, iter);
                freeNodesList.push_back(tmpNode); //vector to be shuffled
                freeNodeMerit.push_back(nodeMeritPair(tmpNode, 0));  //init all nodes with a merti value of 0
            }
        }
    }

    HBPlayerElem tempNext = player;

    //shuffle missing places...
    std::random_device rd;
    std::mt19937 g(rd());

    int trials = 10;
    do {
        std::shuffle(freeNodesList.begin(), freeNodesList.end(), g);
        //alternate blue and red, starting from who's next, with random nodes
        for (int it = 0; it < freeNodesList.size(); it++) {
            SetElement(freeNodesList[it], tempNext);
            //toggle player
            if (tempNext == HBPlayerElem::BLUE) {
                tempNext = HBPlayerElem::RED;
            }
            else {
                tempNext = HBPlayerElem::BLUE;
            }
        }

        //DEBUG - print shuffled vector placed over matrix, to fill it
        PrintValues();

        if (EvaluateWin() == player) {
            for (auto& nod : freeNodeMerit) {
                nod.second++;
            }
        }
        trials--;
    } while (trials > 0);


    int max = 0;
    int bestMoveNode = -1;
    for (auto nod : freeNodeMerit) {
        if (nod.second > max) {
            bestMoveNode = nod.first;
        }
    }


    //Restore original matrix - set empty positions
    for (int it = 0; it < freeNodesList.size(); it++) {
        SetElement(freeNodesList[it], HBPlayerElem::EMPTY);
    }
    PrintValues();

    return bestMoveNode;
}

HBPlayerElem HexBoard::EvaluateWin() {
    //evaluate if RED wins...

    //create a temporary matrix
    
    std::vector< std::vector<int>> tempMat;
    tempMat.resize(size);
    for (int iter = 0; iter < size; iter++) {
        tempMat[iter].resize(size);
    }

    //copy game matrix in temp matrix
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            if (graphMatrix[outerIter][iter] == HBPlayerElem::RED) {
                tempMat[outerIter][iter] = 0;  //0 means RED element, to be checked
            } else {
                tempMat[outerIter][iter] = -1; //-1 means not RED
            }
        }
    }



    //create an 
    for (int colIter = 0; colIter < size; colIter++) {
        if (tempMat[0][colIter] == 0) {  //check first row for RED chips
            tempMat[0][colIter] = colIter;
            MarkNeighbors(tempMat, 0, colIter);
        }
    }

    bool isWinning = false;
    for (int colIter = 0; colIter < size; colIter++) {
        if (tempMat[size - 1][colIter] > 0) {
            cout << "RED WIN" << endl;
            isWinning = true;
        }
    }
    //DEBUG!!!!!!!!!!!!!!!!!!!!
    string offset = "  ";
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        cout << endl;
        //plot ". - . - . - "
        for (int lOff = 0; lOff < outerIter; lOff++) {
            cout << offset;
        }
        cout << " ";
        cout << (tempMat[outerIter][0] > 0 ? tempMat[outerIter][0] : 0);
        for (int iter = 1; iter < tempMat.size(); iter++) {
            cout << " - " << (tempMat[outerIter][iter] > 0 ? tempMat[outerIter][iter] : 0);
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
    cout << endl;

    //print marked matrix
    if (isWinning == true) {
        return HBPlayerElem::RED;
    }  
    cout << " BLUE WIN" << endl;
    return HBPlayerElem::BLUE;
}

void HexBoard::MarkNeighbors(std::vector< std::vector<int>> & matrix, int row, int col) {

    if (row == 0 && col == 0) {
        cout << "eval 00";
    }
    //if top left element is zero, and neighboring, and not checked yet
    if ((row > 0) && (matrix[row - 1][col] == 0)) {
        matrix[row - 1][col] = matrix[row][col]; //mark it with same code...
        MarkNeighbors(matrix, row - 1, col);
    }
    //if top right element is zero, and neighboring, and not checked yet
    if ((row > 0) && ((col + 1) < matrix.size()) && (matrix[row - 1][col+1] == 0)) {
        matrix[row - 1][col + 1] = matrix[row][col]; //mark it with same code...
        MarkNeighbors(matrix, row - 1, col + 1);
    }
    //if left element....
    if ((col > 0) && (matrix[row][col - 1] == 0)) {
        matrix[row][col - 1] = matrix[row][col]; //mark it with same code...
        MarkNeighbors(matrix, row, col - 1);
    }
    //if right element....
    if (((col+1) < matrix.size()) && (matrix[row][col + 1] == 0)) {
        matrix[row][col + 1] = matrix[row][col]; //mark it with same code...
        MarkNeighbors(matrix, row, col + 1);
    }
    //if bottom right ...
    if (((row + 1) < matrix.size()) && (matrix[row+1][col] == 0)) {
        matrix[row+1][col] = matrix[row][col]; //mark it with same code...
        MarkNeighbors(matrix, row+1, col);
    }
    //if bottom left...
    if (((row + 1) < matrix.size()) && (col > 0) && (matrix[row + 1][col-1] == 0)) {
        matrix[row + 1][col-1] = matrix[row][col]; //mark it with same code...
        MarkNeighbors(matrix, row + 1, col-1);
    }

}

JarnikPrimMST::JarnikPrimMST(std::vector<EdgesElement>& fullGraphRef, int numOfNodes) : fullGraph(fullGraphRef) {
    this->SetGraphSize(numOfNodes);
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
        }
        else {
            return Gr_Infinity; //no MST found!
        }
    } while (nodesReached.size() < GetGraphSize());

    EdgesType totalMstCost = 0;
    for (auto& el : mstGraph) {
        totalMstCost += el.edgeCost;
    }
    return totalMstCost;
}
