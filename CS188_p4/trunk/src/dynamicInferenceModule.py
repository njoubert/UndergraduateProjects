from ghostbusters import *
from util import *
import util
import random

class DynamicInferenceModule:
  """
  A dynamic inference module must track a belief distribution over (tuples of) ghost locations.
  
  It responds to requests for incremental updates for observations and elapsed time.
  
  This is an abstract class, which you should not modify.
  """
  
  def initialize(self):
    """
    Set the belief state to an initial, prior value.
    """
    abstract
  
  def observe(self, observation):
    """
    Update beliefs based on the given observation.
    """
    abstract
    
  def elapseTime(self):
    """
    Update beliefs for a time step elapsing.
    """
    abstract
    
  def getInitialDistribution(self):
    """
    Return the agent's current belief state, a distribution over
    ghost locations conditioned on all evidence and time passage.
    """
    abstract


class ExactDynamicInferenceModule(DynamicInferenceModule):
  """
  The exact dynamic inference module should use forward-algorithm
  updates to compute the exact belief function at each time / observation.
  
  The provided implementation is broken; it does not update its beliefs.
  """
  
  def __init__(self, game):
    self.beliefs = None
    self.game = game
    
  def initialize(self):
    """
    Initialize the agent's beliefs to the game's prior over tuples.
    """
    self.beliefs = Counter(self.game.getInitialDistribution())
    
  def observe(self, observation):
    """
    Update beliefs in response to the given observations.
    
    Provided implementation is broken.
    """
    
    # BROKEN
    pass
    
  def elapseTime(self):
    """
    Update beliefs in response to a time step passing.
    
    Provided implementation is broken.
    """
    
    # BROKEN
    pass
        
  def getInitialDistribution(self):
    """
    Return the agent's current beliefs distribution over ghost tuples.
    """
    return self.beliefs


class ApproximateDynamicInferenceModule(DynamicInferenceModule):
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
    
    util.raiseNotDefined()
    
  def observe(self, observation):
    """
    Update beliefs to reflect the given observations.
    """
    
    util.raiseNotDefined()

  def elapseTime(self):
    """
    Update beliefs to reflect the passage of a time step.
    """    
    
    util.raiseNotDefined()
    
  def getInitialDistribution(self):
    """
    Return the agent's current belief (approximation) as a distribution
    over ghost tuples.  Note that this distribution can and should be
    sparse in the sense that many tuples may not be represented in the 
    distribution if there are more tuples than particles.  The probability
    over these missing tuples will be treated as zero by the GUI.
    """
    
    util.raiseNotDefined()
    
