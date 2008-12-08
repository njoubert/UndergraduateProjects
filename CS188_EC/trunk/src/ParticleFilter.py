"""
This file contains a Distancer object which computes and 
caches the shortest path between any two points in the maze. It 
returns a Manhattan distance between two points if the maze distance
has not yet been calculated. 

Example:
distancer = Distancer(gameState.data.layout)
distancer.getDistance( (1,1), (10,10) )

The Distancer object also serves as an example of sharing data 
safely among agents via a global dictionary (distanceMap), 
and performing asynchronous computation via threads. These
examples may help you in designing your own objects, but you
shouldn't need to modify the Distancer code in order to use its
distances.
"""
from game import Agent
from game import Directions
from game import Actions
import threading, sys, time, random

DISTANCE_RANGE = 5 # Must be odd
DISTANCE_VALUES = [i - (DISTANCE_RANGE - 1)/2 for i in range(DISTANCE_RANGE)]

class ApproximateAgent:
  def __init__(self, state):
    """
    
    """
    self.trackers = [None] # Empty tracker for Pacman
    for ghostState in state.getGhostStates():
      self.trackers.append(GhostTracker(state, ghostState))
    # Start computing distances in the background; when the dc finishes,
    # it will fill in self._distances for us.
    
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
    return distributions

  
##########################################
# MACHINERY FOR COMPUTING MAZE DISTANCES #
##########################################
class ApproximateDynamicInferenceModule:
  """
  The approximate dynamic inference module should use particle filtering
  to compute the approximate belief function at each time / observation.
  
  The provided implementation is entirely broken and will not run.
  """
  def __init__(self, game, numParticles = 10000):
    self.game = game
    self.numParticles = numParticles
    self.particles = None
    
  def initialize(self):
    """
    Initialize the agent's beliefs to a prior sampling over positions.
    """
    
    "*** YOUR CODE HERE ***"  
    self.particles = util.sampleMultiple(self.game.getInitialDistribution(), self.numParticles)
    
    
  def observe(self, observation):
    """
    Update beliefs to reflect the given observations.
    Observation will require that you resample from your particles, 
    where each particle is weighted by the observation's likelihood 
    given the state represented by that particle.
    """
    
    "*** YOUR CODE HERE ***"  
    weight = util.Counter()
    for particle in self.particles:
         weight[particle] = self.game.getReadingDistributionGivenGhostTuple(particle,observation[0]).getCount(observation[1])
    
    weight.normalize()
    if weight.totalCount() == 0:
        self.initialize()
    else:
        self.particles = util.sampleMultiple(weight, self.numParticles)

class GhostTracker:
  def __init__(self, state, ghostState):
    self.adapter = GhostbustersGameAdapter(state, ghostState)
    self.inference = ApproximateDynamicInferenceModule(self.adapter,900)
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

"""
util methods for particle filtering
"""

def sample(distribution):
  """
  Sample a value from a U(0,1) distribution, and use this to generate
  a random sample from this particular distribution.
  """
  cumulative = 0.0
  uniform_sample = random.random()
  for entry, prob in distribution.items():
    cumulative += prob
    if cumulative >= uniform_sample:
      return entry
  raise 'sampling error: bad distribution'

def sampleMultiple(distribution, n):
  keys = distribution.keys()
  cumulativeProbs = []
  total = 0
  for key in keys:
    value = distribution.getCount(key)
    total += value
    cumulativeProbs.append(total)
  if total > 1.0001: raise 'sampling error: bad distribution'
  if total < 0.9999: raise 'sampling error: bad distribution'

  samples = []
  for i in range(n):
    uniform_sample = random.random()
    if uniform_sample > total: uniform_sample = total
    samples.append(keys[binarySearch(cumulativeProbs, uniform_sample)])
  return samples
  
  
def listToDistribution(list):
  distribution = Counter()
  for item in list:
    distribution.incrementCount(item, 1.0)
  return normalize(distribution)
  
