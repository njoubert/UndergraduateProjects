"""
Capture.py holds the logic for Pacman capture the flag.
  
  (i)  Your interface to the pacman world:
          Pacman is a complex environment.  You probably don't want to 
          read through all of the code we wrote to make the game runs
          correctly.  This section contains the parts of the code
          that you will need to understand in order to complete the 
          project.  There is also some code in game.py that you should 
          understand.

  (ii)  The hidden secrets of pacman:
          This section contains all of the logic code that the pacman
          environment uses to decide who can move where, who dies when
          things collide, etc.  You shouldn't need to read this section
          of code, but you can if you want.
          
  (iii) Framework to start a game:
          The final section contains the code for reading the command
          you use to set up the game, then starting up a new game, along with 
          linking in all the external parts (agent functions, graphics).
          Check this section out to see all the options available to you.
          
To play your first game, type 'python capture.py' from the command line.
The keys are 
  P1: 'a', 's', 'd', and 'w' to move
  P2: 'l', ';', ',' and 'p' to move
"""
from game import GameStateData
from game import Game
from game import Directions
from game import Actions
from util import nearestPoint
from util import manhattanDistance
from game import Grid
from game import Configuration
from game import Agent
from game import reconstituteGrid
import sys, util, types, time, random

# If you change these, you won't affect the server, so you can't cheat
KILL_POINTS = 0 
SONAR_NOISE_RANGE = 9 # Must be odd
SONAR_NOISE_VALUES = [i - (SONAR_NOISE_RANGE - 1)/2 for i in range(SONAR_NOISE_RANGE)]
SIGHT_RANGE = 5 # Manhattan distance

def noisyDistance(pos1, pos2):
  return int(util.manhattanDistance(pos1, pos2) + random.choice(SONAR_NOISE_VALUES))

###################################################
# YOUR INTERFACE TO THE PACMAN WORLD: A GameState #
###################################################

class GameState:
  """
  A GameState specifies the full game state, including the food, capsules, 
  agent configurations and score changes.
  
  GameStates are used by the Game object to capture the actual state of the game and
  can be used by agents to reason about the game.
  
  Much of the information in a GameState is stored in a GameStateData object.  We 
  strongly suggest that you access that data via the accessor methods below rather
  than referring to the GameStateData object directly.
  """
  
  ####################################################
  # Accessor methods: use these to access state data #
  ####################################################
  
  def getLegalActions( self, agentIndex=0 ):
    """
    Returns the legal actions for the agent specified.
    """
    return AgentRules.getLegalActions( self, agentIndex )
    
  def generateSuccessor( self, agentIndex, action):
    """
    Returns the successor state (a GameState object) after the specified agent takes the action.
    """
    # Copy current state
    state = GameState(self)

    # Find appropriate rules for the agent
    AgentRules.applyAction( state, action, agentIndex )
    AgentRules.checkDeath(state, agentIndex)
    
    # Book keeping
    state.data._agentMoved = agentIndex
    state.data.score += state.data.scoreChange
    return state
  
  def getAgentState(self, index):
    return self.data.agentStates[index]

  def getAgentPosition(self, index):
    """
    Returns a location tuple if the agent with the given index is observable;
    if the agent is unobservable, returns None.
    """
    agentState = self.data.agentStates[index]
    return agentState.getPosition()
  
  def getNumAgents( self ):
    return len( self.data.agentStates )
  
  def getScore( self ):
    """
    Returns a number corresponding to the current score.
    """
    return self.data.score
  
  def getRedFood(self):
    """
    Returns a matrix of food that corresponds to the food on the red team's side.
    For the matrix m, m[x][y]=true if there is food in (x,y) that belongs to
    red (meaning red is protecting it, blue is trying to eat it).
    """
    return halfGrid(self.data.food, red = True)

  def getBlueFood(self):
    """
    Returns a matrix of food that corresponds to the food on the blue team's side.
    For the matrix m, m[x][y]=true if there is food in (x,y) that belongs to
    blue (meaning blue is protecting it, red is trying to eat it).
    """
    return halfGrid(self.data.food, red = False)
  
  def getWalls(self):
    """
    Just like getFood but for walls
    """
    return self.data.layout.walls

  def hasFood(self, x, y):
    """
    Returns true if the location (x,y) has food, regardless of 
    whether it's blue team food or red team food.
    """
    return self.data.food[x][y]
  
  def hasWall(self, x, y):
    """
    Returns true if (x,y) has a wall, false otherwise.
    """
    return self.data.layout.walls[x][y]

  def isOver( self ):
    return self.data._win
  
  def getRedTeamIndices(self):
    """
    Returns a list of agent index numbers for the agents on the red team.
    """
    return self.redTeam[:]

  def getBlueTeamIndices(self):
    """
    Returns a list of the agent index numbers for the agents on the blue team.
    """
    return self.blueTeam[:]
  
  def isOnRedTeam(self, agentIndex):
    """
    Returns true if the agent with the given agentIndex is on the red team.
    """
    return self.teams[agentIndex]
  
  def getAgentDistances(self):
    """
    Returns a noisy distance to each agent."
    """
    if 'agentDistances' in dir(self) : 
      return self.agentDistances
    else:
      return None
      
  #############################################
  #             Helper methods:               #          
  # You shouldn't need to call these directly #
  #############################################
  
  def __init__( self, prevState = None ):
    """ 
    Generates a new state by copying information from its predecessor.
    """
    if prevState != None: # Initial state
      self.data = GameStateData(prevState.data)
      self.blueTeam = prevState.blueTeam
      self.redTeam = prevState.redTeam
      self.teams = prevState.teams
      self.agentDistances = prevState.agentDistances
    else:
      self.data = GameStateData()
      self.agentDistances = []
    
  def deepCopy( self ):
    state = GameState( self )
    state.data = self.data.deepCopy()
    state.blueTeam = self.blueTeam[:]
    state.redTeam = self.redTeam[:]
    state.teams = self.teams[:]
    state.agentDistances = self.agentDistances[:]
    return state
    
  def makeObservation(self, index):
    state = self.deepCopy()
    
    # Adds the sonar signal
    pos = state.getAgentPosition(index)
    n = state.getNumAgents()
    distances = [noisyDistance(pos, state.getAgentPosition(i)) for i in range(n)]
    state.agentDistances = distances
    
    # Remove states of distant opponents
    if index in self.blueTeam:
      team = self.blueTeam
      otherTeam = self.redTeam
    else:
      otherTeam = self.blueTeam
      team = self.redTeam
      
    for enemy in otherTeam:
      seen = False
      enemyPos = state.getAgentPosition(enemy)
      for teammate in team:
        if util.manhattanDistance(enemyPos, state.getAgentPosition(teammate)) <= SIGHT_RANGE:
          seen = True
      if not seen: state.data.agentStates[enemy].configuration = None
    return state    

  def __eq__( self, other ):
    """
    Allows two states to be compared.
    """
    if other == None: return False
    return self.data == other.data
                                                      
  def __hash__( self ):
    """
    Allows states to be keys of dictionaries.
    """
    return int(hash( self.data ))

  def __str__( self ):
    
    return str(self.data)
      
  def initialize( self, layout, numAgents):
    """
    Creates an initial game state from a layout array (see layout.py).
    """
    self.data.initialize(layout, numAgents)
    positions = [a.configuration for a in self.data.agentStates]
    self.blueTeam = [i for i,p in enumerate(positions) if not self.isRed(p)]
    self.redTeam = [i for i,p in enumerate(positions) if self.isRed(p)]
    self.teams = [self.isRed(p) for p in positions]
  
  def isRed(self, configOrPos):
    width = self.data.layout.width
    if type(configOrPos) == type( (0,0) ):
      return configOrPos[0] < width / 2
    else:
      return configOrPos.pos[0] < width / 2
  
def halfGrid(grid, red):
  halfway = grid.width / 2
  halfgrid = Grid(grid.width, grid.height, False)
  if red:    xrange = range(halfway)
  else:       xrange = range(halfway, grid.width)
    
  for y in range(grid.height):
    for x in xrange:
      if grid[x][y]: halfgrid[x][y] = True
  
  return halfgrid
    
############################################################################
#                     THE HIDDEN SECRETS OF PACMAN                         #
#                                                                          #
# You shouldn't need to look through the code in this section of the file. #
############################################################################  

COLLISION_TOLERANCE = 0.7 # How close ghosts must be to Pacman to kill

class CaptureRules:
  """
  These game rules manage the control flow of a game, deciding when
  and how the game starts and ends.
  """
  
  def newGame( self, layout, agents, display, length ):
    initState = GameState()
    initState.initialize( layout, len(agents) )
    game = Game(agents, display, self)
    game.state = initState
    game.length = length
    if 'drawCenterLine' in dir(display):
      display.drawCenterLine()
    return game
    
  def process(self, state, game):
    """
    Checks to see whether it is time to end the game.
    """
    if 'moveHistory' in dir(game):
      if len(game.moveHistory) == game.length:
        state.data._win = True
    
    if state.isOver(): 
      game.gameOver = True
      if state.getRedFood().count() == 0:
        print 'The Blue team has captured all of the opponents\' dots.'
      if state.getBlueFood().count() == 0:
        print 'The Red team has captured all of the opponents\' dots.'
      if state.getBlueFood().count() > 0 and state.getRedFood().count() > 0:
        print 'Time is up.'
        if state.data.score == 0: print 'Tie game!'
        else:
          winner = 'Red'
          if state.data.score < 0: winner = 'Blue'
          print 'The %s team wins by %d points.' % (winner, abs(state.data.score))
      
class AgentRules:
  """
  These functions govern how each agent interacts with her environment.
  """

  def getLegalActions( state, agentIndex ):
    """
    Returns a list of legal actions (which are both possible & allowed)
    """
    agentState = state.getAgentState(agentIndex)
    conf = agentState.configuration
    possibleActions = Actions.getPossibleActions( conf, state.data.layout.walls )
    return AgentRules.filterForAllowedActions( agentState, possibleActions)
  getLegalActions = staticmethod( getLegalActions )

  def filterForAllowedActions(agentState, possibleActions):
    conf = agentState.configuration
    if not agentState.isPacman:
      reverse = Actions.reverseDirection( conf.direction )
      if Directions.STOP in possibleActions:
        possibleActions.remove( Directions.STOP )
      if reverse in possibleActions and len( possibleActions ) > 1:
        possibleActions.remove( reverse )
    return possibleActions
  filterForAllowedActions = staticmethod( filterForAllowedActions )
      
  
  def applyAction( state, action, agentIndex ):
    """
    Edits the state to reflect the results of the action.
    """
    legal = AgentRules.getLegalActions( state, agentIndex )
    if action not in legal:
      raise Exception("Illegal action " + str(action))
    
    # Update Configuration
    agentState = state.data.agentStates[agentIndex]    
    speed = 1.0
    if agentState.isPacman: speed = 0.5
    vector = Actions.directionToVector( action, speed )
    oldConfig = agentState.configuration
    agentState.configuration = oldConfig.generateSuccessor( vector )
    
    # Eat
    next = agentState.configuration.getPosition()
    nearest = nearestPoint( next )
    if agentState.isPacman and manhattanDistance( nearest, next ) <= 0.4 :
      AgentRules.consume( nearest, state, state.isOnRedTeam(agentIndex) )
      
    # Change agent type
    if next == nearest:
      agentState.isPacman = [state.isOnRedTeam(agentIndex), state.isRed(agentState.configuration)].count(True) == 1
  applyAction = staticmethod( applyAction )

  def consume( position, state, isRed ):
    x,y = position
    # Eat food
    if state.data.food[x][y]:
      score = -1
      if isRed: score = 1
      state.data.scoreChange += score

      state.data.food = state.data.food.copy()
      state.data.food[x][y] = False
      state.data._foodEaten = position
      if (isRed and state.getBlueFood().count() == 0) or (not isRed and state.getRedFood().count() == 0):
        state.data._win = True
  consume = staticmethod( consume )

  def checkDeath( state, agentIndex):
    agentState = state.data.agentStates[agentIndex]    
    if agentState.isPacman:
      if state.isOnRedTeam(agentIndex):
        otherTeam = state.getBlueTeamIndices()
      else:
        otherTeam = state.getRedTeamIndices()
      
      for index in otherTeam:
        otherAgentState = state.data.agentStates[index]
        if otherAgentState.isPacman: continue
        ghostPosition = otherAgentState.getPosition()
        if ghostPosition == None: continue
        if manhattanDistance( ghostPosition, agentState.getPosition() ) <= COLLISION_TOLERANCE:
          #award points to the other team for killing Pacmen
          score = KILL_POINTS
          if state.isOnRedTeam(agentIndex):
            score = -score
          state.data.scoreChange += score
          agentState.isPacman = False
          agentState.configuration = agentState.start
  checkDeath = staticmethod( checkDeath )

  def placeGhost(state, ghostState):
    ghostState.configuration = ghostState.start
  placeGhost = staticmethod( placeGhost )

#############################
# FRAMEWORK TO START A GAME #
#############################

def default(str):
  return str + ' [Default: %default]'

def readCommand( argv ):
  """
  Processes the command used to run pacman from the command line.
  """
  from optparse import OptionParser
  usageStr = """
  USAGE:      python pacman.py <options>
  EXAMPLES:   (1) python capture.py
                  - starts an interactive game against a random opponent
              (2) python capture.py --numPlayers 4 --player2 KeyboardAgent2
                  - starts a two-player interactive game with w,a,s,d & i,j,k,l keys
              (2) python capture.py --numPlayers 4 --player1 RandomAgent
                  - starts a chaotic random game
  """
  parser = OptionParser(usageStr)
  
  parser.add_option('-1', '--player1', dest='p1', 
                    help=default('the agent TYPE to use for player 1'), 
                    metavar='TYPE', default='KeyboardAgent')
  parser.add_option('-2', '--player2', dest='p2', 
                    help=default('the agent TYPE to use for player 2 (KeyboardAgent2 for i,j,k,l keys)'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  parser.add_option('-3', '--player3', dest='p3', 
                    help=default('the agent TYPE to use for player 3'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  parser.add_option('-4', '--player4', dest='p4', 
                    help=default('the agent TYPE to use for player 4'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  parser.add_option('-5', '--player5', dest='p5', 
                    help=default('the agent TYPE to use for player 5'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  parser.add_option('-6', '--player6', dest='p6', 
                    help=default('the agent TYPE to use for player 6'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  parser.add_option('-l', '--layout', dest='layout', 
                    help=default('the LAYOUT_FILE from which to load the map layout'), 
                    metavar='LAYOUT_FILE', default='mediumCapture')
  parser.add_option('-n', '--nographics', action='store_true', dest='nographics', 
                    help='Display output as text only', default=False)
  #parser.add_option('-G', '--pygame', action='store_true', dest='pygame', 
  #                  help='Display output with Pygame', default=False)
  parser.add_option('-k', '--numPlayers', type='int', dest='numPlayers', 
                    help=default('The maximum number of players'), default=4)
  parser.add_option('-z', '--zoom', type='float', dest='zoom', 
                    help=default('Zoom in the graphics'), default=1)
  parser.add_option('-t', '--time', type='int', dest='time', 
                    help=default('TIME limit of a game in moves'), default=3000, metavar='TIME')
  parser.add_option('-f', '--fast', action='store_true', dest='fast', 
                    help=default('Remove animation to speed up the game'), default=False)
  
  options, otherjunk = parser.parse_args()
  assert len(otherjunk) == 0
  args = dict()
  
  # Choose a pacman agent
  p1 = loadAgent(options.p1, options.nographics)(0)
  p2 = loadAgent(options.p2, options.nographics)(1)
  p3 = loadAgent(options.p3, options.nographics)(2)
  p4 = loadAgent(options.p4, options.nographics)(3)
  p5 = loadAgent(options.p5, options.nographics)(4)
  p6 = loadAgent(options.p6, options.nographics)(5)
  args['agents'] = [p1, p2, p3, p4, p5, p6]

  # Choose a display format
  #if options.pygame:
  #   import pygameDisplay
  #    args['display'] = pygameDisplay.PacmanGraphics()
  if not options.nographics:
    import graphicsDisplay
    graphicsDisplay.FRAME_TIME = 0
    if not options.fast:
      graphicsDisplay.PAUSE_TIME = .1
    args['display'] = graphicsDisplay.FirstPersonPacmanGraphics(options.zoom, capture=True)
  else:
    import textDisplay
    args['display'] = textDisplay.PacmanGraphics()
  
  # Choose a layout
  if options.layout.lower().find('capture') == -1:
    raise Exception( 'You must use a capture layout with capture.py')
  import layout
  args['layout'] = layout.getLayout( options.layout )
  if args['layout'] == None: raise Exception("The layout " + options.layout + " cannot be found")
  
  args['agents'] = args['agents'][:min(args['layout'].getNumGhosts(), options.numPlayers)]
  args['time'] = options.time
  return args

def loadAgent(pacman, nographics):
  import os
  moduleNames = [f for f in os.listdir('.') if f.endswith('gents.py')]
  moduleNames += ['keyboardAgents.py', 'pacmanAgents.py', 'searchAgents.py']
  for modulename in moduleNames:
    try:
      module = __import__(modulename[:-3])
    except ImportError: 
      continue
    if pacman in dir(module):
      if nographics and modulename == 'keyboardAgents.py':
        raise 'Using the keyboard requires graphics (not text display)'
      return getattr(module, pacman)
  raise 'The agent ' + str(pacman) + ' is not specified in any *Agents.py.'

def runGames( layout, agents, display, time, numGames=1 ):
  # Hack for agents writing to the display
  import __main__
  __main__.__dict__['_display'] = display
  
  rules = CaptureRules()
  games = []
  
  for i in range( numGames ):
    if numGames > 1: print 'Starting game %d' % (i + 1)
    game = rules.newGame( layout, agents, display, time )              
    game.run()
    games.append(game)
  
  return games
  
if __name__ == '__main__':
  """
  The main function called when pacman.py is run
  from the command line:

  > python capture.py

  See the usage string for more details.

  > python capture.py --help
  """
  options = readCommand( sys.argv[1:] ) # Get game components based on input
  runGames(**options)
  # import cProfile
  # cProfile.run('runGames( **options )', 'profile')
