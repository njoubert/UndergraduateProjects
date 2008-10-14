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
    self.qvalues = {}
    #self.qvalues = util.Counter()
  
  def getQValue(self, state, action):
    """
      Returns Q(state,action)    
      Should return 0.0 if we never seen
      a state or (state,action) tuple 
    """
    if not self.qvalues.has_key(state):
        return 0.0
    else:
        return self.qvalues[state].getCount(action)
  
    
  def getValue(self, state):
    """
      Returns max_action Q(state,action)        
      where is max is over legal actions
    """
    if not self.qvalues.has_key(state):
        self.qvalues[state] = util.Counter()
        actions = self.getLegalActions(state)
        for action in actions:
            self.qvalues[state][action] = 0.0
    bestAction =  self.qvalues[state].argMax()
    return self.qvalues[state].getCount(bestAction)

    
  def getPolicy(self, state):
    """
    What is the best action to take in a state
    """
    if not self.qvalues.has_key(state):
        self.qvalues[state] = util.Counter()
        actions = self.getLegalActions(state)
        for action in actions:
            self.qvalues[state][action] = 0.0
    return self.qvalues[state].argMax()
    
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
    # Pick Actio
    action = None
    greedy = util.flipCoin(self.epsilon)
    if greedy:
        action = random.choice(self.getLegalActions(state))
    else:
        action = self.getPolicy(state)
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
    if not self.qvalues.has_key(state):
        self.qvalues[state] = util.Counter()
        actions = self.getLegalActions(state)
        for action in actions:
            self.qvalues[state][action] = 0.0
    currentQ = self.qvalues[state].getCount(action)
    sample = reward + self.gamma*self.getValue(nextState)
    self.qvalues[state][action] = (1-self.alpha)*currentQ + self.alpha*sample
    
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
    # features -> weights
    self.weights = util.Counter()
    
  def getQValue(self, state, action):
    """
      Should return Q(state,action) = w * featureVector
      where * is the dotProduct operator
    """
    if not self.qvalues.has_key(state):
        self.qvalues[state] = util.Counter()
        actions = self.getLegalActions(state)
        for action in actions:
            self.qvalues[state][action] = 0.0
    featureVector = self.featExtractor.getFeatures(state,action)
    q_value = 0
    for feature in featureVector:
      q_value += featureVector[feature]*self.weights.getCount(feature)
    self.qvalues[state][action]=q_value
    return q_value
    
  def update(self, state, action, nextState, reward):
    """
       Should update your weights based on transition  
    """
    # First we update the weights related to this feature
    correction = reward + self.gamma*self.getValue(nextState) - self.getQValue(state,action)
    featureVector = self.featExtractor.getFeatures(state,action)
    for feature in featureVector:
      self.weights[feature] = self.weights.getCount(feature) + self.alpha*correction*featureVector[feature]
    # Now we update out Q-Values I think?
    if not self.qvalues.has_key(state):
        self.qvalues[state] = util.Counter()
        actions = self.getLegalActions(state)
        for action in actions:
            self.qvalues[state][action] = 0.0
    currentQ = self.qvalues[state].getCount(action)
    self.qvalues[state][action] = currentQ + self.alpha*correction

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