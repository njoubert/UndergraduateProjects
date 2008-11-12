package game;

import java.util.regex.Matcher;
import java.util.regex.Pattern;
import java.io.IOException;
import static game.Game.State.*;


/**
 * <h2>Command Parsing</h2>
 * <P>A command represents a specific event, triggered by input from some source.
 * <BR>Commands have the ability to be executed, and the ability to be reported.
 * <BR>Upon execution, a command calls the required methods inside Game that 
 * corresponds with the event it represents.
 * <BR>Upon reporting, a command sends itself in the required format to the required players.
 * This makes communication with a remote player or a local player mostly transparent from
 * outside the command class - a beautiful advantage of OOP programming.</P>
 * <h4>Error handling</h4>
 * <P>
 * Very little error handling occurs inside the Commands, since errors are normally generated 
 * either during execution/reporting, and these errors are simply passed through to executor of
 * the command, and handled there. 
 * </P>
 * </P>
 * <H2>Commands in effect at any moment</H2>
 * <P>Different commands are in effect at different times (different states, modes and points of execution) 
 * and has different meaning according to the time and origin of the command.
 * I make two major separations between the commands:
 * <br><b>Commands from local players</b> as represented by the remote*CommandTypes arrays of commands, and Remote*Command classes.
 * This is NOT the commands that gets sent to remote players, simply the commands that is recognized
 * * when deposited in the remote player's inbox.
 * <br><b>Commands from remote players</b> as represented by the *CommandTypes arrays of commands, and *Command classes.
 * <br>Each possible command has a class, which is indexed in the appropriate array corresponding to its legality
 * during eac state. This means that a command that might be legal during PLAYING (eg. move commands) will
 * be signalled as an erroneous command simply by the fact that it is only manifested in the 
 * arrays corresponding to remote and local playing.
 * </P>
 * <h3>Commands</h3>
 * <P>Remote commands come from remote players, other commands come from local players.
 * <h4>Source of commands in remote mode</h4>
 * <P>The remote commands has an interesting twist in naming conventions. If you recieve a message from your Client player,
 * that means that the HOST has sent it, and vice versa. Thus, often when giving user response, you would check that you
 * have a host player and if you have report that the Client did something wrong. This is a simple matter
 * of shifting the blame to those that are responsible for it.
 *
 */
abstract class Command {
	
	/** This array contains instances of all the possible commands, which is used to create new instances of specific commands. */
	static final Command[] allCommandTypes = {
		new ClearCommand(),
		new StartCommand(),
		new QuitCommand(),
		new GameCommand(),
		new AutoCommand(),
		new ManualCommand(),
		new HostCommand(),
		new JoinCommand(),
		new HelpCommand(),
		new DumpCommand(),
		new LoadCommand(),
		new SeedCommand(),
		new MoveCommand(),
		new NullCommand()
	};
	
	/** Mainly fore debugging purposes, this contains string examples of all known commands. */
	static final String[] allCommandStrings = {
		"clear",
		"start",
		"quit",
		"game 3 red",
		"auto",
		"manual",
		"host id",
		"join id@hostname",
		"help",
		"dump",
		"load filename",
		"seed 5",
		"1:1",
		""
	};
	
	/** Represents the legal commands recognized from (thus, the commands RECIEVED from) client players.
	 * Thus, this is the commands that is legal for host players to send to client players.
	 * This is NOT the commands that gets sent to remote players, simply the commands that is recognized
	 * when deposited in the remote player's inbox. */
	static final Command[] remoteClientCommandTypes = {
		new RemoteClearCommand(),
		new RemoteQuitCommand(),
		new RemoteMoveCommand(),
		new RemoteStartCommand(),
		new RemoteGameCommand(),
		new NullCommand()
	};
	
	/** Represents the legal commands recognized from (thus, the commands RECIEVED from) host players.
	 * Thus, this is the commands that some other player can send to you when you are the host.
	 * This is NOT the commands that gets sent to remote players, simply the commands that is recognized
	 * when deposited in the remote player's inbox. */
	static final Command[] remoteHostCommandTypes = {
		new RemoteClearCommand(),
		new RemoteQuitCommand(),
		new RemoteMoveCommand(),
		new NullCommand()
	};
	
	/** This is the legal commands while the host player waits for a connection. */
	static final Command[] waitingConnectionCommandTypes = {
		new ClearToConnectCommand(),
		new RemoteQuitCommand(),
		new RemoteStartCommand(),
		new NullCommand()
	};
	
	/** Represents the legal commands recognized from human (terminal input) players. */
	static final Command[] humanCommandTypes = {
		new ClearCommand(),
		new StartCommand(),
		new QuitCommand(),
		new GameCommand(),
		new AutoCommand(),
		new ManualCommand(),
		new HostCommand(),
		new JoinCommand(),
		new HelpCommand(),
		new DumpCommand(),
		new LoadCommand(),
		new SeedCommand(),
		new MoveCommand(),
		new NullCommand()
	};
	
	/** Represents the legal commands recognized from the local player during the setup state. */
	static final Command[] setupCommandTypes = {
		new ClearCommand(),
		new StartCommand(),
		new QuitCommand(),
		new GameCommand(),
		new AutoCommand(),
		new ManualCommand(),
		new HostCommand(),
		new JoinCommand(),
		new HelpCommand(),
		new DumpCommand(),
		new LoadCommand(),
		new SeedCommand(),
		new MoveCommand(),
		new NullCommand()
	};
	
	/** Represents the legal commands recognized from the local player during the playing state. */
	static final Command[] playingCommandTypes = {
		new ClearCommand(),
		new StartCommand(),
		new QuitCommand(),
		new HelpCommand(),
		new DumpCommand(),
		new LoadCommand(),
		new SeedCommand(),
		new MoveCommand(),
		new NullCommand()
	};
	
	/** Represents the legal commands recognized from the local player during the finished state. */
	static final Command[] finishedCommandTypes = {
		new ClearCommand(),
		new StartCommand(),
		new QuitCommand(),
		new ManualCommand(),
		new AutoCommand(),
		new HelpCommand(),
		new DumpCommand(),
		new LoadCommand(),
		new SeedCommand(),
		new NullCommand()
	};
	
	/** Executes this command by calling the game class' method for this command. */
	public abstract void execute() throws IOException;
	
	/** Reports this comman to the players.
	 * This can involve printing a message to the terminal, displaying a message on the GUI,
	 * ignoring it or sending the message to the remote player, suitable modified.
	 * Assumes that the command is legal (it will not pass game's executor if it is not. */
	public abstract void report() throws IOException;
	
	/** Returns a specific class if the given argument is valid */
	public abstract Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color);
	
	/**
	 * The Creation method for Commands.
	 * This method takes in all the required arguments to construct a command
	 * from a String containing a line representing a single command
	 * recieved from any type of player in any state or mode.
	 * It contains a decision making structure for the types of commands
	 * that is legal at this moment in time in the game
	 * (thus it looks at the type of player and the state of the game)
	 * and creates the required Command objects accordingly.
	 * 
	 * 
	 * @param unparsedCommand The string to be parsed. Assumed to be a full line from a player.
	 * @param game The current game being played.
	 * @param sentPlayer The player who issues this command.
	 * @param otherPlayer The other player in the game.
	 * @param color The color associated with this command.
	 * @return A command object that can execute the given string.
	 * @throws IllegalArgumentException When the command in unrecognized.
	 */
	public static Command create(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) throws IllegalArgumentException {
		Command parsedCommand;
		Command[] activeCommands;
		
		if (unparsedCommand == null) return new NullCommand();
		
		if (sentPlayer.type().equals("Host")) {
			if (!game.isConnected()) {
				activeCommands = waitingConnectionCommandTypes;
			} else {
				activeCommands = remoteHostCommandTypes;
			}
		} else if (sentPlayer.type().equals("Client")) {
			activeCommands = remoteClientCommandTypes;
		}
		
		else if (game.getState() == SETUP) activeCommands = setupCommandTypes;
		else if (game.getState() == PLAYING) activeCommands = playingCommandTypes;
		else if (game.getState() == FINISHED) activeCommands = finishedCommandTypes;
		
		else activeCommands = allCommandTypes;
		
		for (Command commandType : activeCommands) {
			parsedCommand = commandType.parse (unparsedCommand, game, sentPlayer, otherPlayer, color);
			if (parsedCommand != null) {
				return parsedCommand;
			}
		}
		if (game.inRemoteMode()) {
			game.illegalRemoteOperation("The command " + unparsedCommand + " sent by the remote user was illegal!");
			return new NullCommand();
		} else {
			throw new IllegalArgumentException("The given command {" + unparsedCommand + "} from player {" + sentPlayer.type() + "} was not understood.");
		}
	}
	
	Game game;
	Player sentPlayer;
	Player otherPlayer;
	Color color;
	String type;
	
}

/**
 * Represents the clear command typed at the terminal. 
 * This command sends itself to the remote computer if we are in remote mode and we call report(). */
class ClearCommand extends Command {
	
	private String pattern = "\\s*clear\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public ClearCommand() {this.type = "clear"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public ClearCommand(Game game) {
		this.game = game;
		this.type = "clear";
	}
	
	/** calls the game's start method. */
	public void execute() {
		game.clear();
	}
	/** Reports this player to the local, and if applicable, remote host. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Board cleared. Game now in " + game.getState().toString() + " state.\n");
		if (game.inRemoteMode()) {
			game.getPlayer(2).sendCommand("clear");
			game.getPlayer(2).flush(1000);
		}
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {		
			return new ClearCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the clear command when recieved from a remote player.
 * Does NOT send itself to another computer.*/
class RemoteClearCommand extends Command {
	
	private String pattern = "\\s*clear\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public RemoteClearCommand() {this.type = "clear"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public RemoteClearCommand(Game game) {
		this.game = game;
		this.type = "clear";
	}
	
	/** calls game's clearRemote method. */
	public void execute() {
		game.clearRemote();
	}
	
	/** Reports this command to the local player. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Remote clear recieved, Board cleared.\n");
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {		
			return new RemoteClearCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the Start command as entered by the local user. */
class StartCommand extends Command {
	
	private String pattern = "\\s*start\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public StartCommand() {this.type = "start"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public StartCommand(Game game) {
		this.game = game;
		this.type = "start";
	}
	
	/** Sends this command to the remote user if we are the host, and notifiy the local player. */
	public void report() throws IOException {
		if (game.inRemoteMode()) {
			if (game.getPlayer(2).type().equals("Host")) {
				game.getPlayer(2).sendCommand("start");
			}
		} 
		game.getPlayer(1).sendCommand("The game now in Playing state.\n");
		
	}
	
	/** Starts the game if we are the host or in local mode. 
	 * Throws an exception if the client tries this.*/
	public void execute() {
		if (game.inRemoteMode()) {
			if (game.getPlayer(2).type().equals("Host")) {
				game.start();
			} else {
				throw new IllegalArgumentException("You cannot execute the Start command as a client!");
			}
		} else {
			game.start();
		}
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new StartCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the start command as recieved from a remote host. */
class RemoteStartCommand extends Command {
	
	private String pattern = "\\s*start\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public RemoteStartCommand() {this.type = "start"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public RemoteStartCommand(Game game, Player player) {
		this.game = game;
		this.sentPlayer = player;
		this.type = "start";
	}
	
	/** Informs the local user that the game has started. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("The game is now in Playing state.\n");	
	}
	
	/** Starts the game using game.start() */
	public void execute() {
		game.start();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new RemoteStartCommand(game, sentPlayer);
		} else {
			return null;
		}
	}
}

/** Represents the quit command as issues by a local player, in both its
 * manifestations during local or remote mode.*/
class QuitCommand extends Command {
	
	private String pattern = "\\s*quit\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public QuitCommand() {this.type = "quit"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public QuitCommand(Game game) {
		this.game = game;
		this.type = "quit";
	}
	
	/** Notifies the user that remote mode has been quitted. If this command has been issued in local
	 * mode the program would have closed before this, and we need not handle that case. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Remote Mode Quitted.\n");

	}
	
	/** Throws an ExitException. */
	public void execute() {
		throw new ExitException();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new QuitCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the quit command as recieved from a remote player.
 * Thus the program sghould not terminate, simply we should be the winner and end th game.*/
class RemoteQuitCommand extends Command {
	
	private String pattern = "\\s*quit\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public RemoteQuitCommand() {this.type = "quit"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public RemoteQuitCommand(Game game) {
		this.game = game;
		this.type = "quit";
	}
	
	/** Informs the local player that a remote quit has been recieved. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Quit message recieved from other player. Remote mode quitted.\n");
	}
	
	/** Call game's quitRemote method. */
	public void execute() {
		game.quitRemote();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new RemoteQuitCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the game command from the local player. */
class GameCommand extends Command {
	
	private int size;
	private String pattern = "\\s*game\\s+(\\d+)\\s+([a-z]+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public GameCommand() {this.type = "game"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public GameCommand(Game game, Player sentPlayer, Player otherPlayer, int size, Color color) {
		this.game = game;
		this.sentPlayer = sentPlayer;
		this.otherPlayer = otherPlayer;
		this.color = color;
		this.size = size;
		this.type = "game";
	}
	
	/** Sends across the command to the remote player if applicable, and informs the user.*/
	public void report() throws IOException {

		if (game.inRemoteMode()) {
			game.getPlayer(2).sendCommand("game " + size + " " + color.opposite().toString());
		}
		game.getPlayer(1).sendCommand("Game initialized to board size " + size + " with your color as " + color.toString() + "\n");
		
	}
	
	/** sets the game's size and color using game's game method if we are the host or in local mode. 
	 * Throws an exception if we, as client, illegally enter this. */
	public void execute() {
		if (game.inRemoteMode()) {
			if (game.getPlayer(2).type().equals("Host")) {
				game.game(size, color);
			} else {
				throw new IllegalArgumentException("You cannot use the game command when you are the client in a remote game.");
			}
		} else {
			game.game(size, color);
		}
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			
			try {
				return new GameCommand(game, sentPlayer, otherPlayer, Integer.parseInt(matcher.group(1)), Color.parseColor(matcher.group(2)));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the game command recieved from the remote computer. */
class RemoteGameCommand extends Command {
	
	private int size;
	private String pattern = "\\s*game\\s+(\\d+)\\s+([a-z]+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public RemoteGameCommand() {this.type = "game"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public RemoteGameCommand(Game game, Player sentPlayer, Player otherPlayer, int size, Color color) {
		this.game = game;
		this.sentPlayer = sentPlayer;
		this.otherPlayer = otherPlayer;
		this.color = color;
		this.size = size;
		this.type = "game";
	}
	
	/** informs the local player of the reception of a game command if the command is legal - that is, we are
	 * the client. */
	public void report() throws IOException {
		if (sentPlayer.type().equals("client"))
			game.getPlayer(1).sendCommand("Game initialized to board size " + size + " with your color as " + color.toString() + "\n");
	}
	
	/** executes game's game method if we are the client, else calls game's illegalRemoteOperation method. */
	public void execute() {
		if (sentPlayer.type().equals("Client"))
			game.game(size,color);
		else game.illegalRemoteOperation("Illegally received the game command.");
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			
			try {
				return new RemoteGameCommand(game, sentPlayer, otherPlayer, Integer.parseInt(matcher.group(1)), Color.parseColor(matcher.group(2)));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The remote player's move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the auto command. */
class AutoCommand extends Command {
	
	private String pattern = "\\s*auto\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public AutoCommand() {this.type = "auto"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public AutoCommand(Game game) {
		this.game = game;
		this.type = "auto";
	}
	
	/** Informs the local player that he is now an automatic player. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Player 1 is now an automatic player.\n");
	}
	
	/** Executes game's auto method. */
	public void execute() {
		game.auto();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new AutoCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the manual command, which counters auto.*/
class ManualCommand extends Command {
	
	private String pattern = "\\s*manual\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public ManualCommand() {this.type = "manual"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public ManualCommand(Game game) {
		this.game = game;
		this.type = "manual";
	}
	
	/** Informs the user that he is now a manual player. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Player 1 is now a manual player.\n");
	}
	
	/** executes the game's manual command, countering auto. */
	public void execute() {
		game.manual();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new ManualCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the host command.*/
class HostCommand extends Command {
	
	private String id;
	private String pattern = "\\s*host\\s+(\\w+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public HostCommand() {this.type = "host"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public HostCommand(Game game, String id) {
		this.game = game;
		this.id = id;
		this.type = "host";
	}
	
	/** Informs the local player that he is now hosting a game. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("You are now hosting a game with id: " + id + "\n");
	}
	
	/** If we are already in remote mode, this throws an error, else we call game's host method. */
	public void execute() throws IOException {
		if (game.inRemoteMode()) throw new IllegalArgumentException("You cannot host if you are already in remote mode.\nQuit remote mode first with the quit command.");
		game.host(id);
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			
			try {
				return new HostCommand(game, matcher.group(1));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the join command from the terminal. */
class JoinCommand extends Command {
	
	private String id;
	private String host;
	private String pattern = "\\s*join\\s+(\\w+)@(.+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public JoinCommand() {this.type = "join"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public JoinCommand(Game game, String id, String host) {
		this.game = game;
		this.id = id;
		this.host = host;
		this.type = "join";
	}
	
	/** Informs the user that he is connected to the specific player. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("You have joined the game " + id + " on computer " + host + "\n");
	}
	
	/** Throws an exxeption if we are already connected, else uses game's join method. */
	public void execute() throws IOException {
		if (game.inRemoteMode()) throw new IllegalArgumentException("You cannot join a host if you are already in remote mode.\nQuit remote mode first with the quit command.");
		game.join(id, host);
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			
			try {
				return new JoinCommand(game, matcher.group(1), matcher.group(2));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the help command. */
class HelpCommand extends Command {
	
	private String pattern = "\\s*help\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public HelpCommand() {this.type = "help"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public HelpCommand(Game game) {
		this.game = game;
		this.type = "help";
	}
	
	public void report() throws IOException {
		
	}
	
	public void execute() throws IOException {
		game.help();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new HelpCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the dump command. */
class DumpCommand extends Command {
	
	private String pattern = "\\s*dump\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public DumpCommand() {this.type = "dump"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public DumpCommand(Game game) {
		this.game = game;
		this.type = "dump";
	}
	
	public void report() throws IOException {
		
	}
	
	public void execute() throws IOException {
		game.dump();
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new DumpCommand(game);
		} else {
			return null;
		}
	}
}

/** Represents the load command*/
class LoadCommand extends Command {
	
	private String filename;
	private String pattern = "\\s*load\\s+(.+)";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public LoadCommand() {this.type = "load"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public LoadCommand(Game game, String filename) {
		this.game = game;
		this.filename = filename;
		this.type = "load";
	}
	
	public void report() throws IOException {
		game.getPlayer(1).sendCommand(filename + " loaded as new input.\n");
	}
	
	public void execute() throws IOException {
		game.load(filename);
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
		
			try {
				return new LoadCommand(game, matcher.group(1));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the seed command. */
class SeedCommand extends Command {
	
	private long n;
	private String pattern = "\\s*seed\\s+(\\d+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public SeedCommand() {this.type = "seed"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public SeedCommand(Game game, long n) {
		this.game = game;
		this.n = n;
		this.type = "seed";
	}
	
	public void report() throws IOException {
		if (game.getPlayer(2).type().equals("Machine") || game.getPlayer(1).type().equals("Machine")) game.getPlayer(1).sendCommand("Seed set to " + n + " value.\n");
		else game.getPlayer(1).sendCommand("No automatic players on which to set seed.\n");
	}
	
	public void execute() throws IOException {
		game.seed(n);
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			
			try {
				return new SeedCommand(game, Long.parseLong(matcher.group(1)));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid numer value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the move command as typed into the terminal. */
class MoveCommand extends Command {
	
	private int r;
	private int c;
	private String pattern = "\\s*(\\d+):(\\d+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public MoveCommand() {this.type = "move"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public MoveCommand(Game game, Player sentPlayer, Player otherPlayer, Color color, int r, int c) {
		this.game = game;
		this.sentPlayer = sentPlayer;
		this.otherPlayer = otherPlayer;
		this.color = color;
		this.r = r;
		this.c = c;
		this.type = "move";
	}
	
	/** Informs the user of the move and sends the move accross if we are in remote mode. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand(color.toString() + " spot added at " + r + ":" + c + "\n");
		if (game.inRemoteMode()) {
			game.getPlayer(2).sendCommand(r + ":" + c);
		}
	}
	
	/** Makes the move using game's move method. */
	public void execute() throws IOException {
		game.move(r, c, color);
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			try {
				return new MoveCommand(game, sentPlayer, otherPlayer, color, Integer.parseInt(matcher.group(1)), Integer.parseInt(matcher.group(2)));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents the move commands as received from remote players. */
class RemoteMoveCommand extends Command {
	
	private int r;
	private int c;
	private String pattern = "\\s*(\\d+):(\\d+)\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public RemoteMoveCommand() {this.type = "move"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public RemoteMoveCommand(Game game, Player sentPlayer, Player otherPlayer, Color color, int r, int c) {
		this.game = game;
		this.sentPlayer = sentPlayer;
		this.otherPlayer = otherPlayer;
		this.color = color;
		this.r = r;
		this.c = c;
		this.type = "move";
	}
	
	/** Reports the move to the local user.*/
	public void report() throws IOException {
		game.getPlayer(1).sendCommand(color.toString() + " spot added at " + r + ":" + c + "\n");
	}
	
	/** Makes the move using game's move method. */
	public void execute() throws IOException {
		game.move(r, c, color);
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			try {
				return new RemoteMoveCommand(game, sentPlayer, otherPlayer, color, Integer.parseInt(matcher.group(1)), Integer.parseInt(matcher.group(2)));
			} catch (NumberFormatException e) {
				throw new IllegalArgumentException("The move could not be parsed into a valid R:C value.");
			}
			
		} else {
			return null;
		}
	}
}

/** Represents an empty string input. */
class NullCommand extends Command {
	
	private String pattern = "\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public NullCommand() {this.type = "null"; }
	
	public void report() throws IOException {
		
	}
	
	public void execute() throws IOException {
		
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {
			return new NullCommand();
		} else {
			return null;
		}
	}
}

/** Represents the clear command recieved to signify that we are connected to a remote computer.*/
class ClearToConnectCommand extends Command {
	
	private String pattern = "\\s*clear\\s*";
	
	/** Blank constructor for 'sentinal' instance that creates other instances of this command */
	public ClearToConnectCommand() {this.type = "clear"; }
	
	/** Constructor for specific instance that represents this command with data associated with game.*/
	public ClearToConnectCommand(Game game) {
		this.game = game;
		this.type = "clear";
	}
	
	/** Set the next expected command to 'game' and sets the connection to true.*/
	public void execute() {
		game.setConnected(true);
	}
	
	/** Informs the local player that a remote player has connected. */
	public void report() throws IOException {
		game.getPlayer(1).sendCommand("Remote player has connected.\n");
	}
	
	/** Returns a specific class if the given argument is valid */
	public Command parse(String unparsedCommand, Game game, Player sentPlayer, Player otherPlayer, Color color) {
		Matcher matcher = Pattern.compile(pattern).matcher(unparsedCommand);
		
		if (matcher.matches()) {		
			return new ClearToConnectCommand(game);
		} else {
			return null;
		}
	}
}