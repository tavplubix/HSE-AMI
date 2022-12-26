import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Scanner;


public class Main {
    public static void main(String[] args) {
        readData();
        solve();
        System.out.println(aIdx);
        System.out.println(bIdx);
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
        B = readIntArray(scanner, size);
        scanner.close();

    }

    private static void solve() {
        int curMax = Integer.MIN_VALUE;
        for (int i = 0; i < A.length; ++i) {
            if (curMax < A[i])
                curMax = A[i];
            A[i] = curMax;
        }

        int maxSum = Integer.MIN_VALUE;
        bIdx = 0;
        for (int i = 0; i < B.length; ++i) {
            int sum = A[i] + B[i];
            if (maxSum < sum) {
                maxSum = sum;
                bIdx = i;
            }
        }

        maxSum -= B[bIdx];
        aIdx = 0;
        while (aIdx < A.length) {
            if (A[aIdx] == maxSum)
                break;
            ++aIdx;
        }
    }

    private static int[] A;
    private static int[] B;
    private static int aIdx;
    private static int bIdx;
}



