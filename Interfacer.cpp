//
//  Interfacer.cpp
//  Surface
//
//  Created by André Muricy on 2/1/17.
//  Copyright © 2017 André Muricy. All rights reserved.
//

#include "Interfacer.hpp"
surface Interfacer::generate_random(double perim, int pts){
    surface surf;
    for (size_t i = 0; i < pts; i++)
    {
		double randX = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) - 0.5;
		double randY = static_cast<double>( rand() )/ static_cast<double> (RAND_MAX) - 0.5;
		
		// Digamos que o cluster de pontos seja organizado numa área de metade do perímetro
		randX *= 0.5 * perim; randY *= 0.5 * perim;
		
		
		surf.v.push_back(point(randX, randY, (int)i));
	}
	
	// Inicializamos os vizinhos agora, pois a reestruturação do vetor pode complicar isso
	for (size_t i = 0; i < surf.v.size(); i++)
	{
		surf.v[i].neighbor0 = &(surf.v[i]); // Consideram-se vizinhos de si
		surf.v[i].neighbor1 = &(surf.v[i]); // próprios a princípio
	}
	
	std::vector<point*> R;
	std::vector<link> e;
	
	
	double minDist = INFINITY;
	point *minC, *minR; int toRemoveR, toRemoveE;
	
	// Primeiro adicionamos a primeira aresta ao conjunto de vértices (a menor de todas)
	for (size_t i = 0; i < surf.v.size(); i++)
	{
		for (int j = (int)surf.v.size() - 1; j > i; j--)
		{
			double thisDist = dist(surf.v[i], surf.v[j]);
			if (thisDist < minDist)
			{
				minDist = thisDist;
				minC = &surf.v[i];
				minR = &surf.v[j];
			}
		}
	}
	std::cout << "Antes de tudo, nosso ponto de partida: ";
	printPoint(*minC); std::cout << " -> ";
	printPoint(*minR); std::cout << "\n\n";
	// Ao final desse loop, minC e minR brevemente conterão o nosso conjunto C inicial. A aresta entreeles
	// é nosso ponto de partida.
	minC->neighbor1 = minC->neighbor0 = minR;
	minR->neighbor0 = minR->neighbor0 = minC;
	for (size_t i = 0; i < surf.v.size(); i++)
	{
		if (&surf.v[i] != minC && &surf.v[i] != minR)
			R.push_back(&surf.v[i]);
	} // E claro, adicionamos todos os outros vértices a R
	surf.e.push_back(std::make_pair(minR, minC));
	surf.e.push_back(std::make_pair(minC, minR)); // Duplicada pelo bem do algoritmo
	
	
	minDist = INFINITY;
	
	while (R.size() > 0)
	{
		for (size_t i = 0; i < surf.e.size(); i++)
		{
			for (size_t j = 0; j < R.size(); j++)
			{
				double thisDist = dist(*surf.e[i].first, *R[j]) + dist(*R[j], *surf.e[i].second);
				if (thisDist < minDist)
				{
					minDist = thisDist;
					toRemoveR = (int)j;
					toRemoveE = (int)i;
				}
			}
		}
		// Depois daqui, sabemos que o vértice de índice J em R é o mais próximo de uma das arestas
		// que já incluímos.
		point *u, *v, *x; // Para legibilidade
		u = surf.e[toRemoveE].first;
		v = surf.e[toRemoveE].second;
		x = R[toRemoveR];
		
		u->neighbor1 = x;
		v->neighbor0 = x;
		
		x->neighbor0 = u;
		x->neighbor1 = v;
		
		// Diminuímos R, eliminamos (u,v)
		R.erase(R.begin() + toRemoveR);
		
		
		std::cout << "Estamos tirando:\n";
		printEdge(surf.e[toRemoveE]); std::cout << std::endl;
		std::cout << "e colocando:\n";
		printEdge(std::make_pair(u, x)); std::cout << std::endl;
		printEdge(std::make_pair(x, u)); std::cout << std::endl;
		std::cout << std::endl;
		surf.e.erase(surf.e.begin() + toRemoveE);
		// Adicionamos (u,x) e (x,u)
		surf.e.push_back(std::make_pair(u, x));
		surf.e.push_back(std::make_pair(x, v));
		
		/*
		for (size_t i = 0; i < C.size(); i++)
		{
			for (int j = 0; j < (int)R.size(); j++)
			{
				std::cout << "C: " << std::setprecision(2) << C[i]->x << ", " << std::setprecision(2) << C[i]->y << " // R: " << std::setprecision(2) << R[j]->x << ", " << std::setprecision(2) << R[j]->y  << "; dist: ";
				double thisDist = dist(*C[i]->neighbor0, *R[j]) + dist(*C[i]->neighbor1, *R[j]);
				 std::cout << thisDist << std::endl;
				if (thisDist < minDist)
				{
					minDist = thisDist;
					minC = C[i];
					minR = R[j];
					toRemoveR = j;
				}
			}
		}
		
		
		std::cout << "Ligaremos: Cmin = " << minC->x << ", " << minC->y;
		std::cout << "; Rmin = " << minR->x << ", " << minR->y << std::endl << std::endl;
		
		
		
		// Achamos a dupla mais próxima. Agora, o nó de R tem que escolher dentre os dois vizinhos
		// do nó de C, a qual ele vai se ligar.
		double dist0 = dist(*minR, *(minC->neighbor0) );
		double dist1 = dist(*minR, *(minC->neighbor1) );
		
		if (dist0 < dist1) // Nesse caso, o novo neighbor 0 de minC será minR, e o neighbor1 de minR, minC
		{
			minR->neighbor1 = minC;
			minR->neighbor0 = minC->neighbor0;
			std::cout << "Ligamos Rmin a Cmin (" << minC->x << ", " << minC->y << ") e ao antigo n0 de minC (" << minC->neighbor0->x << ", " << minC->neighbor0->y << ")" << std::endl;
			minC->neighbor0 = minR;
			
		}
		
		else
		{
			minR->neighbor0 = minC;
			minR->neighbor1 = minC->neighbor1;
			std::cout << "Ligamos Rmin a Cmin (" << minC->x << ", " << minC->y << ") e ao antigo n1 de minC (" << minC->neighbor1->x << ", " << minC->neighbor1->y << ")" << std::endl;
			minC->neighbor1 = minR;
			
		}
		*/
		minDist = INFINITY;
	}
	
	
	return surf;
}
