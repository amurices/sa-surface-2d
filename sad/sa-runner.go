package main

import (
	"fmt"
	"github.com/go-gl/gl/all-core/gl"
	"github.com/go-gl/glfw/v3.2/glfw"
	"runtime"
	"sad/surface"
)

func init(){
	runtime.LockOSThread()
}

func main() {
	glfwError := glfw.Init()
	glError := gl.Init()

	if glfwError != nil {
		panic(glfwError)
	} else if glError != nil {
		panic (glError)
	}
	defer glfw.Terminate()

	window, glfwError := glfw.CreateWindow(640, 480, "Testing", nil, nil)
	if glfwError != nil {
		panic (glfwError)
	}

	window.MakeContextCurrent()

	//for !window.ShouldClose() {
	//	gl.ClearColor(0, 0, 0, 0,)
	//	gl.Clear(gl.COLOR_BUFFER_BIT | gl.DEPTH_BUFFER_BIT)
	//	window.SwapBuffers()
	//	glfw.PollEvents()
	//}
	circle := surface.GenerateCircularSurface(2,4, 1.0, 0.75, surface.Point2d{X: 0, Y: 0})
	for i := 0; i < len(circle.Nodes); i++ {
		fmt.Print(circle.Nodes[i].Coords, "\n")
		for j := 0; j < len(circle.Nodes[i].Outgoing); j++ {
			fmt.Print("\t->\t", circle.Nodes[i].Outgoing[j].Target.Coords, "\n")
		}
	}
}
