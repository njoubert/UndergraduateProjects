public class Sorter {

  /** Permutes VALUES so that its items are in nondecreasing order. */
  public static void sort (int [ ] values) {
      /* Elements 0 through k-1 are in nondecreasing order:
      values[0] <= values[1] <= ... <= values[k-1].
      Insert element k into its correct position, so that
      values[0] <= values[1] <= ... <= values[k]. */
	  for (int k = 1; k < values.length; k += 1) {

		  for (int i = k; i > 0; i--) {
			  if (values[i] > values[i-1]) {
				  break;
			  } else {
				  int temp = values[i-1];
				  values[i-1] = values[i];
				  values[i] = temp;
			  }
		  }
		  
	  }   
    }
  }
