assignment_name = final_project

$(assignment_name): $(assignment_name).cpp
	g++ -w -o $(assignment_name) $(assignment_name).cpp glslprogram.cpp -lm -framework OpenGL -framework GLUT

tar:
	tar -cvf final_project_ben_friedman.tar *.cpp *.h *.frag *.vert final_project Makefile

clean:
	rm -rf $(assignment_name)
