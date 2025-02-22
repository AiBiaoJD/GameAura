// Copyright ABiao


#include "My_Controler/My_Aura_Controller.h"

#include "EnhancedInputSubsystems.h"

AMy_Aura_Controller::AMy_Aura_Controller()
{
	bReplicates = true;
}

void AMy_Aura_Controller::BeginPlay()
{
	Super::BeginPlay();

	check(AuraContext);

	UEnhancedInputLocalPlayerSubsystem* Subsystem = ULocalPlayer::GetSubsystem<>()
}
