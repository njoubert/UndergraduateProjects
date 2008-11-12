package org.red5.io.composite;


import org.junit.After;
import org.junit.Before;
import org.red5.io.composite.impl.*;
import java.util.ArrayList;
import java.util.List;
import java.io.File;
import junit.framework.Assert;
import junit.framework.TestCase;

public class TestCompositeMapCreator extends TestCase {
	
	public void testXMLRead() {
		
		File file = new File("test.pxml");
		List<CompositeElement> map = new ArrayList<CompositeElement>();
		//NIELS JOUBERT - Fix needed:
		//assertTrue(CompositeMapCreator.readElementsFile(file, map));
		assertTrue(map.size() == 4);
	}
	
	
	public void testXMLWrite() {
		
	}

}
