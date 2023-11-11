// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Kismet/BlueprintFunctionLibrary.h"
#include "FarkleUtils.generated.h"

class ADiceActor;

USTRUCT(BlueprintType)
struct FDiceCombination
{
	GENERATED_BODY()

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Dice")
	TArray<ADiceActor *> Dices;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Score")
	int32 Score;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float Probability;

	UPROPERTY(EditAnywhere, BlueprintReadWrite, Category = "Stats")
	float WeightedScore;

	FDiceCombination()
		: Score(0), Probability(0.0f), WeightedScore(0.0f)
	{
	}

	FDiceCombination(const TArray<ADiceActor *> &InDices, int32 InScore)
		: Dices(InDices), Score(InScore), Probability(0.0f), WeightedScore(0.0f)
	{
	}
};


DECLARE_DYNAMIC_DELEGATE(FDiceSelectedCallback);

/**
 *
 */
UCLASS()
class FARKLE_API UFarkleUtils : public UBlueprintFunctionLibrary
{
	GENERATED_BODY()

public:

	UFUNCTION(BlueprintCallable, Category = "Farkle")
	static TArray<FDiceCombination> GetBestDiceCombinations(const TArray<ADiceActor *> &Dices);

	UFUNCTION(BlueprintCallable, Category = "Farkle")
	static void GenerateCombinations(const TArray<ADiceActor *> &Dices, int32 Index, TArray<ADiceActor *> Current, TArray<FDiceCombination> &OutCombinations);

	UFUNCTION(BlueprintCallable, Category = "Farkle")
	static int32 CountScore(const TArray<ADiceActor *> &Dices);

	UFUNCTION(BlueprintCallable, Category = "Farkle")
	static bool CanScore(const TArray<ADiceActor *> &Dices);

	UFUNCTION(BlueprintCallable, Category = "Farkle")
	static void SelectDicesWithDelay(const TArray<ADiceActor *> &Dices, float Delay, FDiceSelectedCallback Callback);
};
