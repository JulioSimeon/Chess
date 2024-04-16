// Fill out your copyright notice in the Description page of Project Settings.


#include "KnightChessPiece.h"
#include "ChessBoard.h"

TArray<FIntPoint> AKnightChessPiece::GetPossibleMovePositions()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        const int XOffset[] = {1, 1, 2, 2, -1, -1, -2, -2};
        const int YOffset[] = {2, -2, 1, -1, 2, -2, 1, -1};

        for(int i{}; i < AChessBoard::BoardLength; i++)
        {
            if(IsLocationValid(FIntPoint(CurrentPosition.X + XOffset[i], CurrentPosition.Y + YOffset[i])))
            {
                PossibleMoves.Emplace(FIntPoint(CurrentPosition.X + XOffset[i], CurrentPosition.Y + YOffset[i]));
            }            
        }
    }
    return PossibleMoves;
}

int AKnightChessPiece::GetValue() const 
{
    return Super::GetValue() * 3;
}
