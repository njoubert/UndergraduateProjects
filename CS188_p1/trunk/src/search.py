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

def depthFirstSearch(problem):
  """
  Search the deepest nodes in the search tree first. [p 74].

  Your search algorithm needs to return a 3-item tuple (path, actions, totalcost)
    path: a list of states in the search problem from start to goal
    actions: the sequence of actions that reaches the goal
    totalcost: the total cost of the path

  Of course, the actions are the most important by far; we will use these
  to determine the correctness of your algorithm.
  """
  "*** YOUR CODE HERE ***"
  closed = []
  fringe = util.Stack()
  #while(problem.isGoalState(currentState)== False):
  fringe.push(problem.getStartState)
  while 1:
      if (fringe.isEmpty()):
          return None
      else:
          currentElement = fringe.pop()
          currentState = currentElement[0]
      if(problem.isGoalState(currentState)==False):
          return #THE LIST
      if (closed.count(currentState) == 0):
          closed.append(currentState)
          successors = problem.getSuccessors(currentState)
          for one_successor in successors:
              fringe.push(currentElement+one_successor)
      #print currentState
  #util.raiseNotDefined()

def breadthFirstSearch(problem):
  "Search the shallowest nodes in the search tree first. [p 74]"
  "*** YOUR CODE HERE ***"
  util.raiseNotDefined()

def uniformCostSearch(problem):
  "Search the node of least total cost first. "
  "*** YOUR CODE HERE ***"
  util.raiseNotDefined()

def nullHeuristic(state):
  """
  A heuristic function estimates the cost from the current state to the nearest
  goal in the provided searchProblem.  This one is trivial.
  """
  return 0

def aStarSearch(problem, heuristic=nullHeuristic):
  "Search the node that has the lowest combined cost and heuristic first."
  "*** YOUR CODE HERE ***"
  util.raiseNotDefined()

def greedySearch(problem, heuristic=nullHeuristic):
  "Search the node that has the lowest heuristic first."
  "*** YOUR CODE HERE ***"
  util.raiseNotDefined()


