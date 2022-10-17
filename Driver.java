package assignment2;
import java.util.Scanner;

public class Driver {
    public static void main(String args[]) {
        boolean test = false;
        //int numTests = GameConfiguration.guessNumber;
        Scanner i = new Scanner(System.in);
        if(args.length > 0){
            if(args[0].equals("1")){
                test = true;
            }
        }
        Game g = new Game(i,test);
        g.runGame();

    }
}
