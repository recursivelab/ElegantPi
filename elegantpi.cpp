/*
    ElegantPi - The elegant software for computing number pi.
    Copyright (c) by Nedeljko Stefanovic (2014).

    This is free software. You are free to use, copy, modify and distribute it
    until this notice is present and all modified versions are marked as
    software different than original.

    THIS SOWTWARE IS PROVIDED "AS IS" WITH NO WARRANTY OF ANY KIND.
*/

#include <cmath>
#include <cstdlib>
#include <fstream>
#include <iostream>
#include <iomanip>
#include <vector>
#include <sys/timeb.h>

using namespace std;

double elapsed()
{
    static bool started = false;
    static timeb start;

    if (started == false) {
        ftime(&start);
        started = true;
    }

    timeb now;

    ftime(&now);

    return now.time - start.time + 0.001*(now.millitm - start.millitm);
}

void printNum(int num, int digits, char c = ' ')
{
    if (num == 0) {
        for (int i = 1; i < digits; ++i) {
            cout << c;
        }

        cout << 0;

        return;
    }

    int max = 1;

    for (int i = 1; i < digits; ++i) {
        max *= 10;
    }

    while (num < max && digits > 1) {
        --digits;
        max /= 10;
        cout << c;
    }

    cout << num;
}

void printTime(double t)
{
    int sec = t;
    int min = sec/60;

    sec -= min*60;

    int h = min/60;

    min -= h*60;

    int d = h/24;

    h -= d*24;

    if (d > 0) {
        cout << d << " ";
    }

    printNum(h, 2, '0');
    cout << ":";
    printNum(min, 2, '0');
    cout << ":";
    printNum(sec, 2, '0');
}

typedef long long integer;
typedef vector<integer> num;

unsigned int digits;
integer base = 10;
int groupBy = 5;
int perLine = 50;

void add(num &dest, const num &what)
{
    for (size_t i = digits; i > 0; --i) {
        dest[i] += what[i];

        if (dest[i] >= base) {
            dest[i] -= base;
            ++dest[i - 1];
        }
    }

    dest[0] += what[0];
}

void sub(num &dest, const num &what)
{
    for (size_t i = digits; i > 0; --i) {
        if (dest[i] >= what[i]) {
            dest[i] -= what[i];
        } else {
            dest[i] += base - what[i];
            --dest[i - 1];
        }
    }

    dest[0] -= what[0];
}

void mul(num &dest, const num &what, integer by)
{
    integer carrier = 0;

    for (size_t i = digits; i > 0; --i) {
        long long product = (what[i] + base*carrier)*by;

        carrier = product/base;
        dest[i] = product - carrier*base;
    }

    dest[0] = what[0]*by;
}

void div(num &dest, const num &what, integer by, size_t &pos)
{
    long long carrier = 0;

    for (size_t i = pos; i <= digits; ++i) {
        long long numerator = base*carrier + what[i];

        dest[i] = numerator/by;
        carrier = numerator - dest[i]*(long long)by;
    }

    while (pos <= digits && dest[pos] == 0) {
        ++pos;
    }
}

integer digitsNumber(integer n)
{
    integer result = 1;

    if (n < 0) {
        ++result;
        n = -n;
    }

    while (n >= 10) {
        n /= 10;
        ++result;
    }

    return result;
}

void arctan(num &dest, integer arg, integer factor, double timeFactor, double &totalTimeProgress)
{
    bool positive = true;
    num potention(digits + 1);
    num summand(digits + 1);
    integer n = 1;
    integer multiplicant = arg*arg;
    size_t dummy = 0;
    integer digitsNum = digitsNumber(arg) + digitsNumber(factor);

    for (size_t i = 0; i <= digits; ++i) {
        dest[i] = 0;
        potention[i] = 0;
    }

    potention[0] = abs(factor);
    div(potention, potention, arg, dummy);

    int oldProgress = -1;
    int progress = 0;
    int oldTotalProgress = -1;
    double totalProgress = 0;
    size_t pos = 0;
    int oldElapsedTime = 0;
    double elapsedTime = 0;
    int oldRemainedTime = -1;
    double remainedTime = 0;

    while (true) {
        if (pos > digits) {
            progress = 100;
        }

        elapsedTime = elapsed();

        if (totalProgress > 0) {
            remainedTime = elapsedTime*(100/totalProgress - 1);
        }

        if (progress > oldProgress || (int) totalProgress > oldTotalProgress || (int) elapsedTime > oldElapsedTime || (int) remainedTime < oldRemainedTime) {
            cout << "\r";
            cout << factor << "*arctan(1/" << arg << ')';

            for (integer i = digitsNum; i < 20; ++i) {
                cout << ' ';
            }

            printNum(progress, 3);
            cout << '%' << " ";
            printNum(totalProgress, 3);
            cout << '%' << flush;
            oldProgress = progress;
            oldTotalProgress = totalProgress;

            if (totalProgress > 0) {
                cout << " elapsed : ";
                printTime(elapsedTime);
                cout << " remained : ";
                printTime(remainedTime);
                cout << flush;
                oldElapsedTime = elapsedTime;
                oldRemainedTime = remainedTime;
            }
        }

        if (pos > digits) {
            break;
        }

        div(summand, potention, n, pos);

        if (positive) {
            add(dest, summand);
        } else {
            sub(dest, summand);
        }

        div(potention, potention, multiplicant, dummy);
        n += 2;
        positive = positive == false;

        double realProgress = (2.0*digits - pos + 1)*pos*100.0/(digits*(digits + 1.0));

        progress = realProgress;
        totalProgress = totalTimeProgress + realProgress*timeFactor;
    }

    totalTimeProgress += 100*timeFactor;
    cout << endl;
}

double factor(double x)
{
    return 1/log(x);
}

int main()
{
    cout << "ElegantPi - The elegant software for computing number pi.\n"
        "Copyright (c) by Nedeljko Stefanovic (2014).\n\n"
        "This is free software. You are free to use, copy, modify and distribute it\n"
        "until this notice is present and all modified versions are marked as\n"
        "software different than original.\n\n"
        "THIS SOWTWARE IS PROVIDED \"AS IS\" WITH NO WARRANTY OF ANY KIND.\n\n\n";
    cout << "How many digits? " << flush;
    cin >> digits;
    cout << endl;

    ofstream outFile("pi.txt");
    ostream &out = outFile;
    integer b = base;
    unsigned int d = (digits + groupBy - 1)/groupBy;
    double totalTime = 0;
    int coefficients[] =
    {
        4,
        4, 5,
        -1, 239
    };

    unsigned int size = sizeof(coefficients)/sizeof(int);

    for (unsigned int i = 2; i < size; i += 2) {
        totalTime += factor(coefficients[i]);
    }

    base = 1;

    for (int i = 0; i < groupBy; ++i) {
        base *= b;
    }

    perLine = (perLine + groupBy - 1)/groupBy;
    digits = (digits + groupBy - 1 + 64)/groupBy;
    num result(digits + 1);
    num summand(digits + 1);
    double time = 0;

    for (unsigned int i = 0; i <= digits; ++i) {
        result[i] = 0;
    }

    elapsed();

    for (unsigned int i = 2; i < size; i += 2) {
        arctan(summand, coefficients[i], coefficients[i - 1]*coefficients[0], factor(coefficients[i])/totalTime, time);

        if (coefficients[i - 1] > 0) {
            add(result, summand);
        } else {
            sub(result, summand);
        }
    }

    out << result[0] << '.';

    int remainder = 1;

    for (size_t i = 1; i <= d; ++i) {
        if (remainder == 1 && i > 1) {
            out << "\n  ";
        }

        out << setw(groupBy) << setfill('0') << result[i];

        if (i < d && remainder != 0) {
            out << " ";

            if ((i&1) == 0) {
                out << " ";
            }
        }

        ++remainder;

        if (remainder == perLine) {
            remainder = 0;
        }
    }

    out << endl;

    if (d%perLine != 0 && &out == &cout) {
        cout << endl;
    }

    return EXIT_SUCCESS;
}
