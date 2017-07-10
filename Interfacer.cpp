//
//  Interfacer.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Interfacer.hpp"
void Interfacer::generate_random(ThickSurface_t &ts, double perim, int pts, std::vector<point_t> &is)
{
	generate_outer_s(ts.outer, perim, pts, is);
	ts.thickness = 0.02; // placeholder
	generate_inner_s(ts.inner, ts.outer, ts.thickness);
}

void Interfacer::generate_outer_s(SurfaceData_t &surf, double perim, int pts, std::vector<point_t> &is){
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

	pointsSurface = convex_hull(pointsCluster, is);
	
	// Adicionamos um nó ao grafo, e as suas coordenadas ao mapa
	SNode fnode = surf.graph.addNode();
	SNode prevToMap = fnode;
	SNode currToMap;
	
	surf.nNodes++;
	( *(surf.coords) )[prevToMap] = point_t(pointsSurface[0].x, pointsSurface[0].y);
//	std::cout << "Node we're adding: " << (*surf.coords)[prevToMap] << std::endl;

	for (size_t i = 1; i < pointsSurface.size(); i++)
	{
		// Adicionamos um nó ao grafo, e as suas coordenadas ao mapa
		currToMap = surf.graph.addNode();
		surf.nNodes++;
		(*surf.coords)[currToMap] = point_t(pointsSurface[i].x, pointsSurface[i].y);
	//	std::cout << "Node we're adding: " << (*surf.coords)[currToMap] << std::endl;
		surf.graph.addArc(prevToMap, currToMap);
		surf.nEdges++;
		prevToMap = currToMap;
	}
	
//	std::cout << "Rn we have " << surf.nNodes << " nodes\n";
	
	surf.graph.addArc(prevToMap, fnode);
	surf.nEdges++;
	prevToMap = currToMap;
	
	
}

void Interfacer::generate_inner_s(SurfaceData_t &inner, SurfaceData_t &surf, double thickness)
{
	if (inner.nNodes > 0) // Anti-memory leakage
	{
		inner.nNodes = 0;
		inner.nEdges = 0;
		inner.graph.clear();
	}
	
	
	SNode	prev, next, last;
	point_t	pPrev, pNext, pCurr, vd;

	SNode fnode		= surf.graph.nodeFromId(0);				// fnode = P0
	
	ListDigraph::InArcIt	inCurrI(surf.graph, fnode);		// get Pn
	ListDigraph::OutArcIt	outCurrI(surf.graph, fnode);	// and P1
	
	prev = surf.graph.source(inCurrI);		// cont ^
	next = surf.graph.target(outCurrI);		// cont ^
	
	pNext = (*surf.coords)[next];			// get coordinates Pn
	pPrev = (*surf.coords)[prev];			// and P0


	vd = find_direction_vector(pPrev, pNext, (*surf.coords)[fnode], MEDIAN_ANGLE);	// Pega direção
	vd *= thickness;
	
	
	SNode finode = inner.graph.addNode();	// Add node à inner; guarda a referencia ao primeiro no
	SNode innerCurrToMap = finode;			// Primeiro no agora é o atual
	inner.nNodes++;							// incrementa no. de nós
	(*inner.coords)[innerCurrToMap] = (*surf.coords)[fnode] - vd;	// seta coordenadas
	SNode innerPrevToMap = innerCurrToMap;							// prev agora é o atual
	
	inner.correspondence[inner.graph.id(finode)] = surf.graph.id(fnode);		// Set
	surf.correspondence[surf.graph.id(fnode)] = inner.graph.id(finode);		// Correspondences
	
	SNode curr; int count = 0;
	for (curr = next; curr != fnode; curr = next)
	{
		ListDigraph::OutArcIt	outCurr(surf.graph, curr);
		ListDigraph::InArcIt	inCurr(surf.graph, curr);
	
		next = surf.graph.target(outCurr);
		prev = surf.graph.source(inCurr);
		
		pNext = (*surf.coords)[next];
		pPrev = (*surf.coords)[prev];
		
		vd = find_direction_vector(pPrev, pNext, (*surf.coords)[curr], MEDIAN_ANGLE);
		vd *= thickness;
		
		innerCurrToMap = inner.graph.addNode();
		inner.nNodes++;
		(*inner.coords)[innerCurrToMap] = (*surf.coords)[curr] - vd;
		
		inner.correspondence[inner.graph.id(innerCurrToMap)] = surf.graph.id(curr);		// Set
		surf.correspondence[surf.graph.id(curr)] = inner.graph.id(innerCurrToMap);		// Correspondences
		
		inner.graph.addArc(innerPrevToMap, innerCurrToMap);
		inner.nEdges++;
		innerPrevToMap = innerCurrToMap;
	}
	
	inner.graph.addArc(innerPrevToMap, finode);
	inner.nEdges++;
}

void Interfacer::update_inner_s(SurfaceData_t &inner,  SurfaceData_t &surf, double thickness)
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
	
	
	vd = find_direction_vector(pPrev, pNext, (*surf.coords)[fnode], MEDIAN_ANGLE);	// Pega direção
	vd *= thickness;									// proto-thickness
	
	
	SNode finode = inner.graph.nodeFromId(surf.correspondence[0]); // Updating
	SNode innerCurrToMap = finode;			// Primeiro no agora é o atual
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
		
		vd = find_direction_vector(pPrev, pNext, (*surf.coords)[curr], MEDIAN_ANGLE);
		vd *= thickness;
		
		
		std::cout << "Correspondence in inner(" << inner.graph.id(innerCurrToMap) << "): " <<inner.correspondence[inner.graph.id(innerCurrToMap)] << std::endl;
		
		std::cout << "Correspondence in outer(" << surf.graph.id(curr) << "): " << surf.correspondence[surf.graph.id(curr)] << std::endl << std::endl;
		
		inner.correspondence[inner.graph.id(innerCurrToMap)] = surf.graph.id(curr);		// Set
		surf.correspondence[surf.graph.id(curr)] = inner.graph.id(innerCurrToMap);		// Correspondences
		
		innerCurrToMap = inner.graph.nodeFromId(surf.correspondence[surf.graph.id(curr)]);
		(*inner.coords)[innerCurrToMap] = (*surf.coords)[curr] - vd;
		innerPrevToMap = innerCurrToMap;
	}
}

void Interfacer::update_inner_node(SurfaceData_t &inner,  SurfaceData_t &surf, double thicks, int index)
{
	SNode updatedOuterNode	= surf.graph.nodeFromId(index);
	int corrInnerIndex		= surf.correspondence[index];
	SNode corrInnerNode		= inner.graph.nodeFromId(corrInnerIndex);	// innerNode = corrs[index]
	
	ListDigraph::InArcIt	inCurrI(surf.graph, updatedOuterNode);	// get P(index - 1) and P(index + 1)
	ListDigraph::OutArcIt	outCurrI(surf.graph, updatedOuterNode);	// ^ neighbors of correspondent
	
	SNode prev = surf.graph.source(inCurrI);		// Get neighbor nodes
	SNode next = surf.graph.target(outCurrI);		// ^
	
	point_t pNext = (*surf.coords)[next];			// Get coordinates of neighbor nodes
	point_t pPrev = (*surf.coords)[prev];			// ^
	
	point_t vd = find_direction_vector(pPrev, pNext, (*surf.coords)[updatedOuterNode], MEDIAN_ANGLE);	// Get direction
	vd *= thicks;									// After normalized, mult. by thickness of that point
	
	(*inner.coords)[corrInnerNode] = (*surf.coords)[updatedOuterNode] - vd; // Actually update this one node
}

void Interfacer::generate_circle(ThickSurface_t &ts, double radius, int pts, std::vector<point_t> &is)
{
	// (1) Add one node to the graph, and its coordinates to the map
	SNode fnode = ts.outer.graph.addNode();
	SNode prevToMap = fnode;
	SNode currToMap;
	
	ts.outer.nNodes++;
	( *(ts.outer.coords) )[prevToMap] = point_t(radius, 0);
	
	for (size_t i = 1; i < pts; i++)
	{
		// Do (1) again, at the point resolution provided (number of pts)
		currToMap = ts.outer.graph.addNode();
		ts.outer.nNodes++;
		(*ts.outer.coords)[currToMap] = point_t(cos(i * (2 * PI) / pts) * radius, sin(i * (2 * PI) / pts) * radius);
		ts.outer.graph.addArc(prevToMap, currToMap);
		ts.outer.nEdges++;
		prevToMap = currToMap;
	}
	
		std::cout << "Rn we have " << ts.outer.nNodes << " nodes\n";
	
	ts.outer.graph.addArc(prevToMap, fnode);
	ts.outer.nEdges++;
	prevToMap = currToMap;
	
	
	ts.thickness = 0.27 * radius; // placeholder
	generate_inner_s(ts.inner, ts.outer, ts.thickness);
	generate_bridges(ts);
}

void Interfacer::generate_bridges(ThickSurface_t &ts)
{
	if (ts.bridges.nNodes > 0) // Anti-memory leakage
	{
		ts.bridges.nNodes = 0;
		ts.bridges.nEdges = 0;
		ts.bridges.graph.clear();
	}
	for (ListDigraph::NodeIt no(ts.outer.graph); no != INVALID; ++no)
	{
		SNode curr = ts.bridges.graph.addNode();
		SNode pair = ts.bridges.graph.addNode();
		ts.bridges.nNodes++;
		ts.bridges.nNodes++;

		ts.bridges.graph.addArc(curr, pair);
		ts.bridges.nEdges++;
		
		(*ts.bridges.coords)[curr] = (*ts.outer.coords)[no];
		int innerId = ts.outer.correspondence[ts.outer.graph.id(no)];
		(*ts.bridges.coords)[pair] = (*ts.inner.coords)[ ts.outer.graph.nodeFromId(innerId) ];
	}
}

void Interfacer::update_bridges(ThickSurface_t &ts)
{
	ListDigraph::NodeIt curr(ts.bridges.graph);
	ListDigraph::NodeIt pair = curr;
	++pair;
	for (ListDigraph::NodeIt no(ts.outer.graph); no != INVALID; ++no)
	{
		
		(*ts.bridges.coords)[curr] = (*ts.outer.coords)[no];
		int innerId = ts.outer.correspondence[ts.outer.graph.id(no)];
		(*ts.bridges.coords)[pair] = (*ts.inner.coords)[ ts.outer.graph.nodeFromId(innerId) ];
		
		++curr; ++curr;
		++pair; ++pair;
	}
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

void Interfacer::get_from_ttf(FTGLPixmapFont *fonti, const char* ttfFile)
{
	fonti = new FTGLPixmapFont(ttfFile);
	if(fonti->Error())
		exit(EXIT_FAILURE);
}


