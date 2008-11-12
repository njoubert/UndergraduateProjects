/**
 * Shows a cool status bar at the bottom of the game.
 */
package game;

import java.awt.BorderLayout;
import javax.swing.JPanel;
import javax.swing.JLabel;

import java.awt.Dimension;


/**
 * Shows a cool status bar below the board, giving handy info to the user.
 */
public class JumpingCubeStatusbar extends JPanel {

	private static final long serialVersionUID = 1L;

	public JumpingCubeStatusbar() {
		super();
		setPreferredSize(new Dimension(0, 24));	
		display = new JLabel("Jump61B Ready for Action.");
		add(display, BorderLayout.CENTER);
		display.setVisible(true);
	}
	
	public void showMessage(String msg) {
		display.setText(msg);
	}
	
	JLabel display;

}
