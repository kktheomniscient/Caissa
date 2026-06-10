#include "json.hpp"
#include <fstream>
#include <string>
#include <iostream>
#include <windows.h> 
#include <cctype>
#include <climits>
using json = nlohmann::json;
using namespace std;

struct chess_move{
    char piece, captured;
    int source, target;

    chess_move(int s, int t, char p, char c)
        : source(s), target(t), piece(p), captured(c) {}
};

int INF = 10000;

class chess{
    private:
        json data;
        int side;
        string board;
        int gBestSource;
        int gBestTarget;
        vector<chess_move> move_history;

    public:
        chess(){
            ifstream file("settings.json");
            file >> data;
            size_t pos = data.value("fen", "").find(" ");
            if(data.value("fen","")[pos+1] == 'w')
                side = 0;
            else
                side = 1;
            gBestSource = -1;
            gBestTarget = -1;
            move_history.emplace_back(0, 20, 'p', 'P');
        }

        string getFEN(){
            return data.value("fen", "");
        }

        vector<int> getPieceDir(const string &piece){
            if(data["directions"].contains(piece)){
                return data["directions"][piece].get<vector<int>>();
            }
            return {};
        }

        int getColor(const string &piece){
            if (data["colors"].contains(piece)) {
                return data["colors"][piece].get<int>();
            }
            return -1;
        }

        int getPieceWt(const string &piece){
            if (data["weights"].contains(piece)) {
                return data["weights"][piece].get<int>();
            }
            return 0;
        }

        string getPrettyPiece(string &piece){
            if(data["pieces"].contains(piece)){
                return data["pieces"][piece];
            }
            return "";
        }

        //god knows what this is
        // void makeBoard(){
        //     string rawBoard = "         \n         \n " + getFEN();
        //     //testing
        //     size_t pos = 0;
        //     while((pos = rawBoard.find("/",pos)) != string::npos){
        //         rawBoard.replace(pos, 1, "\n ");
        //         pos++;
        //     }
        //     pos = 0;
        //     while((pos = rawBoard.find("8",pos)) != string::npos){
        //         rawBoard.replace(pos, 1, "........");
        //         pos++;
        //     }
        //     for (int i = 0; i < 11;i++){
        //         rawBoard.pop_back();
        //     }
        //     rawBoard.append("\n         \n         \n");
        //     board = rawBoard;
        //     cout << board;
        // }

        void makeBoard() {
            string fen = getFEN();
            size_t spacePos = fen.find(' ');
            if (spacePos != string::npos)
                fen = fen.substr(0, spacePos);

            string rawBoard;
            rawBoard.append(string("         \n") + string("         \n"));

            rawBoard.append(" ");

            for (char c : fen) {
                if (isdigit(c)) {
                    rawBoard.append(string(c - '0', '.')); // e.g. '3' → "..."
                } else if (c == '/') {
                    rawBoard.append("\n "); // newline + leading space
                } else {
                    rawBoard.push_back(c);  // actual piece char
                }
            }
            rawBoard.append("\n");
            rawBoard.append(string("         \n") + string("         \n"));

            board = rawBoard;
        }

        string prettyBoard(){
            string newBoard;
            for (int i = 0; i < board.size();i++){
                string temp(1, board[i]);
                newBoard.append(" " + getPrettyPiece(temp));
            }
            return newBoard;
        }

        vector<int> getRankVec(int rank){
            string s = "rank_" + to_string(rank);
            return data[s];
        }

        int getPST(int Sqr){
            vector<int> pst = data["pst"];
            return pst[Sqr];
        }

        string getCoordinates(int Sqr){
            vector<string> s = data["coordinates"];
            return s[Sqr];
        }

        vector<string> getCoordinatesVec(){
            return data["coordinates"];
        }

        vector<chess_move> genMoves()
        {
            // cout << board;
            vector<char> avoid = {' ', '.', '\n'};
            vector<char> breaker = {' ', '\n'};
            vector<char> leaper = {'k', 'K', 'n', 'N', 'p', 'P'};
            vector<char> pawns = {'p', 'P'};
            vector<char> kings = {'k', 'K'};
            vector<int> diagPawnMoves = {9, 11, -9, -11};
            vector<int> verticalPawnMoves = {10, 20, -10, -20};
            vector<chess_move> move_list;
            for (int Sqr = 0; Sqr < board.size();Sqr++){
                char piece = board[Sqr];
                if((find(avoid.begin(),avoid.end(),piece) == avoid.end()) && (getColor(string(1,piece)) == side)){
                    vector<int> offsets = getPieceDir(string(1,piece));
                    for(int offset:offsets){
                        int targetSqr = Sqr;
                        while(1){
                            targetSqr += offset;
                            if (targetSqr < 0 || targetSqr >= (int)board.size())
                                break;
                            char capturedPiece = board[targetSqr];
                            if(find(breaker.begin(),breaker.end(),capturedPiece) != breaker.end())
                                break;
                            if(side == getColor(string(1,capturedPiece)))
                                break;
                            if((find(pawns.begin(),pawns.end(),piece) != pawns.end())
                            && (capturedPiece == '.')
                            && (find(diagPawnMoves.begin(),diagPawnMoves.end(),offset) != diagPawnMoves.end()))
                            {
                                break;
                                if(!double_push())
                                    break;
                                // need to figure out best way to implement en passent not only to do it
                                // but also to be able to take back that move
                            }
                                
                
                            if((find(pawns.begin(),pawns.end(),piece) != pawns.end())
                            && (capturedPiece != '.')
                            && (find(verticalPawnMoves.begin(),verticalPawnMoves.end(),offset) != verticalPawnMoves.end()))
                                break;
                            if(piece == 'P' && offset == -20){
                                vector<int>  rank_2 = getRankVec(2);
                                if(find(rank_2.begin(),rank_2.end(),Sqr) == rank_2.end())
                                    break;
                                if(board[Sqr - 10] != '.')
                                    break;
                            }
                            if(piece == 'p' && offset == 20 && board[Sqr + 10] != '.'){
                                vector<int> rank_7 = getRankVec(7);
                                if(find(rank_7.begin(),rank_7.end(),Sqr) == rank_7.end())
                                    break;
                                if(board[Sqr + 10] != '.')
                                    break;
                            }
                            if(find(kings.begin(),kings.end(),capturedPiece) != kings.end())
                                return {};
                            move_list.emplace_back(Sqr, targetSqr, piece, capturedPiece);

                            // string temp;
                            // makeMove(move_list[move_list.size() - 1]);
                            // cout << prettyBoard();
                            // cin >> temp;
                            // takeBack(move_list[move_list.size() - 1]);
                            // cout << prettyBoard();
                            // cin >> temp;

                            if(getColor(string(1,capturedPiece)) == (side^1))
                                break;
                            if(find(leaper.begin(),leaper.end(),piece) != leaper.end())
                                break;
                        }
                    }
                }
            }
            return move_list;
        }

        void makeMove(chess_move move)
        {
            board[move.target] = move.piece;
            board[move.source] = '.';

            vector<int> rank_2 = getRankVec(2);
            vector<int> rank_7 = getRankVec(7);
            if(move.piece == 'p' && (find(rank_2.begin(),rank_2.end(),move.source) != rank_2.end()))
                board[move.target] = 'q';
            if(move.piece == 'P' && (find(rank_7.begin(),rank_7.end(),move.source) != rank_7.end()))
                board[move.target] = 'Q';
            
            // cout << prettyBoard();
            side ^= 1;
            char temp;
            // cin >> temp;

            move_history.emplace_back(move);
        }

        void takeBack(chess_move move)
        {
            board[move.target] = move.captured;
            board[move.source] = move.piece;

            // cout << prettyBoard();
            side ^= 1;
            char temp;
            // cin >> temp;

            move_history.pop_back();
        }

        int eval()
        {
            int score = 0;
            vector<char> nonPiece = {' ', '.', '\n'};
            for (int Sqr = 0; Sqr < board.size();Sqr++)
            {
                char piece = board[Sqr];
                if(find(nonPiece.begin(),nonPiece.end(),piece) == nonPiece.end())
                {
                    score += getPieceWt(string(1,piece));
                    if(isupper(piece))
                        score += getPST(Sqr);
                    if(islower(piece))
                        score -= getPST(Sqr);
                }
            }

                if (side)
                    return -score;
                else
                    return score;
        }

        int search(int depth)
        {
            if(depth == 0)
                return eval();
            int bestScore = INT_MIN;
            int bestSource = -1, bestTarget = -1;
            vector<chess_move> moves = genMoves();
            if(moves.empty())
                return INF;
            for(chess_move move:moves)
            {
                makeMove(move);
                int score = -search(depth - 1);
                takeBack(move);
                if(score > bestScore)
                {
                    bestScore = score;
                    bestSource = move.source;
                    bestTarget = move.target;
                }
            }
            gBestSource = bestSource;
            gBestTarget = bestTarget;
            // cout << gBestSource;
            return bestScore;
        }

        string getBestSource(){
            return getCoordinates(gBestSource);
        }

        string getBestTarget(){
            return getCoordinates(gBestTarget);
        }

        bool double_push(){
            chess_move move = move_history[move_history.size() - 1];
            cout << '\n'<< abs(move.source - move.target) << '\n';
            return abs(move.source - move.target) == 20 ? 1 : 0;
        }

        void play(){
            cout << prettyBoard();
            while(1)
            {
                cout << "   your move: ";
                string userInput;
                cin >> userInput;
                if(userInput.size() < 4)
                    continue;

                vector<string> coordinates = getCoordinatesVec();

                // god forbid i need to debug this later on
                int userSrc = distance(coordinates.begin(), find(coordinates.begin(), coordinates.end(), userInput.substr(0, 2)));
                int userTrgt = distance(coordinates.begin(), find(coordinates.begin(), coordinates.end(), userInput.substr(2, 2)));

                chess_move userMove{userSrc,userTrgt,board[userSrc],board[userTrgt]};
                makeMove(userMove);
                cout << prettyBoard();

                int score = search(3);
                chess_move move{gBestSource, gBestTarget, board[gBestSource], board[gBestTarget]};
                makeMove(move);
                cout << prettyBoard();
                if(abs(score) == INF){
                    cout << "  checkmated!!";
                    break;
                }
            }
        }

};

int main(){
    SetConsoleOutputCP(CP_UTF8);
    chess obj = chess();
    obj.makeBoard();
    obj.play();
}
