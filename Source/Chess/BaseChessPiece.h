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
	
	FName GetPlayerSide() const;

	bool IsFirstMove() const;

	virtual void MoveChessPiece(FIntPoint NewPosition);

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

	bool IsLocationValid(FIntPoint Index) const;

	class AChessBoard* ChessBoard;

	FIntPoint CurrentPosition;

	FName PlayerSide;

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