
public class IslamicDate {
	
	private int days;
	
	/** IslamicDate instantiation by giving a month and day value */
	public IslamicDate(int month, int day) {
		this.setDate(month, day);
		
	}
	/** IslamicDate instantiation by giving a day of the year value */
	public IslamicDate(int dayOfYear) {
		this.setDate(dayOfYear);
	}
	/** IslamicDate instantiation by giving a String of the form "mm/dd" */
	public IslamicDate(String islamicDate) {
		String monthdate[] = islamicDate.split("/");
		this.setDate(Integer.parseInt(monthdate[0]), Integer.parseInt(monthdate[1]));
	}
	/** rewrites the date of this object to the given date */
	private void setDate (int dayOfYear) {
		while (dayOfYear > 354) dayOfYear = dayOfYear - 354;
		days = dayOfYear;
	}
	private int getDaysOfYear () {
		return days;
	}
	/** sets the date of this object to the given date.
	 * the date must be zero beforehand!
	 *  0 < month <= 13
	 *  0 < day <= 30
	 *  */
	private void setDate(int month, int day) {
		days = 0;
		while (month > 1) {
			if (month == 3 || month == 5 || month == 7 || month == 9 || month == 11) {
				days = days + 30;
				month -= 1;
			} else if (month == 2 || month == 4 || month == 6 || month == 8 || month == 10 || month == 12) {
				days = days + 29;
				month -= 1;
			}
		}
		days = days + day;
	}
	
	private int getMonth() {
		int daysLeft = days;
		int month = 1;
		while (daysLeft > 0) {
			if (daysLeft >= 59) {
				month = month + 2;
				daysLeft = daysLeft - 59;
			} else if (daysLeft >= 30) {
				month = month + 1;
				daysLeft = daysLeft - 30;
			} else if (daysLeft == 29) {
				month = month + 1;
				daysLeft = daysLeft - 29;
			} else {
				daysLeft = 0;
			}
		}
		return month;	
	}
	
	private int getDay() {
		int dayOfMonth = days;
		
		while (dayOfMonth > 29) {
			if (dayOfMonth > 59) {
				dayOfMonth = dayOfMonth - 59;
			} else if (dayOfMonth > 30) {
				dayOfMonth = dayOfMonth - 29;
			} else {
				break;
			}
		}
		return dayOfMonth;
	}
			

	/** Returns the value of this object's date in the form "mm/dd" */
	public String toString() {
		String returnDate = "";
		returnDate = this.getMonth() + "/" + this.getDay();
		return returnDate;
	}
	/** Returns a new IslamicDate object with tomorrow's date */
	public IslamicDate tomorrow() {
		return new IslamicDate(days + 1);
		
	}
	/** Increments the day value of this object by one */
	public void makeTomorrow() {
		this.setDate(days + 1);
	}
	/** Returns boolean whether the argument date is equal to this date */
	public boolean equals(IslamicDate otherDate) {
		return days == otherDate.getDaysOfYear();
	}
	/** Returns the integer of the amount of days from this date to the argument date */
	public int daySpan(IslamicDate otherDate) {
		return otherDate.getDaysOfYear() - this.getDaysOfYear();
	}
	

}
