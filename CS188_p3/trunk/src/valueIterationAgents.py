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
    
    self.utilities = {}
    self.qvalues = {}
    states = mdp.getStates()
    for state in states:
        self.utilities[state] = 0
        self.qvalues[state] = util.Counter()
        
    for i in range(iterations):
        newUtilities = {}
        for state in states:
            if self.mdp.isTerminal(state):
                continue
            childQs = []
            for action in mdp.getPossibleActions(state):
                q_value = 0
                for transition in mdp.getTransitionStatesAndProbs(state,action):
                    q_value += transition[1]*(mdp.getReward(state,action,transition[0]) + \
                        discount*self.utilities[transition[0]])
                childQs.append(q_value)
            newUtilities[state] = max(childQs)
        self.utilities.update(newUtilities)
    
    """ q-values are a dictionary from states to dictionaries of action => qvalue mappings"""
    
    for state in states:
        for action in mdp.getPossibleActions(state):
            q_value = 0
            for transition in mdp.getTransitionStatesAndProbs(state,action):
                q_value += transition[1]*(mdp.getReward(state,action,transition[0]) + \
                    discount*self.utilities[transition[0]])
            self.qvalues[state][action] = q_value
    
    
  def getValue(self, state):
    """
      Return the value of the state 
      (after the indicated number of value iteration passes).      
    """
    #print state, self.utilities[state]
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
    #print self.qvalues[state]
    #print self.qvalues[state].argMax()
    return self.qvalues[state].argMax()

  def getAction(self, state):
    """
      Return the action recommended by the policy.  We have provided this
      for you.
    """
    return self.getPolicy(state)