#include <iostream>
#include <fstream>
#include <omp.h>
#include <random>
#include <string>

using namespace std;


random_device rd;
mt19937 gen(time(NULL));

long double solveOMP(int r, int n) {
    long double L = 4 * r * r;
    int sum[1024];
    fill(sum, sum + 1024, 0);
    int i;
    uniform_real_distribution<long double> dist(-r, r);
#pragma omp parallel for shared(sum) private(i) schedule(dynamic)
    for (i = 0; i < n; i++) {
        long double x = dist(gen);
        long double y = dist(gen);
        if (sqrt(x * x + y * y) <= r) {
            sum[omp_get_thread_num()] += 1;
        }
    }
    int m = 0;
    for (i = 0; i < 1024; i++) {
        m += sum[i];
    }
    return L * m / n;
}


long double solveOMP2(int r, int n) {
    long double L = 4 * r * r;
    int i;
    int m = 0;
    uniform_real_distribution<long double> dist(-r, r);
#pragma omp parallel for shared(m) private(i) schedule(dynamic, 17)
    for (i = 0; i < n; i++) {
        long double x = dist(gen);
        long double y = dist(gen);
        if (sqrt(x * x + y * y) <= r) {
#pragma omp atomic
            m++;
        }
    }
    return L * m / n;
}


long double solve(int r, int n) {
    long double L = 4 * r * r;
    int sum[1024];
    fill(sum, sum + 1024, 0);
    int i;
    uniform_real_distribution<long double> dist(-r, r);
    for (i = 0; i < n; i++) {
        long double x = dist(gen);
        long double y = dist(gen);
        if (sqrt(x * x + y * y) <= r) {
            sum[omp_get_thread_num()] += 1;
        }
    }
    int m = 0;
    for (i = 0; i < 1024; i++) {
        m += sum[i];
    }
    return L * m / n;
}


int main(int argc, char *argv[])
{
    if (argc != 4) {
        cout << "Please, follow this format: omp4 <кол-во_потоков> <имя_входного_файла> <имя_выходного_файла>" << endl;
        return 0;
    }
    if (stoi(argv[1]) > 0) {
        omp_set_dynamic(0);
        omp_set_num_threads(stoi(argv[1]));
    }
    ifstream in(argv[2]);
    int r = 0, n = 0;
    if (in.is_open()) {
        in >> r >> n;
    }
    in.close();
    long double ans = 0;
    double timer1 = omp_get_wtime();
    if (stoi(argv[1]) == -1) {
        ans = solve(r, n);
    } else {
        ans = solveOMP(r, n);
    }
    double timer2 = omp_get_wtime();
    if (stoi(argv[1]) == -1) {
        printf("Time (1 thread(s)): %g ms\n", timer2 - timer1);
    } else {
        printf("Time (%i thread(s)): %g ms\n", omp_get_max_threads(), timer2 - timer1);
    }

    ofstream out(argv[3]);
    if (out.is_open()) {
        out << ans << endl;
    }
    return 0;
}
