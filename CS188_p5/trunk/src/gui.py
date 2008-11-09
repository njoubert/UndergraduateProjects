from graphicsUtils import *
from ghostbusters import *
import math

COLOR_BLACK = formatColor(0,0,0)    
COLOR_WHITE = formatColor(1,1,1)
COLOR_RED = formatColor(0.9,0,0)
COLOR_ORANGE = formatColor(0.9,.5,0)
COLOR_YELLOW = formatColor(1,1,0)
COLOR_GREEN = formatColor(0,0.9,0)
COLOR_OCEAN_BLUE = formatColor(0,.5,1)
COLOR_GREY = formatColor(0.5,0.5,0.5)

GRID_SIZE = 100
MARGIN = GRID_SIZE
PANEL_WIDTH = 1.6 * GRID_SIZE
INFO_LINES = 3

SHOW_BELIEFS = True
SHOW_GHOSTS = False
USE_TIME = False

DISPLAY_FONT = "Courier New"
#DISPLAY_FONT = "Times"

FONT_SMALL = GRID_SIZE / 8
FONT_LARGE = GRID_SIZE / 6

GHOST_SHAPE = [                
    ( 0,    0.3 ),            
    ( 0.25, 0.75 ),           
    ( 0.5,  0.3 ),
    ( 0.75, 0.75 ),
    ( 0.75, -0.5 ),
    ( 0.5,  -0.75 ),
    (-0.5,  -0.75 ),
    (-0.75, -0.5 ),
    (-0.75, 0.75 ),
    (-0.5,  0.3 ),
    (-0.25, 0.75 )
  ]
GHOST_SIZE = 0.65

GHOST_COLORS = []                       
GHOST_COLORS.append(formatColor(1.0,0.6,0.0)) # Yellow
GHOST_COLORS.append(formatColor(.1,.75,.7)) # Green
GHOST_COLORS.append(formatColor(0,.3,.9)) # Blue
GHOST_COLORS.append(formatColor(.98,.41,.07)) # Orange
GHOST_COLORS.append(formatColor(.4,0.13,0.91)) # Purple
GHOST_COLORS.append(formatColor(.9,0,0)) # Red



def scaleGridSize(scale):
  global GRID_SIZE, MARGIN, FONT_SMALL, FONT_LARGE, PANEL_WIDTH
  GRID_SIZE = GRID_SIZE * scale
  MARGIN = GRID_SIZE
  PANEL_WIDTH = 1.6 * GRID_SIZE
  FONT_SMALL = int(GRID_SIZE) / 8
  FONT_LARGE = int(GRID_SIZE) / 6

class GlobalPosition:
  def __init__(self, topLeft=(0,0), bottomRight=(0,0)):
    self.topLeft = topLeft
    self.bottomRight = bottomRight
    self.status = 'clickable'
  def contains(self, point):
    x,y = point
    if x < self.topLeft[0]: return False
    if x > self.bottomRight[0]: return False
    if y < self.topLeft[1]: return False
    if y > self.bottomRight[1]: return False
    return True

GRID_POSITION = GlobalPosition()
BUTTON_BUST = GlobalPosition()
BUTTON_TIME = GlobalPosition()

class InfoPane:
  def __init__(self, layout):
    self.width = PANEL_WIDTH
    self.xbase = MARGIN*2 + (layout.cols-1)*GRID_SIZE
    self.ybase = MARGIN/2
    self.height = MARGIN + (layout.rows-1)*GRID_SIZE    
    self.info = []
    self.infoText = []
    self.drawPane()

  def to_screen(self, pos, y = None):
    """
      Translates a point relative from the top left of the info pane.
    """
    if y == None: x,y = pos
    else: x = pos
    x = self.xbase + x
    y = self.ybase + y
    return x,y
  
  def drawBustButton(self, status='clickable'):
    size_x = 0.75 * self.width
    size_y = 0.25 * GRID_SIZE
    height = (FONT_SMALL+3) * 14
    screen_x, screen_y = self.to_screen((size_x, height))
    BUTTON_BUST.status = status
    if status == 'clickable':
      color = COLOR_OCEAN_BLUE
    elif status == 'unclickable':
      color = COLOR_GREY
    elif status == 'clicked':
      color = COLOR_RED
    sq = square( (screen_x, screen_y), 
               (size_x, size_y), 
               icolor = color,
               ocolor = COLOR_WHITE,                     
               filled = 1,
               width = 1,
               smooth = 0)
    text( (screen_x, screen_y), COLOR_WHITE, 'BUST', DISPLAY_FONT, FONT_LARGE, "bold", anchor = 'c')
    BUTTON_BUST.topLeft = (screen_x - size_x, screen_y - size_y)
    BUTTON_BUST.bottomRight = (screen_x + size_x, screen_y + size_y)

  def drawTimeButton(self, status='clickable'):
    size_x = 0.75 * self.width
    size_y = 0.25 * GRID_SIZE
    height = ((FONT_SMALL+3) * 14) + (2* size_y) + 10
    screen_x, screen_y = self.to_screen((size_x, height))
    BUTTON_TIME.status = status
    if status == 'clickable':
      color = COLOR_OCEAN_BLUE
    elif status == 'unclickable':
      color = COLOR_GREY
    elif status == 'clicked':
      color = COLOR_RED
    sq = square( (screen_x, screen_y), 
               (size_x, size_y), 
               icolor = color,
               ocolor = COLOR_WHITE,                     
               filled = 1,
               width = 1,
               smooth = 0)
    text( (screen_x, screen_y), COLOR_WHITE, 'TIME+1', DISPLAY_FONT, FONT_LARGE, "bold", anchor = 'c')
    BUTTON_TIME.topLeft = (screen_x - size_x, screen_y - size_y)
    BUTTON_TIME.bottomRight = (screen_x + size_x, screen_y + size_y)

  def drawPane(self):
    self.drawBustButton()
    if USE_TIME: status = 'clickable'
    else: status = 'unclickable'
    self.drawTimeButton(status)
    
    color = COLOR_WHITE
    size = FONT_SMALL
    lineHeight = size+3
    self.ghostText  = text( self.to_screen(0, 0), 
                           color, 'GHOSTS REMAINING:    0', DISPLAY_FONT, size, 'bold')
    self.bustText  = text( self.to_screen(0, lineHeight), 
                           color, 'BUSTS REMAINING:    0', DISPLAY_FONT, size, 'bold')
    self.scoreText = text( self.to_screen(0, 2*lineHeight), 
                           color, 'SCORE:              0', DISPLAY_FONT, size, 'bold')

    linepos = 5*lineHeight
    screenpos = self.to_screen((0, linepos))
    text( screenpos, color, 'MESSAGES:', DISPLAY_FONT, size, 'bold')

    count = INFO_LINES
    for line in range(INFO_LINES):
      height = linepos + (count * (size+3))
      pos = self.to_screen(0,height)
      self.infoText.append(text( pos, COLOR_WHITE, '', DISPLAY_FONT, size))
      self.info.append('')
      count -= 1
        
  def updateScore(self, state, busts):
    changeText(self.ghostText, "GHOSTS REMAINING: % 4d" % state.getNumGhosts())
    changeText(self.bustText, "BUSTS REMAINING: % 4d" % busts)
    changeText(self.scoreText, "SCORE: % 14d" % state.score)

  def updateMessages(self, msg):
    self.info.append(msg)
    self.info = self.info[-1*INFO_LINES:]
    count = 0
    for line in self.info:
      changeText(self.infoText[count], line)
      count += 1
      
class GhostbusterGraphics:
  def __init__(self):
    pass
  
  def initialize(self, state):
    """
    initialize the gui
    state is a ghostbusters.GameState
    """
    self.layout = state.layout
    self.setup()
    self.infoPane = InfoPane(self.layout)
    self.infoPane.updateScore(state, state.getNumGhosts())
    self.showState(state)
      
  def setup(self):
    width = self.layout.cols - 1
    height = self.layout.rows - 1
    screen_width = MARGIN + (width*GRID_SIZE) + MARGIN + PANEL_WIDTH + MARGIN
    screen_height = (2*MARGIN) + (height*GRID_SIZE)
    GRID_POSITION.topLeft = (MARGIN/2, MARGIN/2)
    GRID_POSITION.bottomRight = ((1.5*MARGIN) + (width*GRID_SIZE), (1.5*MARGIN) + (height*GRID_SIZE))
    begin_graphics(screen_width,    
                   screen_height,
                   COLOR_BLACK,
                   title='ghostbusters')
                   
    # trying to create a button
    #self.bustButton = createButton('bust', self.startBusting(), pos=(0,0))

  def showGhosts(self, ghosts):
    """
    currently not used
    show the position of each ghost
    """
    for ghost in ghosts:
      row, col = ghost
      screen_x, screen_y = self.to_screen((row, col))
      sq = square( (screen_x, screen_y), 
               (0.25*GRID_SIZE, 0.25*GRID_SIZE), 
               icolor = COLOR_BLACK,
               ocolor = COLOR_BLACK,                     
               filled = 1,
               width = 1,
               smooth = 1)
            
  
  def showState(self, state, revealGhosts=False):
    """
    draws the board to the gui
    default is the board with no special info
    """

    displayBoard = self.layout.board
    if revealGhosts:
      for ghost in state.ghosts:
        row, col = ghost.pos
        displayBoard[row][col] = 'B'
    
    square_color = COLOR_OCEAN_BLUE
    sensor_color = square_color
    for row in range(self.layout.rows):
      for col in range(self.layout.cols):
        screen_x, screen_y = self.to_screen((row, col))

        # start with a blank board
        sq = square( (screen_x, screen_y), 
                       (0.5*GRID_SIZE, 0.5*GRID_SIZE), 
                       icolor = COLOR_OCEAN_BLUE,
                       ocolor = COLOR_WHITE,                     
                       filled = 1,
                       width = 1,
                       smooth = 0)

        value = displayBoard[row][col]
        if value == ' ':
          continue

  def showMove(self, observation):
    pos, result = observation
    # get screen position
    screen_x, screen_y = self.to_screen(pos)
    # get a sensor display color
    if result == Readings.RED:
      sensor_color = COLOR_RED
    elif result == Readings.ORANGE:
      sensor_color = COLOR_ORANGE
    elif result == Readings.YELLOW:
      sensor_color = COLOR_YELLOW
    elif result == Readings.GREEN:
      sensor_color = COLOR_GREEN
    # show sensor reading
    squares( (screen_x, screen_y), 
             (0.5*GRID_SIZE, 0.5*GRID_SIZE), 
             icolor = COLOR_OCEAN_BLUE,
             ocolor = sensor_color,                     
             filled = 1,
             width = 1,
             smooth = 0,
             count = GRID_SIZE / 10)
                 
    desc = 'sensor at ' + str(pos) + ' [' + str(result) + ']'    
    self.infoPane.updateMessages(desc)

  def endGame(self, bustResults):
    """
    show the bust results
    """
    for bustResult in bustResults:
      pos, result = bustResult
      screen_x, screen_y = self.to_screen(pos)
      if result == 'hit':
        sq = square( (screen_x, screen_y), 
                       (0.5*GRID_SIZE, 0.5*GRID_SIZE), 
                       icolor = COLOR_RED,
                       ocolor = COLOR_WHITE,
                       filled = 1,
                       width = 1,
                       smooth = 0)
        text( (screen_x, screen_y), COLOR_WHITE, 'HIT!', DISPLAY_FONT, int(GRID_SIZE / 6), 'bold', anchor = 'c')
      elif result == 'miss':
        sq = square( (screen_x, screen_y), 
                       (0.5*GRID_SIZE, 0.5*GRID_SIZE), 
                       icolor = COLOR_GREEN,
                       ocolor = COLOR_WHITE,
                       filled = 1,
                       width = 1,
                       smooth = 0)
        text( (screen_x, screen_y), COLOR_WHITE, 'MISS', DISPLAY_FONT, int(GRID_SIZE / 6), 'bold', anchor = 'c')
 
    msg = 'game over [click to close]'
    self.infoPane.updateMessages(msg)
    wait_for_click()
    
  def showBeliefs(self, locations, beliefs, ghosts):
    nGhostDrawn = 0;
    for pos in locations:
      if beliefs.has_key(pos):
        belief = beliefs[pos]
      else:
        belief = 0
      row, col = pos
      (screen_x, screen_y) = self.to_screen((row, col))
      
      # heat map
      scaled_belief = belief
      scaled_belief = min(scaled_belief, 1.0)
      scaled_belief = max(scaled_belief, 1e-20)
      
      scaled_belief = 1.0 / (1.0 - math.log(scaled_belief))
      #scaled_belief = scaled_belief ** 0.5
      #heat_color = formatColor(0,0,scaled_belief)
      heat_color = formatColor(scaled_belief/1.1,0, (1-scaled_belief)**2)
      
      if SHOW_BELIEFS:
        sq = square( (screen_x, screen_y), 
                     (0.40*GRID_SIZE, 0.40*GRID_SIZE), 
                     icolor = heat_color,
                     ocolor = heat_color, 
                     filled = 1,
                     width = 1,
                     smooth = 0)

      if SHOW_GHOSTS and pos in ghosts:
        screen_x, screen_y = self.to_screen(pos)
        #sq = square( (screen_x, screen_y), 
        #         (0.25*GRID_SIZE, 0.25*GRID_SIZE), 
        #         icolor = COLOR_BLACK,
        #         ocolor = COLOR_BLACK,                     
        #         filled = 1,
        #         width = 1,
        #         smooth = 1)
        coords = []          
        for (x, y) in GHOST_SHAPE:
          coords.append((x*0.7*GRID_SIZE*GHOST_SIZE + screen_x, y*0.7*GRID_SIZE*GHOST_SIZE + screen_y - GRID_SIZE*0.0 ))
  
        colour = GHOST_COLORS[nGhostDrawn]
        nGhostDrawn += 1
        body = polygon(coords, colour, colour, filled = 1)
        WHITE = formatColor(1.0, 1.0, 1.0)
        BLACK = formatColor(0.0, 0.0, 0.0)
        dx = 0
        dy = 0

        leftEye = circle((screen_x+0.7*GRID_SIZE*GHOST_SIZE*(-0.24+dx/1.5), screen_y-0.7*GRID_SIZE*GHOST_SIZE*(0.31-dy/1.5)), 0.7*GRID_SIZE*GHOST_SIZE*0.1, WHITE, WHITE)
        rightEye = circle((screen_x+0.7*GRID_SIZE*GHOST_SIZE*(0.24+dx/1.5), screen_y-0.7*GRID_SIZE*GHOST_SIZE*(0.31-dy/1.5)), 0.7*GRID_SIZE*GHOST_SIZE*0.1, WHITE, WHITE)
        leftPupil = circle((screen_x+0.7*GRID_SIZE*GHOST_SIZE*(-0.24+dx), screen_y-0.7*GRID_SIZE*GHOST_SIZE*(0.31-dy)), 0.7*GRID_SIZE*GHOST_SIZE*0.04, BLACK, BLACK)
        rightPupil = circle((screen_x+0.7*GRID_SIZE*GHOST_SIZE*(0.24+dx), screen_y-0.7*GRID_SIZE*GHOST_SIZE*(0.31-dy)), 0.7*GRID_SIZE*GHOST_SIZE*0.04, BLACK, BLACK)


      # belief values
      if SHOW_BELIEFS:
        belief_text = '%0.2f' % belief
        if belief < 0.01:
          belief_text = '<0.01'
        text( (screen_x, screen_y), COLOR_WHITE, belief_text, DISPLAY_FONT, FONT_LARGE, "bold", anchor = 'c')
    
  def to_screen(self, point):
    ( row, col ) = point
    x = col*GRID_SIZE + MARGIN  
    y = row*GRID_SIZE + MARGIN  
    return ( x, y )
          
  def win(self, state):
    msg = 'YOU WIN! [click to close]'
    self.infoPane.updateMessages(msg)
    wait_for_click()
    
  def pauseGUI(self):
    wait_for_click()
  

def square(pos, size, icolor, ocolor, filled, width, smooth=0):
  x, y = pos
  dx, dy = size
  return polygon([(x - dx, y - dy), (x - dx, y + dy), (x + dx, y + dy), (x + dx, y - dy)], icolor, ocolor, filled, smooth)

def squares(pos, size, icolor, ocolor, filled, width, smooth=0, count=1):
  for i in range(1, int(count)+1):
    square(pos,(size[0]-i,size[1]-i),icolor,ocolor,filled,width)
    
def getEvent(point):
  (x, y) = point
  if GRID_POSITION.contains(point):
    row = int ((y - MARGIN + GRID_SIZE * 0.5) / GRID_SIZE)
    col = int ((x - MARGIN + GRID_SIZE * 0.5) / GRID_SIZE)
    return 'grid', (row, col)
  elif BUTTON_BUST.contains(point):
    if BUTTON_BUST.status != 'clickable':
      return None, None
    return 'bust-button', None
  elif BUTTON_TIME.contains(point):
    if BUTTON_TIME.status != 'clickable':
      return None, None
    return 'time-button', None
  return None, None
