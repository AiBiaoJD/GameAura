// Copyright ABiao


#include "My_UI/Widget/My_AuraUserWidget.h"

void UMy_AuraUserWidget::SetWidgetController(UObject* InWidgetController)
{
	WidgetController = InWidgetController;
	WidgetControllerSet();
}
