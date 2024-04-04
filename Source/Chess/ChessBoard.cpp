// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBoard.h"

// Sets default values
AChessBoard::AChessBoard()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	//InitializeBoard is in constructor because calling it in begin play will mean it is called later than ChessPieces' begin play resulting in NG
	InitializeBoard();
}

FVector AChessBoard::GetLocation(FIntPoint index) const
{
	return BoardArray[index.X][index.Y].Location;
}

void AChessBoard::SetChessPiece(FIntPoint index, AActor* ChessPiece)
{
	BoardArray[index.X][index.Y].ChessPiece = ChessPiece;
}

AActor* AChessBoard::GetChessPiece(FIntPoint index) const
{
	return BoardArray[index.X][index.Y].ChessPiece;
}

bool AChessBoard::IsOccupied(FIntPoint index) const
{
	return BoardArray[index.X][index.Y].ChessPiece != nullptr;
}

FIntPoint AChessBoard::GetIndex(FVector location) const
{
	location.Z = 25.f;
	for(int i{}; i < BoardHeight; i++)
	{
		for(int j{}; j < BoardLength; j++)
		{
			if(location == BoardArray[i][j].Location)
			{
				return FIntPoint(i, j);
			}
		}
	}
	return FIntPoint(-1, -1);
}

// Called when the game starts or when spawned
void AChessBoard::BeginPlay()
{
	Super::BeginPlay();
}

// Called every frame
void AChessBoard::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void AChessBoard::InitializeBoard()
{
	const float Height = 25.f;
	const float Offset = 100.f;
	float RowLocation = 350.f;
	
	for(int i{}; i < BoardHeight; i++)
	{
		float ColumnLocation = -350.f;
		for(int j{}; j < BoardLength; j++)
		{
			BoardArray[j][i] = BoardSquare(FVector(ColumnLocation, RowLocation, Height));
			ColumnLocation += Offset;
			UE_LOG(LogTemp, Display, TEXT("Location: %s, Index: %s"), *BoardArray[i][j].Location.ToString(), *FIntPoint(i, j).ToString());
		}
		RowLocation -= Offset;
	}
}

