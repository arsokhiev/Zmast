// ZMAST, All Rights Reserved

#pragma once

class AnimUtils
{
public:
	template<typename T>
	static T* FindNotifyByClass(UAnimSequenceBase* Animation)
	{
		if (Animation)
		{
			const TArray<FAnimNotifyEvent> NotifyEvents = Animation->Notifies;
			for (auto NotifyEvent : NotifyEvents)
			{
				if (auto AnimNotify = Cast<T>(NotifyEvent.Notify))
				{
					return AnimNotify;
				}
			}
		}
		return nullptr;
	}
};