Compiler = g++
Args = -std=c++20
Objs = Crossover.o Era.o Fitness.o GeneticAlgorithm.o Graph.o Individual.o
Srcs_dir = .
Objs_dir = ./build

# Creare cartella sources
# nella cartella principale rimangono poi solo il Makefile e gli eseguibili una volta compilati  

$(shell mkdir -p $(Objs_dir))

graph_partitioner: $(Objs)
	$(Compiler) $(Args) -I $(Objs_dir)/*.o graph_partitioner.cpp -o /graph_partitioner
	$(Compiler) $(Args) $(Srcs_dir)/graph_generator.cpp -o /graph_generator

Crossover.o: 
	$(Compiler) $(Args) -c $(Srcs_dir)/Crossover/Crossover.cpp -o $(Objs_dir)/Crossover.o 

Era.o: 
	$(Compiler) $(Args) -c $(Srcs_dir)/Era/Era_Parallel.cpp -o $(Objs_dir)/Era_Parallel.o 
	$(Compiler) $(Args) -c $(Srcs_dir)/Era/Era.cpp -o $(Objs_dir)/Era.o 

Fitness.o: 
	$(Compiler) $(Args) -c $(Srcs_dir)/Fitness/Fitness.cpp -o $(Objs_dir)/Fitness.o 

GeneticAlgorithm.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/GeneticAlgorithm/GeneticAlgorithm.cpp -o $(Objs_dir)/GeneticAlgorithm.o 

Graph.o: 
	$(Compiler) $(Args) -c $(Srcs_dir)/Graph/Graph.cpp -o $(Objs_dir)/Graph.o 

Individual.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/Individual/Individual.cpp -o $(Objs_dir)/Individual.o 

Utility.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/Utility/utility.cpp -o $(Objs_dir)/utility.o

clean:
	rm -rf $(Objs_dir) ./graph_generator ./graph_partitioning