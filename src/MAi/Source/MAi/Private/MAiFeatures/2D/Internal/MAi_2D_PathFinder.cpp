// Fill out your copyright notice in the Description page of Project Settings.


#include "MAiFeatures/2D/Internal/MAi_2D_PathFinder.h"

#include "Utils/MComponent.h"

void UMAi_2D_PathFinder::Initialize()
{
	Graph = FMAi_2D_SplineGraph();
}

FMAi_2D_Path UMAi_2D_PathFinder::FindPath(FVector FromPoint, FVector ToPoint)
{
	FMAi_2D_Path StructuredPath;
	TArray<FMGraphEdge<UMAi_2D_SplineGraphVertex>*> Edges;

	// Find a path through our collected splines
	const auto FoundValidPath = Graph.Path(FromPoint, ToPoint, Edges);
	if (!FoundValidPath)
	{
		StructuredPath.IsValidPath = false;
		return StructuredPath;
	}

	// If this is a zero length path; ie. path is Here -> A -> A -> There, then only do the single trivial linear step.
	if (Edges.Num() == 0)
	{
		StructuredPath.Steps.Add(FMAi_2D_PathStep(FromPoint, ToPoint));
		StructuredPath.IsValidPath = true;
		return StructuredPath;
	}

	// Add From -> first node
	StructuredPath.Steps.Add(FMAi_2D_PathStep(FromPoint, Edges[0]->FromVertex->VertexData.Position));

	// Add each step
	for (auto const Step : Edges)
	{
		// If this is a bridge node, it's a linear step, otherwise it's a spline step
		if (Step->FromVertex->VertexData.Spline != Step->ToVertex->VertexData.Spline)
		{
			StructuredPath.Steps.Add(FMAi_2D_PathStep(Step->FromVertex->VertexData.Position, Step->ToVertex->VertexData.Position));
		}
		else
		{
			auto const Spline = Step->FromVertex->VertexData.Spline.Get();
			auto const A = Spline->GetDistanceAlongSplineAtSplinePoint(Step->FromVertex->VertexData.SplinePoint);
			auto const B = Spline->GetDistanceAlongSplineAtSplinePoint(Step->ToVertex->VertexData.SplinePoint);
			UE_LOG(LogTemp, Warning, TEXT("Spline: %s:%s"), *Spline->GetOwner()->GetName(), *Spline->GetName());
			StructuredPath.Steps.Add(FMAi_2D_PathStep(Spline, A, B));
		}
	}

	// Add last node -> To
	StructuredPath.Steps.Add(FMAi_2D_PathStep(Edges[Edges.Num() - 1]->ToVertex->VertexData.Position, ToPoint));

	StructuredPath.IsValidPath = true;
	return StructuredPath;
}

void UMAi_2D_PathFinder::Pack()
{
	Graph.Pack();
}

void UMAi_2D_PathFinder::DiscoverSplinesAndNodes(AActor* Actor)
{
	TArray<USplineComponent*> Splines;
	TArray<UMAi_2D_PathNode*> Nodes;
	FMComponent::GetComponents<USplineComponent>(Actor, Splines);
	FMComponent::GetComponents<UMAi_2D_PathNode>(Actor, Nodes);
	Graph.AddRange(Splines);
	Graph.AddRange(Nodes);
}

UMAi_2D_PathFinder* UMAi_2D_PathFinder::MakePathFinder(UObject* WorldContextObject)
{
	const auto Instance = NewObject<UMAi_2D_PathFinder>(WorldContextObject);
	Instance->Initialize();
	return Instance;
}

int UMAi_2D_PathFinder::NumNodes() const
{
	return Graph.NumNodes();
}

int UMAi_2D_PathFinder::NumSplines() const
{
	return Graph.NumSplines();
}
