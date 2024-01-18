Compiler = g++
Args = -std=c++20 -w
Objs = GA.o Graph.o Utility.o Individual.o
Srcs_dir = .
Objs_dir = ./build

# Creare cartella sources
# nella cartella principale rimangono poi solo il Makefile e gli eseguibili una volta compilati  

$(shell mkdir -p $(Objs_dir))

graph_partitioner: $(Objs)
	$(Compiler) $(Args) $(Objs_dir)/*.o $(Srcs_dir)/graph_partitioner.cpp -o ./graph_partitioner
	$(Compiler) $(Args) $(Srcs_dir)/graph_generator.cpp -o ./graph_generator

GA.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/GeneticAlgorithm/GA.cpp -o $(Objs_dir)/GA.o

Graph.o: 
	$(Compiler) $(Args) -c $(Srcs_dir)/Graph/Graph.cpp -o $(Objs_dir)/Graph.o 

Individual.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/Individual/Individual.cpp -o $(Objs_dir)/Individual.o 

Utility.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/Utility/utility.cpp -o $(Objs_dir)/utility.o

clean:
	rm -rf $(Objs_dir) ./graph_generator ./graph_partitioner