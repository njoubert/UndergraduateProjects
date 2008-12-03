import util
import classificationMethod
import math

class NaiveBayesClassifier(classificationMethod.ClassificationMethod):
  """
  See the project description for the specifications of the Naive Bayes classifier.
  
  Note that the variable 'datum' in this code refers to a counter of features
  (not to a raw samples.Datum).
  """
  def __init__(self, legalLabels):
    # DO NOT DELETE or CHANGE any of those variables!
    self.legalLabels = legalLabels
    self.type = "naivebayes"
    self.k = 1 # this is the smoothing parameter
    self.automaticTuning = False # Flat for automatic tuning of the parameters
    
    self.labelDistribution = util.Counter()
    self.featuresGivenLabelsDistribution = util.Counter()
    
  def setSmoothing(self, k):
    """
    This is used by the main method to change the smoothing parameter before training.
    Do not modify this method.
    """
    self.k = k

  def train(self, trainingData, trainingLabels, validationData, validationLabels):
    """
    Outside shell to call your method. Do not modify this method.
    """  

    self.features = trainingData[0].keys() # this could be useful for your code later...
    
    if (self.automaticTuning):
        kgrid = [0.001, 0.01, 0.05, 0.1, 0.5, 1, 5, 10, 20, 50]
    else:
        kgrid = [self.k]
        
    return self.trainAndTune(trainingData, trainingLabels, validationData, validationLabels, kgrid)
  
  def getCounterOfFeatureLabelPairs(self, data, labels):
    counter = util.Counter()
    for i in range(len(labels)):
      y = labels[i]
      for (feat, featval) in data[i].items():
        counter.incrementCount((y, feat, featval), 1)                           
    return counter
    
      
  def trainAndTune(self, trainingData, trainingLabels, validationData, validationLabels, kgrid):
    """
    Train the classifier by collecting counts over the training data 
    and choose the smoothing parameter among the choices in kgrid by
    using the validation data. This method stores the right parameters
    as a side-effect and should return the best smoothing parameters.

    See the project description for details.
    
    Note that trainingData is a list of feature Counters.
    
    Assume that we do not use sparse encoding (normally we would); so that you can
    figure out what are the list of possible features by just looking
    at trainingData[0].keys() generically. Your code should not make any assumption
    about the feature keys apart that they are all in trainingData[0].keys().
    
    If you want to simplify your code, you can assume that each feature is binary
    (can only take the value 0 or 1).

    You should also keep track of the priors and conditional probabilities for
    further usage in calculateLogJointProbabilities method
    
    FeaturesGivenLabelsDistribution:
        Key: (label, feature, featureValue)
        Value: Probability
        
    """
    
    ## YOUR CODE HERE
    
    # Compute P(Y = y) from training data.
    for label in self.legalLabels:
        self.labelDistribution[label] = trainingLabels.count(label)
    self.labelDistribution.normalize()
    
    
    featureLabelCount = self.getCounterOfFeatureLabelPairs(trainingData, trainingLabels)
    
    
    conditionalDistributionList = []
    for k in kgrid:
      P_feature_label = util.Counter()
      for y in self.legalLabels:
        for f in self.features:
          ### For F = 1:
          probability = float((featureLabelCount.getCount((y, f, 1)) + k)) / (2*k + featureLabelCount.getCount((y, f, 1)) + featureLabelCount.getCount((y, f, 0)))
          P_feature_label.incrementCount((y, f, 1), probability)
           ### For F = 0:
          probability = float((featureLabelCount.getCount((y, f, 0)) + k)) / (2*k + featureLabelCount.getCount((y, f, 1)) + featureLabelCount.getCount((y, f, 0)))
          P_feature_label.incrementCount((y, f, 0), probability)   
          
      conditionalDistributionList.append((k, P_feature_label))
    
    """
    conditionalDistributionList = []
    for k in kgrid:
        tempFeaturesGivenLabelsDistribution = util.Counter()
        
        #first we calculate top = count(Fi = fi, Y = y) and bottom:
        for y in self.legalLabels:
            for fi in self.features:
                
                top = float(k)
                bottom = float(0)
                for i in range(len(trainingLabels)):
                    if (trainingLabels[i] == y):
                        top += trainingData[i][fi]
                        bottom += 2*k + 1
                
                tempFeaturesGivenLabelsDistribution[(y, fi, 1)] = top / bottom
                tempFeaturesGivenLabelsDistribution[(y, fi, 0)] = 1 - (top / bottom)
                
        conditionalDistributionList.append((k, tempFeaturesGivenLabelsDistribution))        
       
    """    
        
    bestAccuracy = float(0)    
    for (k, dist) in conditionalDistributionList:
        tempDist = self.featuresGivenLabelsDistribution
        tempK = self.k
        
        self.featuresGivenLabelsDistribution = dist
        self.k = k
        
        guessedLabels = self.classify(validationData)
        netright = 0
        for i in range(len(guessedLabels)):
            if (guessedLabels[i] == validationLabels[i]):
                netright += 1
        
        accuracy = float(netright) / len(guessedLabels)
        if (accuracy < bestAccuracy):
            self.featuresGivenLabelsDistribution = tempDist
            self.k = tempK
        else:
            bestAccuracy = accuracy
            self.k = k
            
    return self.k
    
  def classify(self, testData):
    """
    Classify the data based on the posterior distribution over labels.
    
    You shouldn't modify this method.
    """
    guesses = []
    self.posteriors = [] # Log posteriors are stored for later data analysis (autograder).
    for datum in testData:
      posterior = self.calculateLogJointProbabilities(datum)
      guesses.append(posterior.argMax())
      self.posteriors.append(posterior)
    return guesses
      
  def calculateLogJointProbabilities(self, datum):
    """
    Returns the log-joint distribution over legal labels and the datum.
    Each log-probability should be stored in the log-joint counter, e.g.    
    logJoint['face'] = <Estimate of log( P(Label = face, datum) )>
    """
    logJoint = util.Counter()
    
    ## YOUR CODE HERE
    
    # example of type of values: logJoint["SomeLabel"] = math.log(1e-301) 
    for y in self.legalLabels:
      logJoint[y] = math.log(self.labelDistribution[y])
      for fi in datum.keys():
        #print self.featuresGivenLabelsDistribution.has_key((y, fi, datum[fi]))
        #print self.featuresGivenLabelsDistribution.getCount((y, fi, datum[fi]))
        logJoint[y] += math.log(self.featuresGivenLabelsDistribution.getCount((y, fi, datum[fi])))
    
    return logJoint
  
  def findHighOddsFeatures(self, class1, class2):
    """
    Returns: 
    featuresClass1 -- the 100 best features for P(feature=on|class1) (as a list)
    featuresClass2 -- the 100 best features for P(feature=on|class2)
    featuresOdds -- the 100 best features for the odds ratio 
                     P(feature=on|class1)/P(feature=on|class2) 
    """

    featuresClass1 = []
    featuresClass2 = []
    featuresOdds = []
    
    ## YOUR CODE HERE

    return featuresClass1,featuresClass2,featuresOdds
    

    
      
