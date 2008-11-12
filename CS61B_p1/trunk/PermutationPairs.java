class PermutationPairs {

	
	private PermutationGenerator innerPerms;
	private PermutationGenerator outerPerms;
	
	private int[] currentInnerPerm;
	private int[] currentOuterPerm;
	private int[][] currentPair = new int[2][];
	
	public PermutationPairs(int N) {
		innerPerms = new PermutationGenerator(N);
		outerPerms = new PermutationGenerator(N);
	}
	
	public boolean hasNextPair() {
		if (outerPerms.hasMore()) return true;
		else if (innerPerms.hasMore()) return true;
		return false;
	}
	
	public int[][] getLastPair() {
		return currentPair;
	}
	
	/** 
	 * This method will generate a pair and return it to you. It will sequence through the 
	 * innerPerms repeatedly until there is no outerPerms
	 * @return
	 */
	public int[][] nextPair() {
		if (hasNextPair()) {
			if (currentInnerPerm == null || currentOuterPerm == null) {
				
				currentOuterPerm = outerPerms.getNext().clone();
				currentInnerPerm = innerPerms.getNext().clone();
				currentPair[0] = currentOuterPerm;
				currentPair[1] = currentInnerPerm;
				return currentPair;
				
			} else {
				
				if (innerPerms.hasMore()) {
					currentInnerPerm = innerPerms.getNext().clone();
					currentPair[1] = currentInnerPerm;
					return currentPair;
				} else if (outerPerms.hasMore()) {
					innerPerms.reset();
					currentOuterPerm = outerPerms.getNext().clone();
					currentInnerPerm = innerPerms.getNext().clone();
					currentPair[0] = currentOuterPerm;
					currentPair[1] = currentInnerPerm;
					return currentPair;
					
				}
			}
		}
		return null;
	}  
}