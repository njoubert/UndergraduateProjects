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
import util
import threading, sys, time, random

DISTANCE_RANGE = 5 # Must be odd
DISTANCE_VALUES = [i - (DISTANCE_RANGE - 1)/2 for i in range(DISTANCE_RANGE)]

class ApproximateAgent:
  def __init__(self, state,enemies):
    """
    
    """
    self.trackers = [None] # Empty tracker for Pacman
    self.enemies = enemies
    for enemyState in self.enemies:
      self.trackers.append(EnemyTracker(state, enemyState))
    # Start computing distances in the background; when the dc finishes,
    # it will fill in self._distances for us.
    
  def getEnemyPositionDistributions(self, state,myPos):
    distributions = []
    for index, enemy in enumerate(self.enemies):
      index += 1
      if enemy:
        tracker, distance = self.trackers[index], state.getAgentDistances()[index]
        print distance
        if distance != None:
            tracker.observe(myPos, distance)
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
  """
  def __init__(self, game, numParticles = 10000):
    self.game = game
    self.numParticles = numParticles
    self.particles = None
    
  def initialize(self):
    """
    Initialize the agent's beliefs to a prior sampling over positions.
    """
    self.particles = sampleMultiple(self.game.getInitialDistribution(), self.numParticles)
    
    
  def observe(self, observation):
    weight = util.Counter()
    for particle in self.particles:
         weight[particle] = self.game.getReadingDistributionGivenEnemyTuple(particle,observation[0]).getCount(observation[1])
    if weight.totalCount() == 0:
        self.initialize()
    else:
        weight.normalize()
        self.particles = sampleMultiple(weight, self.numParticles)

  def elapseTime(self):
    temp = []
    for particle in self.particles:
         temp.append(sample(self.game.getEnemyTupleDistributionGivenPreviousEnemyTuple(particle)))
    self.particles = temp

    
  def getBeliefDistribution(self):
    return listToDistribution(self.particles)
    


class EnemyTracker:
  def __init__(self, state, enemyState):
    self.adapter = GhostbustersGameAdapter(state, enemyState)
    self.inference = ApproximateDynamicInferenceModule(self.adapter,900)
    self.inference.initialize()
    
  def observe(self, myPosition, distance):
    return self.inference.observe( (myPosition, distance) )
    
  def elapseTime(self):
    return self.inference.elapseTime()
    
  def getStateDistribution(self):
    dist = self.inference.getBeliefDistribution()
    c = util.Counter()
    for key, value in dist.items():
      c[key[0]] = value
    return c

class GhostbustersGameAdapter:
  def __init__(self, state, enemyState):
    self.startingEnemyPosition = enemyState.getPosition()
    self.walls = state.data.layout.walls

  def getInitialDistribution(self):
    c = util.Counter()
    c[(self.startingEnemyPosition, )] = 1
    return c

  def getEnemyTuples(self):
    return [( (x,y), ) for x in range(self.walls.width) for y in range(self.walls.height)]
  
  def getReadingDistributionGivenEnemyTuple(self, shipTuple, location):
    print shipTuple
    enemyPos = shipTuple[0]
    c = util.Counter()
    trueDistance = util.manhattanDistance(enemyPos, location)
    for d in DISTANCE_VALUES:
      c.incrementCount(d + trueDistance, 1.0 / DISTANCE_RANGE)
    return c
  
  def getEnemyTupleDistributionGivenPreviousEnemyTuple(self, oldGhosts):
    enemyPos = oldGhosts[0]
    neighbors = Actions.getLegalNeighbors(enemyPos, self.walls)
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
  
def binarySearch(list, value):
  low = 0
  high = len(list)-1
  while True:
    if low == high:
      return low
    mid = (low + high) / 2
    if value <= list[mid]:
      high = mid
    elif value > list[mid]:
      low = mid + 1
  raise 'sampling error: bad distribution'
  
def listToDistribution(list):
  distribution = util.Counter()
  for item in list:
    distribution.incrementCount(item, 1.0)
  return util.normalize(distribution)
  
