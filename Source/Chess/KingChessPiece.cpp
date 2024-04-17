// Fill out your copyright notice in the Description page of Project Settings.


#include "KingChessPiece.h"
#include "Kismet/GameplayStatics.h"
#include "ChessBoard.h"

TArray<FIntPoint> AKingChessPiece::GetPossibleMovePositions()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        int XOffset[] = {1, 0, 0, -1, 1, -1, -1, 1};
        int YOffset[] = {0, 1, -1, 0, 1, 1, -1, -1};
        TArray<FIntPoint> PossibleEnemyMoves = GetPossibleMovesOfEnemyPieces();

        for(int i{}; i < AChessBoard::BoardLength; i++)
        {
            if(IsLocationValid(FIntPoint(CurrentPosition.X + XOffset[i], CurrentPosition.Y + YOffset[i])) && !PossibleEnemyMoves.Contains(FIntPoint(CurrentPosition.X + XOffset[i], CurrentPosition.Y + YOffset[i])))
            {
                PossibleMoves.Emplace(FIntPoint(CurrentPosition.X + XOffset[i], CurrentPosition.Y + YOffset[i]));
            }
        }
        //Castling
            //Check if castling with right rook is possible
            //Conditions:
                //1. King and Rook are on first move
                //2. King's new position is not under possiblenemymoves (it will not be checkmated)
                //3. Spaces between King and Rook are vacant 

        //if King is in first move
        if(bIsFirstMove)
        {
            for(int i = 0, j = -1; i <= 7; i += 7, j *= -1)
            {
                if(ABaseChessPiece* CastledRook = Cast<ABaseChessPiece>(ChessBoard->GetChessPiece(FIntPoint(i, CurrentPosition.Y))))
                {
                    //if said rook is in first move (then it is a rook) and King's new position is not under possiblenemymoves (it will not be checkmated)
                    if(CastledRook->IsFirstMove())
                    {
                        //if spaces between King and Rook are vacant 
                        if(!ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X + j, CurrentPosition.Y)) && !ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X + (2 * j), CurrentPosition.Y)))
                        {
                            PossibleMoves.Emplace(FIntPoint(CurrentPosition.X + (2 * j), CurrentPosition.Y));
                            bIsCastling = true;
                        }
                    }
                }
            }
            
        }
    }
    return PossibleMoves;
}

TArray<FIntPoint> AKingChessPiece::GetPossibleMovePositionsForEnemy()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        int XOffset[] = {1, 0, 0, -1, 1, -1, -1, 1};
        int YOffset[] = {0, 1, -1, 0, 1, 1, -1, -1};

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

void AKingChessPiece::MoveChessPiece(FIntPoint NewPosition)
{
    Super::MoveChessPiece(NewPosition);
    bIsCastling = false;
}

bool AKingChessPiece::IsCastling() const
{
    return bIsCastling;
}

bool AKingChessPiece::IsChecked() const
{
    TArray<FIntPoint> PossibleEnemyMoves = GetPossibleMovesOfEnemyPieces();
    for(FIntPoint& index : PossibleEnemyMoves)
    {
        if(index == GetCurrentPosition())
        {
            return true;
        }
    }
    return false;
}

int AKingChessPiece::GetValue() const 
{
    return 0;
}

TArray<FIntPoint> AKingChessPiece::GetPossibleMovesOfEnemyPieces() const
{
    TArray<FIntPoint> EnemyMoves;
    TArray<AActor*> EnemyPieces;
    UGameplayStatics::GetAllActorsWithTag(this, EnemySide, EnemyPieces);
    
    for(AActor* EnemyPiece : EnemyPieces)
    {
        if(ABaseChessPiece* Enemy = Cast<ABaseChessPiece>(EnemyPiece))
        {
            
            EnemyMoves.Append(Enemy->GetPossibleMovePositionsForEnemy());
        }
    }
    return EnemyMoves;
}