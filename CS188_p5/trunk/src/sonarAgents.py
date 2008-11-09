from game import Agent
from game import Directions
import sonar
import random
import util
from dynamicInferenceModule import ExactDynamicInferenceModule
from game import Actions

DISTANCE_RANGE = 5 # Must be odd
DISTANCE_VALUES = [i - (DISTANCE_RANGE - 1)/2 for i in range(DISTANCE_RANGE)]
DISPLAY = None

def noisyDist(pos1, pos2):
  return util.manhattanDistance(pos1, pos2) + random.choice(DISTANCE_VALUES)

class SonarAgent(Agent):
  def observationFunction(self, state):
    # Adds the sonar signal
    obs = state.deepCopy()
    pos = obs.getPacmanPosition()
    n = obs.getNumAgents()
    distances = [0] + [noisyDist(pos, obs.getGhostPosition(i)) for i in range(1, n)]
    obs.ghostDistances = distances

    # Update the display
    words = []
    for alive, dist in zip(obs.getLivingGhosts(), distances)[1:]:
      if alive: words.append(dist)
      else: words.append('XXX')
    import __main__
    global DISPLAY
    DISPLAY = __main__._display
    DISPLAY.infoPane.updateGhostDistances(words)
    # We should also delete ghost states, but we'll just trust our agents not to cheat
    return obs
  
  def registerInitialState(self, state):
    self.trackers = [None] # Empty tracker for Pacman
    for ghostState in state.getGhostStates():
      self.trackers.append(GhostTracker(state, ghostState))
    
  def getGhostPositionDistributions(self, state):
    distributions = []
    for index, liveGhost in enumerate(state.getLivingGhosts()[1:]):
      index += 1
      if liveGhost:
        tracker, distance = self.trackers[index], state.ghostDistances[index]
        tracker.observe(state.getPacmanPosition(), distance)
        distribution = tracker.getStateDistribution()
        distributions.append(distribution)
        tracker.elapseTime()
      else:
        distributions.append(util.Counter())
    DISPLAY.updateDistributions(distributions)
    return distributions
  
class GhostTracker:
  def __init__(self, state, ghostState):
    self.adapter = GhostbustersGameAdapter(state, ghostState)
    self.inference = ExactDynamicInferenceModule(self.adapter)
    self.inference.initialize()
    
  def observe(self, pacPosition, distance):
    return self.inference.observe( (pacPosition, distance) )
    
  def elapseTime(self):
    return self.inference.elapseTime()
    
  def getStateDistribution(self):
    dist = self.inference.getBeliefDistribution()
    c = util.Counter()
    for key, value in dist.items():
      c[key[0]] = value
    return c

class GhostbustersGameAdapter:
  def __init__(self, state, ghostState):
    self.startingGhostPosition = ghostState.getPosition()
    self.walls = state.data.layout.walls

  def getInitialDistribution(self):
    c = util.Counter()
    c[(self.startingGhostPosition, )] = 1
    return c

  def getGhostTuples(self):
    return [( (x,y), ) for x in range(self.walls.width) for y in range(self.walls.height)]
  
  def getReadingDistributionGivenGhostTuple(self, shipTuple, location):
    ghostPos = shipTuple[0]
    c = util.Counter()
    trueDistance = util.manhattanDistance(ghostPos, location)
    for d in DISTANCE_VALUES:
      c.incrementCount(d + trueDistance, 1.0 / DISTANCE_RANGE)
    return c
  
  def getGhostTupleDistributionGivenPreviousGhostTuple(self, oldGhosts):
    ghostPos = oldGhosts[0]
    neighbors = Actions.getLegalNeighbors(ghostPos, self.walls)
    c = util.Counter()
    for n in neighbors:
      c[(n,)] = 1
    c.normalize()
    return c

class SonarKeyboardAgent(SonarAgent):
  """
  An agent controlled by the keyboard.
  """
  # NOTE: Arrow keys also work.
  WEST_KEY  = 'a' 
  EAST_KEY  = 'd' 
  NORTH_KEY = 'w' 
  SOUTH_KEY = 's'

  def __init__( self, index = 0 ):
    
    self.lastMove = Directions.STOP
    self.index = index
    self.keys = []
    
  def getAction( self, state):
    from graphicsDisplay import keys_waiting
    from graphicsDisplay import keys_pressed
    keys = keys_waiting() + keys_pressed()
    if keys != []:
      self.keys = keys
    
    legal = state.getLegalActions(self.index)
    move = self.getMove(legal)
    
    if move == Directions.STOP:
      # Try to move in the same direction as before
      if self.lastMove in legal:
        move = self.lastMove
        
    if move not in legal:
      move = random.choice(legal)
      
    self.lastMove = move
    return move

  def getMove(self, legal):
    move = Directions.STOP
    if   (self.WEST_KEY in self.keys or 'Left' in self.keys) and Directions.WEST in legal:  move = Directions.WEST
    if   (self.EAST_KEY in self.keys or 'Right' in self.keys) and Directions.EAST in legal: move = Directions.EAST
    if   (self.NORTH_KEY in self.keys or 'Up' in self.keys) and Directions.NORTH in legal:   move = Directions.NORTH
    if   (self.SOUTH_KEY in self.keys or 'Down' in self.keys) and Directions.SOUTH in legal: move = Directions.SOUTH
    return move

class TrackingKeyboardAgent(SonarKeyboardAgent):
  def getAction(self, state):
    self.getGhostPositionDistributions(state)
    return SonarKeyboardAgent.getAction(self, state)

import distanceCalculator
class GreedyTrackingAgent(SonarAgent):
  def registerInitialState(self, state):
    SonarAgent.registerInitialState(self, state)
    self.distancer = distanceCalculator.Distancer(state.data.layout)
    
  def getAction(self, state):
    distanceCalculator.waitOnDistanceCalculator(0.5)
    legal = state.getLegalActions(self.index)
    distributions = self.getGhostPositionDistributions(state)
    modes = []
    for dist in distributions:
      if len(dist.keys()) > 0:
        modes.append(dist.argMax())
    if len(modes) == 0: return random.choice(legal)
    
    options = []
    for l in legal:
      succ = state.generatePacmanSuccessor(l)
      pos = succ.getPacmanPosition()
      closestMode = min([self.distancer.getDistance(pos, mode) for mode in modes]) 
      options.append((closestMode, l))
    return min(options)[1]
