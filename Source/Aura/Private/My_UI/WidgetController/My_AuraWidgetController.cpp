// Copyright ABiao


#include "My_UI/WidgetController/My_AuraWidgetController.h"

void UMy_AuraWidgetController::SetWidgetControllerParams(const FMY_WidgetControllerParams& WCParams)
{
	PlayerController = WCParams.PlayerController;
	PlayerState = WCParams.PlayerState;
	AbilitySystemComponent = WCParams.AbilitySystemComponent;
	AttributeSet = WCParams.AttributeSet;
}
