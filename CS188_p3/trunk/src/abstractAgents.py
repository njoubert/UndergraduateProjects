from game import Directions, Agent, Actions

import random,util

class AbstractValueEstimationAgent(Agent):
  """
    Abstract agent which assigns values to (state,action)
    Q-Values for an enviornment. As well as a value to a 
    state and a policy given respectively by,
    
    V(s) = max_{a in actions} Q(s,a)
    policy(s) = arg_max_{a in actions} Q(s,a)
    
    Both ValueIterationAgent and QLearningAgent inherit 
    from this agent. Whereas a ValueIterationAgent has
    a model of the environment via a MarkovDecisionProcess
    (see mdp.py) that is used to estimate Q-Values before
    ever actually acting, the QLearningAgent estimates 
    Q-Values while acting in the environment. 
  """
  ####################################
  #    Override These Functions      #  
  ####################################
  def getQValue(self, state, action):
    """
    Should return Q(state,action)
    """
    abstract
    
  def getValue(self, state):
    """
    What is the value of this state under the best action? 
    Concretely, this is given by
    
    V(s) = max_{a in actions} Q(s,a)
    """
    abstract  
    
  def getPolicy(self, state):
    """
    What is the best action to take in the state. Note that because
    we might want to explore, this might not coincide with getAction
    Concretely, this is given by
    
    policy(s) = arg_max_{a in actions} Q(s,a)
    
    If many actions achieve the maximal Q-value,
    it doesn't matter which is selected.
    """
    abstract  
    
  def getAction(self, state):
    """
    state: can call state.getLegalActions()
    Choose an action and return it. Be sure to
    inform the parent class via doAction(state, action)    
    """
    abstract    
  
  ######################################
  # Pacman Specific #  
  ######################################
  def setOptions(self, opts):
    """
    opts: Dictionary of (string,string) options
    
    This function is used only to interact with
    the Pacman engine
    """
    self.alpha = float(opts.get('alpha',1.0))
    self.epsilon = float(opts.get('epsilon',0.05))
    self.gamma = float(opts.get('gamma',0.8))
    self.numTrainEpisodes = int(opts.get('numTrain',1e300))
    if 'numWatchTrain' in opts:
        self.numTrainEpisodes = int(opts['numWatchTrain'])
   
class AbstractReinforcementAgent(AbstractValueEstimationAgent):
  """
      Abstract Reinforcemnt Agent: An AbstractValueEstimationAgent
	  which estimates Q-Values (as well as policies) from experience
	  rather than a model
      
      What you need to know:
		- The environment will call 
		  observeTransition(state,action,nextState,deltaReward)
		  This will call update(state, action, nextState, deltaReward)
		  which you should override. 
        - Use self.getLegalActions(state) to know which actions
		  are available in a state
  """
  ####################################
  #    Override These Functions      #  
  ####################################
  
  def update(self, state, action, nextState, reward):
    """
	    This class will call this function, which you write, after
	    observing a transition and reward
    """
    abstract
        
  ####################################
  #    Read These Functions          #  
  ####################################  
  
  def getLegalActions(self,state):
    """
      Get the actions available for a given
      state. This is what you should use to
      obtain legal actions for a state
    """
    return self.actionFn(state)
  
  def observeTransition(self, state,action,nextState,deltaReward):
    """
    	Called by environment to inform agent that a transition has
    	been observed. This will result in a call to self.update
    	on the same arguments
    	
    	NOTE: Do *not* override or call this function
    """  	
    self.episodeRewards += deltaReward
    self.update(state,action,nextState,deltaReward)
		    
  def startEpisode(self):
    """
      Start Episode
      
      Called by environment when new episode
      is starting
    """
    self.lastState = None
    self.lastAction = None
    self.episodeRewards = 0.0
    
  def stopEpisode(self):
    """
      Stop Episodes
      
      Called by environment when episode 
      is done
    """ 
    if self.episodesSoFar < self.numTrainEpisodes:
		  self.accumTrainRewards += self.episodeRewards
    else:
		  self.accumTrainRewards += self.episodeRewards
    self.episodesSoFar += 1    
    if self.episodesSoFar >= self.numTrainEpisodes:
      # Take off the training wheels
      self.epsilon = 0.0
      self.alpha = 0.0

  def isInTraining(self): 
      return self.episodesSoFar <= self.numTrainEpisodes
  
  def isInTesting(self):
      return not self.isInTraining()
      
  def __init__(self, actionFn = None):
    """
    actionFn: Function which takes a state
    and returns the list of legal actions
    """
    if actionFn == None:
        actionFn = lambda state: state.getLegalActions()
    self.actionFn = actionFn
    self.episodesSoFar = 0
    self.accumTrainRewards = 0.0 
    self.accumTestRewards = 0.0
    # Positive Infinity
    self.numTrainEpisodes = 1e300
  ###################
  # Controls needed for Crawler  #
  ###################
  def setEpsilon(self, epsilon):
    self.epsilon = epsilon
    
  def setLearningRate(self, alpha):
    self.alpha = alpha
    
  def setDiscount(self, discount):
    self.gamma = discount
    
  def doAction(self,state,action):
    """
        Called by inherited class when
        an action is taken in a state
    """
    self.lastState = state
    self.lastAction = action
  
  ############
  # Pacman Specific #
  ############
  def observationFunction(self, state):
    """
        This is where we ended up after our last action. 
        The simulation should somehow ensure this is called    
    """
    if not self.lastState is None: 
        reward = state.getScore() - self.lastState.getScore()
        self.observeTransition(self.lastState, self.lastAction, state, reward)
    return state    
     
  def registerInitialState(self, state):
    self.startEpisode()      
    if self.episodesSoFar == 0:
        print 'Beggining %d episodes of Training' % (
                self.numTrainEpisodes)
    
  def final(self, state):
    """
      Called by Pacman game at the terminal state
    """
    deltaReward = state.getScore() - self.lastState.getScore()
    self.observeTransition(self.lastState, self.lastAction, state, deltaReward)
    self.stopEpisode()
    
    # Make sure we have this var
    if not 'lastWindowAccumRewards' in self.__dict__:
        self.lastWindowAccumRewards = 0.0
    self.lastWindowAccumRewards += state.getScore()
    
    NUM_EPS_UPDATE = 100
    if self.episodesSoFar % NUM_EPS_UPDATE == 0:
        if self.isInTraining():
            windowAvg = self.lastWindowAccumRewards / float(NUM_EPS_UPDATE)
            trainAvg = self.accumTrainRewards / float(self.episodesSoFar)                
            print 'Abstract Reinforcement Agent {'
            print '\tCompleted %d out of %d training episodes' % (
                   self.episodesSoFar,self.numTrainEpisodes)
            print '\tAverage Rewards for last %d episodes: %.2f'  % (
                    NUM_EPS_UPDATE,windowAvg)
            print '\tAverage Rewards over training: %.2f' % (
                    trainAvg)
            print '}'
            self.lastWindowAccumRewards = 0.0
    if self.episodesSoFar == self.numTrainEpisodes:
        msg = 'Training Done (turning off epsilon and alpha)'
        print '%s\n%s' % (msg,'-' * len(msg))