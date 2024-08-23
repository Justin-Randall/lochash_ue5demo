#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "lochash/lochash.hpp"
#include "lochash/location_hash.hpp"
#include "lochash/location_hash_query_distance_squared.hpp"
#include "lochash/location_hash_quantized_coordinate.hpp"

// ---------------------------------------------------------
// Some helpers/wrappers to make the interface look and feel
// more like Unreal Engine.
// ---------------------------------------------------------


// Must be a power of 2 for the location hash to work correctly. There is a compile-time check for this.
// Set to 1024 for this demo. 2048 may also be a good choice.
// For perf testing, take it down to 128.
constexpr size_t HashPrecision = 1024;

struct UQuantizedCoordinate : public lochash::QuantizedCoordinate<HashPrecision, double, 3>
{
	UQuantizedCoordinate(const lochash::QuantizedCoordinate<HashPrecision, double, 3>& Coordinate)
		: lochash::QuantizedCoordinate<HashPrecision, double, 3>(Coordinate)
	{
	}
};

class FLocationHash : public lochash::LocationHash<HashPrecision, double, 3, AActor>
{
public:
	TArray<UQuantizedCoordinate> Add(AActor& Object)
	{
		const auto Location = Object.GetActorLocation();
		const FLocationHash::CoordinateArray Center = { Location.X, Location.Y, Location.Z };
		const auto Sphere = Object.GetRootComponent()->Bounds.GetSphere();
		const auto Keys = add(&Object, Center, Sphere.W);

		TArray<UQuantizedCoordinate> Results;
		for (const auto& Key : Keys)
		{
			Results.Add(Key);
		}

		return Results;
	}

	TArray<UQuantizedCoordinate> Move(AActor& Object, const FVector& NewLocation)
	{
		const auto OldLocation = Object.GetActorLocation();
		const FLocationHash::CoordinateArray OldCenter = { OldLocation.X, OldLocation.Y, OldLocation.Z };
		const FLocationHash::CoordinateArray NewCenter = { NewLocation.X, NewLocation.Y, NewLocation.Z };
		const auto Sphere = Object.GetRootComponent()->Bounds.GetSphere();
		const auto Keys = move(&Object, Sphere.W, OldCenter, NewCenter);

		TArray<UQuantizedCoordinate> Results;
		for (const auto& Key : Keys)
		{
			Results.Add(Key);
		}

		return Results;
	}

	TArray<AActor*> Query(const TArray<UQuantizedCoordinate>& Keys)
	{
		TArray<AActor*> Results;
		const auto& Data = get_data();
		for (const auto& Key : Keys)
		{
			const auto it = Data.find(Key);
			if (it != Data.end())
			{
				auto& Bucket = it->second;
				for (const auto& item : Bucket)
				{
					Results.Add(item.second);
				}
			}
		}
		return Results;
	}
};

inline uint32_t GetTypeHash(const UQuantizedCoordinate Coordinate)
{
	std::hash<lochash::QuantizedCoordinate<HashPrecision, double, 3>> hasher;
	uint32_t result = hasher(Coordinate);
	return result;
}
