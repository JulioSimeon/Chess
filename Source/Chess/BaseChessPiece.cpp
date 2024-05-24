// Fill out your copyright notice in the Description page of Project Settings.


#include "BaseChessPiece.h"
#include "Components/CapsuleComponent.h"
#include "Kismet/GameplayStatics.h"
#include "ChessBoard.h"

// Sets default values
ABaseChessPiece::ABaseChessPiece()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

	CapsuleComp = CreateDefaultSubobject<UCapsuleComponent>(TEXT("Capsule Component"));
	StaticMesh = CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Static Mesh"));
	StaticMesh->SetupAttachment(CapsuleComp);
	RootComponent = CapsuleComp;
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
			else
			{
				return (ChessBoard->GetChessPiece(Index)->IsWhite() != bIsWhite);
			}
    	}
	}
        
    return false;
}

bool ABaseChessPiece::IsIndexValid(FIntPoint Index) const
{
	if(ChessBoard)
    {
		if(Index.X >= 0 && Index.X < AChessBoard::BoardLength && Index.Y >= 0 && Index.Y < AChessBoard::BoardHeight)
		{
			return true;
    	}
	}
    return false;
}

bool ABaseChessPiece::IsFirstMove() const
{
	return bIsFirstMove;
}

void ABaseChessPiece::SetIsFirstMove(bool NewIsFirstMove)
{
	bIsFirstMove = NewIsFirstMove;
}

void ABaseChessPiece::MoveChessPiece(FIntPoint NewPosition)
{
	SetCurrentPosition(NewPosition);
	bIsFirstMove = false;
}

int ABaseChessPiece::GetValue() const
{
	return (bIsWhite ? 1 : -1);
}

void ABaseChessPiece::SynchronizePosition()
{
	if(ChessBoard)
	{
		ChessBoard->SetChessPiece(CurrentPosition, this);
	}
}

bool ABaseChessPiece::IsSlidingPiece() const
{
	return ChessPieceType == Type::Bishop || ChessPieceType == Type::Queen || ChessPieceType == Type::Rook;
}

Type ABaseChessPiece::GetType() const
{
	return ChessPieceType;
}

bool ABaseChessPiece::IsWhite() const
{
	return bIsWhite;
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

