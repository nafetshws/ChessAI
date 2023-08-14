#include <string>
#include <iostream>
#include "../include/functions.hpp"
#include "../include/types.hpp"


std::string convertSquareToCoordinate(Square square) {
    int file = square % 8; //0-7
    int row = square / 8; //0-7

    std::string coordinate = "";
    switch(file) {
        case 0:
            coordinate += 'a';
            break;
        case 1:
            coordinate += 'b';
            break;
        case 2:
            coordinate += 'c';
            break;
        case 3:
            coordinate += 'd';
            break;
        case 4:
            coordinate += 'e';
            break;
        case 5:
            coordinate += 'f';
            break;
        case 6:
            coordinate += 'g';
            break;
        case 7:
            coordinate += 'h';
            break;
        default:
            return "error";
    }
    coordinate += std::to_string(row+1);
    return coordinate;
}

Color getOppositeColor(Color color) {
    if(color == WHITE) return BLACK;
    return WHITE;
}

void printMoves(std::vector<Move> moves, int max){
    int i = 0;
    while(i < moves.size() && i < max) {
        std::cout << convertSquareToCoordinate(moves.at(i).origin) << convertSquareToCoordinate(moves.at(i).destination) << std::endl;
        i++;
    }
}

Bitboard squareToBitboard(Square square) {
    return 1ULL << square;
}

Square bitboardToSquare(Bitboard b) {
    return __builtin_ctzll(b);
}

std::vector<Square> convertBitboardToSquares(Bitboard bitboard) {
    std::vector<Square> squares;
    while(bitboard != 0){
        Square index = __builtin_ctzll(bitboard);
        squares.push_back(index);
        //remove index from bitboard
        bitboard = (~squareToBitboard(index)) & bitboard;
    }
    return squares;
}

void convertBitbaordToMoves(Bitboard intersect, Bitboard destination, PieceType pieceType, Color color, MoveType moveType, int &numberOfAttacks, Attack_Info &attack_info){
    if(intersect != 0) {
        std::vector<Square> origins = convertBitboardToSquares(intersect);
        for(int i = 0; i < origins.size(); i++) {
            Move move(origins.at(i), (Square)__builtin_ctzll(destination), pieceType, color, moveType);
            numberOfAttacks++;
            attack_info.moves.push_back(move);
        }
    }
}
