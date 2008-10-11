import mdp, util

from abstractAgents import AbstractValueEstimationAgent

class ValueIterationAgent(AbstractValueEstimationAgent):
  """
      * Please read abstractAgents.py before reading this.*

      A ValueIterationAgent takes a markov decision process
      (see mdp.py) on initialization and runs value iteration
      for a given number of iterations as well as a discount 
      value.
  """
  def __init__(self, mdp, discount = 0.9, iterations = 100):
    """
      Your value iteration agent should take an mdp on
      construction, run the indicated number of iterations
      and then act according to the resulting policy.
    
      Some useful mdp methods you will use:
          mdp.getStates()
          mdp.getPossibleActions(state)
          mdp.getTransitionStatesAndProbs(state, action)
          mdp.getReward(state, action, nextState)
    """
    self.mdp = mdp
    self.discount = discount
    self.iterations = iterations
    
    self.utilities = util.Counter()
    states = mdp.getStates()
    for state in states:
        self.utilities[state] = 0
        
    for i in range(1,iterations+1):
        newUtilities = util.Counter()
        for state in states:
            newUtility = 0
            for action in mdp.getPossibleActions(state):
                q_value = 0
                for transition in mdp.getTransitionStatesAndProbs(state,action):
                    nextState = transition[0]
                    probability = transition[1]
                    q_value += probability*(mdp.getReward(state,action,nextState) + \
                        discount*self.utilities[nextState])
                newUtility = max(newUtility, q_value)
            newUtilities[state] = newUtility
        self.utilities = newUtilities
    
    """ q-values are a dictionary from states to dictionaries of action => qvalue mappings"""
    self.qvalues = {}
    for state in states:
        self.qvalues[state] = util.Counter()
        for action in mdp.getPossibleActions(state):
            q_value = 0
            for transition in mdp.getTransitionStatesAndProbs(state,action):
                nextState = transition[0]
                probability = transition[1]
                q_value += probability*(mdp.getReward(state,action,nextState) + \
                        discount*self.utilities[nextState])
            self.qvalues[state][action] = q_value
    
    
  def getValue(self, state):
    """
      Return the value of the state 
      (after the indicated number of value iteration passes).      
    """
    return self.utilities[state]

  def getQValue(self, state, action):
    """
      Look up the q-value of the state action pair
      (after the indicated number of value iteration
      passes).  Note that value iteration does not
      necessarily create this quantity and you may have
      to derive it on the fly.
    """
    return self.qvalues[state][action]

  def getPolicy(self, state):
    """
      Look up the policy's recommendation for the state
      (after the indicated number of value iteration passes).

      This method should return exactly one legal action for each state.
      You may break ties any way you see fit. The getPolicy method is used 
      for display purposes & in the getAction method below.
    """
    return self.qvalues[state].argMax()

  def getAction(self, state):
    """
      Return the action recommended by the policy.  We have provided this
      for you.
    """
    return self.getPolicy(state)