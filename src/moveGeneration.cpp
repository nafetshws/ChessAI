#include <vector>
#include "../include/moveGeneration.hpp"
#include "../include/board.hpp"
#include "../include/types.hpp"
#include "../include/functions.hpp"

MoveGeneration::MoveGeneration(Board board) {
    this->board = board;
}

std::vector<Move> MoveGeneration::generateMoves(Color color) {
    std::vector<Move> moves;
    //TODO
    //move types are not accurate!!

    //generate all king moves
    Bitboard kingMoves = generateKingMoves(CURRENT_POSITION, color);
    Square kingOrigin = __builtin_ctzll(this->board.getKing(color));

    //transform moves from bitboard into a vector
    while(kingMoves != 0) {
        Square destination = __builtin_ctzll(kingMoves);
        moves.push_back(Move(kingOrigin, destination, PieceType::KING, color, MoveType::QUIET));
        //delete move from bitboard
        kingMoves = ~(1ULL << destination) & kingMoves;
    }

    /*
    * Generate moves by pinned pieces and append them to the pinnedPieces Bitboard
    */

    Pins pinnedPieces = getPinnedPieces(color);
    Bitboard pinnedPiecesBitboard = pinnedPieces.absolutePins;

    for(Pin pin : pinnedPieces.pins) {
        Direction direction = pin.direction;
        Square pinnedPieceOriginSquare = __builtin_ctzll(pin.pinnedPiece);

        switch(direction) {
            case Direction::NW: case Direction::SE:
                //move along pin
                if(pin.pinnedPieceType == BISHOP || pin.pinnedPieceType == QUEEN) {
                    Bitboard legalMoves = getLegalNorthwestMoves(pin.pinnedPiece, color) | getLegalSoutheastMoves(pin.pinnedPiece, color);
                    std::vector<Square> destinations = convertBitboardToSquares(legalMoves);

                    for(Square destination : destinations) {
                        MoveType moveType = (squareToBitboard(destination) & this->board.getOccupiedBy(getOppositeColor(color))) != 0 ? MoveType::CAPTURE : MoveType::QUIET;

                        moves.push_back(Move(pinnedPieceOriginSquare, destination, pin.pinnedPieceType, color, moveType));
                    }
                } else if (pin.pinnedPieceType == PAWN) {
                    //caputres only
                    if(color == BLACK && direction == Direction::SE) {
                        if(((pin.pinnedPiece >> SOUTH_EAST) & this->board.getOccupiedBy(getOppositeColor(color))) != 0) {
                            moves.push_back(Move(pinnedPieceOriginSquare, bitboardToSquare(pin.pinnedPiece >> SOUTH_EAST), PAWN, color, MoveType::CAPTURE));
                        }
                    } else if(color == WHITE && direction == Direction::NW) {
                        if(((pin.pinnedPiece << NORTH_WEST) & this->board.getOccupiedBy(getOppositeColor(color))) != 0) {
                            moves.push_back(Move(pinnedPieceOriginSquare, bitboardToSquare(pin.pinnedPiece << NORTH_WEST), PAWN, color, MoveType::CAPTURE));
                        }
                    } 
                }
                break;
            case Direction::NE: case Direction::SW:
                //move along pin
                if(pin.pinnedPieceType == BISHOP || pin.pinnedPieceType == QUEEN) {
                    Bitboard legalMoves = getLegalNortheastMoves(pin.pinnedPiece, color) | getLegalSouthwestMoves(pin.pinnedPiece, color);
                    std::vector<Square> destinations = convertBitboardToSquares(legalMoves);

                    for(Square destination : destinations) {
                        MoveType moveType = (squareToBitboard(destination) & this->board.getOccupiedBy(getOppositeColor(color))) != 0 ? MoveType::CAPTURE : MoveType::QUIET;

                        moves.push_back(Move(pinnedPieceOriginSquare, destination, pin.pinnedPieceType, color, moveType));
                    }

                } else if (pin.pinnedPieceType == PAWN) {
                    //caputres only
                    if(color == BLACK && direction == Direction::SW) {
                        if(((pin.pinnedPiece >> SOUTH_WEST) & this->board.getOccupiedBy(getOppositeColor(color))) != 0) {
                            moves.push_back(Move(pinnedPieceOriginSquare, bitboardToSquare(pin.pinnedPiece << SOUTH_WEST), PAWN, color, MoveType::CAPTURE));
                        }
                    } else if(color == WHITE && direction == Direction::NE) {
                        if(((pin.pinnedPiece << NORTH_EAST) & this->board.getOccupiedBy(getOppositeColor(color))) != 0) {
                            moves.push_back(Move(pinnedPieceOriginSquare, bitboardToSquare(pin.pinnedPiece << NORTH_EAST), PAWN, color, MoveType::CAPTURE));
                        }
                    } 
                }
                break;
            case Direction::N: case Direction::S:
                if(pin.pinnedPieceType == ROOK || pin.pinnedPieceType == QUEEN) {
                    Bitboard legalMoves = getLegalNorthMoves(pin.pinnedPiece, color) | getLegalSouthMoves(pin.pinnedPiece, color);
                    std::vector<Square> destinations = convertBitboardToSquares(legalMoves);

                    for(Square destination : destinations) {
                        MoveType moveType = (squareToBitboard(destination) & this->board.getOccupiedBy(getOppositeColor(color))) != 0 ? MoveType::CAPTURE : MoveType::QUIET;

                        moves.push_back(Move(pinnedPieceOriginSquare, destination, pin.pinnedPieceType, color, moveType));
                    }

                } else if (pin.pinnedPieceType == PAWN) {
                    //pawn pushes only
                    if(color == BLACK && direction == Direction::S) {
                        Bitboard pawnSinglePushes = (pin.pinnedPiece >> SOUTH) & (~board.getOccupied());
                        Bitboard pawnDoublePushes = ((pin.pinnedPiece & RANK_7) >> 2*SOUTH) & (~board.getOccupied());
                        Bitboard pawnMoves = pawnSinglePushes | pawnDoublePushes;

                        Square pinnedPieceOriginSquare = (pin.pinnedPiece);
                        std::vector<Square> destinations = convertBitboardToSquares(pawnMoves);

                        for(Square destination : destinations) {
                            MoveType moveType = MoveType::PAWN_PUSH; 
                            moves.push_back(Move(pinnedPieceOriginSquare, destination, pin.pinnedPieceType, color, moveType));
                        }
                    } else if(color == WHITE && direction == Direction::N) {
                        Bitboard pawnSinglePushes = (pin.pinnedPiece << NORTH) & (~board.getOccupied());
                        Bitboard pawnDoublePushes = ((pin.pinnedPiece & RANK_2) << 2*NORTH) & (~board.getOccupied());
                        Bitboard pawnMoves = pawnSinglePushes | pawnDoublePushes;

                        Square pinnedPieceOriginSquare = (pin.pinnedPiece);
                        std::vector<Square> destinations = convertBitboardToSquares(pawnMoves);

                        for(Square destination : destinations) {
                            MoveType moveType = MoveType::PAWN_PUSH; 
                            moves.push_back(Move(pinnedPieceOriginSquare, destination, pin.pinnedPieceType, color, moveType));
                        }
                    } 
                }
                break;
            case Direction::E: case Direction::W:
                if(pin.pinnedPieceType == ROOK || pin.pinnedPieceType == QUEEN) {
                    Bitboard legalMoves = getLegalEastMoves(pin.pinnedPiece, color) | getLegalWestMoves(pin.pinnedPiece, color);
                    std::vector<Square> destinations = convertBitboardToSquares(legalMoves);

                    for(Square destination : destinations) {
                        MoveType moveType = (squareToBitboard(destination) & this->board.getOccupiedBy(getOppositeColor(color))) != 0 ? MoveType::CAPTURE : MoveType::QUIET;

                        moves.push_back(Move(pinnedPieceOriginSquare, destination, pin.pinnedPieceType, color, moveType));
                    }
                } 
                break;
            default:
                break; 
        }
    }


    /*
    * Generate moves when <<color>> is in check
    */
    Check_Info check_info = isInCheck(color);

    //if the king is in double check he has to move
    if(check_info.numberOfChecks >= 2) return moves;

    //TODO: consider pinned pieces
    else if(check_info.numberOfChecks == 1) {
        //Possiblites to react when in check:
        //1. move king out of check -> already in moves
        //2. capture the checking piece
        //3. block the checking piece (only rooks, bishops and queens)

        //2
        Square checkingPiecePos = check_info.moves.at(0).origin;
        //find every move that captures the checking piece
        Attack_Info a = isUnderAttack((Square)checkingPiecePos, getOppositeColor(color));
        //add those moves to the vector
        moves.insert(moves.end(), a.moves.begin(), a.moves.end());

        //3
        PieceType checkingPieceType = check_info.moves.at(0).pieceType;
        //only rooks, bishops and queens can be blocked
        if(checkingPieceType == QUEEN || checkingPieceType == ROOK || checkingPieceType == BISHOP) { 
            Bitboard kingAsSlidingPieceMoves; 
            //there could be multiple queens on the board -> look up pos of checking piece
            Bitboard enemyMoves;
            switch(checkingPieceType) {
                case QUEEN:
                    enemyMoves = generateQueenMoves(1ULL << check_info.moves.at(0).origin, getOppositeColor(color));
                    kingAsSlidingPieceMoves = generateQueenMoves(this->board.getKing(color), color);
                    break;
                case ROOK:
                    enemyMoves = generateRookMoves(1ULL << check_info.moves.at(0).origin, getOppositeColor(color));
                    kingAsSlidingPieceMoves = generateRookMoves(this->board.getKing(color), color);
                    break;
                case BISHOP:
                    enemyMoves = generateBishopMoves(1ULL << check_info.moves.at(0).origin, getOppositeColor(color));
                    kingAsSlidingPieceMoves = generateBishopMoves(this->board.getKing(color), color);
                    break;
                default:
                    std::cout << "Error while calculating blocking pieces" << std::endl;
                    break;
            }

            //the intersection of the enemy pieces moves and the king as the sliding piece move mark the squares that can block the enemy piece
            Bitboard intersectionRay = kingAsSlidingPieceMoves & enemyMoves;

            //transform bitboard of squares to block the attacker into moves by the attacked player
            while(intersectionRay != 0) {
                Square destination = __builtin_ctzll(intersectionRay);
                Attack_Info a_info = isUnderAttack(destination, getOppositeColor(color));
                moves.insert(moves.end(), a_info.moves.begin(), a_info.moves.end());
                intersectionRay = ~(1ULL << destination) & intersectionRay;
            }
        }
        return moves;
    }

    /*
    * Generate mvoes if the king isn't in check
    */

    //generate knight moves
    Bitboard knights = this->board.getKnights(color) & (~pinnedPiecesBitboard);
    while(knights != 0) {
        Square origin = __builtin_ctzll(knights);
        Bitboard knight = 1ULL << origin; 
        Bitboard knightMoves = generateKnightMoves(knight, color);

        while(knightMoves != 0) {
            Square destination = __builtin_ctzll(knightMoves);
            moves.push_back(Move(origin, destination, PieceType::KNIGHT, color, MoveType::QUIET));
            //delete move from bitboard
            knightMoves = ~(1ULL << destination) & knightMoves;
        }
        //remove  from knights
        knights = (~knight) & knights;
    }

    //generate rook moves
    Bitboard rooks = this->board.getRooks(color) & (~pinnedPiecesBitboard);

    while(rooks != 0) {
        Square origin = __builtin_ctzll(rooks);
        Bitboard rook = 1ULL << origin; 
        Bitboard rookMoves = generateRookMoves(rook, color);

        while(rookMoves != 0) {
            Square destination = __builtin_ctzll(rookMoves);
            moves.push_back(Move(origin, destination, PieceType::ROOK, color, MoveType::QUIET));
            //delete move from bitboard
            rookMoves = ~(1ULL << destination) & rookMoves;
        }
        //remove rook from
        rooks = (~rook) & rooks;
    }

    //generate bishop moves
    Bitboard bishops = this->board.getBishops(color) & (~pinnedPiecesBitboard);
    while(bishops != 0) {
        Square origin = __builtin_ctzll(bishops);
        Bitboard bishop = 1ULL << origin; 
        Bitboard bishopMoves = generateBishopMoves(bishop, color);

        while(bishopMoves != 0) {
            Square destination = __builtin_ctzll(bishopMoves);
            moves.push_back(Move(origin, destination, PieceType::BISHOP, color, MoveType::QUIET));
            //delete move from bitboard
            bishopMoves = ~(1ULL << destination) & bishopMoves;
        }
        //remove bishop from
        bishops = (~bishop) & bishop;
    }

    //generate queen moves
    Bitboard queens = this->board.getQueens(color) &(~pinnedPiecesBitboard);
    while(queens != 0) {
        Square origin = __builtin_ctzll(queens);
        Bitboard queen = 1ULL << origin; 
        Bitboard queenMoves = generateQueenMoves(queen, color);

        while(queenMoves != 0) {
            Square destination = __builtin_ctzll(queenMoves);
            moves.push_back(Move(origin, destination, PieceType::QUEEN, color, MoveType::QUIET));
            //delete move from bitboard
            queenMoves = ~(1ULL << destination) & queenMoves;
        }
        //remove queen from
        queens = (~queen) & queens;
    }

    //generate pawn moves
    Bitboard pawns = this->board.getPawns(color) &(~pinnedPiecesBitboard);
    while(pawns != 0) {
        Square origin = __builtin_ctzll(pawns);
        Bitboard pawn = 1ULL << origin; 
        Bitboard pawnMoves = generatePawnMoves(pawn, color);

        while(pawnMoves != 0) {
            Square destination = __builtin_ctzll(pawnMoves);
            moves.push_back(Move(origin, destination, PieceType::PAWN, color, MoveType::PAWN_PUSH));
            //delete move from bitboard
            pawnMoves = ~(1ULL << destination) & pawnMoves;
        }
        //remove pawn from bitboard
        pawns = (~pawn) & pawns;
    }
        
    return moves;
}

Bitboard MoveGeneration::generateBishopMoves(Bitboard bishops, Color color) {
    if(bishops == (CURRENT_POSITION)) bishops = this->board.getBishops(color); 

    Bitboard allSinlgeMoves = EMPTY;
    Bitboard totalMoves = EMPTY;

    while(bishops > 0) {
        int index = 63 - __builtin_clzll(bishops);
        Bitboard finalNorthWestMoves = MoveGeneration::getLegalNorthwestMoves(bishops, color);
        Bitboard finalNorthEastMoves = MoveGeneration::getLegalNortheastMoves(bishops, color);
        Bitboard finalSouthWestMoves = MoveGeneration::getLegalSouthwestMoves(bishops, color);
        Bitboard finalSouthEastMoves = MoveGeneration::getLegalSoutheastMoves(bishops, color);


        allSinlgeMoves = finalNorthWestMoves | finalNorthEastMoves | finalSouthWestMoves | finalSouthEastMoves;
        totalMoves = (totalMoves | allSinlgeMoves);
        //erase bishop
        bishops = ((1ULL << index) ^ bishops);
    }
    return totalMoves;
}

Bitboard MoveGeneration::generateRookMoves(Bitboard rooks, Color color) {
    if(rooks == (CURRENT_POSITION)) rooks = this->board.getRooks(color); 

    Bitboard allSinlgeMoves = EMPTY;
    Bitboard totalMoves = EMPTY;

    while(rooks > 0) {
        Bitboard finalNorthMoves = MoveGeneration::getLegalNorthMoves(rooks, color);
        Bitboard finalSouthMoves = MoveGeneration::getLegalSouthMoves(rooks, color);
        Bitboard finalWestMoves  = MoveGeneration::getLegalWestMoves(rooks, color);
        Bitboard finalEastMoves  = MoveGeneration::getLegalEastMoves(rooks, color);

        //clzll returns the number of leading zeros
        int index = 63 - __builtin_clzll(rooks);

        allSinlgeMoves = finalNorthMoves | finalSouthMoves | finalWestMoves | finalEastMoves;
        totalMoves = (totalMoves | allSinlgeMoves);
        //erase rook
        rooks = ((1ULL << index) ^ rooks);
    }
    return totalMoves;
}

Bitboard MoveGeneration::generateQueenMoves(Bitboard queens, Color color) {
    if(queens == (CURRENT_POSITION)) queens = this->board.getQueens(color); 
    return (generateBishopMoves(queens, color) | generateRookMoves(queens, color));
}

Bitboard MoveGeneration::generatePawnAttacks(Bitboard pawns, Color color) {
    if(pawns == (CURRENT_POSITION)) pawns = this->board.getPawns(color); 
    if(color == WHITE) {
        //SouthWest
        Bitboard swAttacks = ((pawns & ~FILE_A) << NORTH_WEST); 
        //SouthEast
        Bitboard seAttacks = ((pawns & ~ FILE_H) << NORTH_EAST);

        Bitboard attacks = swAttacks | seAttacks;
        
        return attacks;
    } else {
        //SouthWest
        Bitboard swAttacks = ((pawns & ~FILE_A) >> SOUTH_WEST);
        //SouthEast
        Bitboard seAttacks = ((pawns & ~ FILE_H) >> SOUTH_EAST);

        Bitboard attacks = swAttacks | seAttacks;

        return attacks;
    }
}

Bitboard MoveGeneration::generateEnPassentMoves(Bitboard pawns, Color color) {
    if(pawns == (CURRENT_POSITION)) pawns = this->board.getPawns(color) ;
    if(color == WHITE) {
        //en passent
        bool enPassentPossible = false;
        Bitboard enPassentSquare = EMPTY;
        if(this->board.enPassentTargetSquare != -1) {
            enPassentSquare = (1ULL << this->board.enPassentTargetSquare);
            if(((((pawns & ~FILE_A) << NORTH_WEST) | ((pawns & ~ FILE_H) << NORTH_EAST)) & enPassentSquare) == enPassentSquare) {
                enPassentPossible = true;
            }
        }
        if(enPassentPossible) return enPassentSquare;
        return 0ULL;
    } else {
        //en passent
        bool enPassentPossible = false;
        Bitboard enPassentSquare = EMPTY;
        if(this->board.enPassentTargetSquare != -1) {
            enPassentSquare = (1ULL << this->board.enPassentTargetSquare);
            if(((((pawns & (~FILE_A)) >> SOUTH_WEST) | ((pawns & (~FILE_H)) >> SOUTH_EAST)) & enPassentSquare) == enPassentSquare) {
                enPassentPossible = true;
            }
        }
        if(enPassentPossible) return enPassentSquare;
        return 0ULL;
    }
}

Bitboard MoveGeneration::generatePawnMoves(Bitboard pawns, Color color) {
    if(pawns == (CURRENT_POSITION)) pawns = this->board.getPawns(color); 
    if(color == WHITE) {
        //pawn pushes
        pawns = (pawns == CURRENT_POSITION) ? this->board.whitePawns : pawns;
        //single push
        Bitboard pawnSinglePushes = (pawns << NORTH) & (~board.getOccupied());
        //double push
        Bitboard pawnDoublePushes = ((pawns & RANK_2) << 2*NORTH) & (~board.getOccupied());
        Bitboard pushes = (pawnSinglePushes | pawnDoublePushes);

        Bitboard attacks = (generatePawnAttacks(pawns, color) & this->board.getOccupiedByBlack()) | generateEnPassentMoves(pawns, color); 

        return (pushes | attacks);
    } else {
        //pawn pushes
        pawns = (pawns == CURRENT_POSITION) ? this->board.blackPawns : pawns;
        //single push
        Bitboard pawnSinglePushes = (pawns >> SOUTH) & (~board.getOccupied());
        //double push
        Bitboard pawnDoublePushes = ((pawns & RANK_7) >> 2*SOUTH) & (~board.getOccupied());
        Bitboard pushes = (pawnSinglePushes | pawnDoublePushes);

        Bitboard attacks = (generatePawnAttacks(pawns, color) & this->board.getOccupiedByWhite()) | generateEnPassentMoves(pawns, color);

        return (pushes | attacks);
    }
}

Bitboard MoveGeneration::generateKnightAttacks(Bitboard knights, Color color) {
    if(knights == (CURRENT_POSITION)) knights = (color == WHITE) ? this->board.whiteKnights : this->board.blackKnights;

    Bitboard noNoEaAttacks = ((knights & ~(RANK_7 | RANK_8 | FILE_H)) << NORTH_NORTH_EAST);
    Bitboard noEaEaAttacks = ((knights & ~(RANK_8 | FILE_G | FILE_H)) << NORTH_EAST_EAST);
    Bitboard soEaEaAttacks = ((knights & ~(RANK_1 | FILE_G | FILE_H)) >> SOUTH_EAST_EAST);
    Bitboard soSoEaAttacks = ((knights & ~(RANK_1 | RANK_2 | FILE_H)) >> SOUTH_SOUTH_EAST);
    Bitboard soSoWeAttacks = ((knights & ~(RANK_1 | RANK_2 | FILE_A)) >> SOUTH_SOUTH_WEST);
    Bitboard SoWeWeAttacks = ((knights & ~(RANK_1 | FILE_A | FILE_B)) >> SOUTH_WEST_WEST);
    Bitboard noWeWeAttacks = ((knights & ~(RANK_8 | FILE_A | FILE_B)) << NORTH_WEST_WEST);
    Bitboard noNoWeAttacks = ((knights & ~(RANK_7 | RANK_8 | FILE_A)) << NORTH_NORTH_WEST);

    return noNoEaAttacks | noEaEaAttacks | soEaEaAttacks | soSoEaAttacks | soSoWeAttacks | SoWeWeAttacks | noWeWeAttacks | noNoWeAttacks;
}

Bitboard MoveGeneration::generateKnightMoves(Bitboard knights, Color color) {
    if(knights == (CURRENT_POSITION)) knights = this->board.getKnights(color); 

    Bitboard attacks = generateKnightAttacks(knights, color);

    Bitboard moves;
    if(color == BLACK) {
        moves = (attacks & ~this->board.getOccupiedByBlack());
    } else {
        moves = (attacks & ~this->board.getOccupiedByWhite());
    }

    return moves;
}

Bitboard MoveGeneration::generateKingAttacks(Bitboard king, Color color) {
    if(king == (CURRENT_POSITION)) king = this->board.getKing(color); 

    Bitboard northAttack = ((king & ~(RANK_8)) << NORTH);
    Bitboard westAttack = ((king & ~(FILE_A)) >> WEST);
    Bitboard southAttack = ((king & ~(RANK_1)) >> SOUTH);
    Bitboard eastAttack = ((king & ~(FILE_H)) << EAST);

    Bitboard directAttacks = westAttack; 

    Bitboard northeastAttack = (king & ~RANK_8 & ~FILE_H) << NORTH_EAST; 
    Bitboard northwestAttack = (king & ~RANK_8 & ~FILE_A) << NORTH_WEST;
    Bitboard southeastAttack = (king & ~RANK_1 & ~FILE_H) >> SOUTH_EAST;
    Bitboard southwestAttack = (king & ~RANK_1 & ~FILE_A) >> SOUTH_WEST;

    return northAttack | westAttack | southAttack | eastAttack | northeastAttack | northwestAttack | southeastAttack | southwestAttack;
}

Bitboard MoveGeneration::generateKingMoves(Bitboard king, Color color) {
    if(king == (CURRENT_POSITION)) king = this->board.getKing(color); 

    Bitboard attacks = generateKingAttacks(king, color);

    //castling
    Bitboard castle = EMPTY;

    if(isInCheck(color).numberOfChecks == 0) {
        //king castle ability
        if(isUnderAttack(this->board.getKing(color) << 1, color).numberOfAttacks == 0
        && isUnderAttack(this->board.getKing(color) << 2, color).numberOfAttacks == 0) {
            if(this->board.getKingSideCastleAbility(color)) {
               castle = castle | (this->board.getKing(color) << 2); 
            }        
        }

        if(isUnderAttack(this->board.getKing(color) >> 1, color).numberOfAttacks == 0
        && isUnderAttack(this->board.getKing(color) >> 2, color).numberOfAttacks == 0) {
            if(this->board.getQueenSideCastleAbility(color)) {
               castle = castle | (this->board.getKing(color) >> 2); 
            }        
        }
    }

    Bitboard pseudoLegalMoves = attacks | castle;

    Bitboard legalMoves;
    if(color == BLACK) {
        legalMoves = (pseudoLegalMoves & ~this->board.getOccupiedByBlack()) & (~generateAttackedSquaresWithoutKing(WHITE)) & (~generateKingAttacks(CURRENT_POSITION, WHITE));
    } else {
        legalMoves = (pseudoLegalMoves & ~this->board.getOccupiedByWhite()) & (~generateAttackedSquaresWithoutKing(BLACK)) & (~generateKingAttacks(CURRENT_POSITION, BLACK));
    }

    return legalMoves;
}

Attack_Info MoveGeneration::isUnderAttack(Square square, Color color) {
    Bitboard squareAsBitboard = 1ULL << square;
    return isUnderAttack(squareAsBitboard, color);
}

Attack_Info MoveGeneration::isUnderAttack(Bitboard squareAsBitboard, Color color) {
    int numberOfAttacks = 0;
    Attack_Info attack_info; 

    //add bishop and diagonal queen moves
    Bitboard moves = generateBishopMoves(squareAsBitboard, color);
    Bitboard intersect = moves & this->board.getBishops(getOppositeColor(color));
    convertBitbaordToMoves(intersect, squareAsBitboard, PieceType::BISHOP, color, MoveType::CAPTURE, numberOfAttacks, attack_info);

    intersect = moves & this->board.getQueens(getOppositeColor(color));
    convertBitbaordToMoves(intersect, squareAsBitboard, PieceType::QUEEN, color, MoveType::CAPTURE, numberOfAttacks, attack_info);

    //add rook and horizontal/vertical queen moves
    moves = generateRookMoves(squareAsBitboard, color);
    intersect = moves & this->board.getRooks(getOppositeColor(color));
    convertBitbaordToMoves(intersect, squareAsBitboard, PieceType::ROOK, color, MoveType::CAPTURE, numberOfAttacks, attack_info);

    intersect = moves & this->board.getQueens(getOppositeColor(color));
    convertBitbaordToMoves(intersect, squareAsBitboard, PieceType::QUEEN, color, MoveType::CAPTURE, numberOfAttacks, attack_info);

    //add knight moves
    moves = generateKnightMoves(squareAsBitboard, color);
    intersect = moves & this->board.getKnights(getOppositeColor(color));
    convertBitbaordToMoves(intersect, squareAsBitboard, PieceType::KNIGHT, color, MoveType::CAPTURE, numberOfAttacks, attack_info);

    //add pawn moves
    moves = generatePawnAttacks(squareAsBitboard, color);
    intersect = moves & this->board.getPawns(getOppositeColor(color));
    convertBitbaordToMoves(intersect, squareAsBitboard, PieceType::PAWN, color, MoveType::CAPTURE, numberOfAttacks, attack_info);

    //add en passent moves
    Bitboard pawns = this->board.getPawns(getOppositeColor(color));
    while(pawns != 0){
        Square pawn = __builtin_ctzll(pawns);
        Bitboard moves = generateEnPassentMoves(squareToBitboard(pawn), getOppositeColor(color));
        if(color == WHITE && moves != 0){
            //en passent square is moved up one row and compared with attacked square
            if((moves << DirectionValues::NORTH) == squareAsBitboard){
                //piece can be captured by en passent
                Move move(pawn, (Square)__builtin_ctzll(moves), PieceType::PAWN, color, MoveType::EN_PASSENT_CAPTURE);
                numberOfAttacks++;
                attack_info.moves.push_back(move);
            }
        } else if(color == BLACK && moves != 0){
            //en passent square is moved down one row and compared with attacked square
            if((moves >> DirectionValues::SOUTH) == squareAsBitboard) {
                Move move(pawn, (Square)__builtin_ctzll(moves), PieceType::PAWN, color, MoveType::EN_PASSENT_CAPTURE);
                numberOfAttacks++;
                attack_info.moves.push_back(move);
            }
        }
        pawns = (~squareToBitboard(pawn)) & pawns;
    }
    attack_info.numberOfAttacks = numberOfAttacks;
    return attack_info;
}

Check_Info MoveGeneration::isInCheck(Color color) {
    return Check_Info(isUnderAttack(__builtin_ctzll(this->board.getKing(color)), color));
}


Bitboard MoveGeneration::generateAttackedSquares(Color color) {
    return generateRookMoves(CURRENT_POSITION, color) | generateBishopMoves(CURRENT_POSITION, color) | generateQueenMoves(CURRENT_POSITION, color) | generateKnightMoves(CURRENT_POSITION, color) | generateKingMoves(CURRENT_POSITION, color) | generatePawnAttacks(CURRENT_POSITION, color);
}

Bitboard MoveGeneration::generateAttackedSquaresWithoutKing(Color color) {
    return generateRookMoves(CURRENT_POSITION, color) | generateBishopMoves(CURRENT_POSITION, color) | generateQueenMoves(CURRENT_POSITION, color) | generateKnightMoves(CURRENT_POSITION, color) | generatePawnAttacks(CURRENT_POSITION, color);
}


Pins MoveGeneration::getPinnedPieces(Color color){
    //TODO: IMPROVE LENGTH OF CODE (REUSE)
    Bitboard pinnedPiecesBitboard = EMPTY;
    Color oppositeColor = getOppositeColor(color);

    Pins pins;
    Bitboard intersection = EMPTY;

    //look up absolute pins by bishops and queens
    Bitboard kingSw = getLegalSouthwestMoves(this->board.getKing(color), oppositeColor);
    Bitboard bishopsNe = getLegalNortheastMoves(this->board.getBishops(oppositeColor), oppositeColor);
    Bitboard queensNe = getLegalNortheastMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingSw & bishopsNe;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), BISHOP, intersection, Direction::SW));
    intersection = kingSw & queensNe;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::SW));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingSw & bishopsNe) | (kingSw & queensNe)); 

    Bitboard kingSe = getLegalSoutheastMoves(this->board.getKing(color), oppositeColor);
    Bitboard bishopsNw = getLegalNorthwestMoves(this->board.getBishops(oppositeColor), oppositeColor);
    Bitboard queensNw = getLegalNorthwestMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingSe & bishopsNw;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), BISHOP, intersection, Direction::SE));
    intersection = kingSe & queensNw;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::SE));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingSe & bishopsNw) | (kingSe & queensNw)); 

    Bitboard kingNw = getLegalNorthwestMoves(this->board.getKing(color), oppositeColor);
    Bitboard bishopsSe = getLegalSoutheastMoves(this->board.getBishops(oppositeColor), oppositeColor);
    Bitboard queensSe = getLegalSoutheastMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingNw & bishopsSe;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), BISHOP, intersection, Direction::NW));
    intersection = kingNw & queensSe;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::NW));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingNw & bishopsSe) | (kingNw & queensSe)); 

    Bitboard kingNe = getLegalNortheastMoves(this->board.getKing(color), oppositeColor);
    Bitboard bishopsSw = getLegalSouthwestMoves(this->board.getBishops(oppositeColor), oppositeColor);
    Bitboard queensSw = getLegalSouthwestMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingNe & bishopsSw;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), BISHOP, intersection, Direction::NE));
    intersection = kingNe & queensSw;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::NE));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingNe & bishopsSw) | (kingNe & queensSw)); 
    
    //look up absoulte pins by bishops and queens
    Bitboard kingN = getLegalNorthMoves(this->board.getKing(color), oppositeColor);
    Bitboard rooksS = getLegalSouthMoves(this->board.getRooks(oppositeColor), oppositeColor); 
    Bitboard queensS = getLegalSouthMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingN & rooksS;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), ROOK, intersection, Direction::N));
    intersection = kingN & queensS;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::N));
    
    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingN & rooksS) | (kingN & queensS)); 

    Bitboard kingS = getLegalSouthMoves(this->board.getKing(color), oppositeColor);
    Bitboard rooksN = getLegalNorthMoves(this->board.getRooks(oppositeColor), oppositeColor); 
    Bitboard queensN = getLegalNorthMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingS & rooksN;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), ROOK, intersection, Direction::S));
    intersection = kingS & queensN;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::S));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingS & rooksN) | (kingS & queensN)); 

    Bitboard kingW = getLegalWestMoves(this->board.getKing(color), oppositeColor);
    Bitboard rooksE = getLegalEastMoves(this->board.getRooks(oppositeColor), oppositeColor); 
    Bitboard queensE = getLegalEastMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingW & rooksE;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), ROOK, intersection, Direction::W));
    intersection = kingW & queensE;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::W));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingW & rooksE) | (kingW & queensE)); 

    Bitboard kingE = getLegalEastMoves(this->board.getKing(color), oppositeColor);
    Bitboard rooksW = getLegalWestMoves(this->board.getRooks(oppositeColor), oppositeColor); 
    Bitboard queensW = getLegalWestMoves(this->board.getQueens(oppositeColor), oppositeColor);

    intersection = kingE & rooksW;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), ROOK, intersection, Direction::E));
    intersection = kingE & queensW;
    if(intersection != 0) pins.pins.push_back(Pin(this->board.getPieceTypeOfSquare(intersection, color), QUEEN, intersection, Direction::E));

    pinnedPiecesBitboard = pinnedPiecesBitboard | ((kingE & rooksW) | (kingE & queensW)); 

    pins.absolutePins = pinnedPiecesBitboard;

    return pins;
}

//returns all squares that are north to the current square
Bitboard MoveGeneration::North(Square square) {
    int rank = square / 8;
    Bitboard attacks = EMPTY;

    if(rank == 7) return attacks;

    attacks = (1ULL << (square + DirectionValues::NORTH));
    rank++;

    while(rank < 7) {
        attacks = attacks | (attacks << DirectionValues::NORTH); 
        rank++;
    }
    return attacks;
}

Bitboard MoveGeneration::Northeast(Square square) {
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = EMPTY;

    if(rank == 7 || file == 7) return attacks;

    attacks = (1ULL << (square + DirectionValues::NORTH_EAST));
    rank++;
    file++;

    while(rank < 7 && file < 7) {
        attacks = attacks | (attacks << DirectionValues::NORTH_EAST); 
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

    attacks = (1ULL << (square + DirectionValues::NORTH_WEST));
    rank++;
    file--;

    while(rank < 7 && file > 0) {
        attacks = attacks | (attacks << DirectionValues::NORTH_WEST); 
        rank++;
        file--;
    }
    return attacks;
}


Bitboard MoveGeneration::South(Square square) {
    int rank = square / 8;
    Bitboard attacks = EMPTY;

    if(rank == 0) return attacks;

    attacks = (1ULL << (square - DirectionValues::SOUTH));
    rank--;

    while(rank > 0) {
        attacks = attacks | (attacks >> DirectionValues::SOUTH); 
        rank--;
    }
    return attacks;
}

Bitboard MoveGeneration::Southeast(Square square) {
    int rank = square / 8;
    int file = square % 8;
    Bitboard attacks = EMPTY;

    if(rank == 0 || file == 7) return attacks;

    attacks = (1ULL << (square - DirectionValues::SOUTH_EAST));
    rank--;
    file++;

    while(rank > 0 && file < 7) {
        attacks = attacks | (attacks >> DirectionValues::SOUTH_EAST); 
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

    attacks = (1ULL << (square - DirectionValues::SOUTH_WEST));
    rank--;
    file--;

    while(rank > 0 && file > 0) {
        attacks = attacks | (attacks >> DirectionValues::SOUTH_WEST); 
        rank--;
        file--;
    }
    return attacks;
}

Bitboard MoveGeneration::West(Square square) {
    int file = square % 8;
    Bitboard attacks = EMPTY; 

    if(file == 0) return attacks;

    attacks = (1ULL << (square - DirectionValues::WEST));
    file--;

    while(file > 0) {
        attacks = attacks | (attacks >> DirectionValues::WEST);
        file--;
    }

    return attacks;
}

Bitboard MoveGeneration::East(Square square) {
    int file = square % 8;
    Bitboard attacks = EMPTY; 

    if(file == 7) return attacks;

    attacks = (1ULL << (square + DirectionValues::EAST));
    file++;

    while(file < 7) {
        attacks = attacks | (attacks << DirectionValues::EAST);
        file++;
    }
    return attacks;
}


Bitboard MoveGeneration::getLegalNorthwestMoves(Bitboard pieces, Color color){
    //clzll returns the number of leading zeros
    int index = 63 - __builtin_clzll(pieces);
    Bitboard northWestAttacks = MoveGeneration::Northwest(index);
    if(northWestAttacks != EMPTY) {
        //LEGAL MOVE GENERATION: Remove King 
        //Bitboard northWestBlockers = (northWestAttacks & this->board.getOccupiedWithoutWhiteKing());
        Bitboard northWestBlockers = (color == WHITE) ? (northWestAttacks & this->board.getOccupiedWithoutBlackKing())
            : (northWestAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(northWestBlockers != 0ULL) {
            //ctzll returns the number of trailing zeros
            int northWestFirstBlockerIndex = __builtin_ctzll(northWestBlockers);
            Bitboard blockedMoves = MoveGeneration::Northwest(northWestFirstBlockerIndex);
            //if the first blocker is of the same color
            if((1ULL << northWestFirstBlockerIndex) == ((1ULL << northWestFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                //add first blocker as a blocked moves (different color -> capture)
                blockedMoves = (blockedMoves | (1ULL << northWestFirstBlockerIndex));
            }
            return (northWestAttacks ^ blockedMoves);
        } else {
            return northWestAttacks;
        }
    }
    return EMPTY;
}

Bitboard MoveGeneration::getLegalNortheastMoves(Bitboard pieces, Color color){
    int index = 63 - __builtin_clzll(pieces);
    Bitboard northEastAttacks = MoveGeneration::Northeast(index);
    if(northEastAttacks!= 0ULL) {
        //Bitboard northEastBlockers = (northEastAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard northEastBlockers = (color == WHITE) ? (northEastAttacks & this->board.getOccupiedWithoutBlackKing())
            : (northEastAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(northEastBlockers != 0ULL) {
            int northEastFirstBlockerIndex = __builtin_ctzll(northEastBlockers);
            Bitboard blockedMoves = MoveGeneration::Northeast(northEastFirstBlockerIndex);
            if((1ULL << northEastFirstBlockerIndex) == ((1ULL << northEastFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << northEastFirstBlockerIndex));
            }
            return (northEastAttacks ^ blockedMoves);
        } else {
            return northEastAttacks;
        }
    }
    return EMPTY;
}
Bitboard MoveGeneration::getLegalSouthwestMoves(Bitboard pieces, Color color){
    int index = 63 - __builtin_clzll(pieces);
    Bitboard southWestAttacks = MoveGeneration::Southwest(index);
    if(southWestAttacks != 0ULL) {
        //Bitboard southWestBlockers = (southWestAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard southWestBlockers = (color == WHITE) ? (southWestAttacks & this->board.getOccupiedWithoutBlackKing())
            : (southWestAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(southWestBlockers != 0ULL) {
            int southWestFirstBlockerIndex = 63 -__builtin_clzll(southWestBlockers);
            Bitboard blockedMoves = MoveGeneration::Southwest(southWestFirstBlockerIndex);
            if((1ULL << southWestFirstBlockerIndex) == ((1ULL << southWestFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << southWestFirstBlockerIndex));
            }
            return (southWestAttacks ^ blockedMoves);
        } else {
            return southWestAttacks;
        }
    }
    return EMPTY;
}
Bitboard MoveGeneration::getLegalSoutheastMoves(Bitboard pieces, Color color){
    int index = 63 - __builtin_clzll(pieces);
    Bitboard southEastAttacks = MoveGeneration::Southeast(index);
    if(southEastAttacks!= 0ULL) {
        //Bitboard southEastBlockers = (southEastAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard southEastBlockers = (color == WHITE) ? (southEastAttacks & this->board.getOccupiedWithoutBlackKing())
            : (southEastAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(southEastBlockers != 0ULL) {
            int southEastFirstBlockerIndex = 63 - __builtin_clzll(southEastBlockers);
            Bitboard blockedMoves = MoveGeneration::Southeast(southEastFirstBlockerIndex);
            if((1ULL << southEastFirstBlockerIndex) == ((1ULL << southEastFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << southEastFirstBlockerIndex));
            }
            return (southEastAttacks ^ blockedMoves);
        } else {
            return southEastAttacks;
        }
    }
    return EMPTY;
}


Bitboard MoveGeneration::getLegalNorthMoves(Bitboard pieces, Color color){
    //clzll returns the number of leading zeros
    int index = 63 - __builtin_clzll(pieces);
    //Bitboard moves = MoveGeneration::North(index) | MoveGeneration::South(index) | MoveGeneration::East(index) | MoveGeneration::West(index);
    Bitboard northAttacks = MoveGeneration::North(index);
    if(northAttacks != 0ULL) {
        //Bitboard northBlockers = (northAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard northBlockers = (color == WHITE) ? (northAttacks & this->board.getOccupiedWithoutBlackKing())
            : (northAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(northBlockers != 0ULL) {
            //ctzll returns the number of trailing zeros
            int northFirstBlockerIndex = __builtin_ctzll(northBlockers);
            Bitboard blockedMoves = MoveGeneration::North(northFirstBlockerIndex);
            //if the first blocker is of the same color
            if((1ULL << northFirstBlockerIndex) == ((1ULL << northFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << northFirstBlockerIndex));
            }
            return (northAttacks ^ blockedMoves);
        } else {
            return northAttacks;
        }
    }
    return EMPTY;
}
Bitboard MoveGeneration::getLegalSouthMoves(Bitboard pieces, Color color){
    int index = 63 - __builtin_clzll(pieces);
    Bitboard southAttacks = MoveGeneration::South(index);
    if(southAttacks!= 0ULL) {
        //Bitboard southBlockers = (southAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard southBlockers = (color == WHITE) ? (southAttacks & this->board.getOccupiedWithoutBlackKing())
            : (southAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(southBlockers != 0ULL) {
            //ctzll returns the number of leading zeros
            int southFirstBlockerIndex = 63 - __builtin_clzll(southBlockers);
            Bitboard blockedMoves = MoveGeneration::South(southFirstBlockerIndex);
            //if the first blocker is of the same color
            if((1ULL << southFirstBlockerIndex) == ((1ULL << southFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << southFirstBlockerIndex));
            }
            return (southAttacks ^ blockedMoves);
        } else {
            return southAttacks;
        }
    }
    return EMPTY;
}
Bitboard MoveGeneration::getLegalWestMoves(Bitboard pieces, Color color){
    int index = 63 - __builtin_clzll(pieces);
    Bitboard westAttacks = MoveGeneration::West(index);
    if(westAttacks != 0ULL) {
        //Bitboard westBlockers = (westAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard westBlockers = (color == WHITE) ? (westAttacks & this->board.getOccupiedWithoutBlackKing())
            : (westAttacks & this->board.getOccupiedWithoutWhiteKing());

        if(westBlockers != 0ULL) {
            //ctzll returns the number of trailing zeros
            int westFirstBlockerIndex = 63 -__builtin_clzll(westBlockers);
            Bitboard blockedMoves = MoveGeneration::West(westFirstBlockerIndex);
            //if the first blocker is of the same color
            if((1ULL << westFirstBlockerIndex) == ((1ULL << westFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << westFirstBlockerIndex));
            }
            return (westAttacks ^ blockedMoves);
        } else {
            return westAttacks;
        }
    }
    return EMPTY;
}
Bitboard MoveGeneration::getLegalEastMoves(Bitboard pieces, Color color){
    int index = 63 - __builtin_clzll(pieces);
    Bitboard eastAttacks = MoveGeneration::East(index);
    if(eastAttacks!= 0ULL) {
        //Bitboard eastBlockers = (eastAttacks & this->board.getOccupied());
        //LEGAL MOVE GENERATION: Remove King 
        Bitboard eastBlockers = (color == WHITE) ? (eastAttacks & this->board.getOccupiedWithoutBlackKing())
            : (eastAttacks & this->board.getOccupiedWithoutWhiteKing());
        
        if(eastBlockers != 0ULL) {
            //ctzll returns the number of leading zeros
            int eastFirstBlockerIndex = __builtin_ctzll(eastBlockers);
            Bitboard blockedMoves = MoveGeneration::East(eastFirstBlockerIndex);
            //if the first blocker is of the same color
            if((1ULL << eastFirstBlockerIndex) == ((1ULL << eastFirstBlockerIndex) & (color == BLACK ? this->board.getOccupiedByBlack() : this->board.getOccupiedByWhite()))) {
                blockedMoves = (blockedMoves | (1ULL << eastFirstBlockerIndex));
            }
            return (eastAttacks ^ blockedMoves);
        } else {
            return eastAttacks;
        }
    }
    return EMPTY;
}