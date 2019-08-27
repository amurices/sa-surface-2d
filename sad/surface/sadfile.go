package surface

import (
	"math"
)

type Point2d struct {
	X float64
	Y float64
}

func add2dPoints (a Point2d, b Point2d) Point2d {
	return Point2d{X: a.X + b.X, Y: a.Y + b.Y}
}

type node struct {
	Outgoing []*edge
	Incoming []*edge
	Coords   Point2d
}

type edge struct {
	Source *node
	Target *node
}

type triangle struct {
	Edges []*edge
	Nodes []*node
}

type surface struct {
//	Triangles []triangle
	Edges []*edge
	Nodes []*node
}

func IntMod (x int, y int) int {
	return int(math.Mod(float64(x), float64(y)))
}

func makeOutgoingEdgesOutOfNode (currentCircle []*node, belowCircle []*node, index int) []*edge {
	circlesLength := len(currentCircle) // All circles should have the same number of points
	followingNodeCurrentCircle := currentCircle[IntMod(index + 1, circlesLength)]
	followingNodeBelowCircle := belowCircle[IntMod(index + 1, circlesLength)]
	equivalentNodeBelowCircle := belowCircle[index]
	edges := []*edge{
		{Source: currentCircle[index], Target: followingNodeCurrentCircle},
		{Source: currentCircle[index], Target: followingNodeBelowCircle},
		{Source: currentCircle[index], Target: equivalentNodeBelowCircle}}
	// Side effects
	currentCircle[index].Outgoing = append(currentCircle[index].Outgoing, edges...)
	followingNodeCurrentCircle.Incoming = append (followingNodeCurrentCircle.Incoming, edges[0])
	followingNodeBelowCircle.Incoming = append (followingNodeBelowCircle.Incoming, edges[1])
	equivalentNodeBelowCircle.Incoming = append (equivalentNodeBelowCircle.Incoming, edges[2])
	return edges
}

func edgesOutOfCircularNodes(nodes [][]*node) []*edge {
	var edges []*edge = nil
	for i := 0; i < len(nodes) - 1; i++ {
		for j := 0; j < len(nodes[i]); j++ {
			edges = append(edges, makeOutgoingEdgesOutOfNode(nodes[i], nodes[i + 1], j)...)
		}
	}
	return edges
}

func generateCircularNodes(numPoints int, radius float64, centerPoint Point2d) []*node {
	var returnNodes []*node = nil
	for i := 0; i < numPoints; i++ {
		currentNodeXY := add2dPoints(centerPoint, Point2d{
			X: math.Cos(float64(i) * (2 * math.Pi) / float64(numPoints)) * radius,
			Y: math.Sin(float64(i) * (2 * math.Pi) / float64(numPoints)) * radius})
		currentNode := node{Outgoing: nil, Incoming: nil, Coords: currentNodeXY}
		returnNodes = append (returnNodes, &currentNode)
	}
	return returnNodes
}

func GenerateCircularSurface(numCircles int, numPoints int, outerRadius float64, innerRadius float64, centerPoint Point2d) surface {
	var nodes [][]*node
	if outerRadius <= innerRadius {
		panic("inner radius must be smaller than outer radius")
	}
	for i := 0; i < numCircles; i++ {
		step := (outerRadius - innerRadius) * float64(i) / float64(numCircles - 1)
		nodes = append(nodes, generateCircularNodes(numPoints, outerRadius - step, centerPoint))
	}
	var flattenedNodes []*node = nil
	for i := 0; i < len(nodes); i++ {
		flattenedNodes = append(flattenedNodes, nodes[i]...)
	}
	edges := edgesOutOfCircularNodes(nodes)
	return surface{Edges: edges, Nodes: flattenedNodes}
}

