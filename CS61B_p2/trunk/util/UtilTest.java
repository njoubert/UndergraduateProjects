/* Author: Niels Joubert cs61b-bo */

package util;

import junit.framework.TestCase;

public class UtilTest extends TestCase {

  public UtilTest (String name) {
    super (name);
  }

  public void testEmpty1 () {
    Set2D set = new QuadTree (-1.0, -1.0, 1.0, 1.0);

    assertEquals ("size 0", 0, set.size ());
    assertEquals ("llx", -1.0, set.llx ());
    assertEquals ("lly", -1.0, set.lly ());
    assertEquals ("urx", 1.0, set.urx ());
    assertEquals ("ury", 1.0, set.ury ());

    Set2DIterator i = set.iterator ();
    assertTrue ("nothing in set", ! i.hasNext ());
  }
  
  /** Does the Quadtree manage to add a node? */
  public void testAddToEmpty() {
	  Set2D Quaddreee = new QuadTree(-10, -10, 10, 10);
	  Quaddreee.add(1, 4.0, 5.0);
	  assertEquals(4.0, Quaddreee.x(1));
	  assertEquals(5.0, Quaddreee.y(1));
  }
  
  /** What if we have a quadtree with a node, and we want to add? */
  public void testAddToOne() {
	  Set2D Quaddreee = new QuadTree(-10, -10, 10, 10);
	  Quaddreee.add(1, 4.0, 5.0);
	  assertEquals(4.0, Quaddreee.x(1));
	  assertEquals(5.0, Quaddreee.y(1));
	  
	  Quaddreee.add(2, -4.0, -5.0);
	  assertEquals(-4.0, Quaddreee.x(2));
	  assertEquals(-5.0, Quaddreee.y(2));
  }
  
  /** What if we are adding to the same quadrant? */
  public void testAddToInnerNode() {
	  Set2D Quaddreee = new QuadTree(-10, -10, 10, 10);
	  Quaddreee.add(1, -4.0, 5.0);
	  assertEquals(-4.0, Quaddreee.x(1));
	  assertEquals(5.0, Quaddreee.y(1));
	  
	  Quaddreee.add(2, 6.0, 5.0);
	  assertEquals(6.0, Quaddreee.x(2));
	  assertEquals(5.0, Quaddreee.y(2));
	  
	  Quaddreee.add(3, -4.0, -5.0);
	  assertEquals(-4.0, Quaddreee.x(3));
	  assertEquals(-5.0, Quaddreee.y(3));
	  
	  Quaddreee.add(4, -5.0, -5.0);
	  assertEquals(-5.0, Quaddreee.x(4));
	  assertEquals(-5.0, Quaddreee.y(4));
  }
  
  /** What if we add a bunch of items? */
  public void testAddBig() {
	  Set2D QT = getQT();
  }
  
  public void testRemove() {
	  Set2D WT = getQT();
	  assertTrue(WT.exists(9));
	  assertEquals(1.0, WT.x(9));
	  assertEquals(-1.0, WT.y(9));
	  
	  WT.remove(1);
	  assertFalse(WT.exists(1));	
	  
	  WT.remove(2);
	  assertFalse(WT.exists(2));
	  
	  WT.remove(3);
	  assertFalse(WT.exists(3));
	  
	  WT.remove(4);
	  assertFalse(WT.exists(4));
	  
	  WT.remove(5);
	  assertFalse(WT.exists(5));
	  
	  WT.remove(6);
	  assertFalse(WT.exists(6));
	  
	  WT.remove(7);
	  assertFalse(WT.exists(7));
	  
	  WT.remove(8);
	  assertFalse(WT.exists(8));
	  
	  WT.remove(9);
	  assertFalse(WT.exists(9));
	  
	  WT.remove(10);
	  assertFalse(WT.exists(10));
	  
	  WT.remove(11);
	  assertFalse(WT.exists(11));
	  
	  WT.remove(12);
	  assertFalse(WT.exists(12));
	  
	  WT.remove(13);
	  assertFalse(WT.exists(13));
	  
	  assertEquals(0, WT.size());
	  
	  WT.remove(20);
	  assertEquals(0, WT.size());
	  
  }
  
  public void testSize() {
	  Set2D WT = getQT();
	  assertEquals(13, WT.size());
  }
  
  public void testWholeIterator() {
	  Set2DIterator iter = getQT().iterator();
	  int[] ids = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13};
	  for (int i = 0; iter.hasNext(); i++) {
		  assertEquals(ids[i], iter.next());
	  }
  }
  
  public void testSortingOfWholeIterator() {
	  Set2DIterator iter = bigRandomQT().iterator();
	  assertEquals(1, iter.next());
  }
  
  public void testHUGEQuadTree() {
	  Set2D WT = hugeQT();
	  Set2DIterator iter = WT.iterator();
	  assertEquals(10000, WT.size());
	  while (iter.hasNext()) {
		  WT.remove(iter.next());
	  }
  }
  
  public void testBoundedIterator() {
	  Set2DIterator iter = getQT().iterator(0, 0, 10, 10);
	  int counter = 0;
	  while (iter.hasNext()) {
		  counter = counter + 1;
		  iter.next();
	  }
	  assertEquals(4, counter);
  }
  
 public void testBoundedIteratorBorder() {
	  Set2D QT = new QuadTree(-10, -10, 10, 10);
	  Set2DIterator iter;
	  
	  QT.add(1, -10, -10);
	  QT.add(2, 0, 0);
	  QT.add(3, 5, 5);
	  
	  iter = QT.iterator(-10, -10, -5, -5);
	  assertEquals(1, iter.next());
	  assertFalse(iter.hasNext());
	  
	  iter = QT.iterator(-10, -10, 0, 0);
	  assertEquals(1, iter.next());
	  assertFalse(iter.hasNext());
	  
  }
 
 public void testOrderOfIterator() {
		Set2DIterator iter = getQT().iterator();
		int lastID = -1;
		int currentID;
		while (iter.hasNext()) {
			currentID = iter.next();
			assertTrue(currentID > lastID);
			lastID = currentID;
		}
 }
  
  /*
   * This QuadTree represents the following box:
   * (where an integer refers to a point of that id in that quadrant)
   * |--------------|---------------|
   * |				|6	|5	|		|
   * |				|-------|	3	|
   * |				|	|	|		|
   * |		1		|---------------|
   * |				|		|		|
   * |				|	4	|		|
   * |				|		|		|
   * |--------------|---------------|
   * |				|9	|8	|		|
   * |				|-------|		|
   * |				|10	|11	|		|
   * |		2		|---------------|
   * |				|		|13	|12	|
   * |				|	7	|-------|
   * |				|		|	|	|
   * |--------------|---------------|
   */
  private Set2D getQT() {
	  Set2D QT = new QuadTree (-10, -10, 10, 10);
	  QT.add(1, -5.0, 5.0);
	  QT.add(2, -5.0, -5.0);
	  QT.add(3, 8.0, 8.0);
	  QT.add(4, 1.0, 1.0);
	  QT.add(5, 3.5, 9.0);
	  QT.add(6, 1.5, 9.0);
	  QT.add(7, 1.0, -9.0);
	  QT.add(8, 3.5, -1.0);
	  QT.add(9, 1.0, -1.0);
	  QT.add(10, 1.0, -4.0);
	  QT.add(11, 4.0, -4.0);
	  QT.add(12, 9.0, -6.0);
	  QT.add(13, 6.0, -6.0);
	  return QT;
  }
  
  private Set2D bigRandomQT() {
	  Set2D QT = new QuadTree (-100, -100, 100, 100);
	  for (int i = 20; i > 0; i--) {
		  QT.add(i, (double) i, 0.0);
	  }
	  for (int i = 21; i < 100; i++) {
		  QT.add(i, 0.0, (double) i);
	  }
	  for (int i = -40; i > -80; i-= 2) {
		  QT.add((-1 * i)+100,(double) i, (double) i);
	  }
	  return QT;
	  
  }
  
  private Set2D hugeQT() {
	  Set2D QT = new QuadTree (-100, -100, 100, 100);
	  int id = 0;
	  for (double x = -100.0; x < 100.0; x+= 2) {
		  for (double y = -100.0; y < 100.0; y+= 2) {
			  QT.add(id, x, y);
			  id += 1;
		  }
	  }
	  return QT;
  }
  


}

