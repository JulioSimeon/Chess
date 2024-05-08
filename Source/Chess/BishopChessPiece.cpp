// Fill out your copyright notice in the Description page of Project Settings.


#include "BishopChessPiece.h"
#include "ChessBoard.h"
#include "Kismet/KismetMathLibrary.h"

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
                if(IsLocationValid(Diagonal[j]) && !PossibleMoves.Contains(Diagonal[j])) //Location is Empty or it has an enemy
                {
                    PossibleMoves.Emplace(Diagonal[j]);
                }
                else //Location is not valid
                {
                    continue;
                }
                if(ChessBoard->GetChessPiece(Diagonal[j])) //Location has a chess piece that has a friendly chess piece
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

TArray<FIntPoint> ABishopChessPiece::GetPossibleMovePositionsForEnemy()
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
                if(IsIndexValid(Diagonal[j]) && !PossibleMoves.Contains(Diagonal[j])) //Location is Empty or it has an enemy
                {
                    PossibleMoves.Emplace(Diagonal[j]);
                }
                else //Location is not valid
                {
                    continue;
                }
                if(ChessBoard->GetChessPiece(Diagonal[j]) && (ChessBoard->GetChessPiece(Diagonal[j])->GetType() != Type::King || ChessBoard->GetChessPiece(Diagonal[j])->IsWhite() == bIsWhite)) //Location has a chess piece that has a friendly chess piece
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
    return Super::GetValue() * 300 + (bIsWhite ? BishopPieceSquareTable[CurrentPosition.X][CurrentPosition.Y] : -BishopPieceSquareTable[UKismetMathLibrary::Abs_Int(7 -CurrentPosition.X)][UKismetMathLibrary::Abs_Int(7 -CurrentPosition.Y)]);
}

ABishopChessPiece::ABishopChessPiece()
{
    ChessPieceType = Type::Bishop;
}
