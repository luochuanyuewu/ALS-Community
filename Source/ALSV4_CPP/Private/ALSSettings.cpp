// Fill out your copyright notice in the Description page of Project Settings.


#include "ALSSettings.h"

const UALSSettings* UALSSettings::Get()
{
	return GetMutableDefault<UALSSettings>();
}
