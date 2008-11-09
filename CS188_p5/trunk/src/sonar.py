"""
Sonar.py is a vengeful variant of Pacman where Pacman hunts ghosts, but 
cannot see them.  Numbers at the bottom of the display are noisy distance 
readings to each remaining ghost.
         
To play your first game, type 'python pacman.py' from the command line.
The keys are 'a', 's', 'd', and 'w' to move (or arrow keys).  Have fun!
"""
from game import GameStateData
from game import Game
from game import Directions
from game import Actions
from game import Configuration
from util import nearestPoint
from util import manhattanDistance
import sys, util, types, time, random
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
  
  Note that in classic Pacman, Pacman is always agent 0.  
  """
  
  ####################################################
  # Accessor methods: use these to access state data #
  ####################################################
  
  def getLegalActions( self, agentIndex=0 ):
    """
    Returns the legal actions for the agent specified.
    """
    if self.isWin() or self.isLose(): return []
    
    if agentIndex == 0:  # Pacman is moving
      return PacmanRules.getLegalActions( self )
    else:
      return GhostRules.getLegalActions( self, agentIndex )
    
  def generateSuccessor( self, agentIndex, action):
    """
    Returns the successor state after the specified agent takes the action.
    """
    # Check that successors exist
    if self.isWin() or self.isLose(): raise Exception('Can\'t generate a successor of a terminal state.')
    
    # Copy current state
    state = GameState(self)
    
    # Let agent's logic deal with its action's effects on the board
    if agentIndex == 0:  # Pacman is moving
      state.data._eaten = [False for i in range(state.getNumAgents())]
      PacmanRules.applyAction( state, action )
    else:                # A ghost is moving
      GhostRules.applyAction( state, action, agentIndex )
      
    # Time passes
    if agentIndex == 0:
      state.data.scoreChange += -TIME_PENALTY # Penalty for waiting around
    else:
      GhostRules.decrementTimer( state.data.agentStates[agentIndex] )
      
    # Resolve multi-agent effects
    GhostRules.checkDeath( state, agentIndex )
    
    # Book keeping
    state.data._agentMoved = agentIndex
    state.data.score += state.data.scoreChange
    return state
  
  def getLegalPacmanActions( self ):
    return self.getLegalActions( 0 )
  
  def generatePacmanSuccessor( self, action ):
    """
    Generates the successor state after the specified pacman move
    """
    return self.generateSuccessor( 0, action )
  
  def getPacmanState( self ):
    """
    Returns an AgentState object for pacman (in game.py)
    
    state.pos gives the current position
    state.direction gives the travel vector
    """
    return self.data.agentStates[0].copy()
  
  def getPacmanPosition( self ):
    return self.data.agentStates[0].getPosition()
  
  def getGhostStates( self ):
    return self.data.agentStates[1:]

  def getGhostState( self, agentIndex ):
    if agentIndex == 0 or agentIndex >= self.getNumAgents():
      raise "Invalid index passed to getGhostState"
    return self.data.agentStates[agentIndex]
  
  def getGhostPosition( self, agentIndex ):
    if agentIndex == 0:
      raise "Pacman's index passed to getGhostPosition"
    return self.data.agentStates[agentIndex].getPosition()
  
  def getNumAgents( self ):
    return len( self.data.agentStates )
  
  def getScore( self ):
    return self.data.score
  
  def getCapsules(self):
    """
    Returns a list of positions (x,y) of the remaining capsules.
    """
    return self.data.capsules
  
  def getNumFood( self ):
    return self.data.food.count()
  
  def getFood(self):
    """
    Returns a Grid of boolean food indicator variables.
    
    Grids can be accessed via list notation, so to check
    if there is food at (x,y), just call
    
    currentFood = state.getFood()
    if currentFood[x][y] == True: ...
    """
    return self.data.food
    
  def getWalls(self):
    """
    Returns a Grid of boolean wall indicator variables.
    
    Grids can be accessed via list notation, so to check
    if there is food at (x,y), just call
    
    walls = state.getWalls()
    if walls[x][y] == True: ...
    """
    return self.data.layout.walls

  def hasFood(self, x, y):
    return self.data.food[x][y]
  
  def hasWall(self, x, y):
    return self.data.layout.walls[x][y]

  ##############################
  # Additions for Sonar Pacman #
  ##############################

  def getLivingGhosts(self):
    return self.livingGhosts
  
  def setGhostNotLiving(self, index):
    self.livingGhosts[index] = False

  def isLose( self ):
    return False
  
  def isWin( self ):
    return self.livingGhosts.count(True) == 0
  
  #############################################
  #             Helper methods:               #          
  # You shouldn't need to call these directly #
  #############################################
  
  def __init__( self, prevState = None ):
    """ 
    Generates a new state by copying information from its predecessor.
    """
    if prevState != None: 
      self.data = GameStateData(prevState.data)
      self.livingGhosts = prevState.livingGhosts[:]
    else: # Initial state
      self.data = GameStateData()
    
  def deepCopy( self ):
    state = GameState( self )
    state.data = self.data.deepCopy()
    return state
    
  def __eq__( self, other ):
    """
    Allows two states to be compared.
    """
    return self.data == other.data
                                                      
  def __hash__( self ):
    """
    Allows states to be keys of dictionaries.
    """
    return hash( str( self ) )

  def __str__( self ):
    
    return str(self.data)
      
  def initialize( self, layout, numGhostAgents=1000 ):
    """
    Creates an initial game state from a layout array (see layout.py).
    """
    self.data.initialize(layout, numGhostAgents)
    self.livingGhosts = [False] + [True for i in range(numGhostAgents)]
  
############################################################################
#                     THE HIDDEN SECRETS OF PACMAN                         #
#                                                                          #
# You shouldn't need to look through the code in this section of the file. #
############################################################################  
  
COLLISION_TOLERANCE = 0.7 # How close ghosts must be to Pacman to kill
TIME_PENALTY = 1 # Number of points lost each round

class SonarGameRules:
  """
  These game rules manage the control flow of a game, deciding when
  and how the game starts and ends.
  """
  
  def newGame( self, layout, pacmanAgent, numGhosts, display ):
    agents = [pacmanAgent] + [RandomGhost(i+1) for i in range(numGhosts)]
    initState = GameState()
    initState.initialize( layout, numGhosts)
    game = Game(agents, display, self)
    game.state = initState
    return game

  def process(self, state, game):
    """
    Checks to see whether it is time to end the game.
    """
    if state.isWin(): self.win(state, game)
    if state.isLose(): self.lose(state, game)
    
  def win( self, state, game ):
    print "Pacman emerges victorious! Score: %d" % state.data.score
    game.gameOver = True

  def lose( self, state, game ):
    print "Pacman died! Score: %d" % state.data.score
    game.gameOver = True
    
class PacmanRules:
  """
  These functions govern how pacman interacts with his environment under
  the classic game rules.
  """
  PACMAN_SPEED=1

  def getLegalActions( state ):
    """
    Returns a list of possible actions.
    """
    return Actions.getPossibleActions( state.getPacmanState().configuration, state.data.layout.walls )
  getLegalActions = staticmethod( getLegalActions )
  
  def applyAction( state, action ):
    """
    Edits the state to reflect the results of the action.
    """
    legal = PacmanRules.getLegalActions( state )
    if action not in legal:
      raise "Illegal action", action
    
    pacmanState = state.data.agentStates[0]
    
    # Update Configuration
    vector = Actions.directionToVector( action, PacmanRules.PACMAN_SPEED )
    pacmanState.configuration = pacmanState.configuration.generateSuccessor( vector )
    
  applyAction = staticmethod( applyAction )

class GhostRules: 
  """
  These functions dictate how ghosts interact with their environment.
  """     
  GHOST_SPEED=1.0               
  def getLegalActions( state, ghostIndex ):
    conf = state.getGhostState( ghostIndex ).configuration
    return Actions.getPossibleActions( conf, state.data.layout.walls )
  getLegalActions = staticmethod( getLegalActions )
    
  def applyAction( state, action, ghostIndex):
    legal = GhostRules.getLegalActions( state, ghostIndex )
    if action not in legal:
      raise "Illegal ghost action", action
    
    ghostState = state.data.agentStates[ghostIndex]
    speed = GhostRules.GHOST_SPEED
    vector = Actions.directionToVector( action, speed )
    ghostState.configuration = ghostState.configuration.generateSuccessor( vector )
  applyAction = staticmethod( applyAction )
    
  def decrementTimer( ghostState):
    timer = ghostState.scaredTimer
    if timer == 1: 
      ghostState.configuration.pos = nearestPoint( ghostState.configuration.pos )
    ghostState.scaredTimer = max( 0, timer - 1 )
  decrementTimer = staticmethod( decrementTimer )
      
  def checkDeath( state, agentIndex):
    pacmanPosition = state.getPacmanPosition()
    if agentIndex == 0: # Pacman just moved; Anyone can kill him
      for index in range( 1, len( state.data.agentStates ) ):
        ghostState = state.data.agentStates[index]
        ghostPosition = ghostState.configuration.getPosition()
        if GhostRules.canKill( pacmanPosition, ghostPosition ):
          GhostRules.collide( state, ghostState, index )
    else:
      ghostState = state.data.agentStates[agentIndex]
      ghostPosition = ghostState.configuration.getPosition()
      if GhostRules.canKill( pacmanPosition, ghostPosition ):
        GhostRules.collide( state, ghostState, agentIndex )  
  checkDeath = staticmethod( checkDeath )
  
  def collide( state, ghostState, agentIndex):
    state.data.scoreChange += 200
    GhostRules.placeGhost(ghostState, agentIndex)
    # Added for first-person
    state.data._eaten[agentIndex] = True
    state.setGhostNotLiving(agentIndex)
  collide = staticmethod( collide )

  def canKill( pacmanPosition, ghostPosition ):
    return manhattanDistance( ghostPosition, pacmanPosition ) <= COLLISION_TOLERANCE
  canKill = staticmethod( canKill )
  
  def placeGhost(ghostState, agentIndex):
    pos = (agentIndex * 2 - 1, 1)
    direction = Directions.STOP
    ghostState.configuration = Configuration(pos, direction)
  placeGhost = staticmethod( placeGhost )

class RandomGhost:
  def __init__( self, index ):
    self.index = index
    
  def getAction( self, state ):
    return random.choice( state.getLegalActions( self.index ) )
  
  def getDistribution( self, state ):
    actions = state.getLegalActions( self.index )
    prob = 1.0 / len( actions )
    return [( prob, action ) for action in actions]
  
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
  EXAMPLE:    python sonar.py --layout bigHunt
                - starts an interactive game on a big board
  """
  parser = OptionParser(usageStr)
  
  parser.add_option('-n', '--numGames', dest='numGames', type='int',
                    help=default('the number of GAMES to play'), metavar='GAMES', default=1)
  parser.add_option('-l', '--layout', dest='layout', 
                    help=default('the LAYOUT_FILE from which to load the map layout'), 
                    metavar='LAYOUT_FILE', default='smallHunt')
  parser.add_option('-p', '--pacman', dest='pacman', 
                    help=default('the agent TYPE in the pacmanAgents module to use'), 
                    metavar='TYPE', default='SonarKeyboardAgent')
  parser.add_option('-k', '--numghosts', type='int', dest='numGhosts', 
                    help=default('The maximum number of ghosts to use'), default=4)
  parser.add_option('-z', '--zoom', type='float', dest='zoom', 
                    help=default('Zoom the size of the graphics window'), default=1.0)
  parser.add_option('-f', '--fixRandomSeed', action='store_true', dest='fixRandomSeed', 
                    help='Fixes the random seed to always play the same game', default=False)
  parser.add_option('-s', '--showGhosts', action='store_true', dest='showGhosts', 
                    help='Renders the ghosts in the display (cheating)', default=False)
  parser.add_option('-r', '--recordActions', action='store_true', dest='record', 
                    help='Writes game histories to a file (named by the time they were played)', default=False)
  parser.add_option('--replay', dest='gameToReplay', 
                    help='A recorded game file (pickle) to replay', default=None)
  
  options, otherjunk = parser.parse_args()
  if len(otherjunk) != 0: 
    raise Exception('Command line input not understood: ' + otherjunk)
  args = dict()

  # Fix the random seed
  if options.fixRandomSeed: random.seed('sonarPacman')
  
  # Choose a layout
  import layout
  args['layout'] = layout.getLayout( options.layout )
  if args['layout'] == None: raise "The layout " + options.layout + " cannot be found"
  
  # Choose a pacman agent
  pacmanType = loadAgent(options.pacman)

  pacman = pacmanType() # Figure out how to instantiate pacman
  args['pacman'] = pacman
    
  # Choose a ghost agent
  args['numGhosts'] =  options.numGhosts
    
  # Choose a display format
  import graphicsDisplay
  args['display'] = graphicsDisplay.FirstPersonPacmanGraphics(options.zoom, options.showGhosts)
  args['numGames'] = options.numGames
  args['record'] = options.record
  
  return args
  
def loadAgent(pacman):
  import os
  moduleNames = [f for f in os.listdir('.') if f.endswith('gents.py')]
  for modulename in moduleNames:
    try:
      module = __import__(modulename[:-3])
    except ImportError, e: 
      print >>sys.stderr, "Import error:", modulename, e
      continue
    if pacman in dir(module):
      return getattr(module, pacman)
  raise Exception('The agent ' + pacman + ' is not specified in any *Agents.py.')

def runGames( layout, pacman, numGhosts, display, numGames, record ):
  # Hack for agents writing to the display
  import __main__
  __main__.__dict__['_display'] = display
  
  rules = SonarGameRules()
  games = []
  
  for i in range( numGames ):
    game = rules.newGame( layout, pacman, numGhosts, display )              
    game.run()
    games.append(game)
    if record:
      import time, cPickle
      fname = ('recorded-game-%d' % (i + 1)) +  '-'.join([str(t) for t in time.localtime()[1:6]])
      f = file(fname, 'w')
      components = {'layout': layout, 'agents': game.agents, 'actions': game.moveHistory}
      cPickle.dump(components, f)
      f.close()
      
  if numGames > 1:
    scores = [game.state.getScore() for game in games]
    wins = [game.state.isWin() for game in games]
    print 'Average Score:', sum(scores) / float(numGames) 
    print 'Scores:       ', ', '.join([str(score) for score in scores])
    
  return games
  
if __name__ == '__main__':
  """
  The main function called when pacman.py is run
  from the command line:

  > python pacman.py

  See the usage string for more details.

  > python pacman.py --help
  """
  args = readCommand( sys.argv[1:] ) # Get game components based on input
  runGames( **args )        