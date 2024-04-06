// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessGameMode.h"
#include "BaseChessPiece.h"

void AChessGameMode::PieceCaptured(AActor* CapturedPiece)
{
    CapturedPiece->Destroy();
}

void AChessGameMode::BeginPlay()
{
    Super::BeginPlay();
    
}
