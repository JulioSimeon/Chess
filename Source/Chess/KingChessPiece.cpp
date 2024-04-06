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
            //if Rook to the right of king is present 
            ABaseChessPiece* CastledRook = Cast<ABaseChessPiece>(ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X + 3, CurrentPosition.Y)));
            if(CastledRook)
            {
                //if said rook is in first move (then it is a rook) and King's new position is not under possiblenemymoves (it will not be checkmated)
                if(CastledRook->IsFirstMove() && !PossibleEnemyMoves.Contains(FIntPoint(CurrentPosition.X + 2, CurrentPosition.Y)))
                {
                    //if spaces between King and Rook are vacant 
                    if(!ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X + 1, CurrentPosition.Y)) && !ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X + 2, CurrentPosition.Y)))
                    {
                        PossibleMoves.Emplace(FIntPoint(CurrentPosition.X + 2, CurrentPosition.Y));
                        bIsCastling = true;
                    }
                }
            }
            //if Rook to the left of king is present 
            CastledRook = Cast<ABaseChessPiece>(ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X - 4, CurrentPosition.Y)));
            if(CastledRook)
            {
                //if said rook is in first move (then it is a rook) and King's new position is not under possiblenemymoves (it will not be checkmated)
                if(CastledRook->IsFirstMove() && !PossibleEnemyMoves.Contains(FIntPoint(CurrentPosition.X - 2, CurrentPosition.Y)))
                {
                    //if spaces between King and Rook are vacant 
                    if(!ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X - 1, CurrentPosition.Y)) && !ChessBoard->GetChessPiece(FIntPoint(CurrentPosition.X - 2, CurrentPosition.Y)))
                    {
                        PossibleMoves.Emplace(FIntPoint(CurrentPosition.X - 2, CurrentPosition.Y));
                        bIsCastling = true;
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

void AKingChessPiece::SetCurrentPosition(FIntPoint NewPosition)
{
    Super::SetCurrentPosition(NewPosition);
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