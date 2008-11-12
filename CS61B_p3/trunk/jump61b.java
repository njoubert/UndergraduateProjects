/* Jump61b Game */

/* This file simply starts the "real" jump61b game in the 'game' package.
 * It is not intended to be changed. */

import game.Main;

/**
 * 	<CENTER><H2>		   The Jump61b Game</H2>
		      <H4>Users' Guide and Reference</H4>

			  <H4> *Niels Joubert CS61B-BO*</H4></CENTER>

<P>Welcome to Jump61B, my implementation of the award-winning, 
critically acclaimed Critical Mass game, also manifested in
such popular forms as KJumpingCube, available for Ubuntu
SUSE, Fedora, MEPIS, Damn Small, Debian, KNOPPIX, Gentoo, 
PCLinuxOS, Slackware, FreeBSD, Vector, Kubuntu, KANOTIX, 
CentOS, Xandros, SLAX, Puppy, Arch, Zenwalk, PC-BSD, 
RR4/RR64, Linspire, Red Hat, DesktopBSD, Ark, Foresight, Linux XP, 
Kororaa, FoX Desktop, Frugalware, Solaris, Elive, BackTrack and	VLOS.
We are hoping to release a Windows version in about 2 hours from now.</P>

<P>Jump61b is a strategic board game involving no element of chance.
Two players - Red and Blue - place spots on the squares of a NxN board. 
Whenever a square becomes overfull - when it has more spots than neighbouring
squares - its spots jump to each of its neighbors, taking the current square's
color with it. The aim of the game is to eliminate your opponent's color from the board
and leaving no squares for him to place spots on.</P>

<P>Initially, the board is filled with all white squares, one spot per square.
White squares signifies that it does not belong to any user, and can thus 
be added to by any player, changing it to their respective color.
The RED player always moves first, and by default you, as the user, is the
red player.</P>

<P>Let's first look at the different ways of using this implementation.</P>

<H3>LOCAL PLAYING</H3>
<P>
By default, the game is in local mode, with you as player 1, and the game's
brain engine playing against you. You can also play the game's own brain 
against itself. 
</P><P>
Initially the game is in SET-UP state, allowing you to make moves
for both players to set up the board. When the game is started, the
source of moves is alternated between the two players - in local playing, 
between the user and the brain, or between two brains if set up in that way.
</P><P>
Once a player wins - by making the whole board his color - we enter
finished state, where you can happily gloat over your win, until
you start another game, or quit.
</P>
<H3>REMOTE PLAYING</H3>
<P>
You can match yourself against another implementation of Jump61B
over a network (or two games on the same computer). 
One instance of the game hosts, while the other player is the client.
</P>
<H4>HOSTING</H4>
	<P>As host, you will open the remote connections, and control the game.
	Once you issue the host command, your game waits for a remote connection.
	When another computer connects, you need to enter the game command to
	set up the game and assign the player colors. You can now set-up the board
	as in normal set-up state. When you start the game, the moves
	will alternate between players.</P>
	
<H4>JOINING</H4>
	<P>As a client, all you do is play the game. The host issues the game
	command as first command to set-up your board and color. From now
	all you get to do is play the game once playing state has started.</P>

<P>Once there is a winner, the loser has to quit his game before the host 
returns to local mode and can resume other activities.</P>

<P>If anything goes wrong during remote play - the network drops out, the
remote player tries to flood you, or he quits without telling you, 
this game will do its best to become aware of this, and return you
safely to local mode.</P>


<H4>COMMANDS</H4>
<P>
<b>clear</b>				
	<br>Abandones the game, clears the board, start in set=up state.
<br><b>start</b>				
	<br>executes clear if necessary, and starts game.
<br><b>quit</b>				
	<br>abandons current game and, if not in remote mode, quits the application.
<br><b>game [size] [color]	</b>
	<br>Sets up the board size and your color for the new game.
<br><b>auto	</b>			
	<br>Sets up player one on this console as an automatic player.
<br><b>manual</b>				
	<br>Sets up player one as a manual player on this console.
<br><b>host [ID]</b>			
	<br>Sets up this program as a host with given ID for a remote game.
<br><b>join [ID]@[host]</b>	
	<br>Joins a remote game and sets this console to remote mode.
<br><b>help	</b>			
	<br>This message
<br><b>dump	</b>			
	<br>Dumps the board as a visual string representation.
<br><b>load [file]	</b>		
	<br>Substitutes the contents of [file] for console input until it is completely read.
<br><b>seed [N]	</b>		
	<br>Sets the random number generator to this seed value.

 * @author Niels Joubert CS61B-BO
 */
public class jump61b {
	
	/** Run the jump61b game, passing ARGS as the command-line argument
	 *  list. */
	public static void main(String[] args) {
		Main.main (args);
	}
	
}


