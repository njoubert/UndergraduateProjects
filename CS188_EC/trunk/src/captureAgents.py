from game import Agent
import distanceCalculator
import ParticleFilter
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
  A random agent that takes too much time. Taking
  too much time results in the server choosing a move 
  for you randomly.
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
    Lists several variables you can query:
    self.index = index for this agent 
    self.red = true if you're on the red team, false otherwise
    self.agentsOnTeam = a list of agent objects that make up your team
    self.distancer = distance calculator (contest code provides this)
    self.observationHistory = list of GameState objects that correspond 
        to the sequential order of states that have occurred so far this game
    self.timeForComputing = an amount of time to give each turn for computing maze distances 
        (part of the provided distance calculator)
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
    """
    This method handles the initial setup of the
    agent to populate useful fields (such as what team
    we're on).  
    """
    self.red = gameState.isOnRedTeam(self.index)
    self.distancer = distanceCalculator.Distancer(gameState.data.layout)
    import __main__
    if '_display' in dir(__main__):
      self.display = __main__._display
         
  def registerTeam(self, agentsOnTeam):
    """
    Fills the self.agentsOnTeam field with a list of the
    indices of the agents on your team.
    """
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
    
    If you subclass CaptureAgent, you shouldn't need to override this method.  It
    takes care of appending the current gameState on to your observation history
    (so you have a record of the game states of the game) and will call your
    choose action method if you're in a state (rather than halfway through your last
    move - this occurs because Pacman agents move half as quickly as ghost agents).
    
    If you aren't going to be using the distance calculator we provide, you can comment
    out the line beginning "distanceCalculator" so as not to lose computing time to the
    calculating distances you're not using.
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
    Override this method to make a good agent. It should return a legal action within
    the time limit (otherwise a random legal action will be chosen for you).
    """
    util.raiseNotDefined()  
  
  #######################
  # Convenience Methods #
  #######################

  def getFood(self, gameState):
    """
    Returns the food you're meant to eat. This is in the form of a matrix
    where m[x][y]=true if there is food you can eat (based on your team) in that square.
    """
    if self.red:
      return gameState.getBlueFood()
    else:
      return gameState.getRedFood()

  def getFoodYouAreDefending(self, gameState):
    """
    Returns the food you're meant to protect (i.e., that your opponent is
    supposed to eat). This is in the form of a matrix where m[x][y]=true if
    there is food at (x,y) that your opponent can eat.
    """
    if self.red:
      return gameState.getRedFood()
    else:
      return gameState.getBlueFood()

  def getOpponents(self, gameState):
    """
    Returns agent indices of your opponents. This is the list of the numbers
    of the agents (e.g., red might be "1,3,5")
    """
    if self.red:
      return gameState.getBlueTeamIndices()
    else:
      return gameState.getRedTeamIndices()
    
  def getTeam(self, gameState):
    """
    Returns agent indices of your team. This is the list of the numbers
    of the agents (e.g., red might be the list of 1,3,5)
    """
    if self.red:
      return gameState.getRedTeamIndices()
    else:
      return gameState.getBlueTeamIndices()

  def getScore(self, gameState):
    """
    Returns how much you are beating the other team by in the form of a number
    that is the difference between your score and the opponents score.  This number
    is negative if you're losing.
    """
    if self.red:
      return gameState.getScore()
    else:
      return gameState.getScore() * -1
    
  def getMazeDistance(self, pos1, pos2):
    """
    Returns the distance between two points; this is either the Manhattan
    distance early in the game, or actual shortest path maze distances once
    the computation is complete. These are calculated using the provided
    distancer object.
    
    The distancer computes the shortest path between pairs of points in the
    background, and starts using them as soon as they are ready. These are
    not just pre-computed ahead of time because of the time limit - we don't
    want to lose our turn because we're doing precomputations!
    """
    d = self.distancer.getDistance(pos1, pos2)
    return d
  
  def getPreviousObservation(self):
    """
    Returns the GameState object corresponding to the last state this agent saw
    (the observed state of the game last time this agent moved - this may not include
    all of your opponent's agent locations exactly).
    """
    if len(self.observationHistory) == 1: return None
    else: return self.observationHistory[-2]

  def getCurrentObservation(self):
    """
    Returns the GameState object corresponding this agent's current observation
    (the observed state of the game - this may not include
    all of your opponent's agent locations exactly).
    """
    return self.observationHistory[-1]

  def displayDistributionsOverPositions(self, distributions):
    """
    Overlays a distribution over positions onto the pacman board that represents
    an agent's beliefs about the positions of each agent.
    
    The arg distributions is a tuple or list of util.Counter objects, where the i'th
    Counter has keys that are board positions (x,y) and values that encode the probability 
    that agent i is at (x,y).
    
    If some elements are None, then they will be ignored.  If a Counter is passed to this
    function, it will be displayed. This is helpful for figuring out if your agent is doing
    inference correctly, and does not affect gameplay.
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

class ParticleTrackingAgent(CaptureAgent):
  
  def registerInitialState(self, gameState):
      CaptureAgent.registerInitialState(self, gameState)
      enemies = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
      self.particlefilter = ParticleFilter.ApproximateAgent(gameState, enemies)   
  
  def chooseAction(self, gameState):
    distanceCalculator.waitOnDistanceCalculator(0.5)
    myState = gameState.getAgentState(self.index)
    myPos = myState.getPosition()
    legal = gameState.getLegalActions(self.index)
    distributions = self.particlefilter.getEnemyPositionDistributions(gameState,myPos)
    modes = []
    for dist in distributions:
      if len(dist.keys()) > 0:
        modes.append(dist.argMax())
    if len(modes) == 0: return random.choice(legal)
    
    options = []
    for l in legal:
      succ = gameState.generateSuccessor(self.index,l)
      closestMode = min([self.distancer.getDistance(myPos, mode) for mode in modes]) 
      options.append((closestMode, l))
    return min(options)[1]

class ReflexCaptureAgent(CaptureAgent):
  """
  A base class for reflex agents that chooses score-maximizing actions
  """
  def chooseAction(self, gameState):
    """
    Picks among the actions with the best results. "Best" is determined
    by an evaluation function.  This method also shows you how to do timing
    of how long something takes, which may be useful as you build more
    complex agents and have to balance time constraints.
    """
    actions = gameState.getLegalActions(self.index)
    results = [self.getSuccessor(gameState, a) for a in actions]
    
    #print "========================="
    #for i in range(gameState.getNumAgents()):
    #  print "Agent", str(i), "on team red team?", gameState.isOnRedTeam(i)," is at positon", gameState.getAgentPosition(i)
    
    # You can profile your evaluation time by uncommenting these lines
    # start = time.time()
    values = [self.evaluate(s) for s in results]
    # print 'eval time for agent %d: %.4f' % (self.index, time.time() - start)
    
    maxValue = max(values)
    bestActions = [a for a, v in zip(actions, values) if v == maxValue]
    return random.choice(bestActions)
  
  def getSuccessor(self, gameState, action):
    """
    Finds the next successor which is a grid position (location tuple).
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
  A reflex agent that seeks food. This is an agent
  we give you to get an idea of what an offensive agent might look like,
  but it is by no means the best or only way to make
  an offensive agent.
  """
  def chooseAction(self, gameState):
    """
    Picks among the actions with the best results. "Best" is determined
    by an evaluation function.  This method also shows you how to do timing
    of how long something takes, which may be useful as you build more
    complex agents and have to balance time constraints.
    """
    from game import Directions
    actions = gameState.getLegalActions(self.index)
    results = [self.getSuccessor(gameState, a) for a in actions]
    
    #Hack to grab the network board.
    #print gameState.data.layout
    
    #print "========================="
    #for i in range(gameState.getNumAgents()):
    #  print "Agent", str(i), "on team red team?", gameState.isOnRedTeam(i)," is at positon", gameState.getAgentPosition(i)
    
    # You can profile your evaluation time by uncommenting these lines
    # start = time.time()
    values = [self.myEvaluate(s) for s in results]
    # print 'eval time for agent %d: %.4f' % (self.index, time.time() - start)
    
    maxValue = max(values)
    bestActions = [a for a, v in zip(actions, values) if v == maxValue]
    return random.choice(bestActions)
    
  def myEvaluate(self, gameState):
    """
    I didn't like how they divided weights and features into two functions, since I want
    to vary my weights depending on the gameState, so they're both in this one function.
    
    Some improvements that can be made:
      use distance to our side as a feature when running away
      avoid the place you last died, or prefer the opposite side of the board from where you died.

    """
    features = util.Counter()
    weights = util.Counter()
    
    myState = gameState.getAgentState(self.index)
    myPos = gameState.getAgentState(self.index).getPosition()
    
    """ Score """
    features['score'] = self.getScore(gameState)
    weights['score'] = 100


    """ Distance to Food """
    # Compute distance to the nearest food
    foodList = self.getFood(gameState).asList()    
    if len(foodList) > 0:  
      minDistance = min([self.getMazeDistance(myPos, food) for food in foodList])
      features['distanceToFood'] = minDistance
      weights['distanceToFood'] = -1

      
    """ Total food """
    features['totalFood'] = len(foodList)
    weights['totalFood'] = -10


    """ IF WE ARE ATTACKING """
    if myState.isPacman or (self.getPreviousObservation() != None and self.getPreviousObservation().getAgentState(self.index).isPacman):
      
      """ Closest Enemy Pacman Distance """
      
      """ Closest Enemy Pacman Direction """
      
      """ In Range ghost distance and direction """
      enemies = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
      ghosts = [a for a in enemies if not a.isPacman and a.getPosition() != None]
      if len(ghosts) > 0:
        direction = [a.getDirection() for a in ghosts]
        ghostDist = [self.getMazeDistance(myPos, ghost.getPosition()) for ghost in ghosts]
        features['inRangeGhostDistance'] = min(ghostDist)
        if (min(ghostDist) < 10):
          weights['inRangeGhostDistance'] = 600
        else:
          weights['inRangeGhostDistance'] = 0
      
      """ IF WE ARE RUNNING AWAY """
      #if not myState.isPacman and self.getPreviousObservation() != None and self.getPreviousObservation().getAgentState(self.index).isPacman:
      #  features['runAway'] = 1
      #  weights['runAway'] = 100
        
    """ IF WE ARE STILL A GHOST """
    if not myState.isPacman:
      enemies = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
      invaders = [a for a in enemies if a.isPacman and a.getPosition() != None]
      features['numInvaders'] = len(invaders)
      weights['numInvaders'] = -100
      if len(invaders) > 0:
        dists = [self.getMazeDistance(myPos, a.getPosition()) for a in invaders]
        features['invaderDistance'] = min(dists)
        weights['invaderDistance'] = -5
    
    return features * weights
      
class DefensiveReflexAgent(ReflexCaptureAgent):
  """
  A reflex agent that keeps its side Pacman-free. Again,
  this is to give you an idea of what a defensive agent
  could be like.  It is not the best or only way to make
  such an agent.
  """
  def registerInitialState(self, gameState):
    CaptureAgent.registerInitialState(self, gameState)
    enemies = [gameState.getAgentState(i) for i in self.getOpponents(gameState)]
    self.particlefilter = ParticleFilter.ApproximateAgent(gameState, enemies)   
  
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
      
    #compute using particle filtering
    distanceCalculator.waitOnDistanceCalculator(0.5)
    myState = gameState.getAgentState(self.index)
    myPos = myState.getPosition()
    legal = gameState.getLegalActions(self.index)
    distributions = self.particlefilter.getEnemyPositionDistributions(gameState,myPos)
    modes = []
    for dist in distributions:
      if len(dist.keys()) > 0:
        modes.append(dist.argMax())
    if len(modes) != 0:
        options = []
        for l in legal:
            succ = gameState.generateSuccessor(self.index,l)
            closestMode = min([self.distancer.getDistance(myPos, mode) for mode in modes]) 
            options.append((closestMode, l))
        features['particleFilter'] = min(options)[0]
      
    # Computes distance to last food eaten
    if 'lastFoodEaten' not in dir(self): self.lastFoodEaten = []
    eatenFood = self.getEatenFoodSinceLastObservation()
    if len(eatenFood) > 0: self.lastFoodEaten = eatenFood
    if len(self.lastFoodEaten) > 0:
      foodDist = [self.getMazeDistance(myPos, food) for food in self.lastFoodEaten]
      features['foodEatenDistance'] = min(foodDist)
      
    features['score'] = self.getScore(gameState)  
      
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
    return {'numInvaders': -1000, 'onDefense': 100, 'invaderDistance': -10, 'foodEatenDistance': -1,'particleFilter':-10 ,'score':1}

