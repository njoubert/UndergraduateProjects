import util
## Abstract Search Classes

class SearchProblem:
  """
  Abstract SearchProblem class. Your classes
  should inherit from this class and override
  all the methods below
  """
  def getStartState(self):
     """
     Returns the start state for the search problem
     """
     print 'testing 1 2 3s'
     util.raiseNotDefined()

  def isGoalState(self, state):
     """
       state: Search state

     Returns True if and only if the state is a valid goal state
     """
     util.raiseNotDefined()

  def getSuccessors(self, state):
     """
       state: Search state

     For a given state, this should return a list of triples,
     (successor, action, stepCost), where 'successor' is a
     successor to the current state, 'action' is the action
     required to get there, and 'stepCost' is the incremental
     cost of expanding to that successor
     """
     util.raiseNotDefined()

  def getCostOfActions(self, actions):
     """
      actions: A list of actions to take

     This method returns the total cost of a particular sequence of actions.  The sequence must
     be composed of legal moves
     """
     util.raiseNotDefined()


def tinyMazeSearch(problem):
  """Returns a sequence of moves that solves tinyMaze.  For any other
  maze, the sequence of moves will be incorrect, so only use this for tinyMaze"""
  from game import Directions
  s = Directions.SOUTH
  w = Directions.WEST
  return  [s,s,w,s,w,w,s,w]
  
def simpleSearch(problem, datastructure):
  """
  Your search algorithm needs to return a list of actions.
  
  A node is a state, and a node is a position of Pacman on the board.
  
  a successor is a tuple: ( successor-node, step-action, step-cost ),
      and returned by problem.getSuccessors
  
  a path is a list of successors, and is stored in the fringe,
  where the last tuple is the current pacman position for each list.
  
  The fringe consists of a list of successors:
  [
      [(node, action, cost) ... (node, action, cost)],
      [(node, action, cost) ... (node, action, cost)]
  ]
  
  Of course, the actions are the most important by far; we will use these
  to determine the correctness of your algorithm.
  
  
  The closed list is a list of nodes.
  
  """
  import copy 
  closed = []
  fringe = datastructure()
  fringe.push([(problem.getStartState(),'',0)])
  while 1:
      if (fringe.isEmpty()):
          return None
      currentPath = fringe.pop()
      currentNode = currentPath[len(currentPath)-1][0]
      if(problem.isGoalState(currentNode)==True):
          currentPath.pop(0)
          return [path[1] for path in currentPath]
      if (closed.count(currentNode) == 0):
          closed.append(currentNode)
          successors = problem.getSuccessors(currentNode)
          for successor in successors:
              newPath = copy.copy(currentPath)
              newPath.append(successor)
              fringe.push(newPath)

def depthFirstSearch(problem):
  """
  Search the deepest nodes in the search tree first. [p 74].
  """

  return simpleSearch(problem, util.Stack)

def breadthFirstSearch(problem):
  "Search the shallowest nodes in the search tree first. [p 74]"
  return simpleSearch(problem, util.Queue)
  
def uniformCostSearch(problem):
  "Search the node of least total cost first. "
  import copy 
  closed = []
  fringe = util.PriorityQueue()
  # PATH = (node, action, parent, total cost)
  fringe.push((problem.getStartState(),'',None,0), 0)
  while 1:
      if (fringe.isEmpty()):
          return None
      currentPath = fringe.pop()
      currentNode = currentPath[0]
      if(problem.isGoalState(currentNode)==True):
          actions = []
          while 1:
              if (currentPath == None or currentPath[1] == ''):
                actions.reverse()
                return actions
              actions.append(currentPath[1])
              currentPath = currentPath[2]
      
      if (closed.count(currentNode) == 0):
          closed.append(currentNode)
          successors = problem.getSuccessors(currentNode)
          for successor in successors:
              cost = currentPath[3]+successor[2]
              newPath = (successor[0], successor[1], currentPath, cost)
              fringe.push(newPath, newPath[3])

def nullHeuristic(state):
  """
  A heuristic function estimates the cost from the current state to the nearest
  goal in the provided searchProblem.  This one is trivial.
  """
  return 0

def aStarSearch(problem, heuristic=nullHeuristic):
  "Search the node that has the lowest combined cost and heuristic first."
  import copy 
  closed = []
  fringe = util.PriorityQueue()
  # PATH = (node, action, parent, total cost)
  fringe.push((problem.getStartState(),'',None,0), 0)
  while 1:
      if (fringe.isEmpty()):
          return None
      currentPath = fringe.pop()
      currentNode = currentPath[0]
      if(problem.isGoalState(currentNode)==True):
          actions = []
          while 1:
              if (currentPath == None or currentPath[1] == ''):
                actions.reverse()
                return actions
              actions.append(currentPath[1])
              currentPath = currentPath[2]
      
      if (closed.count(currentNode) == 0):
          closed.append(currentNode)
          successors = problem.getSuccessors(currentNode)
          for successor in successors:
              cost = currentPath[3]
              newPath = (successor[0], successor[1], currentPath, cost)
              fringe.push(newPath, newPath[3])

def greedySearch(problem, heuristic=nullHeuristic):
  "Search the node that has the lowest heuristic first."
  "*** YOUR CODE HERE ***"
  util.raiseNotDefined()


