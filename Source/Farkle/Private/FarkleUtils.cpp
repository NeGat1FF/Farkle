// Fill out your copyright notice in the Description page of Project Settings.

#include "FarkleUtils.h"
#include "DiceActor.h"

TArray<FDiceCombination> UFarkleUtils::GetBestDiceCombinations(const TArray<ADiceActor *> &Dices)
{
	TArray<FDiceCombination> Combinations;
	GenerateCombinations(Dices, 0, TArray<ADiceActor *>(), Combinations);
	Combinations = Combinations.FilterByPredicate([](const FDiceCombination &Combination)
												  { return Combination.Score > 0; });
	int TotalCombinations = Combinations.Num();

	for (FDiceCombination &Combination : Combinations)
	{
		Combination.Probability = 1.0f / TotalCombinations;
		Combination.WeightedScore = Combination.Score * Combination.Probability;
	}

	Combinations.Sort([](const FDiceCombination &A, const FDiceCombination &B)
					  { return A.WeightedScore > B.WeightedScore; });

	return Combinations;
}

void UFarkleUtils::GenerateCombinations(const TArray<ADiceActor *> &Dices, int32 Index, TArray<ADiceActor *> Current, TArray<FDiceCombination> &OutCombinations)
{
	if (Index == Dices.Num())
	{
		OutCombinations.Add(FDiceCombination(Current, CountScore(Current)));
		return;
	}

	GenerateCombinations(Dices, Index + 1, Current, OutCombinations);

	Current.Add(Dices[Index]);
	GenerateCombinations(Dices, Index + 1, Current, OutCombinations);
	Current.RemoveAt(Current.Num() - 1);
}

int32 UFarkleUtils::CountScore(const TArray<ADiceActor *> &Dices)
{
	int32 count[7]{0};

	for (ADiceActor *Dice : Dices)
	{
		count[Dice->GetValue()]++;
	}

	int32 score = 0;

	for (int32 i = 1; i <= 6; i++)
	{
		if (count[i] == 3)
		{
			if (i == 1)
			{
				score += 1000;
			}
			else
			{
				score += i * 100;
			}
		}
		else if (count[i] >= 4)
		{
			if (i == 1)
			{
				score += 1000 * (int32)std::pow(2.0f, count[i] - 3);
			}
			else
			{
				score += i * 100 * (int32)std::pow(2.0f, count[i] - 3);
			}
		}
		else
		{

			if (i == 1)
			{
				score += count[i] * 100;
			}
			else if (i == 5)
			{
				score += count[i] * 50;
			}
		}
	}

	// 1 to 6 straight
	if (count[1] == 1 && count[2] == 1 && count[3] == 1 && count[4] == 1 && count[5] == 1 && count[6] == 1)
	{
		return 1500;
	}

	// 1 to 5 straight
	if (count[1] != 0 && count[2] == 1 && count[3] == 1 && count[4] == 1 && count[5] != 0 && count[6] == 0)
	{
		return 500 + (count[1] - 1) * 100 + (count[5] - 1) * 50;
	}

	// 2 to 6 straight
	if (count[2] == 1 && count[3] == 1 && count[4] == 1 && count[5] != 0 && count[6] == 1)
	{
		return 750 + (count[5] - 1) * 50;
	}

	if ((count[6] != 0 && count[6] < 3) || (count[4] != 0 && count[4] < 3) || (count[3] != 0 && count[3] < 3) || (count[2] != 0 && count[2] < 3))
	{
		return 0;
	}

	return score;
}

bool UFarkleUtils::CanScore(const TArray<ADiceActor *> &Dices)
{
	int count[7]{0};

	int score = 0;

	// Count the occurrences of each dice value
	for (ADiceActor *Dice : Dices)
	{
		count[Dice->GetValue()]++;
	}

	if (count[1] != 0 || count[5] != 0)
	{
		return true;
	}

	return count[2] >= 3 || count[3] >= 3 || count[4] >= 3 || count[6] >= 3;
}

void UFarkleUtils::SelectDicesWithDelay(const TArray<ADiceActor *> &Dices, float Delay, FDiceSelectedCallback Callback)
{
	float Increment = Delay;
	for (ADiceActor *Dice : Dices)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([Dice]()
								 { Dice->Select(); });
		Dice->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay, false);
		Delay += Increment;
	}

	if (Dices.Num() != 0)
	{
		FTimerHandle TimerHandle;
		FTimerDelegate TimerDelegate;
		TimerDelegate.BindLambda([Callback]()
								 { Callback.ExecuteIfBound(); });
		Dices[0]->GetWorldTimerManager().SetTimer(TimerHandle, TimerDelegate, Delay, false);
	}
}
