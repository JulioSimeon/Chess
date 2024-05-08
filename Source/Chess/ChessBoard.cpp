// Fill out your copyright notice in the Description page of Project Settings.


#include "ChessBoard.h"
#include "BaseChessPiece.h"
#include "Kismet/GameplayStatics.h"

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

void AChessBoard::SetChessPiece(FIntPoint index, ABaseChessPiece* ChessPiece)
{
	BoardArray[index.X][index.Y].ChessPiece = ChessPiece;
}

ABaseChessPiece* AChessBoard::GetChessPiece(FIntPoint index) const
{
	return BoardArray[index.X][index.Y].ChessPiece;
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

void AChessBoard::RemoveChessPiece(ABaseChessPiece* ChessPiece)
{
	ChessPiece->IsWhite() ? WhiteChessPieces.Remove(ChessPiece) : BlackChessPieces.Remove(ChessPiece);
	CapturedChessPieces.Add(ChessPiece);
}

void AChessBoard::ReviveChessPiece(ABaseChessPiece* ChessPiece)
{
	if(CapturedChessPieces.Contains(ChessPiece))
	{
		CapturedChessPieces.Remove(ChessPiece);
		ChessPiece->IsWhite() ? WhiteChessPieces.Add(ChessPiece) : BlackChessPieces.Add(ChessPiece);
	}
}

int AChessBoard::Evaluate() const
{
	int total{};
	for(int i{}; i < BoardHeight; i++)
	{
		for(int j{}; j < BoardLength; j++)
		{
			if(ABaseChessPiece* ChessPiece = BoardArray[i][j].ChessPiece)
			{
				total += ChessPiece->GetValue();
			}
		}
	}
	return total;
}

TArray<ABaseChessPiece*> AChessBoard::GetChessPieces(bool IsWhite)
{
	return IsWhite ? WhiteChessPieces : BlackChessPieces;
}

void AChessBoard::PrintChessPieces()
{
	int whitecount{};
	int blackcount{};
	for(int i{}; i < BoardHeight; i++)
	{
		for(int j{}; j < BoardLength; j++)
		{
			if(ABaseChessPiece* ChessPiece = BoardArray[i][j].ChessPiece)
			{
				UE_LOG(LogTemp, Display, TEXT("%s at %d, %d"), *ChessPiece->GetActorNameOrLabel(), i, j);
				if(ChessPiece->IsWhite())
				{
					whitecount++;
				}
				else
				{
					blackcount++;
				}
			}
		}
	}
	UE_LOG(LogTemp, Warning, TEXT("Total white %d total black %d"), whitecount, blackcount);
}

// Called when the game starts or when spawned
void AChessBoard::BeginPlay()
{
	Super::BeginPlay();

	//Initialize white chess and black chess pieces
	TArray<AActor*> ChessPieces;
	UGameplayStatics::GetAllActorsOfClass(this, ABaseChessPiece::StaticClass(), ChessPieces);
	for(auto piece : ChessPieces)
	{
		if(ABaseChessPiece* ChessPiece = Cast<ABaseChessPiece>(piece))
		{
			ChessPiece->IsWhite() ? WhiteChessPieces.Add(ChessPiece) : BlackChessPieces.Add(ChessPiece);
		}
	}
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
			//UE_LOG(LogTemp, Display, TEXT("Location: %s, Index: %s"), *BoardArray[i][j].Location.ToString(), *FIntPoint(i, j).ToString());
		}
		RowLocation -= Offset;
	}
}

