import game, util
from distanceCalculator import Distancer, waitOnDistanceCalculator

class FeatureExtractor:  
  def getFeatures(self, state, action):    
    """
      Returns a dict from features to counts
      Usually, the count will just be 1.0 for
      indicator functions.  
    """
    abstract

class IdentityFeatureExtractor(FeatureExtractor):
  def getFeatures(self, state, action):
    feats = {}
    feats[(str(state),action)] = 1.0
    return feats

class SimplePacmanFeatureExtractor(FeatureExtractor):    
  
  def ensureDistancer(self, layout):
    if not 'cachedLayout' in self.__dict__:
      self.cachedLayout = None
      self.cachedLayout = layout
      self.distancer = Distancer(layout)              
  
  def getMacroFeatures(self, state, action):
    feats = {}
    feats['#bias'] = 1.0
    
    pacmanPos = state.getPacmanPosition()
    nextState = state.generatePacmanSuccessor(action)
    nextPacPos = nextState.getPacmanPosition()
    if nextPacPos in state.data.food.asList():
     feats['#eat_food'] = 1.0
    numGhosts = state.getNumAgents()-1
    ghostPositions = [x.configuration.pos for x in state.data.agentStates[1:]]
    if nextPacPos in ghostPositions: 
      feats['#get_eaten'] = 1.0

    numFoodLeft = len(state.data.food.asList())+1
    # feats['#numFoodLeft'] = numFoodLeft

    closestFoodDist = min(self.distancer.getDistance(pacmanPos,x) \
      for x in state.data.food.asList())
    feats['#closestFoodDist']  = closestFoodDist

    closesGhostDist = min(self.distancer.getDistance(pacmanPos,x)
      for x in ghostPositions)
    feats['#closesGhostDist'] = closesGhostDist 
    
    return feats

  def getPositionBasedFeatures(self,state,action):
    feats = {}
    # Pacman Position
    pacmanPos = state.getPacmanPosition()    
    feats[(pacmanPos,action)] = 1.0    
    
    radius = 5
    # Nearby Ghosts
    ghostPositions = [z.getPosition() for z \
      in state.data.agentStates[1:]]
    nearbyGhosts = [g for g in ghostPositions \
      if self.distancer.getDistance(pacmanPos,g) < radius]
    pacmanAndGhosts = (pacmanPos,str(nearbyGhosts))    
    feats[(pacmanAndGhosts,action)] = 1.0
    
    # Nearby Food
    topK = 3
    foodPositions = state.data.food.asList()
    foodPosMap = \
      dict([(f,self.distancer.getDistance(pacmanPos,f)) \
        for f in foodPositions])
    closestFoods = sorted(foodPosMap)
    numFood = max(topK, len(foodPositions))
    nearbyFood = closestFoods[:numFood]
    # nearbyFood = [f for f in foodPositions if \
    #   self.distancer.getDistance(pacmanPos,f) < radius]
    pacmanAndFood = (pacmanPos,str(nearbyFood))
    feats[(pacmanAndFood,action)] = 1.0
    
    feats[(state,action)] = 1.0
    
    return feats
  
  def getFeatures(self, state, action):
    self.ensureDistancer(state.data.layout)
    feats = {}
    
    for f,v in self.getPositionBasedFeatures(state,action).items():
      feats[f] = v
    
    if False:
      # Macro-Feats
      for f,v in self.getMacroFeatures(state,action).items():
        feats[f] = v
    
    return feats