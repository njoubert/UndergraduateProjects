import sys
import math
import random
import string
import time
import types
import Tkinter

_Windows = sys.platform == 'win32'	# True if on Win95/98/NT

global _root_window
_root_window	= None			# The root window for graphics output
_canvas		= None			# The canvas which holds graphics
_canvas_xs	= None			# Size of canvas object
_canvas_ys	= None
_canvas_x	= None			# Current position on canvas
_canvas_y	= None
_canvas_col	= None			# Current colour (set to black below)
_canvas_tsize	= 12
_canvas_tserifs	= 0

def formatColor(r,g,b):
  return '#%02x%02x%02x' % (int(r*255), int(g*255), int(b*255))

def colorToVector(color):
  return map(lambda x: int(x, 16)/256.0, [color[1:3], color[3:5], color[5:7]])

if _Windows:
    _canvas_tfonts = ['times new roman', 'lucida console']
else:
    _canvas_tfonts = ['times', 'lucidasans-24']
    pass # XXX need defaults here

def sleep( secs ):
    global _root_window
    if _root_window == None:
        time.sleep( secs )
    else:
        _root_window.update_idletasks()
        _root_window.after( int( 1000*secs ), _root_window.quit )
        _root_window.mainloop()

def begin_graphics( width=640, height=480, color=formatColor(0,0,0), title=None ):

    global _root_window, _canvas, _canvas_x, _canvas_y, _canvas_xs, _canvas_ys

    # Check for duplicate call
    if _root_window is not None:
        # Lose the window.
        _root_window.destroy()
	# raise ExAlreadyStarted('begin_graphics() called twice')

    # Save the canvas size parameters
    _canvas_xs, _canvas_ys = width-1, height-1
    _canvas_x, _canvas_y  = 0, _canvas_ys

    # Create the root window
    _root_window = Tkinter.Tk()
    _root_window.protocol( 'WM_DELETE_WINDOW', _destroy_window )
    _root_window.title( title or 'Graphics Window' )
    _root_window.resizable( 0, 0 )

    # Create the canvas object
    try:
      _canvas = Tkinter.Canvas( _root_window, width=width, height=height,background=color)
      _canvas.pack()
      _canvas.update()
    except:
      _root_window = None
      raise

    # Bind to key-down and key-up events    
    _root_window.bind( "<KeyPress>", _keypress )
    _root_window.bind( "<KeyRelease>", _keyrelease )
    _root_window.bind( "<FocusIn>", _clear_keys )
    _root_window.bind( "<FocusOut>", _clear_keys )
    _root_window.bind( "<Button-1>", _leftclick )
    _root_window.bind( "<Button-2>", _rightclick )
    _root_window.bind( "<Button-3>", _rightclick )
    _root_window.bind( "<Control-Button-1>", _ctrl_leftclick)
    _clear_keys()

_leftclick_loc = None
_rightclick_loc = None
_ctrl_leftclick_loc = None

def _leftclick(event):
  global _leftclick_loc
  _leftclick_loc = (event.x, event.y)

def _rightclick(event):
  global _rightclick_loc
  _rightclick_loc = (event.x, event.y)

def _ctrl_leftclick(event):
  global _ctrl_leftclick_loc
  _ctrl_leftclick_loc = (event.x, event.y)

def wait_for_click():
  while True:
    global _leftclick_loc
    global _rightclick_loc
    global _ctrl_leftclick_loc
    if _leftclick_loc != None:
      val = _leftclick_loc
      _leftclick_loc = None
      return val, 'left'
    if _rightclick_loc != None:
      val = _rightclick_loc
      _rightclick_loc = None
      return val, 'right'
    if _ctrl_leftclick_loc != None:
      val = _ctrl_leftclick_loc
      _ctrl_leftclick_loc = None
      return val, 'ctrl_left'
    sleep(0.05)

def _destroy_window( event=None ):
    sys.exit(0)
#    global _root_window
#    _root_window.destroy()
#    _root_window = None
    #print "DESTROY"

def end_graphics():
    global _root_window, _canvas, _mouse_enabled
    try:
      sleep( 1 )
      _root_window.destroy()
    finally:
      _root_window   = None
      _canvas        = None
      _mouse_enabled = 0
      _clear_keys()

def clear_screen( background=None ):

    global _canvas_x, _canvas_y

    # Remove all drawn items
    _canvas.delete( 'all' )

    # Reset default coordinates
    # XXX Should any other state be reset?
    _canvas_x, _canvas_y = 0, _canvas_ys


def polygon( coords, icolor, ocolor, filled = 1, smooth = 1, width = 1, behind = False ):
  if not ocolor: ocolor = icolor
  c = []
  for coord in coords:
    c.append(coord[0])
    c.append(coord[1])
  poly = _canvas.create_polygon( c, outline=ocolor, fill=icolor, smooth = smooth, width=width )
  if  behind:    _canvas.tag_lower(poly, 1)
  return poly

def square( pos, r, color, filled=1, behind=False):
  x,y = pos
  coords = [(x-r, y-r),(x+r, y-r),(x+r, y+r),(x-r, y+r)]
  return polygon(coords, color, color, filled, 0, 1, behind=behind)

def circle( pos, r, outlineColor, fillColor=0, endpoints=None, style='pieslice', width=2):
    x,y = pos
    x0, x1 = x - r - 1, x + r
    y0, y1 = y - r - 1, y + r
    if endpoints == None:
      e = [0,359]
    else:
      e = list(endpoints)
    while e[0] > e[1]: e[1] = e[1] + 360

    return _canvas.create_arc( x0, y0, x1, y1, 
                               outline=outlineColor, fill=fillColor, 
                               extent=e[1]-e[0], start=e[0], 
                               style=style, width=width)

                                              
def refresh():
      _canvas.update_idletasks()


def moveCircle(id, pos, r, endpoints=None):
    global _canvas_x, _canvas_y
    
    x,y = pos
#    x0, x1 = x - r, x + r + 1
#    y0, y1 = y - r, y + r + 1
    x0, x1 = x - r - 1, x + r
    y0, y1 = y - r - 1, y + r
    if endpoints == None:
      e = [0,359]
    else:
      e = list(endpoints)
    while e[0] > e[1]: e[1] = e[1] + 360

    edit(id, ('start', e[0]), ('extent', e[1] - e[0]))
    move_to(id, x0, y0)

def edit(id, *args ):
    _canvas.itemconfigure(id, **dict(args))
    
def text( pos, color, contents, font='Helvetica', size=12, style='normal', anchor = 'nw'):
    global _canvas_x, _canvas_y
    x,y = pos
    font = (font, str(size), style)
    return _canvas.create_text(x,y,anchor=anchor, fill=color, text=contents, font=font)

def changeText(id, newText, font=None, size=12, style='normal'):
  _canvas.itemconfigure(id, text=newText)
  if font != None:
    _canvas.itemconfigure(id, font=(font, '-%d'%size, style))

def changeColor(id, newColor):
  _canvas.itemconfigure(id, fill=newColor)


def line(here, there, color =  formatColor(0,0,0), width=2):
  x0,y0 = here[0], here[1]
  x1,y1 = there[0], there[1]
  return _canvas.create_line(x0, y0, x1, y1, fill=color, width=width)

##############################################################################
### Keypress handling ########################################################
##############################################################################

# We bind to key-down and key-up events.

_keysdown = {}
_keyswaiting = {}
# This holds an unprocessed key release.  We delay key releases by up to
# one call to keys_pressed() to get round a problem with auto repeat.
_got_release = None

def _keypress( event ):
    global _got_release
    #remap_arrows(event)
    _keysdown[event.keysym] = 1
    _keyswaiting[event.keysym] = 1
#    print event.char, event.keycode
    _got_release = None

def _keyrelease( event ):
    global _got_release
    #remap_arrows(event)
    try:
      del _keysdown[event.keysym]
    except:
      pass
    _got_release = 1
    
def remap_arrows(event):
    # TURN ARROW PRESSES INTO LETTERS (SHOULD BE IN KEYBOARD AGENT)
    if event.char in ['a', 's', 'd', 'w']:
      return
    if event.keycode in [37, 101]: # LEFT ARROW (win / x)
      event.char = 'a'
    if event.keycode in [38, 99]: # UP ARROW
      event.char = 'w'
    if event.keycode in [39, 102]: # RIGHT ARROW
      event.char = 'd'
    if event.keycode in [40, 104]: # DOWN ARROW
      event.char = 's'

def _clear_keys( event=None ):
    global _keysdown, _got_release, _keyswaiting
    _keysdown = {}
    _keyswaiting = {}
    _got_release = None

def keys_pressed( d_o_e=Tkinter.tkinter.dooneevent, 
                 d_w=Tkinter.tkinter.DONT_WAIT ):
    d_o_e( d_w )
    if _got_release:
      d_o_e( d_w )
    return _keysdown.keys()
  
def keys_waiting():
  global _keyswaiting
  keys = _keyswaiting.keys()
  _keyswaiting = {}
  return keys

# Block for a list of keys...

def wait_for_keys():
    keys = []
    while keys==[]:
        keys = keys_pressed()
    thekeys = keys
    while keys != []:
        keys = keys_pressed()
    return thekeys


def remove_from_screen( x, 
                       d_o_e=Tkinter.tkinter.dooneevent, 
                       d_w=Tkinter.tkinter.DONT_WAIT ):
    _canvas.delete( x)
    d_o_e( d_w )

def _adjust_coords( coord_list, x, y ):
    for i in range( 0, len( coord_list ), 2 ):
        coord_list[i]   = coord_list[i] + x
        coord_list[i+1] = coord_list[i+1] + y
    return coord_list

def move_to( object, x, y=None, 
            d_o_e=Tkinter.tkinter.dooneevent, 
            d_w=Tkinter.tkinter.DONT_WAIT ):
    if y is None:
        try: x, y = x
        except: raise ExBadParameters( 'incomprehensible coordinates' )

    horiz = True
    newCoords = []
    current_x, current_y = _canvas.coords(object)[0:2] # first point
    for coord in  _canvas.coords(object):
      if horiz:  
        inc = x - current_x
      else:      
        inc = y - current_y
      horiz = not horiz
      
      newCoords.append(coord + inc)
    
    _canvas.coords( object, *newCoords)
    d_o_e( d_w )

def move_by( object, x, y=None, 
            d_o_e=Tkinter.tkinter.dooneevent, 
            d_w=Tkinter.tkinter.DONT_WAIT ):
    if y is None:
        try: x, y = x
        except: raise Exception, 'incomprehensible coordinates' 
    
    horiz = True
    newCoords = []
    for coord in  _canvas.coords(object):
      if horiz:  
        inc = x
      else:      
        inc = y
      horiz = not horiz
      
      newCoords.append(coord + inc)
      
    _canvas.coords( object, *newCoords)
    d_o_e( d_w )

def createButton(text, f, pos):
  global _root_window
  newButton = Tkinter.Button(_root_window, text, command=f)
  newButton.grid(row=pos[0], column=pos[1], padx=10)
  return newButton

if __name__ == '__main__':
  #global _root_window

  begin_graphics()
  clear_screen()
  
  alpha_minus = Tkinter.Button(_root_window, text="-",command=(lambda: self.incrementAlpha(-0.1)))
  alpha_minus.grid(row=0, column=7, padx=15)
  
  sleep(2)
