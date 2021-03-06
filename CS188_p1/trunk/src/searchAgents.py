"""
This file contains all of the agents that can be selected to
control Pacman.  To select an agent, simple use the '-p' option
when running pacman.py.  That is, to load your DepthFirstSearchAgent,
just run the following command from the command line:

> python pacman.py -p DepthFirstSearchAgent

Please only change the parts of the file where you are asked; look
for the lines that say

"*** YOUR CODE HERE ***"

The parts you fill in start about 3/4 of the way down.  Follow the
project description for details.

Good luck and happy searching!
"""
from game import Directions
from game import Agent
from game import Actions
from util import manhattanDistance
import util
import time
import search

class GoWestAgent(Agent):
  """
  An agent that goes West until it can't.
  """
  def getAction(self, state):
    """
      The agent receives a GameState (pacman.py).
    """
    if Directions.WEST in state.getLegalPacmanActions():
      return Directions.WEST
    else:
      return Directions.STOP

#######################################################
# This portion is written for you, but will only work #
#       after you fill in parts of search.py and      #
#           complete the SearchAgent class            #
#######################################################

class PositionSearchProblem(search.SearchProblem):
  """
  A search problem defines the state space, start state, goal test,
  successor function and cost function.  This search problem can be
  used to find paths to a particular point on the pacman board.

  The state space consists of (x,y) positions in a pacman game.

  This search problem is fully specified and should not require change.
  """

  def __init__(self, gameState, costFn = lambda x: 1, goal=(1,1)):
    """
    Stores the start and goal.

    gameState: A GameState object (pacman.py)
    costFn: A function from a search state (tuple) to a non-negative number
    goal: A position in the gameState
    """
    self.walls = gameState.getWalls()
    self.startState = gameState.getPacmanPosition()
    self.goal = goal
    self.costFn = costFn
    if gameState.getNumFood() != 1 or not gameState.hasFood(*goal):
      print 'Warning: this does not look like a regular search maze'

    # For display purposes
    self._visited, self._visitedlist, self._expanded = {}, [], 0

  def getStartState(self):
    return self.startState

  def isGoalState(self, state):
     isGoal = state == self.goal

     # For display purposes only
     if isGoal:
       print 'Goal found after expanding %d nodes.' % self._expanded
       self._visitedlist.append(state)
       import __main__
       if '_display' in dir(__main__):
         if 'drawExpandedCells' in dir(__main__._display): #@UndefinedVariable
           __main__._display.drawExpandedCells(self._visitedlist) #@UndefinedVariable

     return isGoal

  def getSuccessors(self, state):
    """
    Returns successor states, the actions they require, and a cost of 1.

     As noted in search.py:
         For a given state, this should return a list of triples,
     (successor, action, stepCost), where 'successor' is a
     successor to the current state, 'action' is the action
     required to get there, and 'stepCost' is the incremental
     cost of expanding to that successor
    """

    successors = []
    for action in [Directions.NORTH, Directions.SOUTH, Directions.EAST, Directions.WEST]:
      x,y = state
      dx, dy = Actions.directionToVector(action)
      nextx, nexty = int(x + dx), int(y + dy)
      if not self.walls[nextx][nexty]:
        nextState = (nextx, nexty)
        cost = self.costFn(nextState)
        successors.append( ( nextState, action, cost) )

    # Bookkeeping for display purposes
    self._expanded += 1
    if state not in self._visited:
      self._visited[state] = True
      self._visitedlist.append(state)

    return successors

  def getCostOfActions(self, actions):
    """Returns the cost of a particular sequence of actions.  If those actions
    include an illegal move, return 999999"""
    if not actions:
      return 999999
    x,y= self.getStartState()
    cost = 0
    for action in actions:
      # Check figure out the next state and see whether its' legal
      dx, dy = Actions.directionToVector(action)
      x, y = int(x + dx), int(y + dy)
      if self.walls[x][y]:
        return 999999
      cost += self.costFn((x,y))
    return cost


class CustomPositionSearchProblem(PositionSearchProblem):
  """
  A search problem defines the state space, start state, goal test,
  successor function and cost function.  This search problem can be
  used to find paths to a particular point on the pacman board.

  The state space consists of (x,y) positions in a pacman game.

  This search problem is fully specified and should not require change.
  """

  def __init__(self, gameState, costFn = lambda x: 1, goal=(1,1), customStart=None):
    """
    Stores the start and goal.

    gameState: A GameState object (pacman.py)
    costFn: A function from a search state (tuple) to a non-negative number
    goal: A position in the gameState
    """
    self.walls = gameState.getWalls()
    if (customStart == None):
        self.startState = gameState.getPacmanPosition()
    else:
        self.startState = customStart
    self.goal = goal
    self.costFn = costFn

  def isGoalState(self, state):
     isGoal = state == self.goal
     return isGoal

  def getSuccessors(self, state):
    """
    Returns successor states, the actions they require, and a cost of 1.

     As noted in search.py:
         For a given state, this should return a list of triples,
     (successor, action, stepCost), where 'successor' is a
     successor to the current state, 'action' is the action
     required to get there, and 'stepCost' is the incremental
     cost of expanding to that successor
    """

    successors = []
    for action in [(0,1), (0,-1), (1, 0), (-1, 0)]:
      nextx, nexty = int(state[0] + action[0]), int(state[1] + action[1])
      if not self.walls[nextx][nexty]:
        successors.append( ( (nextx,nexty), action, 1) )

    return successors

class SearchAgent(Agent):
  """
  This very general search agent finds a path using a supplied search algorithm for a
  supplied search problem, then returns actions to follow that path.

  As a default, this agent runs DFS on a PositionSearchProblem to find location (1,1)
  """

  def __init__(self, searchFunction=None, searchType=PositionSearchProblem):
    self.searchFunction = searchFunction
    self.searchType = searchType

  def registerInitialState(self, state):
    """
    This is the first time that the agent sees the layout of the game board. Here, we
    choose a path to the goal.  In this phase, the agent should compute the path to the
    goal and store it in a local variable.

    state: a GameState object (pacman.py)
    """
    if self.searchFunction == None:
      import sys
      print "No search function provided for SearchAgent"
      sys.exit(1)

    # If you wrap your solution in the timing code provided, you'll know how long the pathfinding takes.
    starttime = time.time()
    problem = self.searchType(state)
    self.actions = self.searchFunction(problem)
    if (self.actions == None):
        import sys
        print 'No solution found??!??!??!?! WTF!!! HELP!'
        sys.exit(1)
    print 'Path found with total cost of %d in %.1f seconds' % (problem.getCostOfActions(self.actions), time.time() - starttime)

  def getAction(self, state):
    """
    Returns the next action in the path chosen earlier (in registerInitialState).  Return
    Directions.STOP if there is no further action to take.

    state: a GameState object (pacman.py)
    """
    if (len(self.actions) == 0):
        return Directions.STOP
    return self.actions.pop(0)

class TinyMazeSearchAgent(SearchAgent):
  """
  An agent which uses tinyClassSearch to find a path (which only returns a
  correct path for tinyMaze) and follows that path.
  """
  def __init__(self):
    SearchAgent.__init__(self, search.tinyMazeSearch)


class DepthFirstSearchAgent(SearchAgent):
  """
  An agent that first computes a path to the goal using DFS, then
  follows that path.
  """
  def __init__(self):
    SearchAgent.__init__(self, search.depthFirstSearch)

class BreadthFirstSearchAgent(SearchAgent):
  """
  An agent that first computes a path to the goal using BFS, then
  follows that path.
  """
  def __init__(self):
    SearchAgent.__init__(self, search.breadthFirstSearch)

class UniformCostSearchAgent(SearchAgent):
  """
  An agent that computes a path to the goal position using UCS.
  """
  def __init__(self):
    SearchAgent.__init__(self, search.uniformCostSearch)

class StayEastSearchAgent(SearchAgent):
  """
  An agent that computes a path to the goal position using UCS, but
  lets its cost function guide it eastward.
  """
  def __init__(self):
    problem = lambda x: PositionSearchProblem(x, stayEastCost)
    SearchAgent.__init__(self, search.uniformCostSearch, problem)

class StayWestSearchAgent(SearchAgent):
  """
  An agent that computes a path to eat all the dots using UCS, but
  lets its cost function guide it westward.

  """
  def __init__(self):
    problem = lambda x: PositionSearchProblem(x, stayWestCost)
    SearchAgent.__init__(self, search.uniformCostSearch, problem)

def stayEastCost(position):
  """
  Gives a cost for each (x,y) position that guides a search agent eastward
  """
  return .5 ** position[0]

def stayWestCost(position):
  """
  Gives a cost for each (x,y) position that guides a search agent westward
  """
  return 2 ** position[0]

class FoodSearchProblem(search.SearchProblem):
  """
  A search problem associated with finding the a path that collects all of the
  food (dots) in a Pacman game.

  A search state in this problem is a tuple ( pacmanPosition, foodGrid ) where
    pacmanPosition: a tuple (x,y) of integers specifying Pacman's position
    foodGrid:       a Grid (see game.py) of either True or False, specifying remaining food
  """
  def __init__(self, state):
    self.start = (state.getPacmanPosition(), state.getFood())
    self.walls = state.getWalls()
    self._expanded = 0

  def getStartState(self):
    return self.start

  def isGoalState(self, state):
    if state[1].count() == 0:
      print 'Goal found after expanding %d nodes.' % self._expanded
      return True
    return False

  def getSuccessors(self, state):
    """
    Returns successor states, the actions they require, and a cost of 1.

     As noted in search.py:
         For a given state, this should return a list of triples,
     (successor, action, stepCost), where 'successor' is a
     successor to the current state, 'action' is the action
     required to get there, and 'stepCost' is the incremental
     cost of expanding to that successor

    """
    successors = []
    self._expanded += 1
    for direction in [Directions.NORTH, Directions.SOUTH, Directions.EAST, Directions.WEST]:
      x,y = state[0]
      dx, dy = Actions.directionToVector(direction)
      nextx, nexty = int(x + dx), int(y + dy)
      if not self.walls[nextx][nexty]:
        nextFood = state[1].copy()
        nextFood[nextx][nexty] = False
        successors.append( ( ((nextx, nexty), nextFood), direction, 1) )
    return successors

  def getCostOfActions(self, actions):
    """Returns the cost of a particular sequence of actions.  If those actions
    include an illegal move, return 999999"""
    x,y= self.getStartState()[0]
    cost = 0
    for action in actions:
      # Check figure out the next state and see whether its' legal
      dx, dy = Actions.directionToVector(action)
      x, y = int(x + dx), int(y + dy)
      if self.walls[x][y]:
        return 999999
      cost += 1
    return cost

class UniformCostFoodSearchAgent(SearchAgent):
  """
  An agent that computes a path to eat all the dots using UCS.
  """
  def __init__(self):
    SearchAgent.__init__(self, search.uniformCostSearch, FoodSearchProblem)

def manhattanAStar(problem):
  """
  A wrapper for A* that uses the Manhattan distance heuristic.
  """
  return search.aStarSearch(problem, lambda x: manhattanDistance(x, problem.goal))

class ManhattanAStarSearchAgent(SearchAgent):
  """
  An agent that computes a path to the goal position using AStar and
  the Manhattan distance heuristic.
  """
  def __init__(self):
    SearchAgent.__init__(self, manhattanAStar, PositionSearchProblem)

def trivialFoodHeuristic(state):
  """
   A trivial heuristic for the all-food problem,  which returns 0 for goal states and 1 otherwise.
  """
  if(state[1].count() == 0):
    return 0
  else:
    return 1

###########################################################
# You have to fill in several parts of the following code #
###########################################################

def getFoodHeuristic(gameState):
  """
  Instead of filling in the foodHeuristic function directly, you can fill in
  this function which takes a full gameState for Pacman (see pacman.py) and
  returns a heuristic function.  The heuristic function must
    - take a single parameter, a search state
    - return a non-negative number that is the value of the heuristic at that state

  This function is *only* here for students who want to create more complex
  heuristics that use aspects of the gameState other than the food Grid and
  Pacman's location (such as where the walls are, etc.)

  Note: The state that will be passed to your heuristic function is a tuple
  ( pacmanPosition, foodGrid ) where foodGrid is a Grid (see game.py) of either
  True or False.
  """
  # If you don't want to implement this method, you can leave this default implementation
  #return foodHeuristic

  print "Preprocessing started."
  #get positions of all the food pellets on the board
  #edgeInfo = util.FasterPriorityQueue()
  distanceInfo = {}
  foodGrid = gameState.getFood()
  foodList = foodGridToFoodList(foodGrid)
  for startPellet in foodList:
      distanceInfo[startPellet] = {}
      for endPellet in foodList:
          if (startPellet == endPellet):
              continue
          #if (distanceInfo.has_key((endPellet,startPellet))):
          #    continue
          problem = CustomPositionSearchProblem(gameState, lambda x: 1, endPellet, startPellet)
          actions = search.breadthFirstSearch(problem)
          distance = len(actions) #because costFunction = 1
          distanceInfo[startPellet][endPellet] = distance
          #edgeInfo.push((startPellet,endPellet),distance)

  print "Preprocessing done."
  #This is an example of Closures, which does not exist in C.
  #heuristicFn = lambda state: minSpanningTreeHeuristic(state,gameState, distanceInfo)
  heuristicFn = lambda state: primsMinSpanningTreeHeuristic(state, gameState, distanceInfo)  
  return heuristicFn

def checkOtherNodeInTree(forest,secondNode):
    list = None
    for tree in forest:
        if(tree.count(secondNode)>0):
            list = tree
            break
    return list

def primsMinSpanningTreeHeuristic(state, gameState, edges):
    vNew = []
    totalDist = 0
    
    currentFoodGrid = state[1]
    vertices = foodGridToFoodList(currentFoodGrid)
           
    if (len(vertices) == 0):
        return 0
    
    start = state[0] #Pacman's position
    
    delStart = False
    if (start not in edges.keys()): #This is only to keep the edge hash from becoming really big.
        delStart = True
        edges[start] = {}
        for pellet in vertices:
          problem = CustomPositionSearchProblem(gameState, lambda x: 1, pellet, start)
          actions = search.breadthFirstSearch(problem)
          distance = len(actions) #we can do this because the cost function is 1
          edges[start][pellet] = distance 
          #DO WE NEED TO ADD THE START AS A DESTINATION TO ALL THE OTHER PELLETS TOO? I DONT THINK SO...
    
    vNew.append(start)
    
    nearestV = None
    nearestD = 999999
    while len(vertices) > 0:
        nearestV = None
        nearestD = 999999 
        for sVertex in vNew:
            for eVertex in vertices:
                if (edges[sVertex][eVertex] < nearestD):
                    nearestD = edges[sVertex][eVertex]
                    nearestV = eVertex
        assert nearestV != None
        vNew.append(eVertex)
        vertices.remove(eVertex)
        totalDist += nearestD
        
    if (delStart):
        del edges[start]
    
    #print "h = ",totalDist,", ",foodCount," food, ", len(edges.keys()), "nodes in edge hash: ", edges.keys()
    #print "Prim distance=", totalDist, ", pellet count=", len(vertices), " and pacman position=", start    
    return totalDist
                    
def minSpanningTreeHeuristic(state,gameState, distanceInfo):
    totalDistance =0
    nodeCount=1
    for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              nodeCount +=1
              problem = CustomPositionSearchProblem(gameState, lambda x: 1, (i,j), state[0])
              actions = search.breadthFirstSearch(problem)
              distance = problem.getCostOfActions(actions)
              distanceInfo[(state[0],(i,j))] = distance
    edgeInfo = util.FasterPriorityQueue()
    for distance in distanceInfo.items():
        edgeInfo.push(distance[0],distance[1])
    vertices = []
    forest = []

    while(not edgeInfo.isEmpty()):
        closestNode = edgeInfo.pop()
        oneNode,secondNode = closestNode
        closestDistance = distanceInfo[closestNode]
        cycling = False
        appended = False
        #CHECKING FOR CYCLING
        for tree in forest:
            if(tree.count(oneNode)>0 and tree.count(secondNode) > 0):
                cycling = True
                break
        if(cycling == False):
            for tree in forest:
                if(tree.count(oneNode)>0):
                    otherTree = checkOtherNodeInTree(forest,secondNode)
                    if(otherTree == None):
                        tree.append(secondNode)
                        appended = True
                        break
                    else:
                        tree = tree+otherTree
                        appended = True
                        break
                if(tree.count(secondNode)>0):
                    otherTree = checkOtherNodeInTree(forest,oneNode)
                    if(otherTree == None):
                        tree.append(oneNode)
                        appended = True
                        break
                    else:
                        tree = tree+otherTree
                        appended = True
                        break

        if(appended == False and cycling == False):
            tree = []
            tree.append(oneNode)
            tree.append(secondNode)
            forest.append(tree)
        if(cycling == False):
            totalDistance += closestDistance
            if(vertices.count(oneNode)==0):
                vertices.append(oneNode)
            if(vertices.count(secondNode)==0):
                vertices.append(secondNode)
            if(len(vertices)==nodeCount):
                break
    return totalDistance

def foodHeuristic(state):
  """
  Here, you can write your food heuristic function instead of using getFoodHeuristic.
  This heuristic must be admissible (if your AStarFoodSearchAgent and your
  UniformCostSearchAgent *ever* find solutions of different length, your heuristic
  is *not* admissible).

  The state is a tuple ( pacmanPosition, foodGrid ) where foodGrid is a
  Grid (see game.py) of either True or False.

  Note that this function *does not* have access to the location of walls, capsules,
  ghosts, etc.  If you want to work with this information, you should implement
  getFoodHeuristic instead of this function.

  Hint: getFoodHeuristic can return a heuristic that encapsulates data through a
  function closure (like the manhattanAStar function above).  If you don't know how
  this works, come to office hours.
  """

  #return hMaxDistancePlusPelletAmount(state) #NA
  #return hDistanceToAll(state) #NA
  return hMaxDistance(state)
  #return hMinDistance(state)
  #return hMinOrMaxDistance(state)
  #return hDistanceBetweenAll(state) #NA

def foodGridToFoodList(grid):
  foodList = []
  for i in range (0,grid.width):
      for j in range (0,grid.height):
          if (grid[i][j] == True):
              foodList.append((i,j))
  return foodList

def foodGridToFoodSet(grid):
  foodList = set([])
  for i in range (0,grid.width):
      for j in range (0,grid.height):
          if (grid[i][j] == True):
              foodList.add((i,j))
  return foodList

def findClosestFoodToPointInGrid(grid, point):
  minDistance = 999999
  closest = None
  for i in range (0,grid.width):
      for j in range (0,grid.height):
          if (grid[i][j] == True):
              d = util.manhattanDistance( (i,j), point)
              if (d < minDistance):
                  minDistance = d
                  closest = (i,j)

  if closest == None:
      return point
  return closest

def findClosestFoodToPointInList(list, point):
  minDistance = 999999
  closest = None
  for el in list:
      d = util.manhattanDistance(el, point)
      if (d < minDistance):
          minDistance = d
          closest = el
  return closest

def hMaxDistancePlusPelletAmount(state):
  """
   NOT ADMISSABLE
  """
  maxDistance = 0
  for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              d = util.manhattanDistance( (i,j), state[0])
              if (d > maxDistance):
                  maxDistance = d

  amountOfPelletsOnBoard = state[1].count()
  heuristic = amountOfPelletsOnBoard + maxDistance
  return heuristic

def hMaxDistance(state):
  maxDistance = 0
  for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              d = util.manhattanDistance( (i,j), state[0])
              if (d > maxDistance):
                  maxDistance = d
  heuristic = maxDistance
  return heuristic

def hMinDistance(state):
  minDistance = 999999
  hasSet = False
  for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              d = util.manhattanDistance( (i,j), state[0])
              if (d < minDistance):
                  hasSet = True
                  minDistance = d
  if hasSet == False:
      print "AAAAAAAAAAAAAA"
      minDistance = 0
  heuristic = minDistance
  return heuristic

def hMinOrMaxDistance(state):
  maxDistance = 0
  minDistance = 999999
  hasSetN = False
  for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              d = util.manhattanDistance( (i,j), state[0])
              if (d > maxDistance):
                  maxDistance = d
              if (d < minDistance):
                  hasSetN = True
                  minDistance = d
  if hasSetN == False:
      minDistance = 0
  heuristic = max(maxDistance, minDistance)
  if heuristic != maxDistance:
      print "AAAAAAAAAAAAAAAAA maxDistance=", maxDistance, " minDistance=", minDistance
  #print "max=", maxDistance, " min=", minDistance, " heuristic=", heuristic
  return heuristic

def hDistanceToAll(state):
  """
  This is a NON-ADMISSABLE HEURISTIC!
  """
  totDistance = 0
  for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              d = util.manhattanDistance( (i,j), state[0])
              totDistance += d

  heuristic = totDistance
  return heuristic

def hDistanceBetweenAll(state):
  """
  NOT ADMISSABLE --- WHY?!?!?!?!
  """
  foodPositions = []
  for i in range (0,state[1].width):
      for j in range (0,state[1].height):
          if (state[1][i][j] == True):
              foodPositions.append((i,j))

  heuristic = 0
  currentClosest = findClosestFoodToPointInList(foodPositions, state[0])
  if (currentClosest == None):
      return 0
  heuristic += util.manhattanDistance(state[0], currentClosest)
  while 1:
      foodPositions.remove(currentClosest)
      if (len(foodPositions) == 0):
          break
      newClosest = findClosestFoodToPointInList(foodPositions, currentClosest)
      heuristic += util.manhattanDistance(currentClosest, newClosest)
      currentClosest = newClosest
  #return 0
  return heuristic

def greedyFoodHeuristic(state):
    boardCount = state[1].count()
    #closest = findClosestFoodToPointInGrid(state[1], state[0])
    #distanceToClosest = util.manhattanDistance(closest, state[0])
    return boardCount

def foodAStar(state):
  """
  A wrapper for A* that uses the Food heuristic.
  """
  return search.aStarSearch(problem, getFoodHeuristic(problem.getStartState()))

class AStarFoodSearchAgent(SearchAgent):
  """
  An agent that computes a path to eat all the dots using AStar.

  You should use either foodHeuristic or getFoodHeuristic in your code here.
  """
  def __init__(self, searchFunction=None, searchType=FoodSearchProblem):
    self.searchFunction = search.aStarSearch
    self.searchType = searchType

  def registerInitialState(self, state):
    """
    This is the first time that the agent sees the layout of the game board. Here, we
    choose a path to the goal.  In this phase, the agent should compute the path to the
    goal and store it in a local variable.

    state: a GameState object (pacman.py)
    """
    if self.searchFunction == None:
      import sys
      print "No search function provided for SearchAgent"
      sys.exit(1)

    # If you wrap your solution in the timing code provided, you'll know how long the pathfinding takes.
    starttime = time.time()
    problem = self.searchType(state)
    heuristic = getFoodHeuristic(state)

    self.actions = self.searchFunction(problem, heuristic)
    if (self.actions == None):
        import sys
        print 'No solution found??!??!??!?! WTF!!! HELP!'
        sys.exit(1)
    print 'Path found with total cost of %d in %.1f seconds' % (problem.getCostOfActions(self.actions), time.time() - starttime)

  def getAction(self, state):
    """
    Returns the next action in the path chosen earlier (in registerInitialState).  Return
    Directions.STOP if there is no further action to take.

    state: a GameState object (pacman.py)
    """
    if (len(self.actions) == 0):
        return Directions.STOP
    return self.actions.pop(0)

def greedy(problem):
    return search.greedySearch(problem, greedyFoodHeuristic)

class GreedyFoodSearchAgent(SearchAgent):
  """
  An agent that computes a path to eat all the dots using greedy search.
  """
  def __init__(self, searchFunction=None, searchType=FoodSearchProblem):
    SearchAgent.__init__(self, greedy, searchType)

class TrivialAStarFoodSearchAgent(AStarFoodSearchAgent):
  """
  An AStarFoodSearchAgent that uses the trivial heuristic instead of the one defined by getFoodHeuristic
  """
  def __init__(self, searchFunction=None, searchType=FoodSearchProblem):
    # Redefine getFoodHeuristic to return the trivial one.
    __import__(__name__).getFoodHeuristic = lambda gameState: trivialFoodHeuristic
    __import__(__name__).foodHeuristic    = trivialFoodHeuristic
    AStarFoodSearchAgent.__init__(self, searchFunction, searchType)
