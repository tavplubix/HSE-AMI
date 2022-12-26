import java.io.BufferedReader;
import java.io.InputStreamReader;
import java.util.Scanner;
import java.awt.Point;

public class Main {
    public static void main(String[] args) {
        readData();
        long doubledSquare = solve();
        System.out.println(doubledSquare / 2.0);
    }


    private static void readData() {
        Scanner scanner = new Scanner(new BufferedReader(new InputStreamReader(System.in)));
        int n = scanner.nextInt();
        polygon = new Point[n];
        for (int i = 0; i < n; ++i) {
            int x = scanner.nextInt();
            int y = scanner.nextInt();
            polygon[i] = new Point(x, y);
        }
        scanner.close();
    }

    private static long solve() {
        long doubledSquare = polygon[polygon.length - 1].x + polygon[0].x;
        doubledSquare *= (polygon[polygon.length - 1].y - polygon[0].y);
        for (int i = 1; i < polygon.length; ++i) {
            long m = polygon[i - 1].x + polygon[i].x;
            doubledSquare += m * (polygon[i - 1].y - polygon[i].y);
        }
        return doubledSquare;
    }

    private static Point[] polygon;
}



