// Fill out your copyright notice in the Description page of Project Settings.

#include "MAiFeatures/2D/Internal/MAi_2D_SplineGraph.h"
#include "MGraphs/PathFinder/MGraphPathFinderDijkstra.h"

#define FMAi_2D_SplineGraph_DEBUG 0

FMAi_2D_SplineGraph::FMAi_2D_SplineGraph()
{
	FMGraphConfig<UMAi_2D_SplineGraphVertex> Config;
	Config.GetCost = &GetCost;

	Dirty = true;
	Graph = TMGraph<UMAi_2D_SplineGraphVertex>::Make(Config);
}

void FMAi_2D_SplineGraph::Add(USplineComponent* Spline)
{
	if (Splines.Contains(Spline)) return;
	Splines.Add(Spline);
	Dirty = true;
}

void FMAi_2D_SplineGraph::Add(UMAi_2D_PathNode* Node)
{
	if (Nodes.Contains(Node)) return;
	Nodes.Add(Node);
	Dirty = true;
}

void FMAi_2D_SplineGraph::AddRange(TArray<USplineComponent*> InSplines)
{
	for (const auto Spline : InSplines)
	{
		Add(Spline);
	}
}

void FMAi_2D_SplineGraph::AddRange(TArray<UMAi_2D_PathNode*> InNodes)
{
	for (const auto Node : InNodes)
	{
		Add(Node);
	}
}

int FMAi_2D_SplineGraph::NumSplines() const
{
	return Splines.Num();
}

int FMAi_2D_SplineGraph::NumNodes() const
{
	return Nodes.Num();
}

void FMAi_2D_SplineGraph::Pack()
{
	Pack(DefaultParams());
}

FMAi_2D_SplineGraphPackParams FMAi_2D_SplineGraph::DefaultParams()
{
	auto Params = FMAi_2D_SplineGraphPackParams();
	Params.NodeAssociationThreshold = 50;
	Params.CrossSplineBridgeThreshold = 50;

	return Params;
}

void FMAi_2D_SplineGraph::Pack(FMAi_2D_SplineGraphPackParams Params)
{
	if (!Dirty) return;
	Graph->Reset();
	CollectNativeSplineVertexData(Params);
	CollectCrossSplineBridges(Params);
	Dirty = false;
}

bool FMAi_2D_SplineGraph::Path(FVector FromPoint, FVector ToPoint, TArray<FMGraphEdge<UMAi_2D_SplineGraphVertex>*>& Path) const
{
	auto VertexList = Graph->GetVertexData();
	if (VertexList.Num() == 0) return false;

	auto const PathFinder = MakeUnique<TMGraphPathFinderDijkstra<UMAi_2D_SplineGraphVertex>>();

	FMGraphVertex<UMAi_2D_SplineGraphVertex>* FromNode = VertexList[0];
	FMGraphVertex<UMAi_2D_SplineGraphVertex>* ToNode = VertexList[0];

	auto FromNodeDist = (FromNode->VertexData.Position - FromPoint).SquaredLength();
	auto ToNodeDist = (FromNode->VertexData.Position - FromPoint).SquaredLength();

	// Find closest nodes
	for (auto VRef : VertexList)
	{
		auto D1 = (VRef->VertexData.Position - FromPoint).SquaredLength();
		if (D1 < FromNodeDist)
		{
			FromNodeDist = D1;
			FromNode = VRef;
		}

		auto D2 = (VRef->VertexData.Position - ToPoint).SquaredLength();
		if (D2 < ToNodeDist)
		{
			ToNodeDist = D2;
			ToNode = VRef;
		}
	}

	auto Rtn = PathFinder->FindPath(FromNode, ToNode, *Graph, Path);

#if FMAi_2D_SplineGraph_DEBUG
	UE_LOG(LogTemp, Warning, TEXT("FMAi_2D_SplineGraph::Path: Audit"));
	UE_LOG(LogTemp, Warning, TEXT("FMAi_2D_SplineGraph::Path: Path length: %d"), Path.Num());

	auto VL = Graph->GetVertexData();
	for (int i = 0; i < VL.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("FMAi_2D_SplineGraph::Path: Vertex: %p"), VL[i]);
	}

	for (int i = 0; i < Path.Num(); i++)
	{
		UE_LOG(LogTemp, Warning, TEXT("FMAi_2D_SplineGraph::Path: Path Element: %p"), Path[i]);
		for (int j = 0; j < VL.Num(); j++)
		{
			if (VL[j] == Path[i]->FromVertex)
			{
				UE_LOG(LogTemp, Warning, TEXT("FMAi_2D_SplineGraph::Path: --> FromVertex matched %d"), j);
			}
			if (VL[j] == Path[i]->ToVertex)
			{
				UE_LOG(LogTemp, Warning, TEXT("FMAi_2D_SplineGraph::Path: --> ToVertex matched %d"), j);
			}
		}
	}
#endif
	
	return Rtn;
}

float FMAi_2D_SplineGraph::GetCost(UMAi_2D_SplineGraphVertex* A, UMAi_2D_SplineGraphVertex* B)
{
	if (A == B) return 0;
	return (A->Position - B->Position).Length();
}

FMGraphVertex<UMAi_2D_SplineGraphVertex>* FMAi_2D_SplineGraph::AddGraphVertex(TWeakObjectPtr<USplineComponent>& Spline, int Offset,
                                                                              FMAi_2D_SplineGraphPackParams& Params) const
{
	TArray<TWeakObjectPtr<UMAi_2D_PathNode>> NodesForThisVertex;
	const auto Position = Spline->GetWorldLocationAtSplinePoint(Offset);

	for (auto const Node : Nodes)
	{
		if ((Node->GetWorldLocation() - Position).Length() <= Params.NodeAssociationThreshold)
		{
			NodesForThisVertex.Add(Node);
		}
	}

	return Graph->Add(UMAi_2D_SplineGraphVertex{
		Spline,
		NodesForThisVertex,
		Offset,
		Position,
	});
}

void FMAi_2D_SplineGraph::CollectNativeSplineVertexData(FMAi_2D_SplineGraphPackParams& Params)
{
	for (auto& Spline : Splines)
	{
		FMGraphVertex<UMAi_2D_SplineGraphVertex>* Current = nullptr;

		const auto Count = Spline.Get()->GetNumberOfSplinePoints();
		for (auto i = 0; i < Count; i++)
		{
			// Create a new node
			FMGraphVertex<UMAi_2D_SplineGraphVertex>* Prev = Current;
			Current = AddGraphVertex(Spline, i, Params);

			// Create a connection
			if (Prev != nullptr && Current != nullptr)
			{
				Graph->Connect(Prev, Current, true);
			}
		}
	}
}

/**
 * Any two vertexes which are close but not on the same spline should be connected via a bridge.
 * This is unavoidably an N^2 operation.
 **/
void FMAi_2D_SplineGraph::CollectCrossSplineBridges(const FMAi_2D_SplineGraphPackParams& Params) const
{
	for (auto V1 : Graph->GetVertexData())
	{
		for (auto V2 : Graph->GetVertexData())
		{
			if (V1->VertexData.Spline == V2->VertexData.Spline)
			{
				continue;
			}

			const auto Dist = (V1->VertexData.Position - V2->VertexData.Position).Length();
			if (Dist < Params.CrossSplineBridgeThreshold)
			{
				if (!Graph->Connected(V1, V2))
				{
#if FMAi_2D_SplineGraph_DEBUG
					UE_LOG(LogTemp, Display, TEXT("Created vertex bridge: %d <--> %d"), V1->Id, V2->Id);
#endif
					Graph->Connect(V1, V2, true);
				}
			}
		}
	}
}
