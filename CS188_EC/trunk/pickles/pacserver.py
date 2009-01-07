
# vim:ts=2:expandtab
from twisted.internet import reactor, protocol
from twisted.protocols import basic
from twisted.python.failure import *
import random
import cPickle
import capture
import game
import layout
import time
import threading
import textDisplay

playerPool = []
passwdDict = {}
gamePool = []
randomGame = "ANYGAME"
gameConfig = {}
statsFile = "stats.dict"
TIMEOUT_AFTER = 2.0
GAME_LENGTH = 3000

def secs():
  return '%0.2f' % (time.time() % 100)

class Game:
  def __init__(self, player1, player2):
    self.p1 = player1
    self.p2 = player2
    self.p1.protocol.sendLine("500:0:War was beginning...")
    self.p2.protocol.sendLine("500:1:War was beginning...")
    self.waiting = False
    self.turn = None
    self.numAgents = 0
    self.agentIndex = 0
    self.rules = None

    # Managing incoming moves
    self.nextStateNumber = 0
    self.nextStateSem = threading.Semaphore()
    
    # Start up the game
    self.rules = capture.CaptureRules()
    self.game = self.rules.newGame(layout.getLayout(gameConfig['map']), range(0,gameConfig['nump']), gameConfig['graphics'], gameConfig['length'])

  def run(self):
    "Runs the game according to the global gameConfig map"
    self.game.display.initialize(self.game.state.data)
    self.game.numMoves = 0
    self.agentIndex = 0    
    self.numAgents = gameConfig['nump']
    self.sendNext(0)
        
  def lineReceived(self, line, who):
    "All received lines should be moves"
    s = line.split(":")
    if int(s[0]) == 503:
      stateNumber = int(s[1])
      action = s[2]
      
      # Check for move spoofing
      whoItShouldBe = self.p2
      if stateNumber % 2 == 0: whoItShouldBe = self.p1
      if whoItShouldBe.protocol != who.protocol:
        raise Exception("Someone is spoofing someone else's moves")
      self.submitMove(stateNumber, action)
    else:
      print >>sys.stderr, "Received a non-move message:", line
      
  def timedOut(self, stateNumber):
    action = random.choice( self.game.state.getLegalActions( self.agentIndex ) )  
    self.submitMove(stateNumber, action, timeout=True)
    
  def submitMove(self, stateNumber, action, timeout=False):
    "Checks for validity of the submitted move and applies it if possible."
    if stateNumber < self.nextStateNumber: return
    self.nextStateSem.acquire()
    if stateNumber > self.nextStateNumber:
      print "Attempted out-of-turn move (this shouldn't happen)"
    elif stateNumber < self.nextStateNumber:
      print "Move that came too late:", stateNumber, self.nextStateNumber, timeout
    else:
      legals = self.game.state.getLegalActions(self.agentIndex)
      if action not in legals:
        print "Illegal move attempted:", action, legals, stateNumber, timeout
        action = random.choice(legals)
      # if timeout:
      #   print "Applying a timeout action for state", stateNumber
      self.applyMove(action)
      self.nextStateNumber += 1
      self.sendNext(self.nextStateNumber)
    self.nextStateSem.release()
  
  def applyMove(self, action):
    "Applies a legal move to the current state"
    self.game.moveHistory.append( (self.agentIndex, action) )
    self.game.state = self.game.state.generateSuccessor( self.agentIndex, action )
    self.game.display.update( self.game.state.data )

    # Allow for game specific conditions (winning, losing, etc.)
    self.rules.process(self.game.state, self.game)
    if self.agentIndex == self.numAgents: self.game.numMoves += 1
    
    # Next agent
    self.agentIndex = ( self.agentIndex + 1 ) % self.numAgents
    self.testEnd()
  
  def sendNext(self, stateNumber):
    # Fetch the next agent
    if self.agentIndex % 2 == 0:
      agent = self.p1
    else:
      agent = self.p2 
    self.game.state.data._agentMoved = (self.agentIndex - 1) % self.numAgents
    self.game.display.update( self.game.state.data )

    observation = self.game.state.makeObservation(self.agentIndex)
    if len(self.game.moveHistory) > 10:
      observation.data.layout = None
    observation.data.food = observation.data.food.packBits()
     
    agent.protocol.sendLine("501:" + str(self.agentIndex) + ":Your Turn!")
    agent.protocol.sendLine("502:BEGIN PICKLE")
    thepickle = cPickle.dumps((stateNumber, observation), 1)
    agent.protocol.sendLine(thepickle)
    agent.protocol.sendLine("\r\n511: EOP (End of Pickle)")
    reactor.wakeUp()
    self.startTimeoutTimer(stateNumber)
    
  def startTimeoutTimer(self, stateNumber):
    if not self.game.gameOver:
      timer = threading.Timer(TIMEOUT_AFTER, self.timedOut, (stateNumber,))
      timer.start()

  def onDisconnect(self, player):
    if player == self.p1:
      self.p2.protocol.sendLine("505:WIN BY DISCONNECT")
    if player == self.p2:
      self.p1.protocol.sendLine("505:WIN BY DISCONNECT")
    self.cleanup()

  def cleanup(self):
    self.game.display.finish()
    # self.game.gameOver = True
    self.p1.game = None
    self.p2.game = None
    self.p1.gamename = None
    self.p2.gamename = None
    if self in gamePool: gamePool.remove(self)
   
  def testEnd(self):
    if self.game.gameOver:
      state = self.game.state
      #print "Game OVER!!!!"
      if state.data.score < 0:
        print self.p1.gamename, 'The Blue team wins.'
        self.p2.protocol.sendLine("505:WIN")
        self.p1.protocol.sendLine("506:LOSE")
      elif state.data.score > 0:
        print self.p1.gamename, 'The Red team wins.'
        self.p1.protocol.sendLine("505:WIN")
        self.p2.protocol.sendLine("506:LOSE") 
      else:
        print self.p1.gamename, 'Tie game: both teams lose'
        self.p1.protocol.sendLine("506:LOSE")
        self.p2.protocol.sendLine("506:LOSE") 
      self.cleanup()

class Player:
  def __init__(self, name, protocol):
    self.name = name
    self.id = random.randint(0,1000)
    self.protocol = protocol
    self.game = None
    self.gamename = None
  def __str__(self):
    print self.name + " " + str(self.id)

def matchGames():
  global gameConfig
  for x in playerPool:
    for y in playerPool:
      if y.gamename is None:
        continue
      if x == y:
        continue
      if x.game or y.game:
        continue
      if y.gamename == x.gamename:
        if random.random() < .5: x,y = y,x
        newgame = Game(x,y)
        gamePool.append(newgame)
        x.game = newgame
        y.game = newgame
        if x.name != 'guest':
          passwdDict[x.name]['gameHistory'].append( (newgame.game, 1) )
        if y.name != 'guest':
          passwdDict[y.name]['gameHistory'].append( (newgame.game, 2) )
        newgame.run()
  for x in playerPool:
    if x.game:
      continue
    x.protocol.sendLine("350:Waiting for opponent")


loginSemaphore = threading.Semaphore(1)

class PacmanProtocol(basic.LineReceiver):
  def lineReceived(self, line):
    if self.player and  self.player.game:
      self.player.game.lineReceived(line, self.player)
    else:
      loginSemaphore.acquire()
      self.processLineReceived(line)
      loginSemaphore.release()
      
  def processLineReceived(self, line):
    things = line.split(":")
    num = int(things[0])
    linetosend = ""
    if num == 201:
      newmap = [x.name for x in playerPool]
      username = things[1]
      password = things[2]
      print 'Login attempt:', username
      #      if things[1] in newmap:
      #        self.sendLine("400:DO NOT WANT")
      #        return
      if username in passwdDict:
        print "Player profile found"
        user = passwdDict[username]
        if user['password'] == password:
          print "Login Complete!"
          linetosend += "202:Login accepted\r\n"
        else:
          self.sendLine("401:Wrong password")
          return
      else:
        passwdDict[username] = {'password': password, 'gameHistory': []}
        print "Created Account"
        linetosend += "203:Account created... Fresh Meat!\r\n"
      self.player = Player(username, self)
      playerPool.append(self.player)
      print [x.name for x in playerPool]
      #TODO -- DO THIS RIGHT
      #self.sendLine("250:Welcome to PacManServ 
      self.sendLine(linetosend + "250:Welcome to Aperture Science Server")
      return
    elif num == 300 or num == 301:
      gamename = things[1]
      if self.player not in playerPool:
        self.sendLine("400:UR DOING IT WRONG (Login!)")
        return
      global gameConfig
      if gameConfig['qualify']: gamename = botGameName(gamename) 
      self.player.gamename = gamename
      matchGames()

  def connectionMade(self):
    print "Got a New Connection"
    self.player = None
    self.sendLine("200:Hello!")

  def connectionLost(self, reason="Done"):
    print "Closing Connection"
    for x in playerPool:
      if x.protocol == self:
        playerPool.remove(x)
        if x.game is not None:
          x.game.onDisconnect(x)
    print [x.name for x in playerPool]

def botGameName(gamename):
  """ 
  If the game name is not a generated one, then generate a name and 
  launch a bot for that name as well.
  """
  global gameConfig
  if gamename.startswith(gameConfig['botGamePrefix']): return gamename
  else:
    name = gameConfig['botGamePrefix'] + str(random.randint(0,100000000))
    cmd = 'python pacclient.py -s localhost -p %d -g %s -q -U botbot -P botbot' % (gameConfig['port'], name)
    os.system(cmd + ' -1 OffensiveReflexAgent -2 DefensiveReflexAgent &')
    return name
    
class PacmanFactory(protocol.ServerFactory):
  protocol = PacmanProtocol

STATS_TIME = 30
def saveStats():
  print "Saving player dictionary."
  passwd = open(statsFile, "w")
  cPickle.dump(passwdDict, passwd)
  passwd.close()
  print "Done."
  thread = threading.Timer(STATS_TIME, saveStats)
  thread.setDaemon(True)
  thread.start()

from capture import default
def readCommand( argv ):
  """
  Processes the command used to run pacserver from the command line.
  """
  from optparse import OptionParser
  usageStr = """
  USAGE:      python pacserver.py <options>
  EXAMPLES:   (1) python pacserver.py --port 7226 --layout crowdedCapture --players 6
                    Starts up a server on port 7226
                    
              (2) python pacserver.py ++pacserver.conf
                    Starts up an array of servers described by pacserver.conf
  """
  parser = OptionParser(usageStr)
  
  parser.add_option('-p', '--port', dest='port', type='int', default=7226,
                    help=default('The port to run on'))
  parser.add_option('-l', '--layout', dest='layout',  default='mediumCapture',
                    help=default('The layout to serve'))
  parser.add_option('-k', '--numagents', dest='nump', type='int', default=4,
                    help=default('Number of agents per game'))
  parser.add_option('-t', '--time', dest='time', type='int', default=3000,
                    help=default('Maximum number of moves per game'))
  parser.add_option('-q', '--qualify', dest='qualify', action='store_true', default=False,
                    help=default('Qualifying server: all games are played against bots'))
  parser.add_option('-b', '--botGamePrefix', dest='botGamePrefix',  default='b@tGaM3',
                    help=default('The prefix of games against bots'))
  parser.add_option('-g', '--graphics', dest='graphics', action='store_true', default=False,
                    help=default('Display graphics for the current game'))
  
  options, rest = parser.parse_args()
  rest = ' '.join(rest).strip()
  print rest 
  if rest.startswith('++'): 
    launchServerArray(rest[2:])
    while(1==1): l=1   
  elif rest != '': raise Exception('The following could not be interpreted: %s' % rest)
  
  global statsFile, gameConfig
  gameConfig['nump'] = options.nump
  gameConfig['map'] = options.layout
  gameConfig['port'] = options.port
  gameConfig['length'] = options.time
  gameConfig['qualify'] = options.qualify
  gameConfig['botGamePrefix'] = options.botGamePrefix
  gameConfig['graphics'] = textDisplay.NullGraphics()
  if options.graphics: 
    import graphicsDisplay
    graphicsDisplay.PAUSE_TIME = 0
    graphicsDisplay.FRAME_TIME = 0
    gameConfig['graphics'] = graphicsDisplay.PacmanGraphics(capture=True)
  statsFile = "stats-port-%d.dict" % gameConfig['port']

import os, time
def launchServerArray(configFile):
  print "game: ", gameConfig 
  for line in open(configFile):
    """
    Reads a line like:
    
    7226: --layout mediumCapture --time 3000 --numagents 4
    """
    port, options = line.split(':')
    cmd = 'python pacserver.py %s --port %s > %s.log &' % (options.strip(), port, port) 
    print cmd
    os.system(cmd)
  "sys.exit(0)"

if __name__ == "__main__":
  readCommand(sys.argv)
  try:
    passwd = open(statsFile)
    passwdDict = cPickle.load(passwd)
    passwd.close()
  except IOError, e:
    passwdDict = {}
    
  saveStats()
  print >>sys.stderr, "Server started on port %d." % gameConfig['port']
  reactor.listenTCP(gameConfig['port'], PacmanFactory())
  reactor.run()
  saveStats()
  