import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Scanner;
import java.util.HashMap;
import java.util.function.BiFunction;


public class Main {
    public static void main(String[] args) {
        Scanner scanner = new Scanner(new BufferedReader(new InputStreamReader(System.in)));
        int n = scanner.nextInt();
        int k = scanner.nextInt();
        int answer = solve(n, k);
        System.out.println(answer);
    }


    private static int solve(int n, int k) {
        int answ = 1;
        for (int i = 2; i <= n; ++i) {
            answ = 1 + (answ + k - 1) % i;
        }
        return answ;
    }

}



