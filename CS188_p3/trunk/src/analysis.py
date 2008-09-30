########################
########################
##                    ##
## ANALYSIS QUESTIONS ##
##                    ##
########################
########################

def question2():
  YOUR_ANSWER = None
  return YOUR_ANSWER

def question3():
  YOUR_ANSWER = None
  return YOUR_ANSWER
  
def question4a():
  YOUR_ANSWER = None
  return YOUR_ANSWER

def question4b():
  YOUR_ANSWER = None
  return YOUR_ANSWER

def question4c():
  YOUR_ANSWER = None
  return YOUR_ANSWER

def question4d():
  YOUR_ANSWER = None
  return YOUR_ANSWER

def question4e():
  YOUR_ANSWER = None
  return YOUR_ANSWER

def question7():
  YOUR_ANSWER = None
  return YOUR_ANSWER

if __name__ == '__main__':
  print 'Answers to analysis questions:'
  import agent
  for q in ['2','3','4a','4b','4c','4d','4e','7']:
    response = getattr(agent, 'question' + q)()
    print '  Question %s:\t%s' % (q, str(response))
