from scipy import misc
from queue import Queue

IMAGE = 'images/plane30.jpg'

T = [79, 125, 200]
S = [85, 93, 96]

Sv = (-1, 0)
Tv = (0, -1)


def dist(x, y):
    return sum((x - y) ** 2)


def get_neighbours(i, j, max_i, max_j):
    result = []
    if i > 0:
        result.append((i - 1, j))
    if j > 0:
        result.append((i, j - 1))
    if (i + 1) < max_i:
        result.append((i + 1, j))
    if (j + 1) < max_j:
        result.append((i, j + 1))

    return result


def flip(i, j, A, B):
    if (i, j) in A:
        A.remove((i, j))
        B.add((i, j))
    else:
        B.remove((i, j))
        A.add((i, j))


def init_graph(image):
    graph = dict()
    graph[Sv] = dict()
    graph[Tv] = dict()
    #ihateducktyping
    dim_x, dim_y, _ = image.shape
    for i in range(0, dim_x):
        for j in range(0, dim_y):
#            print('\t init: ', i, j)
            graph[(i, j)] = dict()
            for (ni, nj) in get_neighbours(i, j, dim_x, dim_y):
                graph[(i,j)][(ni, nj)] = dist(image[i, j], image[ni, nj])
            graph[(i, j)][Tv] = dist(image[i, j], T)
            graph[Sv][(i, j)] = dist(image[i, j], S)
            graph[Tv][(i, j)] = 0
            graph[(i, j)][Sv] = 0
    return graph


def findWay(graph, s, t):
    way = []
    queue = Queue()
    queue.put(s)
    parents = dict()
    visited = set()
    while queue.empty() == False:
        v = queue.get()
        visited.add(v)
#        print('\t findway: ', v)
        if v == t:
            break
        for adj in graph[v]:
            if adj not in visited and graph[v][adj] > 0:
                parents[adj] = v
                queue.put(adj)

    if t not in parents:
        return []
    v = t
    while v != s:
        way.append(v)
        v = parents[v]
    return list(reversed(way + [s, ]))


def FordFulkerson(image, graph):
    max_flow = 0
    way = findWay(graph, (-1, 0), (0, -1))
    iteration = 0
    while len(way) != 0:
        min_edge_cost = min([graph[way[i-1]][way[i]] for i in range(1, len(way))])
        max_flow += min_edge_cost
        if iteration % 100 == 0:
            print('\t FF iteration: ', iteration, ' maxflow: ', max_flow)
        for i in range(1, len(way)):
            graph[way[i - 1]][way[i]] -= min_edge_cost
            graph[way[i]][way[i - 1]] += min_edge_cost
        way = findWay(graph, (-1, 0), (0, -1))
        iteration += 1
    return max_flow, graph


def cut(image):
    dim_x, dim_y, colors = image.shape
    A = set()
    B = {(i, j) for i in range(dim_x) for j in range(dim_y)}
    B.add(Sv)
    B.add(Tv)

    graph = init_graph(image)

    max_flow, cut = FordFulkerson(image, graph)

    print('Max flow/min cut: ', max_flow)

    def dfs(x):
        if x not in A:
            B.remove(x)
            A.add(x)
            for adj in graph[x]:
                if graph[x][adj] > 0:
                    dfs(adj)

    dfs(Sv)
#    A.remove(Sv)
#    B.remove(Tv)
    
    return A, B


def main():
    image = misc.imread(IMAGE)
    dim_x, dim_y, colors = image.shape
    print("Loaded image of shape {x}, {y}".format(x=dim_x, y=dim_y))

    A, B = cut(image)
    print(A)
    print(B)

    for i in range(dim_x):
        for j in range(dim_y):
            if (i, j) in A:
                image[i, j, 0] = min(255, image[i, j, 0] + 100)
                image[i, j, 1] = image[i, j, 1] / 3
                image[i, j, 2] = image[i, j, 2] / 3
            elif (i,j) in B:
                image[i, j, 0] = image[i, j, 0] / 3
                image[i, j, 1] = min(255, image[i, j, 1] + 100)
                image[i, j, 2] = image[i, j, 2] / 3

    misc.imsave('out_plane30.png', image)


if __name__ == '__main__':
    main()
