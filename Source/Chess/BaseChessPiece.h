// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseChessPiece.generated.h"

enum class Type
{
	Pawn,
	Knight,
	Bishop,
	Rook,
	Queen,
	King,
};

UCLASS()
class CHESS_API ABaseChessPiece : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ABaseChessPiece();

	virtual TArray<FIntPoint> GetPossibleMovePositions();

	FIntPoint GetCurrentPosition() const;

	virtual TArray<FIntPoint> GetPossibleMovePositionsForEnemy();

	virtual void SetCurrentPosition(FIntPoint NewPosition);

	bool IsFirstMove() const;

	void SetIsFirstMove(bool NewIsFirstMove);

	virtual void MoveChessPiece(FIntPoint NewPosition);

	virtual int GetValue() const;

	void SynchronizePosition();

	bool IsSlidingPiece() const;

	Type GetType() const;

	bool IsWhite() const;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsLocationValid(FIntPoint Index) const;

	bool IsIndexValid(FIntPoint Index) const;

	class AChessBoard* ChessBoard;

	UPROPERTY(VisibleAnywhere)
	FIntPoint CurrentPosition;

	UPROPERTY(EditDefaultsOnly)
	bool bIsWhite;
	
	UPROPERTY(EditAnywhere)
	bool bIsFirstMove = true;

	Type ChessPieceType;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	void InitializeCurrentPosition();

	
};