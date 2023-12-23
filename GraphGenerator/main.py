import networkx as nx


def Symmetric_Adiacency_Matrix(Graph):
    # Creation of adjacense's matrix symmetric
    matrix = []
    # A for loop for row entries
    for edge in range(50):
        a = []
        # A for loop for column entries
        for column in range(50):
            a.append(0)
        matrix.append(a)

    for edge in Graph.edges:
        matrix[edge[0]][edge[1]] = 'X'
        matrix[edge[1]][edge[0]] = 'Y'

    return matrix


def Adiacency_Matrix(Graph):
    # Creation of adjacense's matrix symmetric
    matrix = []
    # A for loop for row entries
    for edge in range(50):
        a = []
        # A for loop for column entries
        for column in range(50):
            a.append(0)
        matrix.append(a)

    for edge in Graph.edges:
        matrix[edge[0]][edge[1]] = 'X'
    return matrix


def print_matrix(matrix):
    # For printing the matrix
    for row in range(50):
        for column in range(50):
            print(matrix[row][column], end=" ")
        print()


if __name__ == '__main__':
    f = open("graph.txt", "w+")
    f.write("50 250\n")
    Graph = nx.Graph()      # Empty Graph generator
    Random_graph = nx.gnm_random_graph(50, 250)     # Random Graph generator with N nodes, M edges
    print_matrix(Adiacency_Matrix(Random_graph))
    print(Random_graph, Random_graph.edges, Random_graph.nodes)
    for edge in Random_graph.edges:
        f.write(str(edge[0]) + " " + str(edge[1]) + "\n")

    f.close()
