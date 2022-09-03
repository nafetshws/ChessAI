#include "MoveGeneration.hpp"
#include "board.hpp"

MoveGeneration::MoveGeneration(Board board) {
    this->board = board;
}

Bitboard MoveGeneration::North(Square square) {
    int rank = square / 8;
    Bitboard attacks = EMPTY;

    if(rank == 7) return attacks;

    attacks = (1 << (square + Direction::NORTH));
    rank++;

    while(rank < 7) {
        attacks = attacks | (attacks << Direction::NORTH); 
        rank++;
    }

    return attacks;
}

Bitboard MoveGeneration::Northeast(Square square) {
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = EMPTY;

    if(rank == 7 || file == 7) return attacks;

    attacks = (1 << (square + Direction::NORTH_EAST));
    rank++;
    file++;

    while(rank < 7 && file < 7) {
        attacks = attacks | (attacks << Direction::NORTH_EAST); 
        rank++;
        file++;
    }

    return attacks;
}

Bitboard MoveGeneration::Northwest(Square square) {
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = EMPTY;

    if(rank == 7 || file == 0) return attacks;

    attacks = (1UL << (square + Direction::NORTH_WEST));
    rank++;
    file--;

    while(rank < 7 && file > 0) {
        attacks = attacks | (attacks << Direction::NORTH_WEST); 
        rank++;
        file--;
    }

    return attacks;
}


Bitboard MoveGeneration::South(Square square) {
    int rank = square / 8;
    Bitboard attacks = EMPTY;

    if(rank == 0) return attacks;

    attacks = (1UL << (square - Direction::SOUTH));
    rank--;

    while(rank > 0) {
        attacks = attacks | (attacks >> Direction::SOUTH); 
        rank--;
    }

    return attacks;
}

Bitboard MoveGeneration::Southeast(Square square) {
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = EMPTY;

    if(rank == 0 || file == 7) return attacks;

    attacks = (1UL << (square - Direction::SOUTH_EAST));
    rank--;
    file++;

    while(rank > 0 && file < 7) {
        attacks = attacks | (attacks >> Direction::SOUTH_EAST); 
        rank--;
        file++;
    }

    return attacks;
}

Bitboard MoveGeneration::Southwest(Square square) {
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = EMPTY;

    if(rank == 0 || file == 0) return attacks;

    attacks = (1UL << (square - Direction::SOUTH_WEST));
    rank--;
    file--;

    while(rank > 0 && file > 0) {
        attacks = attacks | (attacks >> Direction::SOUTH_WEST); 
        rank--;
        file--;
    }

    return attacks;
}

Bitboard MoveGeneration::West(Square square) {
    
    int file = square % 8;
    Bitboard attacks = EMPTY; 

    if(file == 0) return attacks;

    attacks = (1UL << (square - Direction::WEST));
    file--;

    while(file > 0) {
        attacks = attacks | (attacks >> Direction::WEST);
        file--;
    }

    return attacks;

}

Bitboard MoveGeneration::East(Square square) {
    int file = square % 8;
    Bitboard attacks = EMPTY; 

    if(file == 7) return attacks;

    attacks = (1UL << (square + Direction::EAST));
    file++;

    while(file < 7) {
        attacks = attacks | (attacks << Direction::EAST);
        file++;
    }

    return attacks;

}