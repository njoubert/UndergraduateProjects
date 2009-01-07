from game import Agent
import distanceCalculator
from util import nearestPoint
import random, time, util

class RandomAgent( Agent ):
  """
  A random agent that abides by the rules.
  """
  def __init__( self, index ):
    self.index = index
    
  def getAction( self, state ):
    return random.choice( state.getLegalActions( self.index ) )  

class TimeoutAgent( Agent ):
  """
  A random agent that takes too much time.
  """
  def __init__( self, index ):
    self.index = index
    
  def getAction( self, state ):
    if random.random() < 1:    time.sleep(6.0)
    return random.choice( state.getLegalActions( self.index ) )

class CaptureAgent(Agent):
  """
  A base class for capture agents.  The convenience methods herein handle
  some of the complications of a two-team game.
  
  Recommended Usage:  Subclass CaptureAgent and override chooseAction.
  """
  
  #############################
  # Methods to store key info #
  #############################
  
  def __init__( self, index, timeForComputing = .1 ):
    """
    Lists several variables you can query.
    """
    # Agent index for querying state
    self.index = index
    
    # Whether or not you're on the red team
    self.red = None            
    
    # Agent objects controlling you and your teammates
    self.agentsOnTeam = None
    
    # Maze distance calculator
    self.distancer = None

    # A history of observations
    self.observationHistory = []
    
    # Time to spend each turn on computing maze distances
    self.timeForComputing = timeForComputing
    
    # Access to the graphics
    self.display = None

  def registerInitialState(self, gameState):
    self.red = gameState.isOnRedTeam(self.index)
    self.distancer = distanceCalculator.Distancer(gameState.data.layout)
    import __main__
    if '_display' in dir(__main__):
      self.display = __main__._display
         
  def registerTeam(self, agentsOnTeam):
    self.agentsOnTeam = agentsOnTeam
    
  def observationFunction(self, gameState):
    " Changing this won't affect pacclient.py, but will affect capture.py "
    return gameState.makeObservation(self.index)

  #################
  # Action Choice #
  #################

  def getAction(self, gameState):
    """
    Calls chooseAction on a grid position, but continues on half positions.
    
    This method also cedes some processing time to the distance calculator, 
    which computes the shortest path distance between all pairs of points.
    """
    # Give some time to the distance calculator thread
    distanceCalculator.waitOnDistanceCalculator(self.timeForComputing)
    
    self.observationHistory.append(gameState)
    
    myState = gameState.getAgentState(self.index)
    myPos = myState.getPosition()
    if myPos != nearestPoint(myPos): 
      # We're halfway from one position to the next
      return gameState.getLegalActions(self.index)[0]
    else:
      return self.chooseAction(gameState)
  
  def chooseAction(self, gameState):
    """
    Override this method to make a good agent.
    """
    util.raiseNotDefined()  
  
  #######################
  # Convenience Methods #
  #######################

  def getFood(self, gameState):
    "Returns the food you're meant to eat."
    if self.red:
      return gameState.getBlueFood()
    else:
      return gameState.getRedFood()

  def getFoodYouAreDefending(self, gameState):
    "Returns the food you're meant to eat."
    if self.red:
      return gameState.getRedFood()
    else:
      return gameState.getBlueFood()

  def getOpponents(self, gameState):
    "Returns agent indices of your opponents."
    if self.red:
      return gameState.getBlueTeamIndices()
    else:
      return gameState.getRedTeamIndices()
    
  def getTeam(self, gameState):
    "Returns agent indices of your team."
    if self.red:
      return gameState.getRedTeamIndices()
    else:
      return gameState.getBlueTeamIndices()

  def getScore(self, gameState):
    "Returns how much you are beating the other team by."
    if self.red:
      return gameState.getScore()
    else:
      return gameState.getScore() * -1
    
  def getMazeDistance(self, pos1, pos2):
    """
    Returns the distance between two points; this is either the Manhattan
    distance early in the game, or actual shortest path maze distances once
    the computation is complete.  
    
    The distancer computes the shortest path between pairs of points in the
    background, and starts using them as soon as they are ready.
    """
    d = self.distancer.getDistance(pos1, pos2)
    return d
  
  def getPreviousObservation(self):
    if len(self.observationHistory) == 1: return None
    else: return self.observationHistory[-2]

  def getCurrentObservation(self):
    return self.observationHistory[-1]

  def displayDistributionsOverPositions(self, distributions):
    """
    Overlays a distribution over positions onto the pacman board that represents
    an agent's beliefs about the positions of each agent.
    
    The arg distributions is a tuple or list of util.Counter objects, where the i'th
    Counter has keys that are board positions (x,y) and values that encode the probability 
    that agent i is at (x,y).
    
    If some elements are None, then they will be ignored.  If a Counter is passed to this
    function, it will be displayed.
    """
    if self.display != None:
      if 'updateDistributions' in dir(self.display):
        # Fix up the input
        if isinstance(distributions, util.Counter): distributions = [distributions]
        if not isinstance(distributions, list) and not isinstance(distributions, tuple):
          raise Exception("Wrong type of distributions object")
        dists = []
        for dist in distributions:
          if dist != None: 
            if not isinstance(dist, util.Counter): raise Exception("Wrong type of distribution")
            dists.append(dist)
          else: dists.append(util.Counter())
        self.display.updateDistributions(dists)

class ReflexCaptureAgent(CaptureAgent):
  """
  A base class for reflex agents that chooses score-maximizing actions
  """
  def chooseAction(self, gameState):
    """
    Picks among the actions with the best results
    """
    actions = gameState.getLegalActions(self.index)
    results = [self.getSuccessor(gameState, a) for a in actions]
    
    # You can profile your evaluation time by uncommenting these lines
    # start = time.time()
    values = [self.evaluate(s) for s in results]
    # print 'eval time for agent %d: %.4f' % (self.index, time.time() - start)
    
    maxValue = max(values)
    bestActions = [a for a, v in zip(actions, values) if v == maxValue]
    return random.choice(bestActions)
  
  def getSuccessor(self, gameState, action):
    """
    Finds the next successor which is a grid position.
    """
    successor = gameState.generateSuccessor(self.index, action)
    pos = successor.getAgentState(self.index).getPosition()
    if pos != nearestPoint(pos): 
      # Only half a grid position was covered
      return successor.generateSuccessor(self.index, action)
    else:
      return successor
  
  def evaluate(self, gameState):
    """
    Computes a linear combination of features and feature weights
    """
    features = self.getFeatures(gameState)
    weights = self.getWeights(gameState)
    return features * weights
  
  def getFeatures(self, gameState):
    """
    Returns a counter of features for the state
    """
    features = util.Counter()
    features['score'] = self.getScore(gameState)
    return features
  
  def getWeights(self, gameState):
    """
    Normally, weights do not depend on the gamestate.  They can be either
    a counter or a dictionary.
    """
    return {'score': 1.0}
  
class OffensiveReflexAgent(ReflexCaptureAgent):
  """
  A reflex agent that seeks food.
  """
  def getFeatures(self, gameState):
    features = util.Counter()
    features['score'] = self.getScore(gameState)

    # Compute distance to the nearest food
    foodList = self.getFood(gameState).asList()
    if len(foodList) > 0:
      myPos = gameState.getAgentState(self.index).getPosition()
      minDistance = min([self.getMazeDistance(myPos, food) for food in foodList])
      features['distanceToFood'] = minDistance
    return features
    
  def getWeights(self, gameState):
    return {'score': 100, 'distanceToFood': -1}

class DefensiveReflexAgent(ReflexCaptureAgent):
  """
  A reflex agent that keeps its side Pacman-free.
  """
  
  def getFeatures(self, gameState):
    features = util.Counter()

    myState = gameState.getAgentState(self.index)
    myPos = myState.getPosition()

    # Computes whether we're on defense (1) or offense (0)
    features['onDefense'] = 1
    if myState.isPacman: features['onDefense'] = 0
    
    # Computes distance to invaders we can see
    enemies = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
    invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]
    features['numInvaders'] = len(invaders)
    if len(invaders) > 0:
      dists = [self.getMazeDistance(myPos, a.getPosition()) for a in invaders]
      features['invaderDistance'] = min(dists)
      
    # Computes distance to last food eaten
    if 'lastFoodEaten' not in dir(self): self.lastFoodEaten = []
    eatenFood = self.getEatenFoodSinceLastObservation()
    if len(eatenFood) > 0: self.lastFoodEaten = eatenFood
    if len(self.lastFoodEaten) > 0:
      foodDist = [self.getMazeDistance(myPos, food) for food in self.lastFoodEaten]
      features['foodEatenDistance'] = min(foodDist)
      
    return features

  def getEatenFoodSinceLastObservation(self):
    "A list of the opponents' food that was eaten since the last observation"
    gameState = self.getCurrentObservation()
    prevGameState = self.getPreviousObservation()
    if prevGameState == None: return []
    
    prevFoodList = self.getFoodYouAreDefending(prevGameState).asList()
    currentFoodGrid = self.getFoodYouAreDefending(gameState)
    return [(x,y) for x,y in prevFoodList if currentFoodGrid[x][y] == False]
    
  def getWeights(self, gameState):
    return {'numInvaders': -1000, 'onDefense': 100, 'invaderDistance': -10, 'foodEatenDistance': -1}

