"""
The game of ghostbusters!

This file contains the main logic for the game.  You do not need to know about everything
in here.  The critical methods you need are in Game, so start with that class and with
the example code in ghostbusterAgent.py.  From a Game object, your agent can
get most of the important probability distributions, board layout information, and so on.

You should not change this file!
"""

from util import *
import sys, random
import staticInferenceModule, dynamicInferenceModule

# SCORE CONSTANTS

SENSOR_SCORE = -1.0
GHOST_SCORE = 50.0


# GAME COMPONENTS

class Sensors:
  """
  A sensors object manages the distributions for each individual location's sensor.  In this
  implementation, all sensors share a single distribution where the reading is a noisy indicator
  of the true (manhattan) distance.
  """
  def __init__(self, distribution):
    """
    The conditional distribution family passed in maps distances to readings.  Any distance greater
    than the largest specified distance is clipped to the maximum.
    """
    self.readingDistribution = distribution
    self.farthestReading = max(distribution)
    self.distributionCache = {}

  def __getReadingDistribution(self, distance):
    """
    Return a reading distribution for a given distance.
    """
    clippedDistance = min(distance, self.farthestReading)
    return self.readingDistribution[clippedDistance]

  def getReadingDistributionGivenGhostTuple(self, ghostTuple, location):
    """
    The reading is a noisy indicator of the closest ghost.
    
    Cached.
    """
    arg = (tuple(ghostTuple), location)
    if arg not in self.distributionCache:
      distances = [manhattanDistance(location, ghost) for ghost in ghostTuple]
      self.distributionCache[arg] = self.__getReadingDistribution(min(distances))
    return self.distributionCache[arg]



class Readings:
  """
  A reading is what you observe when you drop a sensor.
  Intuitively, RED corresponds to hot (close), ORANGE to warm, 
  YELLOW to cooler, and GREED to cold (distant)
  
  The specific meaning of a reading is specified in a 
  sensor reading conditional distribution, see sensorDistributions.py
  
  python usage note:
    staticmethod(getReadings), below, means that you can call
    Readings.getReadings() without instatiating a Readings object
  """

  RED = 'RED'
  ORANGE = 'ORANGE'
  YELLOW = 'YELLOW'
  GREEN = 'GREEN'
  
  def getReadings():
    return [Readings.RED, Readings.ORANGE, Readings.YELLOW, Readings.GREEN]
  getReadings = staticmethod(getReadings)
  
  
class Actions:
  """
  there are a few basic actions in ghostbusters. they are:
  1. sense at <location> where location is a (row, col)
  2. bust at <locations> where locations is a list of (row, col) tuples
     note that this is a list because you don't find out the result of
     your busting actions until you are finished busting
  3. wait (let time elapse)
  """
  
  def makeSensingAction(location):
    return ('sense', location)
  makeSensingAction = staticmethod(makeSensingAction)

  def makeBustingAction(locations):
    return ('bust', locations)
  makeBustingAction = staticmethod(makeBustingAction)

  def makeWaitAction():
    return ('wait', None)
  makeWaitAction = staticmethod(makeWaitAction)
  

  
class Motion:
  """
  A class for motion plans. a motion plan is a specification of a 
  movement distribution at each position in a layout.
  
  A movement distribution specifies the probability for moving up,
  down, left, right, and staying still.
  
  These distributions are constructed in the following way:
    1. select a base direction for each square (could be stop)
    2. add noise so that with probability p, take the base movement
       and with probability 1-p take one of the other four movements,
       split up uniformly
       
  The basic motion plan has the stop movement as the base for each
  square. with noise=0, the ghosts will not move, and as noise increases,
  ghosts move increasingly randomly (Brownian-style movement).
  
  The center motion plan sets each base movement to point towards the center
  of the board.
  
  The circle motion plan sends ghosts in clockwise circles.
  """
  
  def __init__(self, layout, type = 'basic', noise = 0.0):
    self.rows, self.cols = layout.rows, layout.cols
    self.distributions = [[self.__getClippedDistribution((row, col), type, noise) for col in range(self.cols)] for row in range(self.rows)]

  def __getClippedDistribution(self, location, type, noise):
    motion = self.__getMotion(location, type)
    motionDistribution = self.__getMotionDistribution(motion, noise)
    return self.__motionDistributionToLocationDistribution(motionDistribution, location)

  def __getMotion(self, location, type):
    if type == 'basic':
      return 'stop'
    row, col = location
    cRow, cCol = self.rows/2.0, self.cols/2.0
    if type == 'center':
      if abs(cRow - row) > abs(cCol - col):
        if row < cRow: return 'down'
        else: return 'up'
      else:
        if col < cCol: return 'right'
        else: return 'left'
    if type == 'circle':
      if row < cRow and col >= cCol:
        if col < self.cols - row - 1:
          return 'right'
        else:
          return 'down'
      if row < cRow and col < cCol:
        if col >= row:
          return 'right'
        else:
          return 'up'
      if row >= cRow and col < cCol:
        if col >= self.rows - row:
          return 'left'
        else:
          return 'up'
      if row >= cRow and col >= cCol:
        if self.rows - row <= self.cols - col:
          return 'left'
        else:
          return 'down'
    raise 'Unknown current type: ' + type

  def __getMotionDistribution(self, direction, noise):
    dist = Counter({ 'stop': noise/5.0,
                          'up': noise/5.0,
                          'down': noise/5.0,
                          'left': noise/5.0,
                          'right': noise/5.0 })
    dist.incrementCount(direction, 1.0-noise)
    return dist
    
  
  def __motionDistributionToLocationDistribution(self, distribution, location):
    clippedDistribution = Counter()
    for direction in distribution.keys():
      prob = distribution.getCount(direction)
      row, col = location
      if direction == 'up' and row > 0: row -= 1
      if direction == 'down' and row < self.rows-1: row += 1
      if direction == 'left' and col > 0: col -= 1
      if direction == 'right' and col < self.cols-1: col+= 1
      clippedDistribution.incrementCount((row, col), prob)
    return clippedDistribution
  
  def getLocationDistributionGivenPreviousLocation(self, location):
    row, col = location
    return self.distributions[row][col]
  
  def sampleLocation(self, previousLocation):
    return sample(self.getLocationDistributionGivenPreviousLocation(previousLocation))

class Layout:
  def __init__(self, (rows, cols)):
    self.board = [[(row, col) for col in range(cols)] for row in range(rows)]
    self.rows = rows
    self.cols = cols
    self.locations = []
    for row in range(rows):
      for col in range(cols):
        self.locations.append((row, col))
    self.numLocations = len(self.locations)
    
  def getLocations(self):
    return self.locations

  def isLegalPos(self, pos):
    """
    returns True if the position is a legal position
    on the board
    """
    row, col = pos
    if (row < 0) or (col < 0) or (row >= self.rows) or (col >= self.cols):
      return False
    return True
    
class GameState:
  """
  controls the inner state of a ghostbusters game. it primarily
  maintains ghost locations and decides how to move each ghost
  in the timed version
  """
  def __init__(self):
    self.layout = None
    self.ghostTuple = None
    self.timeStep = 0
    self.score = 0
    
  def initialize(self, layout, numGhosts, sensors):
    """
    creates an initial game state with random start
    positions for each ghost
    """
    self.layout = layout
    self.sensorReadings = [[0 for col in range(layout.cols)] for row in range(layout.rows)]
    ghosts = []
    for i in range(numGhosts):
      row = random.randint(0, self.layout.rows-1)
      col = random.randint(0, self.layout.cols-1)
      ghosts.append((row,col))
    self.ghostTuple = tuple(ghosts)
    self.elapseTime(sensors)
      
  def doBust(self, bustPositions):
    """
    do each bust, removing affected ghosts
    returns a list of (position, hit/miss) results
    """    
    results = []
    ghostsToRemove = []
    hits = 0
    for pos in list(set(bustPositions)):
      result = 'miss'
      for ghost in self.ghostTuple:
        if pos == ghost:
          ghostsToRemove.append(ghost)
          result = 'hit'
          self.score += GHOST_SCORE          
      results.append((pos, result))
      
    ghostsLeft = []
    for ghost in self.ghostTuple:
      if not ghost in ghostsToRemove:
        ghostsLeft.append(ghost)
    self.ghostTuple = tuple(ghostsLeft)

    return results
  
  def elapseTime(self, sensors, motion=None):
    """
    move ghosts if relevant
    precompute sensorReadings at each board position
    """
    # don't move anything on the first time step
    if self.timeStep != 0:
      self.timeStep += 1
      
    if motion != None:
      newGhosts = []
      for ghost in self.ghostTuple:
        ghost = motion.sampleLocation(ghost)
        newGhosts.append(ghost)
      self.ghostTuple = tuple(newGhosts)
    
    for location in self.layout.locations:
      row, col = location
      self.sensorReadings[row][col] = sample(sensors.getReadingDistributionGivenGhostTuple(self.ghostTuple, location))
        
  def getGhostTuple(self):
    """
    returns a tuple of all ghost positions
    """
    return self.ghostTuple
  
  def getNumGhosts(self):
    return len(self.ghostTuple)
  
  def getContents(self, location):
    """
    returns a list of ghosts at the given position
    """
    contents = []
    for ghost in self.ghosts:
      if location == ghost:
        contents.append(ghost)
  

class Game:
  """
  The Game object controls many aspects of a game of ghostbusters, including control logic.
  
  A game takes the following arguments on construction:
  
    layout -- Specifics about the board size (see Layout class)
    
    numGhosts -- The number of ghosts to place on the board. Note that
                the ghost locations are maintained by the GameState class.

    sensors -- An instance of the Sensors class, specifying the
              sensor behavior, in the form of distributions over sensor readings.
              
    motion -- An instance of the Motion class, specifying a motion
              plan for the time version of the game.  All ghosts' motions are
              (up to noise) specified by a map from locations to movement directions.
              You can think of these maps as representing a 'current' at each square.
              
    agentBuilder -- Agents neet to know what game they are in, so are built
              in the Game constructor.
              
    uniformprior -- A boolean indicating whether to use an uniform prior
                 
  """

  def __init__(self, agentBuilder, layout, numGhosts, sensors, motion, uniformprior = True):
    import gui
    self.gameOver = False
    self.state = GameState()
    self.state.initialize(layout, numGhosts, sensors)
    self.display = gui.GhostbusterGraphics()
    self.motion = motion
    self.sensors = sensors
    self.display.initialize(self.state)
    self.busts = self.state.getNumGhosts()
    self.uniformprior = uniformprior
    
    # create agent last
    self.agent = agentBuilder(self)
      
  #############################################################################
  #
  # BEGIN IMPORTANT CONVENIENCE METHODS FOR AGENTS -- READ THESE!
  #
  #############################################################################
  
  def getNumGhosts(self):
    """
    Return the number of ghosts in the current game.
    """
    return len(self.state.ghostTuple)
  
  def getLocations(self):
    """
    Return the list of all possible locations (row, col) in the current layout.
    """
    return self.state.layout.locations

  def getNumLocations(self):
    """
    Return the number of possible locations in the current layout.
    """
    return self.state.layout.numLocations

  def getGhostTuples(self):
    """
    Return the possible ghost tuples in the current game.  A ghost tuple is in the
    form (location1, location2, ..., locationN) when there are N ghosts.  NOTE: if
    there is only one ghost, you will still get tuples, but they will be singleton
    tuples with only one position, like ((0,1),).
    """
    list = factorials(self.getNumGhosts(), self.getLocations())
    return [tuple(i) for i in list]
  
  def getInitialDistribution(self):
    """
    Return the prior distribution over ghost tuples.  If there is only one ghost,
    this will be a lot like a distribution over locations, except that each key
    will be a singleton tuple.  See getGhostTuples().
    """
    ghostTuples = self.getGhostTuples()
    boardSize = max(ghostTuples)
    boardCenter = (boardSize[0][0]/2.0, boardSize[0][1]/2.0)
    p_GhostTuples = Counter()
    for ghostTuple in ghostTuples:
      if (self.uniformprior):
        distanceToCenter = 1
      else :
        distanceToCenter = abs(ghostTuple[0][0]-boardCenter[0]) + abs(ghostTuple[0][1]-boardCenter[1])
      p_GhostTuples.setCount(ghostTuple, distanceToCenter)
    return normalize(p_GhostTuples)
  
  def getBustingOptions(self):
    """
    Return the list of all busting options.  Each option is a tuple of locations,
    such as ((1,4), (5,2)).
    
    Note that if ((1,4), (5,2)) is in the list, ((5,2), (1,4)) will not be, since
    this option would represent the same busting action (since busts are not
    distinguishable).
    
    Note also that an option is not a busting action, just the details of an
    action: use Actions.makeBustingAction() to make a busting action.
    """
    list = choices(self.getNumGhosts(), self.getLocations())
    return [tuple(i) for i in list]
    
  def getReadingDistributionGivenGhostTuple(self, ghostTuple, location):
    """
    Return the distribution over readings for a particular sensor, given a ghostTuple.
    """
    return self.sensors.getReadingDistributionGivenGhostTuple(ghostTuple, location)
  
  def getGhostTupleDistributionGivenPreviousGhostTuple(self, prevGhostTuple):
    """
    Return the distribution over ghostTuples at the current time step given a
    ghostTuple at the previous time step.  Ghosts will (independently) move
    according to the motion object for the game, with some noise.
    
    Note that the resulting distributions will be sparse, since most tuples
    will be unreachable (your code will be faster if you exploit this sparsity).
    """
    p_Location_given_prevGhostTuple_List = [self.getMotion().getLocationDistributionGivenPreviousLocation(location) for location in prevGhostTuple]
    p_GhostTuple_given_prevGhostTuple = cartesianJoint(p_Location_given_prevGhostTuple_List)
    return p_GhostTuple_given_prevGhostTuple
    
  #############################################################################
  #
  # END IMPORTANT CONVENIENCE METHODS FOR AGENTS
  #
  #############################################################################
  
  def getMotion(self):
    """
    Get the current game's motion object.
    """
    return self.motion
    
  def run(self):
    """
    run a game
    """
    self.display.showState(self.state)
    self.display.showBeliefs(self.getLocations(), self.agent.getExpectedGhostCounts(), self.state.ghostTuple)
    observations = []
    while True:
      action = self.agent.getAction()

      if action[0] == 'wait':
        self.agent.elapseTime()
        self.state.elapseTime(self.sensors, self.motion)
        self.display.showState(self.state)
        self.display.showBeliefs(self.getLocations(), self.agent.getExpectedGhostCounts(), self.state.ghostTuple)
        observations = []

      elif action[0] == 'sense':
        row, col = action[1]
        observation = ((row, col), self.state.sensorReadings[row][col])
        if observation in observations:
          continue
        self.state.score += SENSOR_SCORE
        self.display.infoPane.updateScore(self.state, self.busts)
        self.display.showMove(observation)
        observations.append(observation)
        self.agent.observe(observation)
        self.display.showBeliefs(self.getLocations(), self.agent.getExpectedGhostCounts(), self.state.ghostTuple)
        
      elif action[0] == 'bust':
        locations = action[1]
        self.display.infoPane.drawBustButton('clicked')
        bustResults = self.state.doBust(locations)
        self.display.infoPane.updateScore(self.state, self.busts)
        self.display.endGame(bustResults)
        return True
        
        
    
def readCommand( argv ):
  """
  Processes the command used to run from the command line.
  """
  import getopt
  import gui
  
  # Set default options
  options = {'prior': 'uniform',
             'fixrandomseed': False,
             'layout': 'small', 
             'numghosts': 1, 
             'sensortype': 'deterministic',
             'player': 'human',
             'inference': 'exact',
             'zoom' : None,
             'noise' : 0.0,
             'motion': 'basic',
             'samples': 10000,
             'showghosts': False,
             'showbeliefs': True}
             
  args = {} # This dictionary will hold the objects used by the game
  
  # Read input from the command line
  commands = ['help',
              'prior=', 
              'fixrandomseed', 
              'layout=', 
              'ghosts=', 
              'sensortype=', 
              'player=',
              'inference=',
              'zoom=',
              'motionnoise=',
              'motiontype=',
              'samples=',
              'showghosts',
              'hidebeliefs']
  try:
    opts = getopt.getopt( argv, "hl:k:s:p:i:z:n:m:r:wq", commands )
  except getopt.GetoptError:
    print USAGE_STRING
    sys.exit( 2 )
    
  for option, value in opts[0]:
    if option in ['--help', '-h']:
      print USAGE_STRING
      sys.exit( 0 )
    if option in ['--player', '-p']:
      options['player'] = value
    if option in ['--inference', '-i']:
      options['inference'] = value
    if option in ['--layout', '-l']:
      options['layout'] = value
    if option in ['--ghosts', '-k']:
      options['numghosts'] = int( value )
    if option in ['--sensortype', '-s']:
      options['sensortype'] = value
    if option in ['--zoom', '-z']:
      options['zoom'] = float( value )
    if option in ['--motionnoise', '-n']:
      options['noise'] = float( value )
    if option in ['--motiontype', '-m']:
      options['motion'] = value
    if option in ['--samples', '-r']:
      options['samples'] = int( value )
    if option in ['--showghosts', '-w']:
      options['showghosts'] = True
    if option in ['--hidebeliefs', '-q']:
      options['showbeliefs'] = False
    if option in ['--prior']:
      options['prior'] = value
    if option in ['--fixrandomseed']:
      options['fixrandomseed'] = True
      
    
  # numghosts
  args['numghosts'] = options['numghosts']
      
  # Choose a layout
  boardSizes = { 'test': (3,3), 
                 'small': (4,6),
                 'medium': (6,10),
                 'large': (10,16) }
  args['layout'] = Layout( boardSizes[ options['layout'] ] )
  # auto-scaling the gui
  numSquares = args['layout'].cols
  if not options['zoom']:
    options['zoom'] = 1 - (numSquares / 32.0)

  # scale with zoom
  gui.scaleGridSize(options['zoom'])

  # sensor distribution
  import sensorDistributions
  if options['sensortype'] == 'deterministic':
    args['sensors'] = Sensors(sensorDistributions.deterministicSensorReadingDistribution)
  elif options['sensortype'] == 'noisy':
    args['sensors'] = Sensors(sensorDistributions.noisySensorReadingDistribution)

  # time
  args['motion'] = None
  timed = False
  if options['motion'] != 'basic' or options['noise'] > 0:
    timed = True
    gui.USE_TIME = True
    args['motion'] = Motion(args['layout'], options['motion'], options['noise'])

  import ghostbusterAgent
  agentBuilder = None
  if not timed:
      if options['player'] == 'human':
        agentBuilder = lambda game: ghostbusterAgent.StaticKeyboardAgent(staticInferenceModule.ExactStaticInferenceModule(game), game)
      if options['player'] == 'vpi':
        agentBuilder = lambda game: ghostbusterAgent.StaticVPIAgent(staticInferenceModule.ExactStaticInferenceModule(game), game)
  else:
      if options['inference'] == 'exact':
        agentBuilder = lambda game: ghostbusterAgent.DynamicKeyboardAgent(dynamicInferenceModule.ExactDynamicInferenceModule(game), game)
      if options['inference'] == 'approximate':
        agentBuilder = lambda game: ghostbusterAgent.DynamicKeyboardAgent(dynamicInferenceModule.ApproximateDynamicInferenceModule(game, options['samples']), game)
  if agentBuilder == None:
    raise 'Agent not spec\'d correctly!'
  args['agent'] = agentBuilder
  
  # show ghosts, beliefs
  if options['showghosts']:
    gui.SHOW_GHOSTS = True
  if not options['showbeliefs']:
    gui.SHOW_BELIEFS = False
    
  # prior
  args['prior'] = options['prior'] == 'uniform'

  # random seed
  if options['fixrandomseed']: random.seed(1)

  return args

USAGE_STRING = """
  USAGE:      python ghostbusters.py <options>
  EXAMPLES:   (1) python ghostbusters.py
                  - starts a default game
              (2) python ghostbusters.py --layout large --ghosts 5
              OR  python ghostbusters.py -l large -s 5
                  - starts a game on the large layout with 5 ghosts
  
  OPTIONS:    --layout, -l
                  sets the layout of the board to specified configuration
                  Legal values: test, small, medium, large
              --ghosts, -k
                  sets the number of ghosts
                  default: 1
              --player, -p
                  sets the decision-maker
                  legal values: human, vpi
                  default: human
              --inference, -i
                  use inference to calculate beliefs
                  legal values: exact, approximate
              --sensortype, -s
                  select the sensor distribution
                  legal values: deterministic, noisy
                  default: deterministic
              --motiontype, -m
                  select motion type
                  legal values: basic, circle, center
                  default: basic
              --motionnoise, -n
                  a value between 0 and 1. with probability n
                  take the motiontype-specified action and with
                  probability 1-n choose uniformly among other
                  directions
                  default: 0
              --samples, -r
                  number of samples for particle filtering
                  default: 10000
              --showghosts, -w
                  show true ghost positions (for testing, etc.)
                  default: False
              --hidebeliefs, -q
                  toggles belief display
                  default: True
              --zoom, -z
                  magnifies the GUI
                  default is adjusted for layout size
              --prior
                  set the prior distribution
                  legal values: uniform, border
                  default: uniform
              --fixrandomseed
                  fix the random seed to get reproduceable results
                  default: False                  
                 """
  
if __name__ == '__main__':
  """
  running ghostbusters.py from the command line
  """
  
  # Get game components based on input
  args = readCommand( sys.argv[1:] ) 
  
  agentBuilder = args['agent']
  layout = args['layout']
  numghosts = args['numghosts']
  sensors = args['sensors']
  motion = args['motion']
  uniformprior = args['prior']

  game = Game(agentBuilder, layout, numghosts, sensors, motion, uniformprior)
  game.run()

  
  # TODO:
  #   grow margin a little on the right (mine still overflows the text off the window)
  #   improve comments
  #   put some big QUESTION 1 markers in comments in the ghostbusterAgent.py file
  #   test export and scraper
  #   options: say what options were invalid if failure
  #   optional: options package from 2.1 gives more sensible output, including defaults
