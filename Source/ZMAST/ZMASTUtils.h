// ZMAST, All Rights Reserved

#pragma once

class ZMASTUtils
{
public:
	template <typename T>
	static T* GetZMASTPlayerComponent(AActor* PlayerPawn)
	{
		if (!PlayerPawn) return nullptr;
		return Cast<T>(PlayerPawn->GetComponentByClass(T::StaticClass()));
	}
};
