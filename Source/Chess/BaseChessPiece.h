// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "BaseChessPiece.generated.h"

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
	
	FName GetSide() const;

	bool IsFirstMove() const;

	virtual void MoveChessPiece(FIntPoint NewPosition);

	virtual int GetValue() const;

	void SynchronizePosition();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsLocationValid(FIntPoint Index) const;

	class AChessBoard* ChessBoard;

	UPROPERTY(VisibleAnywhere)
	FIntPoint CurrentPosition;

	FName Side;

	FName EnemySide;
	
	UPROPERTY(VisibleAnywhere)
	bool bIsFirstMove = true;

public:	
	// Called every frame
	virtual void Tick(float DeltaTime) override;

private:
	UPROPERTY(VisibleAnywhere, BlueprintReadOnly, meta = (AllowPrivateAccess = "true"))
	UStaticMeshComponent* StaticMesh;

	void InitializeCurrentPosition();

	
};