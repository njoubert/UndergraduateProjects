package org.joubert.UI;

import org.eclipse.swt.widgets.Display;
import org.eclipse.swt.widgets.Shell;

/**
 * Singleton GUI class that contains the main Display
 * for SWT.
 * @author Niels Joubert
 *
 */
public class GUI implements Runnable {

    static GUI me;
    
    Display display;
    Shell shell;
    private GUI() { 
        //empty
    }
    
    public static GUI getGUI() {
        if (me == null)
            me = new GUI();
        return me; 
    }
    
    public void run() {
        display = new Display();
        shell = new Shell(display);
        shell.setText("MusicShare");
        shell.open();
        while(!shell.isDisposed())
            if (!display.readAndDispatch())
                display.sleep();
        display.dispose();
    }
}
