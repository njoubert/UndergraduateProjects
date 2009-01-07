from game import Agent
from game import Directions
from util import nearestPoint
from util import manhattanDistance
from game import Actions
import math
import random
import search

class CaptureAgent(Agent):
  def __init__( self, index ):
    self.index = index
    self.lastAction = Directions.STOP
    
  def registerInitialState(self, state):
    self.west = state.isOnWestTeam(self.index)

  def getAction(self, state):
    myState = state.getAgentState(self.index)
    myPos = myState.getPosition()
    if myPos != nearestPoint(myPos): 
      return self.lastAction

    action = self.chooseAction(state, myState)
    if action == Directions.STOP: print 'stop'
    self.lastAction = action
    return action
  
  def getFood(self, gameState):
    if self.west:
      return gameState.getEastFood()
    else:
      return gameState.getWestFood()

  def getOpponents(self, gameState):
    if self.west:
      return gameState.getEastTeamIndices()
    else:
      return gameState.getWestTeamIndices()
    
  def getTeam(self, gameState):
    if self.west:
      return gameState.getWestTeamIndices()
    else:
      return gameState.getEastTeamIndices()

  def getScore(self, gameState):
    if self.west:
      return gameState.getScore()
    else:
      return gameState.getScore() * -1
  
  def chooseAction(self, gameState, myState):
    raise Exception('Override')

class OffenseAgent( CaptureAgent ):
    
  def chooseAction(self, gameState, myState):
    myPos = myState.getPosition()

    legal = gameState.getLegalActions(self.index)
    if 'plan' in dir(self) and len(self.plan) > 0:
      pos, action = self.plan.pop()
      # Check that the plan still makes sense
      if myPos == pos and action in legal: return action
      
    # Time to replan
    self.plan = self.getPathToNearestDot(gameState, myState)
    self.plan.reverse()
    return self.plan.pop()[1]
  
  def getPathToNearestDot(self, gameState, myState):
    """
    A path takes the form [(whereYouAre, whatYouDo), ...]
    """
    food = self.getFood(gameState)
    problem = AnyDotOnSideWillDo(nearestPoint(myState.getPosition()), food, gameState.getWalls())
    states, actions, cost = search.breadthFirstSearch(problem)
    return zip(states, actions)
  
class CarefulOffenseAgent ( CaptureAgent ):  
  def registerInitialState(self, state):
    self.west = state.isOnWestTeam(self.index)
    self.plan = self.getPathToNearestDot(state, state.getAgentState(self.index))

  def chooseAction(self, gameState, myState):
    myPos = myState.getPosition()

    legal = gameState.getLegalActions(self.index)
    if len(self.plan) > 0:
      pos, action = self.plan.pop()
      # Check that the plan still makes sense
      if action in legal: return action
      
    if myState.isPacman:
      return self.chooseOffenseAction(gameState, myState)
    else:
      self.plan = self.getPathToNearestDot(gameState, myState)
      self.plan.reverse()
      return self.plan.pop()[1]

  def chooseOffenseAction(self, gameState, myState):
    legal = gameState.getLegalActions(self.index)
    if Directions.STOP in legal: legal.remove(Directions.STOP)
    successors = [gameState.generateSuccessor(self.index, action) for action in legal]
    options = [ (self.evalFn(successor, action), action) for successor, action in zip(successors, legal)]
    bestScore = max(options)[0]
    return random.choice([action for score, action in options if score == bestScore])
  
  def evalFn(self, successor, action):
    if self.getFood(successor).count() == 0: return 1000
    myState = successor.getAgentState(self.index)
    pos = myState.getPosition() 
    if pos != nearestPoint(pos): return self.evalFn(successor.generateSuccessor(self.index, action), action)
    
    food = self.getFood(successor)
    problem = AnyDotOnSideWillDo(nearestPoint(pos), food, successor.getWalls())
    distanceToFood = search.breadthFirstSearch(problem)[2]
    
    distanceToOpponent = 100
    for enemyIndex in self.getOpponents(successor):
      opp = successor.getAgentState(enemyIndex)
      if not opp.isPacman:
        distanceToOpponent = min(distanceToOpponent, manhattanDistance(myState.getPosition(), opp.getPosition()))
      
    return -0.3 * distanceToFood + self.getScore(successor) + 2 * math.log(distanceToOpponent)

  def getPathToNearestDot(self, gameState, myState):
    """
    A path takes the form [(whereYouAre, whatYouDo), ...]
    """
    food = self.getFood(gameState)
    problem = AnyDotWithGameStates( gameState, food, self.index)
    states, actions, cost = search.breadthFirstSearch(problem)
    return zip(states, actions)
  
  def getPathToApproachPoint(self, gameState, myState):
    """
    A path takes the form [(whereYouAre, whatYouDo), ...]
    """
    problem = OtherSideProblem(gameState, myState.getPosition(), self.west)
    states, actions, cost = search.breadthFirstSearch(problem)
    return zip(states, actions)  

class AnyDotWithGameStates(search.SearchProblem):
  def __init__(self, startState, targets, agentIndex):
    self.startState = startState
    self.targets = targets
    self.agentIndex = agentIndex
    self._expanded = 0
    
  def getStartState(self):
    return self.startState

  def isGoalState(self, state):
    pos = state.getAgentState(self.agentIndex).getPosition()
    x,y = nearestPoint(pos)
    if pos != (x,y): return False
    if self.targets[x][y]:
      pass
    return self.targets[x][y]
   
  def getSuccessors(self, state):
    self._expanded += 1
    legal = state.getLegalActions(self.agentIndex)
    return [(state.generateSuccessor(self.agentIndex, move), move, 1) for move in legal]

class AnyDotOnSideWillDo(search.SearchProblem):
  def __init__(self, pos, food, walls):
    """
    This search problem should be initialized with
      - pos: the position of Pacman
      - food: the Grid of food left on the board
      - walls: the Grid of walls
      
    You are welcome to change these inputs if you want, though it is not recommended.
    """
    self.walls = walls
    self.startState = pos
    self.food = food

  def getStartState(self):
    return self.startState

  def isGoalState(self, state):
    x,y = state
    return self.food[x][y]
   
  def getSuccessors(self, state):
    successors = []
    for action in [Directions.NORTH, Directions.SOUTH, Directions.EAST, Directions.WEST]:
      x,y = state
      dx, dy = Actions.directionToVector(action)
      nextx, nexty = int(x + dx), int(y + dy)
      if not self.walls[nextx][nexty]:
        nextState = (nextx, nexty)
        successors.append( ( nextState, action, 1) )
        
    return successors

class OtherSideProblem(search.SearchProblem):
  def __init__(self, gameState, startPos, west):
    self.gameState = gameState
    self.start = startPos
    self.west = west
    
  def getStartState(self):
    return self.start
  
  def isGoalState(self, state):
    if self.west:
      return not self.gameState.isWest(state)
    else:
      return self.gameState.isWest(state)

  def getSuccessors(self, state):
    successors = []
    for action in [Directions.NORTH, Directions.SOUTH, Directions.EAST, Directions.WEST]:
      x,y = state
      dx, dy = Actions.directionToVector(action)
      nextx, nexty = int(x + dx), int(y + dy)
      if not self.gameState.data.layout.walls[nextx][nexty]:
        nextState = (nextx, nexty)
        successors.append( ( nextState, action, 1) )
    return successors
