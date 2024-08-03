#pragma once

#include "CoreMinimal.h"
#include "GameFramework/Actor.h"

#include "lochash.hpp"
#include "location_hash.hpp"
#include "location_hash_query_distance_squared.hpp"

// Must be a power of 2 for the location hash to work correctly. There is a compile-time check for this.
constexpr size_t HashPrecision = 256;

class FLocationHash : public lochash::LocationHash<HashPrecision, double, 3, AActor>
{
public:

	TArray<size_t> Add(AActor& Object)
	{
		const auto Location = Object.GetActorLocation();
		const FLocationHash::CoordinateArray Center = { Location.X, Location.Y, Location.Z };
		const auto Sphere = Object.GetRootComponent()->Bounds.GetSphere();
		const auto Keys = add(&Object, Center, Sphere.W);

		TArray<size_t> Results;
		for (const auto& Key : Keys)
		{
			Results.Add(Key);
		}

		return Results;
	}

	TArray<size_t> Move(AActor& Object, const FVector& NewLocation)
	{
		const auto OldLocation = Object.GetActorLocation();
		const FLocationHash::CoordinateArray OldCenter = { OldLocation.X, OldLocation.Y, OldLocation.Z };
		const FLocationHash::CoordinateArray NewCenter = { NewLocation.X, NewLocation.Y, NewLocation.Z };
		const auto Sphere = Object.GetRootComponent()->Bounds.GetSphere();
		const auto Keys = move(&Object, Sphere.W, OldCenter, NewCenter);

		TArray<size_t> Results;
		for (const auto& Key : Keys)
		{
			Results.Add(Key);
		}

		//Object.SetActorLocation(NewLocation);
		return Results;
	}

	TArray<AActor*> Query(const TArray<size_t>& Keys)
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

	// Debug draw helper to visually demonstrate the location hash by collecting all quantized coordinates within distance of a point.
	// This will be later used to draw debug boxes around the quantized coordinates, which represent non-empty buckets.
	// This should be fed to a GameMode where duplicates are removed to prevent overdraw when rendering the debug boxes.
	//
	// TODO: There is a bug here where the quantized coordinates are offset by one unit in the wrong direction on each axis and the debug boxes are not drawn correctly.
	TArray<FVector> GetQuantizedCoordinatesWithinRange(const AActor& Object)
	{
		const auto Location = Object.GetActorLocation();
		const FLocationHash::CoordinateArray Center = { Location.X, Location.Y, Location.Z };
		const auto Radius = Object.GetRootComponent()->Bounds.GetSphere().W;
		const FLocationHash::CoordinateArray MinCorner = { Location.X - Radius, Location.Y - Radius, Location.Z - Radius };
		const FLocationHash::CoordinateArray MaxCorner = { Location.X + Radius, Location.Y + Radius, Location.Z + Radius };

		TArray<FVector> Results;

		// cut-and-waste code from location_hash_algorithm.hpp
		std::array<size_t, 3> Steps = { 0 };
		constexpr size_t PrecisionShift = lochash::calculate_precision_shift<HashPrecision>();
		for (size_t i = 0; i < 3; ++i) {
			Steps[i] = ((lochash::quantize_value<double, HashPrecision>(MaxCorner[i]) -
				lochash::quantize_value<double, HashPrecision>(MinCorner[i])) >>
				PrecisionShift) + 1;
		}

		std::array<size_t, 3> Indices = { 0 };
		bool                           Done = false;

		while (!Done) {
			std::array<double, 3> CurrentCoords;
			for (size_t i = 0; i < 3; ++i) {
				CurrentCoords[i] = lochash::quantize_value<double, HashPrecision>(MinCorner[i] + static_cast<double>(Indices[i] << PrecisionShift));
			}

			// add to the Results array
			Results.Add(FVector(CurrentCoords[0], CurrentCoords[1], CurrentCoords[2]));

			// Increment the indices array to generate the next coordinate in the range.
			for (size_t i = 0; i < 3; ++i) {
				if (++Indices[i] < Steps[i]) {
					break;
				}
				Indices[i] = 0;
				if (i == 3 - 1) {
					Done = true;
				}
			}
		}

		return Results;
	}
};
