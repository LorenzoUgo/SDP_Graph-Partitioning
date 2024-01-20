Compiler = g++
Args = -std=c++20 -w
Objs = GA.o Graph.o Utility.o Individual.o
Srcs_dir = ./src
Objs_dir = ./build

all: mkdir_build graph_partitioner graph_generator extract_data
	@echo "Done!"

mkdir_build:
	$(shell mkdir -p $(Objs_dir))

graph_partitioner: mkdir_build $(Objs)
	$(Compiler) $(Args) $(Objs_dir)/*.o $(Srcs_dir)/graph_partitioner.cpp -o ./graph_partitioner

graph_generator:
	$(Compiler) $(Args) $(Srcs_dir)/graph_generator.cpp -o ./graph_generator

extract_data:
	unzip ./data.zip -d ./
	rm -rf ./data.zip

GA.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/GeneticAlgorithm/GA.cpp -o $(Objs_dir)/GA.o

Graph.o: 
	$(Compiler) $(Args) -c $(Srcs_dir)/Graph/Graph.cpp -o $(Objs_dir)/Graph.o 

Individual.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/Individual/Individual.cpp -o $(Objs_dir)/Individual.o 

Utility.o:
	$(Compiler) $(Args) -c $(Srcs_dir)/Utility/utility.cpp -o $(Objs_dir)/Utility.o

clean:
	rm -rf $(Objs_dir) ./graph_generator ./graph_partitioner

clean_data: 
	rm -rf ./data/
