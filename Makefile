prog: Source.cpp Shader.h Shaders/*.glsl
	g++ Source.cpp -o prog -lGL -lglfw -lGLEW

