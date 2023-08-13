#ifndef TYPES_H
#define TYPES_H

#include <string>
#include <vector>

typedef unsigned long long Bitboard;
typedef int Square;

enum Color {
    WHITE,
    BLACK
};

enum PieceType {
    PAWN,
    ROOK,
    KNIGHT,
    BISHOP,
    QUEEN,
    KING,
    UNOCCUPIED
};

enum MoveType {
    QUIET,
    PROMOTION,
    CAPTURE,
    EN_PASSENT_CAPTURE,
    PAWN_PUSH,
    NULL_MOVE,
    CASTLING
};

struct Piece {
    Square pos;
    PieceType type;
    Color color;

    Piece(Square _pos, PieceType _type, Color _color) : pos(_pos),type(_type),color(_color) {}
};

struct Move {
    Move(Square _origin, Square _destination, PieceType _pieceType, Color _color, MoveType _moveType) : origin(_origin), destination(_destination), pieceType(_pieceType), color(_color), moveType(_moveType) {}

    Square origin;
    Square destination;
    PieceType pieceType;
    Color color;
    MoveType moveType;
};

struct Attack_Info {
    int numberOfAttacks;
    std::vector<Move> moves;

    Attack_Info() : numberOfAttacks(0), moves() {}
    Attack_Info(int _numberOfAttacks, std::vector<Move> _moves) : numberOfAttacks(_numberOfAttacks), moves(_moves) {}
};

struct Check_Info {
    int numberOfChecks;
    std::vector<Move> moves;

    Check_Info() : numberOfChecks(0), moves() {}
    Check_Info(int _numberOfChecks, std::vector<Move> _moves) : numberOfChecks(_numberOfChecks), moves(_moves) {}

    Check_Info(Attack_Info a_info) {
        numberOfChecks = a_info.numberOfAttacks;
        moves = a_info.moves;
    }
};

enum DirectionValues {
    NORTH = 8,
    NORTH_EAST = 9,
    NORTH_WEST = 7,

    SOUTH = 8,
    SOUTH_EAST = 7,
    SOUTH_WEST = 9,

    EAST = 1,
    WEST = 1,
    
    //Knight
    NORTH_NORTH_EAST = 17,
    NORTH_EAST_EAST = 10,
    SOUTH_EAST_EAST = 6,
    SOUTH_SOUTH_EAST = 15,
    SOUTH_SOUTH_WEST = 17,
    SOUTH_WEST_WEST = 10,
    NORTH_WEST_WEST = 6,
    NORTH_NORTH_WEST = 15

};

enum Direction {
    N, NE, NW, S, SE, SW, E, W, NNE, NEE, SEE, SSE, SSW, SWW, NWW, NNW
};

struct Pin {
    PieceType pinnedPieceType;
    PieceType pinnerType;
    Bitboard pinnedPiece;
    Direction direction;

    Pin(PieceType _pinnedPieceType, PieceType _pinnerType, Bitboard _pinnedPiece, Direction _direction)
        : pinnedPieceType(_pinnedPieceType), pinnerType(_pinnerType), pinnedPiece(_pinnedPiece), direction(_direction) {}
};

struct Pins {
    std::vector<Pin> pins;
    Bitboard absolutePins;
};

const Bitboard FILE_A = 0x101010101010101;
const Bitboard FILE_B = 0x202020202020202;
const Bitboard FILE_G = 0x4040404040404040;
const Bitboard FILE_H = 0x8080808080808080;
const Bitboard EMPTY = 0x0;
const Bitboard RANK_1 = 0xff;
const Bitboard RANK_2 = 0xff00;
const Bitboard RANK_5 = 0xff00000000;
const Bitboard RANK_7 = 0xff000000000000;
const Bitboard RANK_8 = 0xff00000000000000;

const Bitboard WHITE_CASTLE_KING_MASK = 0x60;
const Bitboard WHITE_CASTLE_QUEEN_MASK= 0x60;
const Bitboard BLACK_CASTLE_KING_MASK = 0x6000000000000000;
const Bitboard BLACK_CASTLE_QUEEN_MASK= 0xe00000000000000;

const std::string DEFAULT_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR w KQkq - 0 1";

const unsigned long long CURRENT_POSITION = 0ULL-1;


#endif