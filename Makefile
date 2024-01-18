Compiler = g++
Args = -std=c++20
Objs = Crossover.o Era.o Fitness.o GeneticAlgorithm.o Graph.o Individual.o
Srcs_dir = .
Objs_dir = ./build

# Creare cartella sources
# nella cartella principale rimangono poi solo il Makefile e gli eseguibili una volta compilati  

graph_partitioner: $(Objs)
	$(Compiler) $(Args) -I $(Objs_dir)/*.o ./graph_partitioner.cpp -o ./graph_partitioner
	$(Compiler) $(Args) $(Srcs_dir)/graph_generator.cpp -o ./graph_generator

Crossover.o: 
	$(Compiler) $(Args) $(Srcs_dir)/Crossover/Crossover.cpp -c $(Objs_dir)/Crossover.o 

Era.o: 
	$(Compiler) $(Args) $(Srcs_dir)/Era/Era_Parallel.cpp -c $(Objs_dir)/Era_Parallel.o 
	$(Compiler) $(Args) $(Srcs_dir)/Era/Era.cpp -c $(Objs_dir)/Era.o 

Fitness.o: 
	$(Compiler) $(Args) $(Srcs_dir)/Fitness/Fitness.cpp -c $(Objs_dir)/Fitness.o 

GeneticAlgorithm.o:
	$(Compiler) $(Args) $(Srcs_dir)/GeneticAlgorithm/GeneticAlgorithm.cpp -c $(Objs_dir)/GeneticAlgorithm.o 

Graph.o: 
	$(Compiler) $(Args) $(Srcs_dir)/Graph/Graph.cpp -c $(Objs_dir)/Graph.o 

Individual.o:
	$(Compiler) $(Args) $(Srcs_dir)/Individual/Individual.cpp -c $(Objs_dir)/Individual.o 

Utility.o:
	$(Compiler) $(Args) $(Srcs_dir)/Utility/utility.cpp -c $(Objs_dir)/utility.o

clean:
	rm -rf $(Objs_dir) ./graph_generator ./graph_partitioning