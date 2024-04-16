// Fill out your copyright notice in the Description page of Project Settings.


#include "PawnChessPiece.h"
#include "ChessBoard.h"

TArray<FIntPoint> APawnChessPiece::GetPossibleMovePositions()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        if(IsLocationValid(CurrentPosition + FIntPoint(0, Direction)) && !ChessBoard->GetChessPiece(CurrentPosition + FIntPoint(0, Direction)))
        {
            PossibleMoves.Emplace(CurrentPosition + FIntPoint(0, Direction));
            if(IsLocationValid(CurrentPosition + FIntPoint(0, Direction * 2)) && !ChessBoard->GetChessPiece(CurrentPosition + FIntPoint(0, Direction * 2)) && bIsFirstMove)
            {
                PossibleMoves.Emplace(CurrentPosition + FIntPoint(0, Direction * 2));
            }
        }
        if(EnPassantPawn)
        {
            //check which side is EnPassantPawn
            for (int i{-1}; i <= 1; i+=2)
            {
                //if EnPassantPawn found
                if(IsLocationValid(CurrentPosition + FIntPoint(i, 0)))
                {
                    if(EnPassantPawn == Cast<APawnChessPiece>(ChessBoard->GetChessPiece(CurrentPosition + FIntPoint(i, 0))))
                    {
                        PossibleMoves.Emplace(CurrentPosition + FIntPoint(i, 1));
                    }
                }
                
            }
        }
        for (int i{-1}; i <= 1; i+=2)
        {
            if(ChessBoard->GetChessPiece(CurrentPosition + FIntPoint(i, Direction)) && IsLocationValid(CurrentPosition + FIntPoint(i, Direction)))
            {
                PossibleMoves.Emplace(CurrentPosition + FIntPoint(i, Direction));
            }
        }
    }
    return PossibleMoves;
}

TArray<FIntPoint> APawnChessPiece::GetPossibleMovePositionsForEnemy()
{
    TArray<FIntPoint> PossibleMoves;
    if(ChessBoard)
    {
        for (int i{-1}; i <= 1; i+=2)
        {
            if(IsLocationValid(CurrentPosition + FIntPoint(i, Direction)))
            {
                PossibleMoves.Emplace(CurrentPosition + FIntPoint(i, Direction));
            }
        }
    }
    return PossibleMoves;
}

void APawnChessPiece::SetCurrentPosition(FIntPoint NewPosition)
{
    //check if SetEnPassant will be called for any adjacent enemy pawns (can only occur if pawn moves two spaces forward)
    if(NewPosition == CurrentPosition + FIntPoint(0, Direction * 2)) 
    {
        for (int i{-1}; i <= 1; i+=2)
        {
            //check if there are adjacent pawns
            
            if(IsLocationValid(CurrentPosition + FIntPoint(i, Direction * 2)))
            {
                if(APawnChessPiece* AdjacentPawn = Cast<APawnChessPiece>(ChessBoard->GetChessPiece(CurrentPosition + FIntPoint(i, Direction * 2))))
                {
                    //check if adjacent pawns are on enemy side
                    if(AdjacentPawn->ActorHasTag(EnemySide))
                    {
                        //SetEnPassant for adjacent enemy pawn
                        AdjacentPawn->SetEnPassant(this);
                    }
                }
               
            }
        }
    }
    Super::SetCurrentPosition(NewPosition);
}

APawnChessPiece* APawnChessPiece::GetEnPassantPawn() const
{
    return EnPassantPawn;
}

void APawnChessPiece::MoveChessPiece(FIntPoint NewPosition)
{
    Super::MoveChessPiece(NewPosition);
    EnPassantPawn = nullptr; //reset EnPassantPawn
}

int APawnChessPiece::GetValue() const
{
    return Super::GetValue() * 1;
}

void APawnChessPiece::SetEnPassant(APawnChessPiece* PassedPawn)
{
    EnPassantPawn = PassedPawn;
    //UE_LOG(LogTemp, Display, TEXT("EnPassant Activated, EnPassant Pawn = %s"), *PassedPawn->GetActorNameOrLabel());
}

void APawnChessPiece::BeginPlay()
{
    Super::BeginPlay();
    if(PlayerSide == "Black")
    {
        Direction = -1;
    }
    else
    {
        Direction = 1;
    }
}
