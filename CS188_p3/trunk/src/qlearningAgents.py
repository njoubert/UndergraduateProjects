from game import *
from abstractAgents import AbstractReinforcementAgent
from featureExtractors import *

import random,util,math
          
class QLearningAgent(AbstractReinforcementAgent):
  """
    Q-Learning Agent
    
    Functions you should override:
      - getQValue
      - getAction
      - getValue
      - getPolicy
      - update
      
    Instance variables you have access to
      - self.epsilon (exploration prob)
      - self.alpha (learning rate)
      - self.gamma (discount rate)
    
    Parent Functions you should use
      - self.getLegalActions(state) 
        which returns legal actions
        for a state
  """
  def __init__(self,actionFn = None):
    """
       You might want to initialize     
       Q-values here...
    """    
    AbstractReinforcementAgent.__init__(self, actionFn)    
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
  
  def getQValue(self, state, action):
    """
      Returns Q(state,action)    
      Should return 0.0 if we never seen
      a state or (state,action) tuple 
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
  
    
  def getValue(self, state):
    """
      Returns max_action Q(state,action)        
      where is max is over legal actions
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
    
  def getPolicy(self, state):
    """
    What is the best action to take in a state
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
    
  def getAction(self, state):
    """
      What action to take in the current state. With
      probability self.epsilon, we should take a random
      action and take the best policy action otherwise.
    
      After you choose an action make sure to
      inform your parent self.doAction(state,action) 
      This is done for you, just don't clobber it
       
      HINT: you might want to use util.flipCoin
      here..... (see util.py)
    """  
    # Pick Action
    action = None
    "*** YOUR CODE HERE ***"
    # Need to inform parent of action for Pacman
    self.doAction(state,action)    
    return action
  
  def update(self, state, action, nextState, reward):
    """
      The parent class calls this to observe a 
      state = action => nextState and reward transition.
      You should do your Q-Value update here
      
      NOTE: You should never call this function,
      it will be called on your behalf
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
    
class ApproximateQLearningAgent(QLearningAgent):
  """
     ApproximateQLearningAgent
     
     You should only have to overwrite getQValue
     and update and all other QLearningAgent functions
     should work as is
  """
  def __init__(self, actionFn, featExtractorType = None):
    """
        Should initialize weights here...
    """
    QLearningAgent.__init__(self, actionFn)
    if featExtractorType == None:
      featExtractorType = IdentityFeatureExtractor
    self.featExtractor = featExtractorType()
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
    
  def getQValue(self, state, action):
    """
      Should return Q(state,action) = w * featureVector
      where * is the dotProduct operator
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()
    
  def update(self, state, action, nextState, reward):
    """
       Should update your weights based on transition  
    """
    "*** YOUR CODE HERE ***"
    util.raiseNotDefined()

class PacmanQLearningAgent(QLearningAgent):
  def __init__(self):    
    QLearningAgent.__init__(self)
        
class TestPacmanApproximateQLearningAgent(ApproximateQLearningAgent):
  """
    Use this to test your ApproximateQLearner. It
    should perform identically to your QLearner
    since the only feature is the raw (state,action)
    pair. It is mathematically equivalent to the 
    QLearningAgent
  """
  def __init__(self):
    ApproximateQLearningAgent.__init__(self,
      None, # actionFn
      IdentityFeatureExtractor)

class SimplePacmanApproximateQLearningAgent(ApproximateQLearningAgent):
  """
    Just an ApproximateQLearningAgent with 
    a SimplePacmanFeatureExtractor
  """
  def __init__(self):
    ApproximateQLearningAgent.__init__(self,
      None, #actionFn
      SimplePacmanFeatureExtractor)