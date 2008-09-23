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
    Your minimax agent (question 2)
  """
    
  def getAction(self, gameState):
    """
      Returns the minimax action from the current gameState using self.depth 
      and self.evaluationFunction.
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
    
class AlphaBetaAgent(MultiAgentSearchAgent):
  """
    Your minimax agent with alpha-beta pruning (question 3)
  """
    
  def getAction(self, gameState):
    """
      Returns the minimax action using self.depth and self.evaluationFunction
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

class ExpectimaxAgent(MultiAgentSearchAgent):
  """
    Your expectimax agent (question 4)
  """
    
  def getAction(self, gameState):
    """
      Returns the expectimax action using self.depth and self.evaluationFunction
      
      All ghosts should be modeled as choosing uniformly at random from their
      legal moves.
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

def betterEvaluationFunction(currentGameState):
  """
    Your extreme ghost-hunting, pellet-nabbing, food-gobbling, unstoppable
    evaluation function (question 5).
    
    DESCRIPTION: <write something here so we know what you did>
  """
  "*** YOUR CODE HERE ***"
  util.raiseNotDefined()

DISTANCE_CALCULATORS = {}
