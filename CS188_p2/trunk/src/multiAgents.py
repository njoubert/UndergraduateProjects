from util import manhattanDistance
from game import Directions
import random, util

from game import Agent

class ReflexAgent(Agent):
  """
    A reflex agent chooses an action at each choice point by examining
    its alternatives via a state evaluation function.

    The code below is provided as a guide.  You are welcome to change it
    in any way you see fit.
  """

  def getAction(self, gameState):
    """
    You do not need to change this method, but you're welcome to.

    getAction chooses among the best options according to the evaluation function.
    
    Just like in the previous project, getAction takes a GameState and returns
    some Directions.X for some X in the set {North, South, West, East, Stop}
    """
    # Collect legal moves and successor states
    legalMoves = gameState.getLegalActions()

    # Choose one of the best actions
    scores = [self.evaluationFunction(gameState, action) for action in legalMoves]
    bestScore = max(scores)
    bestIndices = [index for index in range(len(scores)) if scores[index] == bestScore]
    chosenIndex = random.choice(bestIndices)
    scoreList = [(scores[index], legalMoves[index]) for index in range(len(scores))]
    print scoreList
    "Add more of your code here if you want to"
    
    return legalMoves[chosenIndex]
  
  def evaluationFunction(self, currentGameState, action):
    """
    Design a better evaluation function here. 
    
    The evaluation function takes in the current and proposed successor
    GameStates (pacman.py) and returns a number, where higher numbers are better.
    
    Features we want to use:
    > Score of the game                  bigger == better
    DOTS:    
    > Amount of dots left                smaller == better
    > Total distance to all dots         smaller == better
    > Distance to closest dot            smaller == batter
    GHOSTS:
    > Distance to closest ghost          bigger == better
    > Direction of closest ghost         our direction = BAD, other direction = GOOD
    
    """
    # Useful information you can extract from a GameState (pacman.py)
    successorGameState = currentGameState.generatePacmanSuccessor(action)
    newPos = successorGameState.getPacmanState().getPosition()
    newDir = successorGameState.getPacmanState().getDirection()
    oldFood = currentGameState.getFood()
    newFood = successorGameState.getFood()
    newGhostStates = successorGameState.getGhostStates() 
    newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]
    
    newFoodPositions = newFood.asList()
    oldFoodPositions = oldFood.asList()
    newFoodDistances = [util.manhattanDistance(x, newPos) for x in newFoodPositions]
    oldFoodDistances = [util.manhattanDistance(x, newPos) for x in oldFoodPositions]
    
    #closestFoodDistance = min(newFoodDistances)
    #totalFoodDistance = sum(newFoodDistances)
    closestFoodDistance = min(oldFoodDistances)
    totalFoodDistance = sum(oldFoodDistances)
    
    newGhostDistances = [util.manhattanDistance(g.getPosition(), newPos) for g in newGhostStates]
    newGhostDirections = [g.getDirection() for g in newGhostStates]
    closestGhostDistance = min(newGhostDistances)
    closestGhostIndices = [index for index in range(len(newGhostDistances)) if newGhostDistances[index] == closestGhostDistance]
    closestGhostDirection = newGhostDirections[random.choice(closestGhostIndices)]
    from game import Directions
    OPPOSITEDIRECTIONS = {Directions.NORTH: Directions.SOUTH,
                 Directions.SOUTH: Directions.NORTH,
                 Directions.EAST:  Directions.WEST,
                 Directions.WEST:  Directions.EAST,
                 Directions.STOP:  Directions.STOP}
    
    
    #CALCULATION OF FEATURES:
    featScore = successorGameState.getScore()
    featTotalDots = 1.0 / oldFood.count()
    featClosestDots = 1.0 / (closestFoodDistance+1)
    featTotalDotsDistance = 1.0 / (totalFoodDistance+1)
    featClosestGhostDistance = (closestGhostDistance+1)
    featClosestGhostDirection = 0
    if (newDir == OPPOSITEDIRECTIONS[closestGhostDirection]):
        featClosestGhostDirection = -1
    elif newDir == closestGhostDirection:
        featClosestGhostDirection = 1
     
#    evaluation = #100 * featScore + \
        #15 * featTotalDots + \
 #       100 * featClosestDots + \
        #10 * featTotalDotsDistance + \
  #      15 * featClosestGhostDistance + \
   #     0 * featClosestGhostDirection
     
    evaluation = 1000 * featClosestDots + 1 * featClosestGhostDistance
      
    return evaluation

def scoreEvaluationFunction(currentGameState):
  """
    This default evaluation function just returns the score of the state.
    The score is the same one displayed in the Pacman GUI.
    
    This evaluation function is meant for use with adversarial search agents
    (not reflex agents).
  """
  return currentGameState.getScore()

class MultiAgentSearchAgent(Agent):
  """
    This abstract class** provides some common elements to all of your
    multi-agent searchers.  Any methods defined here will be available
    to the MinimaxPacmanAgent, AlphaBetaPacmanAgent & ExpectimaxPacmanAgent.
    
    You *do not* need to make any changes here, but you can if you want to
    add functionality to all your adversarial search agents.  Please do not
    remove anything, however.
    
    **An abstract class is one that is not meant to be instantiated.  It's
    only partially specified, and designed to be extended.  Agent (game.py)
    is another abstract class.  
  """
  
  def __init__(self, evalFn = scoreEvaluationFunction):
    self.index = 0 # Pacman is always agent index 0
    self.evaluationFunction = evalFn
    
  def setDepth(self, depth):
    """
      This is a hook for feeding in command line argument -d or --depth
    """
    self.depth = depth # The number of search plies to explore before evaluating
    
  def useBetterEvaluation(self):
    """
      This is a hook for feeding in command line argument -b or --betterEvaluation
    """
    self.evaluationFunction = betterEvaluationFunction
    

class MinimaxAgent(MultiAgentSearchAgent):  
  """
    WEEEEEE.... ^_^ a live picture of Niels Joubert in Python
    /------\ 
    | O   O|    -------------------------------\
    |   |  |   /                               |
    \  --  | ---  I'M A HARD CORE MUTHER FUCKER.|
     \----/     \    ... BITCHES...             |
        |        \ -----------------------------/     
      /---\ 
        |
      /   \  
  """ 
  
  def terminalnode(self, gameState, depth):
    if(gameState.isWin() or gameState.isLose() or depth == 0):
        return True
    else:
        return False
  
  def maxvalue(self, gameState, depth):
    if (self.terminalnode(gameState,depth) ):
      return self.evaluationFunction(gameState)
    #print "PACMAN MOVE, depth=", depth
    value = float("-infinity")
    actions = gameState.getLegalActions(0)
    successors = [(action, gameState.generateSuccessor(0,action)) for action in actions]
    for a, s in successors:
      value = max(value,self.minvalue(s,depth,1) )
    return value
    
  """s
    ghostCount starts at 1 for the first ghost, and counts up to the total amount of ghosts,
    upon which we decrease the depth and call maxvalue.
  """  
  def minvalue(self, gameState, depth, ghostCount):
    if (self.terminalnode(gameState,depth) ):
      return self.evaluationFunction(gameState)
    value = float('inf')
    actions = gameState.getLegalActions(ghostCount)
    successors = [(action, gameState.generateSuccessor(ghostCount, action)) for action in actions]
    ghostCount += 1
    for a, s in successors:
      if ghostCount > gameState.getNumAgents()-1:
        value = min(value, self.maxvalue(s, depth-1) )
      else:
        value = min(value, self.minvalue(s, depth, ghostCount))
    #print "GHOST MOVE, value=", value
    return value
    
    
  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth 
      and self.evaluationFunction.
    """
    #print "INITIAL PACMAN MOVE!"
    nextActions = gameState.getLegalActions(0);
    nextStates = [gameState.generatePacmanSuccessor(action) for action in nextActions]
    stateValues = [self.evaluationFunction(state) for state in nextStates]
    utilityValues = [self.minvalue(state, self.depth, 1) for state in nextStates]
    bestUtility = max(utilityValues)
    bestIndices = [index for index in range(len(utilityValues)) if utilityValues[index] == bestUtility]
    #chosenIndex = bestIndices[0]
    chosenIndex = random.choice(bestIndices)
    print "Utilities: ", utilityValues, " statevalues", stateValues, " best utility ", bestUtility, " action", nextActions[chosenIndex], " from ", nextActions
    return nextActions[chosenIndex]
    
    
class AlphaBetaAgent(MultiAgentSearchAgent):
  """
    Your minimax agent with alpha-beta pruning (question 3)
  """
    
  def terminalnode(self, gameState, depth):
    if(gameState.isWin() or gameState.isLose() or depth == 0):
        return True
    else:
        return False
  
  def maxvalue(self, gameState, depth, alpha, beta):
    if (self.terminalnode(gameState,depth) ):
      return self.evaluationFunction(gameState)
    #print "PACMAN MOVE, depth=", depth
    value = float("-infinity")
    actions = gameState.getLegalActions(0)
    successors = [(action, gameState.generateSuccessor(0,action)) for action in actions]
    for a, s in successors:
      value = max(value,self.minvalue(s,depth,1, alpha, beta) )
      if value > beta:
        #print "VALUE > BETA"
        return value
      else:
        alpha = max(alpha,value)
    return value
    
  """s
    ghostCount starts at 1 for the first ghost, and counts up to the total amount of ghosts,
    upon which we decrease the depth and call maxvalue.
  """  
  def minvalue(self, gameState, depth, ghostCount, alpha, beta):
    if (self.terminalnode(gameState,depth) ):
      return self.evaluationFunction(gameState)
    #print "GHOST MOVE, depth=", depth
    value = float('inf')
    actions = gameState.getLegalActions(ghostCount)
    successors = [gameState.generateSuccessor(ghostCount, action) for action in actions]
    ghostCount += 1
    if ghostCount > gameState.getNumAgents()-1:
      # GO TO PACMAN
      for s in successors:
        value = min(value, self.maxvalue(s, depth-1, alpha, beta) )
        if value < alpha:
          #print "VALUE < ALPHA!"
          return value
        beta = min(beta, value)  
    else:
      # DO ANOTHER GHOST
      for s in successors:  
        value = min(value, self.minvalue(s, depth, ghostCount, alpha, beta))
        if value < alpha:
          #We can still prune here, since finding a smaller value means WE will choose it, but Max will ignore it!
          #print "VALUE < ALPHA!"
          return value
        beta = min(beta, value)
    return value
    
  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth 
      and self.evaluationFunction.
    """
    #print "INITIAL PACMAN MOVE ALPHA BETA!"
    nextActions = gameState.getLegalActions(0);
    nextStates = [gameState.generatePacmanSuccessor(action) for action in nextActions]
    utilityValues = []
    alpha = float('-infinity')
    beta = float('infinity')
    for state in nextStates:
      value = self.minvalue(state, self.depth, 1, alpha, beta) 
      utilityValues.append(value)
      alpha = max(value, alpha)
    bestUtility = max(utilityValues)
    bestIndices = [index for index in range(len(utilityValues)) if utilityValues[index] == bestUtility]
    #chosenIndex = bestIndices[0]
    chosenIndex = random.choice(bestIndices)
    #print "Best utility is", bestUtility
    return nextActions[chosenIndex]
    
class ExpectimaxAgent(MultiAgentSearchAgent):
  """
    Your expectimax agent (question 4)
  """
    
  def terminalnode(self, gameState, depth):
    if(gameState.isWin() or gameState.isLose() or depth == 0):
        return True
    else:
        return False
  
  def maxvalue(self, gameState, depth):
    if (self.terminalnode(gameState,depth) ):
      return self.evaluationFunction(gameState)
    #print "PACMAN MOVE, depth=", depth
    value = float("-infinity")
    actions = gameState.getLegalActions(0)
    successors = [gameState.generateSuccessor(0,action) for action in actions]
    for s in successors:
      value = max(value,self.expectivalue(s,depth,1) )
    return value
    
  """s
    ghostCount starts at 1 for the first ghost, and counts up to the total amount of ghosts,
    upon which we decrease the depth and call maxvalue.
  """  
  def expectivalue(self, gameState, depth, ghostCount):
    if (self.terminalnode(gameState,depth) ):
      return self.evaluationFunction(gameState)
    #print "GHOST MOVE, depth=", depth
    
    actions = gameState.getLegalActions(ghostCount)
    successors = [(action, gameState.generateSuccessor(ghostCount, action)) for action in actions]
    value = 0
    valueAmount = 1.0 * len(successors) #I can do this since I know how many successors I will expand
    ghostCount += 1
    for a, s in successors:
      if ghostCount > gameState.getNumAgents()-1:
        value += (1.0/valueAmount)*(self.maxvalue(s, depth-1) )
      else:
        value += (1.0/valueAmount)*(self.expectivalue(s, depth, ghostCount))
    return value
    
    
  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth 
      and self.evaluationFunction.
    """
    #print "INITIAL PACMAN MOVE!"
    nextActions = gameState.getLegalActions(0);
    nextStates = [gameState.generatePacmanSuccessor(action) for action in nextActions]
    utilityValues = [self.expectivalue(state, self.depth, 1) for state in nextStates]
    bestUtility = max(utilityValues)
    bestIndices = [index for index in range(len(utilityValues)) if utilityValues[index] == bestUtility]
    #chosenIndex = bestIndices[0]
    chosenIndex = random.choice(bestIndices)
    #print "Best utility is", bestUtility
    return nextActions[chosenIndex]

def findClosestFoodToPointInList(list, point):
  minDistance = 999999
  closest = None
  for el in list:
      d = util.manhattanDistance(el, point)
      if (d < minDistance):
          minDistance = d
          closest = el
  return closest

def betterEvaluationFunction(currentGameState):
  """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).
    
    DESCRIPTION: <write something here so we know what you did>
  """
  "*** YOUR CODE HERE ***"
  val = currentGameState.getScore()
  pacmanPosition = currentGameState.getPacmanPosition()
  capsules = currentGameState.getCapsules()
  food = currentGameState.getFood().asList()
  newGhostStates = currentGameState.getGhostStates()
  newScaredTimes = [ghostState.scaredTimer for ghostState in newGhostStates]
  #This is trying to add weight so PAcman can go to the direction of the closest food but it mess up in the end.
  closestFood = findClosestFoodToPointInList(food,currentGameState.getPacmanPosition())
  howClose = 0
  if not (closestFood ==None):
    howClose = util.manhattanDistance(closestFood, currentGameState.getPacmanPosition())
  if howClose>0:
      val += 1.0/howClose
  #distances = [util.manhattanDistance(pacmanPosition, capsulePosition) for capsulePosition in capsules]
  if len(capsules) > 0:
    val += 1.0/len(capsules)
  if len(food) > 0:
    val += 10000.0/len(food)**2.0
  #print 10000.0/len(food)**2.0
  return val

DISTANCE_CALCULATORS = {}
