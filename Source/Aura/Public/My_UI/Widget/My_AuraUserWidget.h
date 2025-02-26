// Copyright ABiao

#pragma once

#include "CoreMinimal.h"
#include "Blueprint/UserWidget.h"
#include "My_AuraUserWidget.generated.h"

/**
 * 
 */
UCLASS()
class AURA_API UMy_AuraUserWidget : public UUserWidget
{
	GENERATED_BODY()
	/*
	����WidgetController
	widget -> widgetController ,
	����������Ҫ֪���Ǹ�widget��������ֻ��Ҫ�㲥��ȥ����
	widget��Ҫ֪�����Ŀ�������ʲô
	*/
public:
	/*
	 *������������� C++ ��ʵ�֣���ͼֻ�ܵ������������޸�����ʵ�֡�
	 *����ϣ���ṩһ���̶��Ĺ��ܽӿ�WidgetController����ͼʹ��,
	 */
	UFUNCTION(BlueprintCallable)
	void SetWidgetController(UObject* InWidgetController);

	UPROPERTY(BlueprintReadOnly)
	TObjectPtr<UObject> WidgetController;
protected:
	/*
	 ����һ����ͼʵ�ֵ��¼���Ŀ���ǵ���������WidgetController�����󣬻�����ͼ��������¼�
	 ��������ͼ�и��� UI�����¼���ִ��������ʼ������
	 TODO:������ֽ��� Widget �� WidgetController
	 Widget ��Ҫ֪������ WidgetController���� WidgetController ����Ҫ֪������� Widget
	 ͨ�� SetWidgetController �� WidgetControllerSet��Widget ������ WidgetController �����ú�ִ���Զ����߼���������Ҫ WidgetController ֱ�Ӳ��� Widget��
	 */
	UFUNCTION(BlueprintImplementableEvent)
	void WidgetControllerSet();
};
