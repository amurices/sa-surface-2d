//
//  Interfacer.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Interfacer.hpp"
void Interfacer::generate_random(SurfaceData_t &surf, double perim, int pts){
	std::vector<point_t> pointsCluster;
	std::vector<point_t> pointsSurface;
	
	for (size_t i = 0; i < pts; i++)
	{
		double randX = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) - 0.5;
		double randY = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) - 0.5;
		
		// Digamos que o cluster de pontos seja organizado numa área de metade do perímetro
		randX *= 0.5 * perim; randY *= 0.5 * perim;
		
		// Empurramos cada ponto do cluster a um vetor
		pointsCluster.push_back(point_t(randX, randY));
	}
	
	pointsSurface = convex_hull(pointsCluster);
	
	// Adicionamos um nó ao grafo, e as suas coordenadas ao mapa
	SNode fnode = surf.graph.addNode();
	SNode prevToMap = fnode;
	SNode currToMap;
	
	
	
	surf.nNodes++;
	( *(surf.coords) )[prevToMap] = point_t(pointsSurface[0].x, pointsSurface[0].y);
	std::cout << "Node we're adding: " << (*surf.coords)[prevToMap] << std::endl;

	for (size_t i = 1; i < pointsSurface.size(); i++)
	{
		// Adicionamos um nó ao grafo, e as suas coordenadas ao mapa
		currToMap = surf.graph.addNode();
		surf.nNodes++;
		(*surf.coords)[currToMap] = point_t(pointsSurface[i].x, pointsSurface[i].y);
		std::cout << "Node we're adding: " << (*surf.coords)[currToMap] << std::endl;
		surf.graph.addArc(prevToMap, currToMap);
		surf.nEdges++;
		prevToMap = currToMap;
	}
	
	std::cout << "Rn we have " << surf.nNodes << " nodes\n";
	
	surf.graph.addArc(prevToMap, fnode);
	surf.nEdges++;
	prevToMap = currToMap;
	
	
}

void Interfacer::generate_inner(SurfaceData_t &inner, const SurfaceData_t &surf)
{
	SNode	prev, next, last;
	point_t	pPrev, pNext, pCurr, vd;

	SNode fnode		= surf.graph.nodeFromId(0);				// fnode = P0
	
	ListDigraph::InArcIt	inCurrI(surf.graph, fnode);		// get Pn
	ListDigraph::OutArcIt	outCurrI(surf.graph, fnode);	// and P1
	
	prev = surf.graph.source(inCurrI);		// cont ^
	next = surf.graph.target(outCurrI);		// cont ^
	
	pNext = (*surf.coords)[next];			// get coordinates Pn
	pPrev = (*surf.coords)[prev];			// and P0


	vd = find_direction_vector(pPrev, pNext);	// Pega direção
	vd *= 0.02;									// proto-thickness
	
	
	SNode finode = inner.graph.addNode();	// Add node à inner; guarda a referencia ao primeiro no
	SNode innerCurrToMap = finode;			// Primeiro no agora é o atual
	inner.nNodes++;							// incrementa no. de nós
	(*inner.coords)[innerCurrToMap] = (*surf.coords)[fnode] - vd;	// seta coordenadas
	SNode innerPrevToMap = innerCurrToMap;							// prev agora é o atual
	SNode curr; int count = 0;
	for (curr = next; curr != fnode; curr = next)
	{
		
		ListDigraph::OutArcIt	outCurr(surf.graph, curr);
		ListDigraph::InArcIt	inCurr(surf.graph, curr);
	
		next = surf.graph.target(outCurr);
		prev = surf.graph.source(inCurr);
		
		pNext = (*surf.coords)[next];
		pPrev = (*surf.coords)[prev];
		
		vd = find_direction_vector(pPrev, pNext);
		vd *= 0.02;
		
		innerCurrToMap = inner.graph.addNode();
		inner.nNodes++;
		(*inner.coords)[innerCurrToMap] = (*surf.coords)[curr] - vd;
		
		inner.graph.addArc(innerPrevToMap, innerCurrToMap);
		inner.nEdges++;
		innerPrevToMap = innerCurrToMap;
	}
	
	inner.graph.addArc(innerPrevToMap, finode);
	inner.nEdges++;
}

void Interfacer::get_from_matlab(SurfaceData_t &surf, const char* matFile)
{
	/*
	const char *filename = "myfile.mat";
	mat_t *matfp = NULL; //matfp contains pointer to MAT file or NULL on failure
	matfp = Mat_CreateVer(filename, NULL, MAT_FT_MAT5); //or MAT_FT_MAT4 / MAT_FT_MAT73
	//don't forget to close file with Mat_Close(matfp);
	Mat_Close(matfp);
	*/
	mat_t		*thisMat;
	matvar_t	*matvar;
	
	thisMat = Mat_Open(matFile, MAT_ACC_RDONLY);
	if ( NULL == thisMat ) {
		printf("Error opening MAT file %s\n", matFile);
	}
	matvar = Mat_VarReadNext(thisMat);
	printf("%-20s", matvar->name);
	if ( NULL != matvar ) {
		Mat_VarPrint(matvar, 1);
		Mat_VarFree(matvar);
	}
}

