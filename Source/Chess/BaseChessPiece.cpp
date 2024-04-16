// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseChessPiece.h"
#include "Kismet/GameplayStatics.h"
#include "ChessBoard.h"

// Sets default values
ABaseChessPiece::ABaseChessPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	RootComponent = StaticMesh;
}

// Called when the game starts or when spawned
void ABaseChessPiece::BeginPlay()
{
	Super::BeginPlay();

	TArray<AActor*> ChessBoards;
    UGameplayStatics::GetAllActorsOfClass(this, AChessBoard::StaticClass(), ChessBoards);
    if(ChessBoards.Num() > 0)
    {
        ChessBoard = Cast<AChessBoard>(ChessBoards[0]);
    }
	InitializeCurrentPosition();
	
	PlayerSide = Tags[0];

	if(PlayerSide == "White")
	{
		EnemySide = "Black";
	}
	else if(PlayerSide == "Black")
	{
		EnemySide = "White";
	}
}

bool ABaseChessPiece::IsLocationValid(FIntPoint Index) const
{
	if(ChessBoard)
    {
		if(Index.X >= 0 && Index.X < AChessBoard::BoardLength && Index.Y >= 0 && Index.Y < AChessBoard::BoardHeight)
		{
			if(!ChessBoard->GetChessPiece(Index))
			{
				return true;
			}
			else if(ChessBoard->GetChessPiece(Index))
			{
				return !(ChessBoard->GetChessPiece(Index)->ActorHasTag(PlayerSide));
			}
    	}
	}
        
    return false;
}

bool ABaseChessPiece::IsFirstMove() const
{
	return bIsFirstMove;
}

void ABaseChessPiece::MoveChessPiece(FIntPoint NewPosition)
{
	SetCurrentPosition(NewPosition);
	bIsFirstMove = false;
}

int ABaseChessPiece::GetValue() const
{
	if(PlayerSide == "White")
	{
		return 1;
	}
	else if(PlayerSide == "Black")
	{
		return -1;
	}
	return 0;
}

void ABaseChessPiece::SynchronizePosition()
{
	if(ChessBoard)
	{
		if(PlayerSide == "Black")
		{	
			UE_LOG(LogTemp, Warning, TEXT("%s synchronized"), *GetActorNameOrLabel());
		}
		ChessBoard->SetChessPiece(CurrentPosition, this);
	}
}

TArray<FIntPoint> ABaseChessPiece::GetPossibleMovePositions()
{
	TArray<FIntPoint> PossibleMoves;
	return PossibleMoves;
}

FIntPoint ABaseChessPiece::GetCurrentPosition() const
{
	return CurrentPosition;
}

TArray<FIntPoint> ABaseChessPiece::GetPossibleMovePositionsForEnemy()
{
	return GetPossibleMovePositions();
}

void ABaseChessPiece::SetCurrentPosition(FIntPoint NewPosition)
{
	CurrentPosition = NewPosition;
}

FName ABaseChessPiece::GetPlayerSide() const
{
	return PlayerSide;
}

// Called every frame
void ABaseChessPiece::Tick(float DeltaTime)
{
	Super::Tick(DeltaTime);

}

void ABaseChessPiece::InitializeCurrentPosition()
{
	if(ChessBoard)
	{
		CurrentPosition = ChessBoard->GetIndex(GetActorLocation());
		if(CurrentPosition == FIntPoint(-1, -1))
		{
			UE_LOG(LogTemp, Error, TEXT("Invalid Index for %s, Location: %s"), *GetActorNameOrLabel(), *GetActorLocation().ToString());
		}
		else
		{
			ChessBoard->SetChessPiece(CurrentPosition, this);
		}
		
	}
	
}

