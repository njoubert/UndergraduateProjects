from ghostbusters import *
from util import *
import util
import random

"""
Classes for ghostbuster agents.  See the write-up for details on what to do.

CODING HINTS:
  -- You can get (location, reading) pairs from an observation map using observations.items()
  -- util.py has utility methods and classes for working with probabilities that will save
     you a LOT of time:
       ** Counters manage counts and distributions better than dictionaries (they give zero
          rather than an exception if you ask for the count of something not in the key set
       ** sample() and sampleMultiple() draw from a distribution encoded as a Counter
       ** normalize() produces a normalized copy of a Counter
       ** listToDistribution() makes a distribution from a list, where each occurence in the
          list has a count of 1.0
       ** maxes() takes a counter and returns the max value along with a list of keys which
          have that value
  -- Most lists and distributions you need can be gotten from self.game objects; see ghostbusters.Game
  -- Some lists come straight from other classes, such as Readings.getReadings() = ['RED', 'ORANGE', ...]
"""

class GhostbusterAgent:
  """
  Abstract class for all ghostbuster agents.
  
  You do not need to modify this class.
  """
  
  def observe(self, observation):
    """
    Update beliefs in response to an observation.
    
    Observations are (location, reading pairs).
    
    Note that it is the responsibility of the game
    to not send a repeat observation in a given time
    step, so the agent can assume each observation
    is of an random variable which is unknown in the
    current time step.
    """
    util.raiseNotDefined()
    
  def elapseTime(self):
    """
    Update beliefs in response to the passage of a time
    step.  This should produce an exception for agents
    for the static game, in which time does not pass.
    """
    raiseNotDefined()

  def getAction(self):
    """
    Select an action.
    
    Actions for ghostbusters game are of the form:
      -- ('sense', location)
      -- ('bust', ghostTuple)
      -- ('wait', None)
    Actions can be constructed using the
    ghostbusters.Actions class methods.
      
    In the static game, 'wait' is always invalid.
    """
    util.raiseNotDefined()
    
  def getExpectedGhostCounts(self):
    """
    Form the joint distribution over ghost position tuples, and extract the expected
    number of ghosts at each position.  Note that the expectation at a position may be
    more than one if multiple ghosts are in play.
    
    This method is used by the gui to display agent beliefs in each square.
    """
    util.raiseNotDefined()
    
  
class StaticGhostbusterAgent(GhostbusterAgent):
  """
  Abstract class for agents which do not model the passage of time.
  
  Static agents accumulate all observations to date, and they always do inference
  over ghost positions from scratch, calculating P(ghosts | observations) directly
  rather than incrementally.
  
  You do not need to modify this class.
  """
  
  def __init__(self, inferenceModule, game):
    """
    Static agents have an inference module which does their belief calculations,
    and they know what game they are in.  Most information about how ghostbusters
    works (conditional probabilities, etc) is available from the game object.
    
    Note that Python allows you to access game.gameState, but you should pretend
    that you cannot -- solution which use this true state information will be
    incorrect.
    """
    self.inferenceModule = inferenceModule
    self.game = game
    self.observations = {}
    
  def observe(self, observation):
    """
    Put the newest observation into the list of observations, but do no
    other computation (i.e. no incremental belief updates) for the static agent.
    
    Observations are (location, reading pairs).
    
    Note that in static ghostbusters, there is no concept of re-sensing a location
    to get another reading.  The readings at each location are fixed (though perhaps
    noisy).
    """
    location, reading = observation
    self.observations[location] = reading
    
  def elapseTime(self):
    raise 'Static agents cannot elapse time!'

  def getExpectedGhostCounts(self, observations = None):
    """
    Form the joint distribution over ghost position tuples, and extract the expected
    number of ghosts at each position.  Note that the expectation at a position may be
    more than one if multiple ghosts are in play.
    
    This method is used by the gui to display agent beliefs in each square.
    """
    if (None == observations):
        observations = self.observations
        
    p_GhostTuples_given_observations = self.inferenceModule.getGhostTupleDistributionGivenObservations(observations)
    expectedCounts = Counter()
    for ghostTuple in p_GhostTuples_given_observations.keys():
      probability = p_GhostTuples_given_observations[ghostTuple]
      for ghost in ghostTuple:
        expectedCounts.incrementCount(ghost, probability)
    return expectedCounts
    

class KeyboardActionSelector:
  """
  GUI interface code which gets an action from the user.
  
  Suitable for use in the static or the dynamic game.
  
  You should not modify this code.
  """
  
  def __init__(self, game):
    self.game = game

  def getAction(self):
    eventType, details = self.__getValidEvent()
    if eventType == 'location':
      return Actions.makeSensingAction(details)
    elif eventType == 'button' and details == 'time-button':
      return Actions.makeWaitAction()
    locations = []
    while len(locations) < self.game.getNumGhosts():
      eventType, details = self.__getValidEvent()
      if eventType == 'location':
        locations.append(details)
        msg = 'bust at ' + str(details)
        self.game.display.infoPane.updateMessages(msg)      
        self.game.busts -= 1
        self.game.display.infoPane.updateScore(self.game.state, self.game.busts)
    return Actions.makeBustingAction(locations)
      
  def __getValidEvent(self):
    while True:
      event = self.__getEvent()
      if event != None:
        return event

  def __getEvent(self):
    import gui
    point, button = gui.wait_for_click()
    object, position = gui.getEvent(point)
    if object == 'grid':
      return 'location', position
    elif object == 'bust-button':
      self.game.display.infoPane.drawBustButton('clicked')
      self.game.display.infoPane.drawTimeButton('unclickable')      
      return 'button', object
    elif object == 'time-button':
      return 'button', object
    else:
      return None


class StaticKeyboardAgent(StaticGhostbusterAgent):
  """
  Agent which plays the static game using user-selected actions.
  
  You do not need to modify this class.
  """
  def __init__(self, inferenceModule, game):
    StaticGhostbusterAgent.__init__(self, inferenceModule, game)
    self.actionSelector = KeyboardActionSelector(game)
    
  def getAction(self):
    return self.actionSelector.getAction()


class StaticVPIAgent(StaticGhostbusterAgent):
  """
  Computer-controlled ghostbuster agent.
  
  This agent plays using value of (perfect) information calculations.
  
  The initial implementation is broken, always taking a random busting action
  without sensing.  You will rewrite it to greedily sense if any sensing action
  has an expected gain in utility / score (taking into account the cost of
  sensing).  If no sensing action has a greedy gain, then you will select a
  position tuple to bust.  In this case, your agent should bust the tuple with
  the highest expected utility / score according to its current beliefs.
  """







  def getAction(self):
    
    "*** YOUR CODE HERE ***"
    
    
    return Actions.makeBustingAction(random.choice(self.game.getBustingOptions()))
    
  

    
class DynamicGhostbusterAgent(GhostbusterAgent):
  """
  Abstract class for agents for the dynamic game, which do model the passage of time.
  
  Unlike static agents, dynamic agents process each observation they get incrementally,
  using belief updates (as in the forward algorithm).
  
  You do not need to modify this abstract class.
  """
  
  def __init__(self, inferenceModule, game):
    """
    Dynamic agents must initialize their beliefs (to the prior over ghost tuples)
    and update as they go.
    """
    self.inferenceModule = inferenceModule
    self.game = game
    self.inferenceModule.initialize()
  
  def observe(self, observation):
    """
    Update beliefs in response to an observation.
    
    Observations are (location, reading pairs).
    
    Note that it is the responsibility of the game
    to not send a repeat observation in a given time
    step, so the agent can assume each observation
    is of an unobserved random variable.
    """    
    self.inferenceModule.observe(observation)
    
  def elapseTime(self):
    """
    Update beliefs in response to the passage of a time step.
    """
    self.inferenceModule.elapseTime()
    
  def getExpectedGhostCounts(self):
    """
    Just as in the static case.
    
    Form the joint distribution over ghost position tuples, and extract the expected
    number of ghosts at each position.  Note that the expectation at a position may be
    more than one if multiple ghosts are in play.
    
    This method is used by the gui to display agent beliefs in each square.
    """
    p_Ghosts_given_observations = self.inferenceModule.getInitialDistribution()
    expectedCounts = Counter()
    for ghosts in p_Ghosts_given_observations.keys():
      probability = p_Ghosts_given_observations[ghosts]
      for ghost in ghosts:
        expectedCounts.incrementCount(ghost, probability)
    return expectedCounts


class DynamicKeyboardAgent(DynamicGhostbusterAgent,KeyboardActionSelector):
  """
  Agent which plays the dynamic game using user-selected actions.
  
  You do not need to modify this class.
  """

  def __init__(self, inferenceModule, game):
    DynamicGhostbusterAgent.__init__(self, inferenceModule, game)
    self.actionSelector = KeyboardActionSelector(game)
    
  def getAction(self):
    return self.actionSelector.getAction()



