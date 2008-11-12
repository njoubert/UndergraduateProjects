 package game;

import java.awt.BorderLayout;
import java.awt.event.ActionListener;
import java.awt.event.ActionEvent;
import javax.swing.JMenu;
import javax.swing.JMenuItem;
import javax.swing.JMenuBar;
import javax.swing.JRadioButtonMenuItem;
import javax.swing.ButtonGroup;
import javax.swing.JFileChooser;
import java.io.IOException;
import ucb.util.mailbox.Mailbox;
import ucb.util.mailbox.QueuedMailbox;

import javax.swing.JOptionPane;

import javax.swing.JFrame;

@SuppressWarnings("serial") class JumpingCubeFrame extends JFrame implements ActionListener {
	
	/** A new JumpingCubeFrame, initially not displayed or connected to
	 *  to anything. */
	JumpingCubeFrame (String title) {
		super (title);
		boardWidget = new JumpingCubeBoard (outBox);
		statusWidget = new JumpingCubeStatusbar();
		
		initMenus ();
		setDefaultCloseOperation (EXIT_ON_CLOSE);
		setDefaultLookAndFeelDecorated(true);
		
		java.awt.Toolkit.getDefaultToolkit().setDynamicLayout(true);
		
		getContentPane ().add (boardWidget, BorderLayout.CENTER);
		getContentPane ().add (statusWidget, BorderLayout.SOUTH);
		
		pack ();
		setVisible (true);
		
		boardWidget.enableMouseInput(true);
		
	}
	

	
	/** Set my internal copy of the playing board to a copy of BOARD.  */
	void setBoard (Board board) {
		boardWidget.setBoard (board);
	}
	
	/** Initialize all menus. */
	private void initMenus () {
		setJMenuBar (new JMenuBar ());
		
		addMenuItem ("Game", "Game", "game");
		addMenuItem ("Game", "Start", "start");
		addMenuItem ("Game", "Clear", "clear");
		addMenuItem ("Game", "Load", "load");
		addSeparator("Game");
		addMenuItem ("Game", "Force Termination", "crash");
		addMenuItem ("Game", "Quit", "quit"); 
		
		addMenuItem ("Local Player", "Player 1 Auto", "auto");
		addMenuItem ("Local Player", "Player 1 Manual", "manual");
		 
		addMenuItem ("Remote", "Host", "host");	//Come back to this
		addMenuItem ("Remote", "Join", "join"); //Come back to this
	}
	
	/** Add a menu button named ITEMNAME to the end of the menu named
	 *  MENUNAME from the menu bar, giving COMMAND as a description of
	 *  what to do when the button is pressed.   Creates the MENUNAME if needed. 
	 *  Returns the new menu item. */
	private JMenuItem addMenuItem (String menuName, String itemName, String command) {
		JMenuItem item = new JMenuItem (itemName);
		item.setActionCommand (command);
		item.addActionListener (this);
		getMenu (menuName).add (item);
		return item;
	}
	
	/** Return the menu whose name is MENUNAME from the menu bar, creating a new
	 *  menu bar item after any existing menu buttons there if no such menu
	 *  is present. */
	private JMenu getMenu (String menuName) {
		JMenuBar bar = getJMenuBar ();
		JMenu menu;
		for (int i = 0; i < bar.getMenuCount (); i += 1) {
			menu = bar.getMenu (i);
			if (menu.getText ().equals (menuName))
				return menu;
		}
		menu = new JMenu (menuName);
		bar.add (menu);
		return menu;
	}
	
	/** Add a radio button named ITEMNAME to the end of the menu named
	 *  MENUNAME from the menu bar, placing it in GROUP (only one radio 
	 *  button in a group is selected at a time), initially selecting it
	 *  iff SELECTED.  When the item is selected by the user, causes
	 *  COMMAND to be used as the action command (see actionPerformed).
	 *  Returns the new menu item. */
	@SuppressWarnings("unused")
	private JMenuItem addRadioMenuItem (String menuName, String itemName, 
			ButtonGroup group, boolean selected,
			String command) {
		JMenuItem item = new JRadioButtonMenuItem (itemName, selected);
		group.add (item);
		getMenu (menuName).add (item);
		item.setActionCommand (command);
		item.addActionListener (this);
		return item;
	}
	
	/** Add a separator to the end of the menu named MENUNAME on the menu bar,
	 *  creating that menu if needed. */
	private void addSeparator (String menuName) {
		getMenu (menuName).addSeparator ();
	}    
	
	/** Send a command from the GUI to the game engine.  The command is
	 *  defined by COMMANDFORMAT and ARGS, which define the same string
	 *  as for String.format (COMMANDFORMAT, ARGS). */
	private void enterCommand (String commandFormat, Object... args) {
		try {
			outBox.deposit (String.format (commandFormat, args));
			return;
		} catch (IOException e) {
		} catch (InterruptedException e) {
		}
		throw new Error ("Fatal internal communication error.");
	}
	
	void showErrorDialog(String message) {
		JOptionPane.showMessageDialog(this, message, "Error",
                JOptionPane.ERROR_MESSAGE);
	}
	
	void showStatusMessage(String message) {
		statusWidget.showMessage(message);
	}
	
	/** Enable input from the mouse iff ENABLE */
	synchronized void enableMouseInput (boolean enable) {
		boardWidget.enableMouseInput(enable);
	}
	
	
	
	
	
	
	
	
	/** Waits for and gets next command given to the GUI. */
	String getCommand () {
		try {
			return outBox.receive ();
		} catch (IOException e) {
			return "quit";
		} catch (InterruptedException e) {
			return "quit";
		}
	}
	
	/** Waits for either the next command given to the GUI or TIMELIMIT
	 *  milliseconds to elapse.  If no command arrives within the time
	 *  limit, returns null.  The time limit may be 0, indicating that
	 *  a command is returned only if the GUI had previously issued it
	 *  prior to this call. */
	String getCommand (long timeLimit) {
		try {
			return outBox.receive (timeLimit);
		} catch (IOException e) {
			return "quit";
		} catch (InterruptedException e) {
			return "quit";
		}
	}
	
	/** Responds to a click on a menu item (see ActionListener). */
	public void actionPerformed (ActionEvent e) {
		String cmnd = e.getActionCommand ();
		if (cmnd == null)
			return;
		if (cmnd.equals ("quit") || cmnd.equals ("auto") || cmnd.equals ("manual") || cmnd.equals("start") || cmnd.equals("clear"))
			enterCommand ("%s", cmnd);
		if (cmnd.equals ("crash"))
			Main.exit(1);
		if (cmnd.equals ("game")) {
            String s = (String)JOptionPane.showInputDialog(this,
            		"Please enter the arguments of the Game command:\n"
                    + "\"game [size] [your-color]\"",
                    "game 6 red");
            //It would be nice to resize at this moment!
            if (s != null)
            	enterCommand("%s", s);
		}
		if (cmnd.equals ("load")) {
			JFileChooser chooser = new JFileChooser();
		    int returnVal = chooser.showOpenDialog(null);
		    if(returnVal == JFileChooser.APPROVE_OPTION) {
		       String s = chooser.getSelectedFile().getAbsolutePath();
		       enterCommand("load %s", s);
		    }
		}
		if (cmnd.equals ("host")) {
            String s = (String)JOptionPane.showInputDialog(this,
            		"Please enter the arguments of the Host command:\n"
                    + "\"host [id]\"",
                    "host sharpie");
            //It would be nice to resize at this moment!
            if (s != null) enterCommand("%s", s);
		}
		if (cmnd.equals ("join")) {
            String s = (String)JOptionPane.showInputDialog(this,
            		"Please enter the arguments of the Join command:\n"
                    + "\"join [id]@[host]\"",
                    "join sharpie@localhost");
            //It would be nice to resize at this moment!
            if (s != null) enterCommand("%s", s);
		}
	}
	
	private Mailbox<String> outBox = QueuedMailbox.create (10);
	private JumpingCubeBoard boardWidget;
	private JumpingCubeStatusbar statusWidget;
	
}

