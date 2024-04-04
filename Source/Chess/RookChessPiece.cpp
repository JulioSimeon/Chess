// Fill out your copyright notice in the Description page of Project Settings.


#include "RookChessPiece.h"
#include "ChessBoard.h"

TArray<FIntPoint> ARookChessPiece::GetPossibleMovePositions()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        const int direc = 4; // number of directions to check
        FIntPoint Direction[direc];

        Direction[0] = FIntPoint(CurrentPosition.X + 1, CurrentPosition.Y);
        Direction[1] = FIntPoint(CurrentPosition.X, CurrentPosition.Y + 1);
        Direction[2] = FIntPoint(CurrentPosition.X - 1, CurrentPosition.Y);
        Direction[3] = FIntPoint(CurrentPosition.X, CurrentPosition.Y - 1);

        for(int i{2}; i <= AChessBoard::BoardHeight; i++)
        {
            for(int j{}; j < direc; j++)
            {
                if(IsLocationValid(Direction[j]))
                {
                    PossibleMoves.Emplace(Direction[j]);
                }
                else 
                {
                    continue;
                }
                if(ChessBoard->GetChessPiece(Direction[j]))
                {
                    continue;
                }
                switch(j)
                {
                    case 0:
                        Direction[0] = FIntPoint(CurrentPosition.X + i, CurrentPosition.Y);
                        break;
                    case 1:
                        Direction[1] = FIntPoint(CurrentPosition.X, CurrentPosition.Y + i);
                        break;
                    case 2:
                        Direction[2] = FIntPoint(CurrentPosition.X - i, CurrentPosition.Y);
                        break;
                    case 3:
                        Direction[3] = FIntPoint(CurrentPosition.X, CurrentPosition.Y - i);
                        break;
                }
            }
        }
    }
    
    return PossibleMoves;
}

void ARookChessPiece::SetCurrentPosition(FIntPoint NewPosition)
{
    Super::SetCurrentPosition(NewPosition);
}
