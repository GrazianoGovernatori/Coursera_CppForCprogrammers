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
    EdgesElement() :nodeA(0), nodeB(0), edgeCost{1} {}
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
    HBPlayerElem winner = HBPlayerElem::EMPTY;

public:
    void ResetBoard(void);
    int GetSize() { return this->size; }
    void PrintValues(bool printParenthesis = true);
    string SymbolToDisplay(HBPlayerElem lElement);

    void SetElement(int i, int j, HBPlayerElem edgeValue);
    void SetElement(int node, HBPlayerElem edgeValue);
    HBPlayerElem GetElement(int node);
    bool PlayerMove(HBPlayerElem player, int i, int j);

    int GetNodeId(int nodaA_x, int nodeA_y);
    bool PlayerWin(HBPlayerElem player, std::vector<EdgesElement> graph);
    int GetNumOfNodes(void) { return size * size; }

    //AI-related
    int ComputeBestMove(HBPlayerElem player);
    bool EvaluateWin(HBPlayerElem player);  //alternative method to compute winner
    void MarkNeighbors(std::vector< std::vector<int>>& matrix, int row, int col);
};

//-------------------------------------------------------------------------------------------------
// * * * program main * * * //
//-------------------------------------------------------------------------------------------------
int main() {

    char myChoice;
    HBPlayerElem myPlayer;
    int tempi, tempj;

    //choose player color
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

    //choose board size
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

            if (tempi >= myBoard.GetSize()) {
                cout << "Error, x coordinate clamped!" << endl;
                tempi = myBoard.GetSize() - 1;
            }
            if (tempj >= myBoard.GetSize()) {
                cout << "Error, y coordinate clamped!" << endl;
                tempj = myBoard.GetSize() - 1;
            }
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
                return 1;
            }
        }

        if (myBoard.PlayerMove(currPlayer, tempi, tempj) == true) {

            if (myBoard.EvaluateWin(currPlayer)) {
                if (currPlayer == HBPlayerElem::BLUE) {
                    cout << "BLUE WINS!!!" << endl;
                } else {
                    cout << "RED WINS!!!" << endl;
                }
                break;
            }
            //switch player
            if (currPlayer == HBPlayerElem::BLUE) {
                currPlayer = HBPlayerElem::RED;
            }
            else {
                currPlayer = HBPlayerElem::BLUE;
            }
            myBoard.PrintValues();
        }
        else {
            if (myPlayer == currPlayer) {
                cout << "Wrong selection! Place already taken" << endl;
            }
        }

    } while (1);

    cout << endl;
    cout << "Program end!" << endl;
    int fkvar;
    cin >> fkvar;
    return 0;
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

HBPlayerElem HexBoard::GetElement(int node) {
    int i, j;

    i = node / (this->size);
    j = node % (this->size);
    return graphMatrix[i][j];
}


bool HexBoard::PlayerMove(HBPlayerElem player, int i, int j) {
    if (graphMatrix[i][j] != HBPlayerElem::EMPTY) {
        return false; //BAD MOVE!
    }

    SetElement(i, j, player);

    return true;
}


int HexBoard::GetNodeId(int nodeA_x, int nodeA_y) {
    return (nodeA_y + (this->size) * nodeA_x);
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

    int trials = 100;
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
        //cout << endl << "trial: " << trials << endl;
        //PrintValues();

        if (EvaluateWin(player) == true) {
            for (auto& nod : freeNodeMerit) {
                if (GetElement(nod.first) == player) {
                    nod.second++;
                }
                
            }
        }
        trials--;
    } while (trials > 0);


    //for (auto nodM : freeNodeMerit) {
    //    cout << "node " << nodM.first << ",     merit: " << nodM.second << endl;
    //}

    int max = 0;
    int bestMoveNode = -1;
    for (auto nod : freeNodeMerit) {
        if (nod.second > max) {
            max = nod.second;
            bestMoveNode = nod.first;
        }
    }


    //Restore original matrix - set empty positions
    for (int it = 0; it < freeNodesList.size(); it++) {
        SetElement(freeNodesList[it], HBPlayerElem::EMPTY);
    }
    //PrintValues();

    return bestMoveNode;
}

bool HexBoard::EvaluateWin(HBPlayerElem player) {

    std::vector< std::vector<int>> tempMat;
    tempMat.resize(size);
    for (int iter = 0; iter < size; iter++) {
        tempMat[iter].resize(size);
    }

    //copy game matrix in temp matrix
    for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
        for (int iter = 0; iter < graphMatrix.size(); iter++) {
            if (graphMatrix[outerIter][iter] == player) {
                tempMat[outerIter][iter] = 0;  //0 means element of player type, to be checked
            } else {
                tempMat[outerIter][iter] = -1; //-1 means not RED
            }
        }
    }
    //create an 
    if (player == HBPlayerElem::RED) {
        for (int colIter = 0; colIter < size; colIter++) {
            if (tempMat[0][colIter] == 0) {  //check first row for RED chips
                tempMat[0][colIter] = colIter+1;  //to avoid assigning "0"
                MarkNeighbors(tempMat, 0, colIter);
            }
        }
    }  else { // check BLUE, check first column
        for (int rowIter = 0; rowIter < size; rowIter++) {
            if (tempMat[rowIter][0] == 0) {  //check first row for RED chips
                tempMat[rowIter][0] = rowIter+1;
                MarkNeighbors(tempMat, 0, rowIter);
            }
        }
    }

    bool isWinning = false;
    if (player == HBPlayerElem::RED) {
        for (int colIter = 0; colIter < size; colIter++) {
            if (tempMat[size - 1][colIter] > 0) {
                //cout << "RED WIN" << endl;
                isWinning = true;
            }
        }
    } else {
        for (int rowIter = 0; rowIter < size; rowIter++) {
            if (tempMat[rowIter][size - 1] > 0) {
                //cout << "BLUE WIN" << endl;
                isWinning = true;
            }
        }

    }
    //DEBUG!!!!!!!!!!!!!!!!!!!!
    //string offset = "  ";
    //for (int outerIter = 0; outerIter < graphMatrix.size(); outerIter++) {
    //    cout << endl;
    //    //plot ". - . - . - "
    //    for (int lOff = 0; lOff < outerIter; lOff++) {
    //        cout << offset;
    //    }
    //    cout << " ";
    //    cout << (tempMat[outerIter][0] > 0 ? tempMat[outerIter][0] : 0);
    //    for (int iter = 1; iter < tempMat.size(); iter++) {
    //        cout << " - " << (tempMat[outerIter][iter] > 0 ? tempMat[outerIter][iter] : 0);
    //    }
    //    cout << endl;
    //    cout << offset;
    //    //plot " \  /  \  / ..."
    //    for (int lOff = 0; lOff < outerIter; lOff++) {
    //        cout << offset;
    //    }
    //    if (outerIter < (graphMatrix.size() - 1)) {
    //        cout << "\\ ";
    //        for (int iter = 1; iter < graphMatrix.size(); iter++) {
    //            cout << "/ \\ ";
    //        }
    //    }
    //}
    //cout << endl;

    //print marked matrix
    return isWinning;
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
