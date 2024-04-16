// Fill out your copyright notice in the Description page of Project Settings.


#include "BishopChessPiece.h"
#include "ChessBoard.h"

TArray<FIntPoint> ABishopChessPiece::GetPossibleMovePositions()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        const int diag = 4; // number of diagonals to check
        FIntPoint Diagonal[diag];
        Diagonal[0] = FIntPoint(CurrentPosition.X + 1, CurrentPosition.Y + 1);
        Diagonal[1] = FIntPoint(CurrentPosition.X - 1, CurrentPosition.Y + 1);
        Diagonal[2] = FIntPoint(CurrentPosition.X - 1, CurrentPosition.Y - 1);
        Diagonal[3] = FIntPoint(CurrentPosition.X + 1, CurrentPosition.Y - 1);

        for(int i{2}; i <= AChessBoard::BoardHeight; i++)
        {
            for(int j{}; j < diag; j++)
            {
                if(IsLocationValid(Diagonal[j]))
                {
                    PossibleMoves.Emplace(Diagonal[j]);
                }
                else 
                {
                    continue;
                }
                if(ChessBoard->GetChessPiece(Diagonal[j]))
                {
                    continue;
                }
                switch(j)
                {
                    case 0:
                        Diagonal[0] = FIntPoint(CurrentPosition.X + i, CurrentPosition.Y + i);
                        break;
                    case 1:
                        Diagonal[1] = FIntPoint(CurrentPosition.X - i, CurrentPosition.Y + i);
                        break;
                    case 2:
                        Diagonal[2] = FIntPoint(CurrentPosition.X - i, CurrentPosition.Y - i);
                        break;
                    case 3:
                        Diagonal[3] = FIntPoint(CurrentPosition.X + i, CurrentPosition.Y - i);
                        break;
                }
                
            }
        }
    }
    return PossibleMoves;
}

int ABishopChessPiece::GetValue() const 
{
     return Super::GetValue() * 3;
}
