from ghostbusters import *
from util import *
import util
import random


class StaticInferenceModule:
  """
  A static inference module must compute two quantities, conditioned on provided observations:
  
    -- The posterior distribution over ghost locations.  This will be a distribution over tuples of
       where the ghosts are.  If there is only one ghost, this distribution will be over the
       (singleton tuples of) the board locations.  If there are two ghosts, this distribution will
       assign a probability to each pair of locations, and so on.  Since the ghosts are interchangeable,
       the probability for, say, ((0,1), (3,2)) will be the same as that for ((3,2), (0,1)).
       
    -- The posterior distribution over the readings at a location, given the existing readings.  Be
       careful that your computation does the right thing when the 'new' location is actually in the
       existing observations, at which point the posterior should put probability one of the known
       reading.

  This is an abstract class, which you should not modify.
  """
  
  def __init__(self, game):
    """
    Inference modules know what game they are reasoning about.
    """
    self.game = game
  
  def getGhostTupleDistributionGivenObservations(self, observations):
    """
    Compute the distribution over ghost tuples, given the evidence.
    
    Note that the observations are given as a dictionary.
    """
    util.raiseNotDefined()
    
  def getReadingDistributionGivenObservations(self, observations, newLocation):
    """
    Compute the distribution over readings for the new location, given the
    current observations (given as a dictionary).
    """
    util.raiseNotDefined()

class ExactStaticInferenceModule(StaticInferenceModule):
  """
  You will implement an exact inference module for the static ghostbusters game.
  
  See the abstract 'StaticInferenceModule' class for descriptions of the methods.
  
  The current implementation below is broken, returning all uniform distributions.
  
  TERMINOLOGY:
    STATE:          A possible set of positions for the ghosts. eg: ((1,2),) for one ghost.
    DISTRIBUTION:   A map from all possible states to probabilities.
    OBSERVATIONS:   A map from sensor positions to sensor readings.
  
  """
  
   #We can just normalize. its faster too/
  def getReadingSetProbability(self, observations):
    # Using Total Probability And Bayes Rule Over All Ghosts
    priorDistribution = self.game.getInitialDistribution()
    probability = 0
    for ghostTuple in priorDistribution:
        probabilityOfThisTuple = priorDistribution[ghostTuple]
        for reading in observations:
            dist = self.game.getReadingDistributionGivenGhostTuple(ghostTuple, reading).getCount(observations[reading])
            probabilityOfThisTuple *= dist
        probability += probabilityOfThisTuple
    return probability
  
  def getGhostTupleProbabilityGivenObservations(self, observations, ghostTuple, ghostTupleValue):
    probability = ghostTupleValue
    for reading in observations:
        probability *= self.game.getReadingDistributionGivenGhostTuple(ghostTuple, reading).getCount(observations[reading])
    return probability
  
  def getGhostTupleDistributionGivenObservations(self, observations):
    
    # QUESTION 1 & 2
    
    priorDistribution = self.game.getInitialDistribution()
    newDistribution = util.Counter();
    readingSetProbability = self.getReadingSetProbability(observations)
    for ghostTuple in priorDistribution:
        newDistribution[ghostTuple] = self.getGhostTupleProbabilityGivenObservations(observations, ghostTuple, priorDistribution[ghostTuple])
    
    if newDistribution.totalCount() > 0:
      newDistribution.normalize()
      
    return newDistribution
    

  def getReadingDistributionGivenObservations(self, observations, newLocation):
    """ Given a current set of observations and a location, it computes the
    probability of each possibly sensor reading at this position given other sensor readings.
    Thus the return value is a mapping from sensor readings to probabilities. """
    # QUESTION 3
    readingDistribution = util.Counter()
    from ghostbusters import Readings
    currentSetProbability = self.getReadingSetProbability(observations)
    for reading in Readings.getReadings():
      newObservations = observations.copy()
      newObservations[newLocation] = reading
      readingDistribution[reading] = self.getReadingSetProbability(newObservations) / currentSetProbability
    
    return readingDistribution
