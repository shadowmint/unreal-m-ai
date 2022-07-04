// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

#include "Components/SplineComponent.h"
#include "MAiFeatures/2D/PathFinding/MAi_2D_PathNode.h"
#include "MGraphs/MGraph.h"

struct MAI_API UMAi_2D_SplineGraphVertex
{
	/** Which spline is this point on? */
	TWeakObjectPtr<USplineComponent> Spline;

	/** If there are any nodes associated with this point, put them here */
	TArray<TWeakObjectPtr<UMAi_2D_PathNode>> Nodes;

	/** The offset into this spline */
	int SplinePoint;

	/** The exact point of this vertex */
	FVector Position;
};

struct MAI_API FMAi_2D_SplineGraphPackParams
{
	/** When a spline node is closer than this to another spline, bridge between the splines */
	float CrossSplineBridgeThreshold;

	/** When a node is closer than this to a spline point, bind the node to the graph vertex */
	float NodeAssociationThreshold;
};

class MAI_API FMAi_2D_SplineGraph
{
private:
	/** The internal graph representation */
	TUniquePtr<TMGraph<UMAi_2D_SplineGraphVertex>> Graph;

	/** The set of splines we're tracking on this graph */
	TArray<TWeakObjectPtr<USplineComponent>> Splines;

	/** The set of nodes we're tracking on this graph */
	TArray<TWeakObjectPtr<UMAi_2D_PathNode>> Nodes;

	/** Does this graph need to be packed? */
	bool Dirty = true;

public:
	FMAi_2D_SplineGraph();

	/** Add a spline to this graph */
	void Add(USplineComponent* Spline);

	/** Add a path node to this graph */
	void Add(UMAi_2D_PathNode* Node);
	
	/** Add a set of splines */
	void AddRange(TArray<USplineComponent*> InSplines);
	
	/** Add a set of nodes */
	void AddRange(TArray<UMAi_2D_PathNode*> InNodes);

	int NumSplines() const;

	int NumNodes() const;

	/** Return the default pack params */
	static FMAi_2D_SplineGraphPackParams DefaultParams();

	/** Convert the held splines into a spline graph */
	void Pack();

	/** Convert the held splines into a spline graph */
	void Pack(FMAi_2D_SplineGraphPackParams Params);

	/** Find a path over the graph */
	bool Path(FVector FromPoint, FVector ToPoint, TArray<FMGraphEdge<UMAi_2D_SplineGraphVertex>*> &Path) const;

public:
	/** Graph API */
	static float GetCost(UMAi_2D_SplineGraphVertex* A, UMAi_2D_SplineGraphVertex* B);

private:
	void CollectNativeSplineVertexData(FMAi_2D_SplineGraphPackParams& Params);

	void CollectCrossSplineBridges(const FMAi_2D_SplineGraphPackParams& Params) const;

	FMGraphVertex<UMAi_2D_SplineGraphVertex>* AddGraphVertex(TWeakObjectPtr<USplineComponent>& Spline, int Offset,
	                                                         FMAi_2D_SplineGraphPackParams& Params) const;
};
