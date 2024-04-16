// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "ChessBoard.generated.h"

struct BoardSquare
{
	AActor* ChessPiece = nullptr;
	FVector Location = FVector::ZeroVector;
	BoardSquare()
	{

	}
	BoardSquare(FVector SquareLocation)
	{
		Location = SquareLocation;
	}
};

UCLASS()
class CHESS_API AChessBoard : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	AChessBoard();

	FVector GetLocation(FIntPoint index) const;

	void SetChessPiece(FIntPoint index, AActor* ChessPiece);

	AActor* GetChessPiece(FIntPoint index) const;

	bool IsOccupied(FIntPoint index) const;

	FIntPoint GetIndex(FVector location) const;

	int Evaluate() const;

	static const int BoardHeight = 8;
	static const int BoardLength = 8;

	//debug
	void PrintChessPieces();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	//TArray<BoardSquare> BoardArray;

	BoardSquare BoardArray[8][8];

	void InitializeBoard();
	
};
