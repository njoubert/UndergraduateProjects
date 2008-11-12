import java.util.GregorianCalendar;
import java.util.Calendar;
import java.util.Date;

public class GregorianDate extends GregorianCalendar {

	/** Construct a new GregorianDate object with the given mm/dd/yy */
	public GregorianDate(int month, int dayOfMonth, int year) {
		super(year, month, dayOfMonth);
	}
	
	/** Construct a new GreogiranDate object with the given mm/dd and current year */
	public GregorianDate(int month, int dayOfMonth) {
		super(Calendar.getInstance().get(Calendar.YEAR), month, dayOfMonth);
	}
	
	/** Return a new GregorianDate object with the day after this GregorianDate's date */
	public GregorianDate tomorrow() {
		return new GregorianDate(this.get(MONTH), this.get(DAY_OF_MONTH) + 1, this.get(YEAR));
	}
	
	/** Increment the current object's date with one dat */
	public void makeTomorrow() {
		this.add(Calendar.DAY_OF_MONTH, 1);
	}
	
	/** Return te string m[m]/d[d]/yy of this object's stored date */
	public String toString() {
		String thisYear = new String(this.get(YEAR) + "");
		thisYear = thisYear.substring(2, 4);
		return new String(this.get(MONTH) + "/" + this.get(DAY_OF_MONTH) + "/" + thisYear);
	}
	
	
	public static void main (String [ ] args) {
	    System.out.printf ("Day I was born = %s%n",
			       new GregorianDate (AUGUST, 31, 1950));
	    GregorianDate my2006birthday = new GregorianDate (AUGUST, 31);
	    System.out.printf ("The day after my birthday is %s.%n", 
			       my2006birthday.tomorrow ( ));
	    System.out.printf ("My birthday is still %s.%n", my2006birthday);
	    my2006birthday.makeTomorrow ( );
	    System.out.printf ("But someone has a birthday on %s.%n",
			       my2006birthday);
	   
	    
	}

}
