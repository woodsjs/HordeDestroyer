// Fill out your copyright notice in the Description page of Project Settings.


#include "HDPlayerState.h"

void AHDPlayerState::AddScore(float ScoreDelta)
{
	SetScore(GetScore() + ScoreDelta);
}
