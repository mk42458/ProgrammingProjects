package assignment2;
import java.util.Scanner;
import java.util.*;

public class Game {
    ArrayList<String> GuessHist;
    ArrayList<String> ResultHist;
    Scanner testMode;
    boolean testing;
    char Answer = 'Y';


    public Game(Scanner in, boolean input){
        testing = input;
        testMode = in;
        GuessHist = new ArrayList<String>();
        ResultHist = new ArrayList<String>();
    }
    public void runGame(){
        System.out.print(" Welcome to Mastermind.  Here are the rules. \n" +
                " \n" +
                "This is a text version of the classic board game Mastermind. \n" +
                " \n" +
                "The  computer  will  think  of  a  secret  code.  The  code  consists  of  4 \n" +
                "colored  pegs.  The  pegs  MUST  be  one  of  six  colors:  blue,  green, \n" +
                "orange, purple, red, or yellow. A color may appear more than once in \n" +
                "the  code.  You  try  to  guess  what  colored  pegs  are  in  the  code  and \n" +
                "what  order  they  are  in.  After  you  make  a  valid  guess  the  result \n" +
                "(feedback) will be displayed. \n" +
                " \n" +
                "The  result  consists  of  a  black  peg  for  each  peg  you  have  guessed \n" +
                "exactly correct (color and position) in your guess.  For each peg in \n" +
                "the guess that is the correct color, but is out of position, you get \n" +
                "a  white  peg.  For  each  peg,  which  is  fully  incorrect,  you  get  no \n" +
                "feedback.  \n" +
                " \n" +
                "Only the first letter of the color is displayed. B for Blue, R for \n" +
                "Red, and so forth. When entering guesses you only need to enter the \n" +
                "first character of each color as a capital letter. \n" +
                " \n" +
                "You  have  12  guesses  to  figure  out  the  secret  code  or  you  lose  the \n" +
                "game.  Are you ready to play? (Y/N): ");

        MasterMind();
    }

    public void MasterMind() {
        String No = "N";
        String s = testMode.nextLine();
        String Yes = "Y";//yes or no?
        String Random = SecretCodeGenerator.getInstance().getNewSecretCode(); //Random contains random generated string
       // System.out.println(Random);

        if (s.equals(Yes)) { //if answer is yes
            if(testing){
                System.out.println("\nGenerating secret code ...(for this example the secret code is " + Random + ")\n");
            }
            else {
                System.out.print("Generating secret code... \n");
            }
//start of loop change to while
            int i = 0;
            int[] pins = new int[2];
            int black = 0;
            int white = 0;
            while (i < GameConfiguration.guessNumber && black != GameConfiguration.pegNumber) {

                if (s.equals(Yes)) { //if answer is yes
                    System.out.print(
                            "\nYou have " + (GameConfiguration.guessNumber - i) + " guesses left. \n" + //change this to decrement with i
                                    "What is your next guess? \n" +
                                    "Type in the characters for your guess and press enter. \n" +
                                    "Enter guess: ");

                    String Guess = testMode.nextLine();

                    if (Guess.equals("HISTORY")) {
                        System.out.println();
                        History();
                        System.out.println();


                    } else {

                        boolean State = validGuess.Guess(Guess); //pass Guess that user types into the function to validate it

                        if (State == true) {
                            GuessHist.add(Guess);
                            pins = pinCount.Pins(Guess, Random);
                            black = pins[0];
                            white = pins[1];

                            String Pins = "" + black + "B_" + white + "W";
                            ResultHist.add(Pins);

// black == 4 then win
                            if (black == 4) {
                                System.out.println("\n" + Guess + " -> Result: 4B_0W - You win !!");
                                System.out.println("Are you ready for another game (Y/N): ");

                              //  s = testMode.nextLine();
                                if (s.equals(Yes)) {
                                    GuessHist.clear();
                                    ResultHist.clear();
                                    MasterMind();
                                }
                                else{
                                    break;
                                }

                            } else {
                                System.out.println("\n" + Guess + " -> Result: " + black + "B_" + white + "W");
                                i++;

                            }

                        } else {

                            System.out.print("\n" + Guess + " -> INVALID GUESS \n");


                        }

                    }
                }
                else{
                    break;
                }


            }
            if(black == 4) {
                System.exit(0);
            }
            System.out.println("Sorry, you are out of guesses. You Lose, boo-hoo.");

            System.out.println("\nAre you ready for another game (Y/N): ");


           // s = testMode.nextLine();
            if (s.equals(Yes)) {
                GuessHist.clear();
                ResultHist.clear();
                MasterMind();

            }

        }

    }



    class pinCount {
        public static int[] Pins(String guess, String random) { //black: correct location and color
            int[] count = new int[2];
            int bcount = 0;
            int wcount = 0;
            char[] GuessArr = new char[guess.length()]; //char array of size guess.length
            char[] RandomArr = new char[random.length()]; //char array of random string

            for (int i = 0; i < guess.length(); i++) {
                GuessArr[i] = guess.charAt(i);
                RandomArr[i] = random.charAt(i);
            }
//black
            for (int i = 0; i < guess.length(); i++) {

                if (GuessArr[i] == RandomArr[i]) {
                    bcount++;
                    GuessArr[i] = ' ';
                    RandomArr[i] = '$';
                }

            }

            count[0] = bcount;

            for (int i = 0; i < guess.length(); i++) {
                for (int j = 0; j < random.length(); j++) {
                    if ((i != j) && (GuessArr[i] == RandomArr[j])) {
                        wcount++;
                    }
                }
            }
            count[1] = wcount;

            return count;
            //pass in string to return int count with correct black pin values
            //turn both into char array
            //create hashmap (correct key and correct value)
            //or array which checks both the index and the value

            // return bcount;
            //change colors already accountd for
        }
    }


    class validGuess {
        public static boolean Guess(String guess) {
            //check if string is valid
            //length must be equal to number of pins
            //characters must be uppercase options available
            //easily changeable as there is a seprate class for this function
            boolean state;

            if(guess.equals("")){
                state = false;
                return state;
            }
            if (guess.length() != GameConfiguration.pegNumber) {
                //System.out.print(" invalid guess ");
                state = false;
                return state;
            }
            boolean notFound = true;
            for (int i = 0; i < guess.length(); i++) {
                int position = 0;
                notFound = true;
                while ((notFound == true) && position < GameConfiguration.colors.length) {
                    if ((GameConfiguration.colors[position]).compareTo(String.valueOf(guess.charAt(i))) == 0) {
                        notFound = false;
                        break;
                    }
                    position++;

                }
                if (position == GameConfiguration.colors.length) {
                    // System.out.print(" invalid guess ");
                    state = false;
                    return state;

                }

            }

            state = true;
            return state;
        }
    }
    public void History() {
        for(int i = 0; i < GuessHist.size(); i++){
            System.out.println(GuessHist.get(i) + "\t\t" + ResultHist.get(i));
        }

    }


}



