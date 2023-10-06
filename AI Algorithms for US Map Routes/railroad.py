# Name: Sophia Evanisko    
import heapq, random, pickle, math, time
from math import pi, acos, sin, cos
from tkinter import *
from collections import deque


class PriorityQueue():
    def __init__(self):
        self.queue = []
        current = 0  # to make this object iterable
        # To make this object iterable: have __iter__ function and assign next function for __next__

    def next(self):
        if self.current >= len(self.queue):
            self.current
            raise StopIteration

        out = self.queue[self.current]
        self.current += 1
        return out

    def __iter__(self):
        return self

        __next__ = next

    def isEmpty(self):
        return len(self.queue) == 0

    def remove(self, index):
        # remove self.queue[index]
        val = self.queue.pop(index)
        return val

    def pop(self):
        # swap first and last. Remove last. Heap down from first.
        # return the removed value
        return heapq.heappop(self.queue)

    def push(self, value):
        heapq.heappush(self.queue, value)
        # append at last. Heap up.
        pass

    def peek(self):
        value = heapq.heappop(self.queue)
        heapq.heappush(self.queue, value)
        return value
        # return min value (index 0)

    pass


def calc_edge_cost(y1, x1, y2, x2):
    # y1 = lat1, x1 = long1
    # y2 = lat2, x2 = long2
    # all assumed to be in decimal degrees
    # if (and only if) the input is strings
    # use the following conversions
    y1 = float(y1)
    x1 = float(x1)
    y2 = float(y2)
    x2 = float(x2)
    R = 3958.76  # miles = 6371 km

    y1 *= pi / 180.0
    x1 *= pi / 180.0
    y2 *= pi / 180.0
    x2 *= pi / 180.0
    # approximate great circle distance with law of cosines
    return acos(sin(y1) * sin(y2) + cos(y1) * cos(y2) * cos(x2 - x1)) * R


# NodeLocations, NodeToCity, CityToNode, Neighbors, EdgeCost
# Node: (lat, long) or (y, x), node: city, city: node, node: neighbors, (n1, n2): cost
def make_graph(nodes="rrNodes.txt", node_city="rrNodeCity.txt", edges="rrEdges.txt"):
    nodeLoc, nodeToCity, cityToNode, neighbors, edgeCost = {}, {}, {}, {}, {}
    map = {}  # have screen coordinate for each node location

    file = open(nodes, "r")
    for info in file.readlines():
        values = info.rstrip('\n').split(" ")
        nodeLoc[values[0]] = (values[1], values[2])

    file2 = open(node_city, "r")
    for info2 in file2.readlines():
        values2 = info2.rstrip('\n').split(" ", 1)
        nodeToCity[values2[0]] = values2[1]
        cityToNode[values2[1]] = values2[0]

    file3 = open(edges, "r")
    for info3 in file3.readlines():
        values3 = info3.rstrip('\n').split(" ")
        edgeCost[(values3[0], values3[1])] = calc_edge_cost(nodeLoc[values3[0]][0], nodeLoc[values3[0]][1],
                                                            nodeLoc[values3[1]][0], nodeLoc[values3[1]][1])
        edgeCost[(values3[1], values3[0])] = edgeCost[(values3[0], values3[1])]
        if values3[0] not in neighbors.keys():
            neighbors[values3[0]] = [values3[1]]
        else:
            neighbors[values3[0]].append(values3[1])
        if values3[1] not in neighbors.keys():
            neighbors[values3[1]] = [values3[0]]
        else:
            neighbors[values3[1]].append(values3[0])

    for node in nodeLoc:  # checks each
        lat = float(nodeLoc[node][0])  # gets latitude
        long = float(nodeLoc[node][1])  # gets long
        modlat = (lat - 10) / 60  # scales to 0-1
        modlong = (long + 130) / 70  # scales to 0-1
        map[node] = [modlat * 800, modlong * 1200]  # scales to fit 800 1200

    return [nodeLoc, nodeToCity, cityToNode, neighbors, edgeCost, map]


# Retuen the direct distance from node1 to node2
# Use calc_edge_cost function.
def dist_heuristic(n1, n2, graph):
    lat1 = (graph[0][n1])[0]
    long1 = (graph[0][n1])[1]
    lat2 = (graph[0][n2])[0]
    long2 = (graph[0][n2])[1]
    return calc_edge_cost(lat1, long1, lat2, long2)


# Create a city path.
# Visit each node in the path. If the node has the city name, add the city name to the path.
# Example: ['Charlotte', 'Hermosillo', 'Mexicali', 'Los Angeles']
def display_path(path, graph):
    cities = []
    for node in path:
        if node in graph[1].keys():
            cities.append(graph[1][node])
    return cities


# Using the explored, make a path by climbing up to "s"
# This method may be used in your BFS and Bi-BFS algorithms.
def generate_path(state, explored, graph):
    path = [state]

    cost = 0
    while explored[state] != "s":  # "s" is initial's parent
        path.append(state)
        cost = cost + graph[4][(state, explored[state])]
        state = explored[state]
    return path[::-1], cost

def calcCost(path, graph):
    cost = 0
    for i in range(len(path)-1):
        cost = cost + graph[4][(path[i], path[i+1])]
    return cost


def drawLine(canvas, y1, x1, y2, x2, col):
    x1, y1, x2, y2 = float(x1), float(y1), float(x2), float(y2)
    canvas.create_line(x1, 800 - y1, x2, 800 - y2, fill=col)


# Draw the final shortest path.
# Use drawLine function.
def draw_final_path(ROOT, canvas, path, graph, col='red'):
    ROOT.geometry("1200x800")
    canvas.pack(fill=BOTH, expand=1)
    for i in range(len(path) - 1):
        drawLine(canvas, *graph[5][path[i]], *graph[5][path[i + 1]], col)
    # Your code goes here

    ROOT.update()
    pass


def draw_all_edges(ROOT, canvas, graph):
    ROOT.geometry("1200x800")  # sets geometry
    canvas.pack(fill=BOTH, expand=1)  # sets fill expand
    for n1, n2 in graph[4]:  # graph[4] keys are edge set
        drawLine(canvas, *graph[5][n1], *graph[5][n2], 'white')  # graph[5] is map dict


# ROOT.update()


def bfs(start, goal, graph, col):
    ROOT = Tk()  # creates new tkinter
    ROOT.title("BFS")
    canvas = Canvas(ROOT, background='black')  # sets background
    draw_all_edges(ROOT, canvas, graph)

    counter = 0
    frontier, explored = deque(), {start: "s"}
    frontier.append(start)
    while frontier:
        s = frontier.popleft()
        if s == goal:
            path, cost = generate_path(s, explored, graph)
            path.insert(0, start)
            path.pop()
            draw_final_path(ROOT, canvas, path, graph)
            print("The number of explored nodes of BFS: ", counter)
            return path, cost
        for a in graph[3][s]:  # graph[3] is neighbors
            if a not in explored:
                explored[a] = s
                frontier.append(a)
                drawLine(canvas, *graph[5][s], *graph[5][a], col)
        counter += 1
    # if counter % 100 == 0: ROOT.update()
    return None, None


def bi_bfs(start, goal, graph, col):
    ROOT = Tk()  # creates new tkinter
    ROOT.title("Bi-BFS")
    canvas = Canvas(ROOT, background='black')  # sets background
    draw_all_edges(ROOT, canvas, graph)

    frontierf = PriorityQueue()
    frontierb = PriorityQueue()
    frontierf.push((0, [start], start))
    frontierb.push((0, [goal], goal))
    exploredf = {start: 0}
    exploredb = {goal: 0}
    if start == goal: return []
    f1 = f = 0
    curr = ""
    counter = 0
    while frontierf and frontierb:
        f1, path1, nodef = frontierf.pop()
        if nodef in exploredb:
             #path1.append(nodef)
            print("The number of explored nodes of Bi-BFS: ", counter)
            draw_final_path(ROOT, canvas, path1 +[curr] + path, graph)
            return  path1 + [curr] + path, f1
        for fr in graph[3][nodef]:
            plen = f1 + graph[4][(fr, nodef)]
            if fr not in exploredf:
                curr = fr
                exploredf[fr] = plen
                frontierf.push((plen, (path1 + [fr]), fr))
                drawLine(canvas, *graph[5][fr], *graph[5][nodef], col)
                counter +=1

        f, path, nodeb = frontierb.pop()
        if nodeb in exploredf:
             #path.append(nodeb)
            print("The number of explored nodes of Bi-BFS: ", counter)
            draw_final_path(ROOT, canvas, path1 + path + [curr], graph)
            return path1 + path + [curr], f
        for b in graph[3][nodeb]:
            plen2 = f + graph[4][(b, nodeb)]
            if b not in exploredb:
                curr = b
                exploredb[b] = plen2
                frontierb.push((plen2, ([b] + path), b))
                drawLine(canvas, *graph[5][b], *graph[5][nodeb], col)
                counter += 1
    return None, None


def a_star(start, goal, graph, col, dist_heuristic, ROOT, canvas):
    heuristic = dist_heuristic
    f = 0
    counter = 0
    frontier = PriorityQueue()
    h = heuristic(start, goal, graph)
    frontier.push((h, start, [start]))
    explored = {start: h}
    if start == goal: return []
    while not frontier.isEmpty():
        f, node, path = frontier.pop()
        if node == goal:
             if not col == 'pink':
                draw_final_path(ROOT, canvas, path, graph)
                print("The number of explored nodes of A star: ", counter)
             return path, f
        for c in graph[3][node]:
            h1 = heuristic(c, goal, graph)
            plen = f + h1 - heuristic(node, goal, graph) + graph[4][(node, c)]
            if c not in explored or (c in explored and plen < explored[c]):
                explored[c] = plen
                frontier.push((plen, c, (path + [c])))
                drawLine(canvas, *graph[5][c], *graph[5][node], col)
            counter += 1
    return None, None


def bi_a_star(start, goal, graph, col, heuristic=dist_heuristic):
    ROOT = Tk()  # creates new tkinter
    ROOT.title("Bi-A-star")
    canvas = Canvas(ROOT, background='black')  # sets background
    draw_all_edges(ROOT, canvas, graph)                                                                                    # Your code goes here
    frontierf = PriorityQueue()
    frontierb = PriorityQueue()
    hf = heuristic(start, goal, graph)
    hb = heuristic(goal, start, graph)
    frontierf.push((hf, [start], start))
    frontierb.push((hb, [goal], goal))
    exploredf = {start: hf}
    exploredb = {goal: hb}
    if start == goal: return []
    f1 = f = 0
    path = []
    counter = 0
    while frontierf and frontierb:
         if not frontierf.isEmpty():
            f1, path1, nodef = frontierf.pop()
            if nodef in exploredb or nodef == goal:
                 #path1.append(nodef)
                print("The number of explored nodes of Bi-A star: ", counter)
                draw_final_path(ROOT, canvas, path1 + path, graph)
                return path1 + path, f1
            for fr in graph[3][nodef]:
                h1 = heuristic(fr, goal, graph)
                plen = f1 + h1 - heuristic(nodef, goal, graph) + graph[4][(nodef, fr)]
                if fr not in exploredf or plen < exploredf[fr]:
                    exploredf[fr] = plen
                    frontierf.push((plen, (path1 + [fr]), fr))
                    drawLine(canvas, *graph[5][fr], *graph[5][nodef], col)
                    counter +=1
         if not frontierb.isEmpty():
            f, path, nodeb = frontierb.pop()
            if nodeb in exploredf or nodeb == start:
                 #path.append(nodeb)
                print("The number of explored nodes of Bi-A star: ", counter)
                draw_final_path(ROOT, canvas, path1 + path, graph)
                return path1 + path, f
            for b in graph[3][nodeb]:
                h2 = heuristic(b, start, graph)
                plen2 = f + h2 - heuristic(nodeb, start, graph) + graph[4][(nodeb, b)]
                if b not in exploredb or plen2 < exploredb[b]:
                    exploredb[b] = plen2
                    frontierb.push((plen2, ([b] + path), b))
                    drawLine(canvas, *graph[5][b], *graph[5][nodeb], col)
                    counter += 1
    return None, None


def tri_directional(city1, city2, city3, graph, col, dist_heuristic, ROOT, canvas):
    c = []
    p = []
    heuristic = dist_heuristic
    path1, cost1 =  a_star(city1, city2, graph, col, heuristic, ROOT, canvas)
    path2, cost2 =  a_star(city2, city3, graph, col, heuristic, ROOT, canvas)
    path3, cost3 =  a_star(city3, city1, graph, col, heuristic, ROOT, canvas)

    if cost1 + cost2 < cost2 + cost3 and cost1 + cost2 < cost3 + cost1:
        path1.pop()
        draw_final_path(ROOT, canvas, path1, graph)
        draw_final_path(ROOT, canvas, path2, graph)
        return ((path1+path2), cost1+cost2)
    if cost2 + cost3 < cost1 + cost2  and cost2 + cost3 < cost3 + cost1:
        path2.pop()
        draw_final_path(ROOT, canvas, path2, graph)
        draw_final_path(ROOT, canvas, path3, graph)
        return ((path2+path3), cost2+cost3)
    if cost3 + cost1 < cost2 + cost3 and cost3 + cost1 < cost1 + cost2:
        path3.pop()
        draw_final_path(ROOT, canvas, path3, graph)
        draw_final_path(ROOT, canvas, path1, graph)
        return ((path3+path1), cost3+cost1)


def main():
    start, goal, third = input("Start city: "), input("Goal city: "), input("Third city for tri-directional: ")
    graph = make_graph("rrNodes.txt", "rrNodeCity.txt", "rrEdges.txt")  # Task 1
     #COMPLETE
    cur_time = time.time()
    path, cost = bfs(graph[2][start], graph[2][goal], graph, 'yellow') #graph[2] is city to node
    if path != None: display_path(path, graph)
    else: print ("No Path Found.")
    print("The whole path: ", path)
    print('The length of the whole path', len(path))
    print(display_path(path, graph))
    print ('BFS Path Cost:', cost)
    print ('BFS duration:', (time.time() - cur_time))
    print ()

    cur_time = time.time()
    path, cost = bi_bfs(graph[2][start], graph[2][goal], graph, 'green')
    if path != None: display_path(path, graph)
    else: print ("No Path Found.")
    print("The whole path: ", path)
    print('The length of the whole path', len(path))
    print(display_path(path, graph))
    print ('Bi-BFS Path Cost:', cost)
    print ('Bi-BFS duration:', (time.time() - cur_time))
    print ()

    ROOT = Tk()  # creates new tkinter
    ROOT.title("A-star")
    canvas = Canvas(ROOT, background='black')  # sets background
    draw_all_edges(ROOT, canvas, graph)
    cur_time = time.time()
    path, cost = a_star(graph[2][start], graph[2][goal], graph, 'blue', dist_heuristic, ROOT, canvas)
    if path != None:
        display_path(path, graph)
    else:
        print("No Path Found.")
    print("The whole path: ", path)
    print('The length of the whole path', len(path))
    print(display_path(path, graph))
    print('A star Path Cost:', cost)
    print('A star duration:', (time.time() - cur_time))
    print()

    cur_time = time.time()
    path, cost = bi_a_star(graph[2][start], graph[2][goal], graph, 'orange', dist_heuristic)
    if path != None: display_path(path, graph)
    else: print ("No Path Found.")
    print("The whole path: ", path)
    print('The length of the whole path', len(path))
    print(display_path(path, graph))
    print ('Bi-A star Path Cost:', cost)
    print ("Bi-A star duration: ", (time.time() - cur_time))
    print ()
#
    ROOT = Tk()  # creates new tkinter
    ROOT.title("Tri-Directional")
    canvas = Canvas(ROOT, background='black')  # sets background
    draw_all_edges(ROOT, canvas, graph)
    print ("Tri-Search of ({}, {}, {})".format(start, goal, third))
    cur_time = time.time()
    path, cost = tri_directional(graph[2][start], graph[2][goal], graph[2][third], graph, 'pink', dist_heuristic, ROOT, canvas)
    if path != None: display_path(path, graph)
    else: print ("No Path Found.")
    print("The whole path: ", path)
    print('The length of the whole path', len(path))
    print(display_path(path, graph))
    print ('Tri-A star Path Cost:', cost)
    print ("Tri-directional search duration:", (time.time() - cur_time))

    mainloop() # Let TK windows stay still

if __name__ == '__main__':
    main()

''' Sample output
  ----jGRASP exec: python Lab10_railroad_Kim_2019_2020.py
 Start city: Charlotte
 Goal city: Los Angeles
 Third city for tri-directional: Chicago
 The number of explored nodes of BFS: 19735
 The whole path: ['3700421', '3700258', '3700256', '3700004', '3700076', '3700075', '0000530', '4500272', '4500042', '4500270', '4500231', '4500069', '4500023', '4500233', '4500094', '4500095', '4500096', '4500097', '4500234', '4500225', '4500104', '4500082', '4500164', '4500015', '4500181', '4500167', '0000533', '1300133', '1300197', '1300132', '1300146', '1300198', '1300204', '1300208', '1300087', '1300279', '1300088', '1300369', '1300459', '1300458', '1300090', '1300460', '1300107', '1300210', '1300398', '1300099', '0000031', '0100343', '0100342', '0100341', '0100084', '0100506', '0100012', '0100325', '0100345', '0100331', '0100520', '0100354', '0100355', '0100042', '0100566', '0100356', '0100357', '0100456', '0100103', '0100515', '0100264', '0100032', '0100263', '0100102', '0100033', '0100062', '0100129', '0100513', '0100061', '0000461', '2800154', '2800153', '2800032', '2800150', '2800031', '2800108', '2800247', '2800191', '2800156', '2800169', '2800001', '2800162', '2800163', '2800164', '2800125', '2800030', '2800028', '0000419', '2200078', '2200143', '2200039', '2200274', '2200379', '2200080', '2200273', '2200205', '2200112', '2200037', '2200076', '2200311', '0000411', '0500250', '0500019', '0500248', '0500005', '0500020', '0500134', '0000573', '4800439', '4800085', '4800410', '4801165', '4800956', '4801086', '4800081', '4800584', '4800082', '4800084', '4800309', '4800898', '4801101', '4800271', '4800578', '4800274', '4800881', '4800882', '4800167', '4800483', '4800464', '4800168', '4801228', '4800170', '4801230', '4800172', '4800462', '4800461', '4800230', '4800199', '4800832', '4800831', '4800198', '4801190', '4800830', '4800197', '4800200', '4800302', '4800648', '4800763', '4800286', '4800759', '4800758', '4800649', '4800675', '4801214', '4800285', '4800674', '4800757', '4800673', '4800672', '4800535', '4800280', '4800279', '4801134', '4800896', '4800357', '0009483', '9100020', '9100502', '9100501', '9100505', '9100507', '9100504', '9100503', '9100515', '9100153', '9100122', '9100478', '9100448', '9100442', '9100477', '9100476', '9100479', '9100436', '9100124', '9100150', '9100427', '9100012', '9100485', '9100484', '9100081', '9100486', '9100007', '9100117', '9100006', '9100116', '9100080', '9100438', '9100001', '0009063', '0600129', '0600577', '0600041', '0600579', '0600117', '0600039', '0600646', '0600797', '0600747', '0600516', '0600750', '0600584', '0600746', '0600585', '0600586', '0600042', '0600770', '0600434', '0600689', '0600464', '0600688', '0600384', '0600588', '0600460', '0600408', '0600799', '0600402', '0600766', '0600686', '0600079', '0600080', '0600086', '0600684', '0600425', '0600088', '0600759', '0600427', '0600316']
 The length of the whole path: 243
 ['Charlotte', 'Hermosillo', 'Mexicali', 'Los Angeles']
 BFS Path Cost: 2965.7640233572088
 BFS duration: 288.9429421424866

 The number of explored nodes of Bi-BFS: 12714
 The whole path: ['3700421', '3700258', '3700256', '3700004', '3700076', '3700075', '0000530', '4500272', '4500042', '4500270', '4500231', '4500069', '4500023', '4500233', '4500094', '4500095', '4500096', '4500097', '4500234', '4500225', '4500104', '4500082', '4500164', '4500015', '4500181', '4500167', '0000533', '1300133', '1300197', '1300132', '1300146', '1300198', '1300204', '1300208', '1300087', '1300279', '1300088', '1300369', '1300459', '1300458', '1300090', '1300460', '1300107', '1300210', '1300398', '1300099', '0000031', '0100343', '0100342', '0100341', '0100084', '0100506', '0100012', '0100325', '0100345', '0100331', '0100520', '0100354', '0100355', '0100042', '0100566', '0100356', '0100357', '0100456', '0100103', '0100515', '0100264', '0100032', '0100263', '0100102', '0100033', '0100062', '0100129', '0100513', '0100061', '0000461', '2800154', '2800153', '2800032', '2800150', '2800031', '2800108', '2800247', '2800191', '2800156', '2800169', '2800001', '2800162', '2800163', '2800164', '2800125', '2800030', '2800028', '0000419', '2200078', '2200143', '2200039', '2200274', '2200379', '2200080', '2200273', '2200205', '2200112', '2200037', '2200076', '2200311', '0000411', '0500250', '0500019', '0500248', '0500005', '0500020', '0500134', '0000573', '4800439', '4800085', '4800410', '4801165', '4800956', '4801086', '4800081', '4800584', '4800082', '4800084', '4800309', '4800898', '4801101', '4800271', '4800578', '4800274', '4800881', '4800882', '4800167', '4800483', '4800464', '4800168', '4801228', '4800170', '4801230', '4800172', '4800462', '4800461', '4800230', '4800199', '4800832', '4800831', '4800198', '4801190', '4800830', '4800197', '4800200', '4800302', '4800648', '4800763', '4800286', '4800759', '4800758', '4800649', '4800675', '4801214', '4800285', '4800674', '4800757', '4800673', '4800672', '4800535', '4800280', '4800279', '4801134', '4800896', '4800357', '0009483', '9100020', '9100502', '9100501', '9100505', '9100018', '9100508', '9100503', '9100515', '9100153', '9100122', '9100478', '9100448', '9100442', '9100477', '9100476', '9100479', '9100436', '9100124', '9100150', '9100427', '9100012', '9100485', '9100484', '9100081', '9100486', '9100007', '9100117', '9100006', '9100116', '9100080', '9100438', '9100001', '0009063', '0600129', '0600577', '0600041', '0600579', '0600117', '0600039', '0600646', '0600797', '0600747', '0600516', '0600750', '0600584', '0600746', '0600585', '0600586', '0600042', '0600770', '0600434', '0600690', '0600875', '0600691', '0600692', '0600082', '0600313', '0600383', '0600312', '0600404', '0600405', '0600403', '0600079', '0600080', '0600086', '0600684', '0600425', '0600088', '0600759', '0600427', '0600316']
 The length of the whole path 243
 ['Charlotte', 'Hermosillo', 'Mexicali', 'Los Angeles']
 Bi-BFS Path Cost: 2965.4128704488785
 Bi-BFS duration: 115.2277946472168

 The number of explored nodes of A star: 7692
 The whole path: ['3700421', '3700258', '3700257', '3700142', '3700422', '3700001', '3700235', '3700234', '3700330', '3700329', '3700002', '3700356', '3700355', '3700357', '3700197', '3700198', '0000529', '4500042', '4500270', '4500231', '4500069', '4500023', '4500233', '4500094', '4500095', '4500096', '4500097', '4500234', '4500225', '4500104', '4500082', '4500164', '4500015', '4500181', '4500167', '0000533', '1300133', '1300197', '1300132', '1300146', '1300198', '1300204', '1300208', '1300087', '1300279', '1300088', '1300369', '1300459', '1300458', '1300090', '1300460', '1300107', '1300210', '1300398', '1300099', '0000031', '0100343', '0100342', '0100341', '0100084', '0100340', '0100276', '0100339', '0100338', '0100324', '0100344', '0100508', '0100273', '0100329', '0100272', '0100303', '0100090', '0100430', '0100429', '0100435', '0100240', '0100239', '0100018', '0100138', '0100139', '0100088', '0100289', '0100569', '0100222', '0100224', '0100227', '0100188', '0100256', '0100101', '0100134', '0100038', '0100317', '0100319', '0100157', '0100253', '0100316', '0100198', '0100030', '0100465', '0100472', '0100028', '0100200', '0100293', '0100104', '0000462', '2800033', '2800152', '2800032', '2800150', '2800031', '2800108', '2800247', '2800191', '2800156', '2800169', '2800001', '2800162', '2800163', '2800164', '2800125', '2800030', '2800028', '0000419', '2200078', '2200143', '2200039', '2200274', '2200379', '2200080', '2200273', '2200205', '2200112', '2200037', '2200076', '2200277', '2200074', '2200322', '2200320', '2200035', '2200212', '2200218', '2200248', '2200036', '2200211', '2200209', '2200208', '2200265', '2200073', '2200312', '2200314', '0000143', '4801029', '4801030', '4800307', '4801033', '4801031', '4801171', '4800227', '4800306', '4800901', '4801289', '4800309', '4800416', '4800531', '4801183', '4800786', '4801181', '4800365', '4801180', '4800530', '4801168', '4800785', '4800096', '4800478', '4800097', '4800107', '4800106', '4800100', '4800586', '4800099', '4801026', '4800058', '4800842', '4800843', '4800467', '4800646', '4800056', '4800645', '4800456', '4800048', '4800455', '4801124', '4800778', '4800046', '4800853', '4800852', '4800045', '4801244', '4800681', '4800738', '4800291', '4800362', '4800363', '4800539', '4800295', '4800288', '4800540', '4800634', '4800554', '4801293', '4801292', '4800549', '4801294', '4800292', '4801290', '4800283', '4800702', '4800754', '4800281', '4800755', '4800756', '4800294', '4800550', '4800552', '4800553', '4800624', '4800823', '4801012', '4800536', '4800751', '4801307', '4801295', '4800743', '4800300', '4800746', '4800749', '4800516', '4801299', '0000588', '3500100', '3500044', '3500086', '3500106', '3500137', '3500015', '3500143', '3500041', '3500024', '0000310', '0400107', '0400029', '0400098', '0400105', '0400097', '0400030', '0400031', '0400033', '0400034', '0400036', '0400111', '0400110', '0400118', '0400037', '0400108', '0400120', '0400119', '0400103', '0400026', '0400079', '0400134', '0400072', '0400099', '0400044', '0400045', '0400135', '0400080', '0400048', '0400112', '0400092', '0400053', '0400060', '0000146', '0600798', '0600648', '0600758', '0600796', '0600039', '0600646', '0600797', '0600747', '0600516', '0600750', '0600584', '0600746', '0600585', '0600586', '0600042', '0600770', '0600434', '0600689', '0600464', '0600688', '0600384', '0600588', '0600460', '0600408', '0600799', '0600402', '0600766', '0600686', '0600079', '0600080', '0600085', '0600685', '0600084', '0600751', '0600322', '0600427', '0600316']
 The length of the whole path 319
 ['Charlotte', 'Dallas', 'Tucson', 'Los Angeles']
 A star Path Cost: 2419.9700735372285
 A star duration: 6.368658781051636

 The number of explored nodes of Bi-A star: 7692
 The whole path: ['3700421', '3700258', '3700257', '3700142', '3700422', '3700001', '3700235', '3700234', '3700330', '3700329', '3700002', '3700356', '3700355', '3700357', '3700197', '3700198', '0000529', '4500042', '4500270', '4500231', '4500069', '4500023', '4500233', '4500094', '4500095', '4500096', '4500097', '4500234', '4500225', '4500104', '4500082', '4500164', '4500015', '4500181', '4500167', '0000533', '1300133', '1300197', '1300132', '1300146', '1300198', '1300204', '1300208', '1300087', '1300279', '1300088', '1300369', '1300459', '1300458', '1300090', '1300460', '1300107', '1300210', '1300398', '1300099', '0000031', '0100343', '0100342', '0100341', '0100084', '0100340', '0100276', '0100339', '0100338', '0100324', '0100344', '0100508', '0100273', '0100329', '0100272', '0100303', '0100090', '0100430', '0100429', '0100435', '0100240', '0100367', '0100368', '0100282', '0100093', '0100152', '0100375', '0100304', '0100180', '0100278', '0100268', '0100387', '0100544', '0100137', '0100525', '0100312', '0100166', '0100167', '0100026', '0100109', '0100361', '0000466', '2800215', '2800039', '2800143', '2800214', '2800213', '2800058', '2800221', '2800059', '2800057', '2800209', '0000410', '4700333', '4700249', '4700217', '4700216', '4700355', '4700352', '4700005', '4700219', '4700220', '0000407', '0500006', '0500054', '0500281', '0500055', '0500074', '0500292', '0500228', '0500073', '0500216', '0500204', '0500071', '0500040', '0500309', '0500144', '0500143', '0500315', '0500314', '0500313', '0500311', '0500312', '0500316', '0500017', '0500117', '0500104', '0500320', '0500018', '0500240', '0500251', '0500019', '0500248', '0500005', '0500020', '0500134', '0000573', '4800439', '4800085', '4800410', '4801165', '4800956', '4801086', '4800081', '4800583', '4800437', '4801087', '4800585', '4800080', '4800079', '4800076', '4800077', '4800075', '4800687', '4800088', '4800440', '4801172', '4800089', '4800112', '4801175', '4800482', '4800481', '4800784', '4800057', '4800054', '4800469', '4800470', '4800471', '4800644', '4800642', '4800641', '4800643', '4800645', '4800456', '4800048', '4800455', '4801124', '4800778', '4800046', '4800853', '4800852', '4800045', '4801244', '4800681', '4800738', '4800291', '4800362', '4800363', '4800539', '4800295', '4800288', '4800540', '4800634', '4800554', '4801293', '4801292', '4800549', '4801294', '4800292', '4801290', '4800283', '4800702', '4800754', '4800281', '4800755', '4800756', '4800294', '4800550', '4800552', '4800553', '4800624', '4800823', '4801012', '4800536', '4800751', '4801307', '4801295', '4800743', '4800300', '4800746', '4800749', '4800516', '4801299', '0000588', '3500100', '3500044', '3500086', '3500106', '3500137', '3500015', '3500143', '3500041', '3500024', '0000310', '0400107', '0400029', '0400098', '0400105', '0400097', '0400030', '0400031', '0400033', '0400034', '0400036', '0400111', '0400110', '0400118', '0400037', '0400108', '0400120', '0400119', '0400103', '0400026', '0400079', '0400134', '0400072', '0400099', '0400044', '0400045', '0400135', '0400080', '0400048', '0400112', '0400092', '0400053', '0400060', '0000146', '0600798', '0600648', '0600758', '0600796', '0600039', '0600646', '0600797', '0600747', '0600516', '0600750', '0600584', '0600746', '0600585', '0600586', '0600042', '0600770', '0600434', '0600689', '0600464', '0600688', '0600384', '0600588', '0600460', '0600408', '0600799', '0600402', '0600766', '0600686', '0600079', '0600080', '0600085', '0600685', '0600084', '0600751', '0600322', '0600427', '0600316']
 The length of the whole path 319
 ['Charlotte', 'Dallas', 'Tucson', 'Los Angeles']
 Bi-A star Path Cost: 2419.9700735372285
 Bi-A star duration:  61.220252990722656

 Tri-Search of (Charlotte, Los Angeles, Chicago)
 The whole path: ['0600316', '0600427', '0600322', '0600751', '0600084', '0600685', '0600085', '0600080', '0600079', '0600686', '0600766', '0600402', '0600799', '0600408', '0600460', '0600588', '0600384', '0600688', '0600463', '0600435', '0600107', '0600775', '0600769', '0600436', '0600032', '0600414', '0600867', '0600866', '0600031', '0600033', '0600795', '0600602', '0600603', '0600036', '0600604', '0600871', '0600870', '0600872', '0600495', '0000144', '0400113', '0400114', '0400009', '0400010', '0400116', '0400117', '0400148', '0400074', '0400146', '0400147', '0400064', '0400005', '0400006', '0400063', '0400100', '0400075', '0400071', '0400070', '0400002', '0400050', '0000312', '3500036', '3500062', '3500063', '3500068', '3500069', '3500101', '3500111', '3500061', '3500109', '3500084', '3500089', '3500102', '3500065', '3500066', '3500032', '3500027', '3500119', '3500071', '3500070', '3500090', '3500107', '3500072', '3500013', '3500047', '3500039', '3500141', '3500025', '3500099', '0000257', '4801203', '4800003', '4801200', '4800002', '0000248', '4000264', '4000138', '4000231', '0000246', '2000206', '2000503', '2000360', '2000427', '2000500', '2000452', '2000207', '2000419', '2000501', '2000502', '2000073', '2000074', '2000075', '2000473', '2000519', '2000506', '2000294', '2000295', '2000296', '2000514', '2000523', '2000077', '2000292', '2000504', '2000293', '2000092', '2000311', '2000472', '2000470', '2000094', '2000095', '2000404', '2000097', '2000277', '2000102', '2000414', '2000103', '2000104', '2000106', '2000356', '2000114', '2000372', '2000117', '2000465', '2000466', '2000467', '2000270', '2000258', '2000257', '2000256', '2000260', '0000232', '2900371', '2900374', '2900378', '2900238', '2900184', '2900358', '2900288', '2900289', '2900098', '2900366', '2900341', '2900367', '2900108', '2900287', '2900241', '2900103', '2900482', '2900102', '2900545', '2900556', '2900111', '2900120', '2900122', '2900494', '2900355', '2900121', '2900162', '2900165', '2900566', '2900468', '2900164', '0000395', '1900057', '1900382', '1900070', '0000393', '1701225', '1700286', '1701010', '1701170', '1700285', '1701321', '1701325', '1701326', '1701323', '1700750', '1701328', '1701327', '1700292', '1700281', '1700280', '1701120', '1700301', '1700922', '1701121', '1700487', '1700480', '1700479', '1700478', '1700477', '1700430', '1700431', '1701157', '1700449', '1700419', '1700465', '1700418', '1701034', '1701194', '1700417', '1700629', '1701394', '1700653', '1700631', '1700415', '1701267', '1701265', '1701291', '1700899', '1700919', '1701607', '1700411', '1700792', '1700624', '1700635', '1700434', '1701056', '1701063', '1701062', '1700651', '1700803', '0000580', '1800373', '1800287', '1800698', '1800697', '1800699', '1800700', '1800377', '1800404', '1800710', '1800709', '1800708', '1800705', '1800706', '1800396', '1800237', '1800228', '1800227', '1800226', '1800474', '1800475', '1800280', '1800416', '1800185', '1800195', '1800200', '1800317', '1800201', '1800145', '1800144', '1800146', '1800167', '1800461', '1800162', '1800460', '1800653', '1800651', '1800652', '1800513', '1800512', '1800511', '1800328', '1800100', '1800577', '1800095', '1800560', '1800096', '1800492', '1800746', '1800744', '1800070', '1800743', '1800069', '0000013', '3901034', '3900294', '3900813', '3900814', '3900526', '3900527', '3900290', '3900293', '3900896', '3900528', '0000183', '2100102', '2100259', '2100101', '2100296', '2100328', '2100321', '2100094', '2100093', '2100092', '2100088', '2100291', '2100398', '2100162', '2100322', '2100246', '2100184', '2100051', '2100186', '2100385', '2100384', '2100383', '2100381', '2100382', '2100053', '2100055', '2100160', '2100059', '2100216', '2100060', '2100158', '2100065', '2100064', '2100152', '2100156', '0000025', '5100179', '5100126', '5100128', '5100122', '5100121', '5100269', '5100384', '5100061', '5100385', '5100062', '5100436', '0000568', '4700230', '4700231', '4700101', '4700302', '4700143', '4700410', '4700141', '4700144', '0000200', '3700359', '3700082', '3700375', '3700126', '3700376', '3700127', '3700224', '3700091', '3700225', '3700125', '3700144', '3700077', '3700075', '3700076', '3700004', '3700256', '3700258', '3700421']
 The length of the whole path 381
 ['Los Angeles', 'Chicago', 'Charlotte']
 Tri-A star Path Cost: 2760.516003492685
 Tri-directional search duration: 22.166738271713257

  ----jGRASP: operation complete.
 '''
