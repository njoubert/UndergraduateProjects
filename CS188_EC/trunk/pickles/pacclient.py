from capture import *
import cPickle, asyncore, socket
import re, time
import threading
import game

SERVER_CONNECTION_TIMEOUT = 30 # seconds

def debugl(s):
  # print s
  return

def secs():
  return '%0.2f' % (time.time() % 100)

class TeamThread(threading.Thread):

  def __init__(self, agents, pacClient):
    threading.Thread.__init__(self)
    self.gotGameStateEvent = threading.Event()
    self.gameState = None # Slot filled by PacClient
    self.gotTimeout = False
    self.agentIndex = 0
    self.stateNumber = 0
    self.agents = agents
    self.semaphore = threading.Semaphore()
    self.gameOver = False
    self.pacClient = pacClient  # For sending actions
    self.rules = CaptureRules()
    self.setDaemon(True)

  def run(self):
    while not self.gameOver:
      debugl( "Waiting for event.." )
      self.gotGameStateEvent.wait(SERVER_CONNECTION_TIMEOUT)
      if not self.gotGameStateEvent.isSet(): 
        print >>sys.stderr, 'Lost connection with server'
        self.pacClient.finishGame()
        return
      if self.gameOver: return
      
      # Grab the next state
      self.semaphore.acquire()
      localGS = self.gameState
      localIndex = self.agentIndex
      localMoveNumber = self.stateNumber
      self.gotGameStateEvent.clear()
      self.semaphore.release()
      
      # Check for the end of the game
      self.rules.process(localGS, self)
      if self.gameOver: return
      
      action = self.agents[localIndex].getAction(localGS)
      debugl(action)
      self.pacClient.sendAction(localMoveNumber, action) 

class PacClient(asyncore.dispatcher):

  def __init__(self, server, port, agents, display, user, password, gamename):
    asyncore.dispatcher.__init__(self)
    self.create_socket(socket.AF_INET, socket.SOCK_STREAM)
    print >>sys.stderr, "Connecting to server(%s:%d)..." % (server, port)
    self.connect((server, port))
    self.bufSema = threading.Semaphore()
    self.sendbuf = ""
    self.recvbuf = ""
    self.__pickled = False
    self.responses = {200:self.userpass,
                      202:self.notify,
                      203:self.notify,
                      250:self.joingame,
                      400:self.error,
                      401:self.error,
                      402:self.error,
                      404:self.error,
                      350:self.notify,
                      500:self.startgame,
                      501:self.setAgent,
                      502:self.gogogadgetpickle,
                      504:self.ok,
                      505:self.win,
                      506:self.lose,
                      510:self.notify,
                      511:self.badness}
    self.agents = []
    self.agentclasses = []
    self.gameState = None
    self.statesReceived = 0
    self.display = display
    self.user = user
    self.password = password
    self.layout = None
    if gamename != '':
      self.gamename = gamename
      self.anygame = False
    else:
      self.anygame = True
      self.gamename = False
    self.teamThread = None # Created on launch of game
    for i in agents:
      if i:
        self.agentclasses.append(loadAgent(i, not display))
    asyncore.loop()
    
  def handle_connect(self):
    pass
  
  def handle_close(self):
    print "Server closed connection"
    self.close()
    
  def handle_read(self):
    stuff = self.recv(4096)
    self.recvbuf += stuff
    self.parse_read(stuff)
    
  def parse_read(self,stuff):
    debugl("Received "+str(len(stuff))+" chars, now "+str(len(self.recvbuf))+" in buffer")
    if(self.__pickled):
      debugl("Searching for a pickle...")
      rxp = re.compile("(.*?)(\r\n511: EOP \(End of Pickle\))\r\n",re.DOTALL)
      m = rxp.match(self.recvbuf)
      if(m):
        debugl("Found a pickle!")
        self.__pickled = False
        self.handle_pickle(m.group(1))
        self.recvbuf = self.recvbuf[len(m.group(1))+len(m.group(2))+2:]
        self.parse_read("")
      else:
        debugl("No pickle :(")
    else:
      rxp = re.compile("(.*?)\r\n")
      m = rxp.match(self.recvbuf)
      if(m):
        debugl("Found a command!")
        self.recvbuf = self.recvbuf[len(m.group(1))+2:]
        self.parse_line(m.group(1))
        self.parse_read("")
    
  def parse_line(self,line):
    if int(line.split(":")[0]) in self.responses.keys():
      code = int(line.split(":")[0])
      debugl( "Dispatching to "+str(code))
      self.responses[code](code,line)
    else:
      debugl("line \""+line+"\" didn't give me a code I understand :(")
      
  def error(self,code,line):
    print >>sys.stderr, "General error: "
    print >>sys.stderr, line
    
  def notify(self,code,line):
    print >>sys.stderr, "The server requests our attention:"
    print >>sys.stderr, line
    
  def userpass(self,code,line):
    print >>sys.stderr, "Logging in..."
    if not self.user:
      print "Please enter your username:"
      self.user = sys.stdin.readline().strip()
    if not self.password:
      print "Please enter your password:"
      self.password = sys.stdin.readline().strip()
    self.bufline("201:"+self.user+":"+self.password)
    
  def joingame(self, code,line):
    if not self.anygame and not self.gamename:
      print line.split(":")[1:]
      print "If you know the name of the game you are trying to reach, please press 1, now. If you would like to wait for a game, please press 2, now."
      choice = sys.stdin.readline().strip()
      print "Thank you. Your input is important to us."
      if (int(choice) == 1):
        print "Please enter the extension of the game you are trying to reach... now."
        gamename = sys.stdin.readline().strip()
        self.bufline("300:" + self.gamename)
      else:
        print "All our games are currently busy. Please hold. You will be matched with a game as soon as one is available."
        self.bufline("301:ANYGAME")
    elif self.gamename:
      self.bufline("300:" + self.gamename)
    elif self.anygame:
      self.bufline("301:ANYGAME")
  
  def gogogadgetpickle(self,code,line):
    self.__pickled = True
    debugl("Waiting for a pickle...")
    
  def setAgent(self, code, line):
    if self.lastAction:
      a, gs, agentIndex = self.lastAction, self.gameState, self.realAgent
      legals = gs.getLegalActions(agentIndex)
      if a in legals:
        self.display.update(gs.generateSuccessor(agentIndex, a).data)
    debugl("Agent " + line.split(":")[1] + ", this is your mission, should you choose to accept it.")
    self.agentNum = int(int(line.split(":")[1])/2)
    self.realAgent = int(line.split(":")[1])

  def startgame(self,code,line):
    print "Initializing agents..."
    agentIndexBase = int(line.split(":")[1]) 
    self.lastAction = None
    self.isBlue = (agentIndexBase == 1)
    for i in range(len(self.agentclasses)):
      self.agents.append(self.agentclasses[i](agentIndexBase+2*i))
    for agent in self.agents:
      if 'registerTeam' in dir(agent):
        agent.registerTeam(self.agents)
    self.teamThread = TeamThread(self.agents, self)
    self.teamThread.gotTimeout = False
    self.teamThread.start()
    
  def handle_pickle(self, pickle):
    # TODO Add error handling for a bad pickle
    stateNumber, gameState = cPickle.loads(pickle)
    # print secs(), 'Received:', stateNumber
    # Cache and reconstitute states
    if gameState.data.layout:
      self.layout = gameState.data.layout
    else:
      gameState.data.layout = self.layout
    gameState.data.food = game.reconstituteGrid(gameState.data.food)

    if not self.gameState: self.registerInitialState(gameState)
    self.gameState = gameState
    
    debugl(gameState.data._agentMoved)
    self.display.update(gameState.data)
    self.gameState = gameState
    self.pushState(stateNumber, gameState) # Send state to agent
  
  def ok(self,code,line):
    debugl("You don't say what to do with 'ok'")
  
  def badness(self,code,line):
    debugl("We were not expecting a "+line+" here ???")
  
  def win(self,code,line):
    print "You Win!"
    self.finishGame()
    self.close()
    
  def lose(self,code,line):
    print "You Lose :("
    self.finishGame()
    self.close()
    
  def bufline(self, s):
    self.sendbuf += (s + "\r\n")
    
  def writable(self):
    return (len(self.sendbuf) > 0)
  
  def handle_write(self):
    sent = self.send(self.sendbuf)
    self.sendbuf = self.sendbuf[sent:]

  def registerInitialState(self, gameState):
    """
    Registers the initial state
    """
    self.display.initialize(gameState.data, self.isBlue)
    for agent in self.agents[:len(gameState.data.agentStates)/2]:
      if 'registerInitialState' in dir(agent):
        agent.registerInitialState(gameState)

  def pushState(self, stateNumber, gameState):    
    debugl("storing a state...")
    self.teamThread.semaphore.acquire()
    self.teamThread.agentIndex = self.agentNum
    self.teamThread.stateNumber = stateNumber
    self.teamThread.gameState = gameState
    self.teamThread.semaphore.release()
    self.teamThread.gotGameStateEvent.set()
 
  def sendAction(self, stateNumber, action):
    self.lastAction = action
    self.bufline("503:"+str(stateNumber) + ':' + action)
    self.handle_write()
    
  def finishGame(self):      
    self.teamThread.gameOver = True
    self.teamThread.gotGameStateEvent.set()
    self.display.finish()

  def run(self):
    print "PacClient is running!"
    asyncore.loop()

def readCommand( argv ):
  """
  Processes the command used to run pacman from the command line.
  """
  from optparse import OptionParser
  usageStr = """
  USAGE:      python pacclient.py <options>
  EXAMPLES:   (1) python pacclient.py 
                  - starts an interactive game with a random teammate.
  EXAMPLES:   (2) python pacclient.py -2 OffenseAgent 
                  - starts an interactive game with a random teammate.
  """
  parser = OptionParser(usageStr)
  
  parser.add_option('-1', '--player1', dest='p1', 
                    help=default('the agent TYPE to use for player 1 (KeyboardAgent for a,s,d,v keys)'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  parser.add_option('-2', '--player2', dest='p2', 
                    help=default('the agent TYPE to use for player 2 (KeyboardAgent2 for i,j,k,l keys)'), 
                    metavar='TYPE', default='DefensiveReflexAgent')
  parser.add_option('-3', '--player3', dest='p3', 
                    help=default('the agent TYPE to use for player 3 (unused)'), 
                    metavar='TYPE', default='OffensiveReflexAgent')
  #  parser.add_option('-4', '--player4', dest='p4', 
  #                    help=default('the agent TYPE to use for player 4 (unused)'), 
  #                    metavar='TYPE', default=None)
  parser.add_option('-n', '--nographics', action='store_true', dest='nographics', 
                    help='Display output as text only', default=False)
  parser.add_option('-q', '--quiet', action='store_true', dest='quiet', 
                    help='Display minimal output', default=False)
  # parser.add_option('-G', '--pygame', action='store_true', dest='pygame', 
  #                   help='Display output with Pygame graphics (faster)', default=False)
  parser.add_option('-z', '--zoom', type='float', dest='zoom', 
                    help=default('Zoom in the graphics'), default=1)
  parser.add_option('-s', '--server', dest='address',
                    help=default('The SERVER to connect to'), default='discourse.millennium.berkeley.edu')
  parser.add_option('-p', '--port', dest='port',
                    help=default('The PORT to connect to'), default='7226')
  parser.add_option('-U', '--user', dest='user',
                    help=default('Your username'), default='guest')
  parser.add_option('-P', '--pass', dest='password',
                    help=default('Your password'), default='guest')
  parser.add_option('-g', '--gamename', dest='gamename',
                    help=default('The name of the game you wish to contact'), default='')
  
  options, otherjunk = parser.parse_args()
  if len(otherjunk) != 0: raise Exception("Illegal args: " + otherjunk)
  args = dict()
  
  # Choose a pacman agent
  args['agents'] = []
  if options.p1:
    args['agents'].append(options.p1)
  if options.p2:
    args['agents'].append(options.p2)
  if options.p3:
    args['agents'].append(options.p3)
  #  if options.p4:
  #    args['agents'].append(options.p4)

  # Choose a display format
  #if options.pygame:
  # import pygameDisplay
  #  args['display'] = pygameDisplay.PacmanGraphics()
  if options.quiet:
    if True in [a.find('Keyboard') >= 0 for a in args['agents']]:
  	  raise Exception('Using the keyboard requires graphics (not text display)')  	  
    import textDisplay
    args['display'] = textDisplay.NullGraphics()
  elif options.nographics:
    import textDisplay
    args['display'] = textDisplay.PacmanGraphics()
  else:
    import graphicsDisplay
    graphicsDisplay.FRAME_TIME = 0
    args['display'] = graphicsDisplay.PacmanGraphics(options.zoom, True)
  
  args['server'] = options.address
  args['port'] = int(options.port)
  args['user'] = options.user
  args['password'] = options.password
  args['gamename'] = options.gamename
    
  return args
  
if __name__ == '__main__':
  """
  The main function called when pacman.py is run
  from the command line:

  > python pacclient.py

  See the usage string for more details.

  > python pacclient.py --help
  """
  args= readCommand( sys.argv[1:] ) # Get game components based on input=======
  PacClient(**args)
