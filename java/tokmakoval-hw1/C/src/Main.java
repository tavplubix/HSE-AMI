import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Scanner;
import java.util.HashMap;
import java.util.function.BiFunction;


public class Main {
    public static void main(String[] args) {
        readData();
        int answer = solve();
        System.out.println(answer);
    }

    private static int[] readIntArray(Scanner scanner, int size) {
        int[] arr = new int[size];
        for (int i = 0; i < arr.length; ++i) {
            arr[i] = scanner.nextInt();
        }
        return arr;
    }

    private static void readData() {
        Scanner scanner = new Scanner(new BufferedReader(new InputStreamReader(System.in)));
        int size = scanner.nextInt();
        A = readIntArray(scanner, size);
        size = scanner.nextInt();
        B = readIntArray(scanner, size);
        k = scanner.nextInt();
        scanner.close();

    }

    private static int solve() {
        HashMap<Integer, Integer> valCount = new HashMap<Integer, Integer>(A.length);
        BiFunction<Integer, Integer,Integer> inc = (key, val) -> {
            if (val == null)
                return 1;
            else
                return val + 1;
        };
        for (int a : A) {
            int b = k - a;
            valCount.compute(b, inc);
        }

        int count = 0;
        for (int i = B.length - 1; i >= 0; --i)
            count += valCount.getOrDefault(B[i], 0);
        return count;
    }

    private static int k;
    private static int[] A;
    private static int[] B;
}



