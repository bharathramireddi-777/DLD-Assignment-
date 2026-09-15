#include <bits/stdc++.h>
using namespace std;

struct Term
{
    string pattern;
    vector<int> minterms;
};

int n;
int rows, cols;
vector<int> ones;
vector<Term> primeTerms;

int gray(int x)
{
    return x ^ (x >> 1);
}

bool validTerm(string pattern)
{
    vector<int> covered;

    for (int m = 0; m < (1 << n); m++)
    {
        bool match = true;

        for (int i = 0; i < n; i++)
        {
            int bit = (m >> (n - 1 - i)) & 1;

            if (pattern[i] != '-' && pattern[i] - '0' != bit)
            {
                match = false;
                break;
            }
        }

        if (match)
            covered.push_back(m);
    }

    if (covered.size() == 0)
        return false;

    for (int x : covered)
    {
        if (find(ones.begin(), ones.end(), x) == ones.end())
            return false;
    }

    return true;
}

bool moreGeneral(string a, string b)
{
    for (int i = 0; i < n; i++)
    {
        if (a[i] != '-' && a[i] != b[i])
            return false;
    }

    return a != b;
}

string patternToExpression(string pattern)
{
    string result = "";
    char variables[] = {'a', 'b', 'c', 'd'};

    for (int i = 0; i < n; i++)
    {
        if (pattern[i] == '-')
            continue;

        result += variables[i];

        if (pattern[i] == '0')
            result += "'";
    }

    return result;
}

vector<int> getMinterms(string pattern)
{
    vector<int> result;

    for (int m = 0; m < (1 << n); m++)
    {
        bool match = true;

        for (int i = 0; i < n; i++)
        {
            int bit = (m >> (n - 1 - i)) & 1;

            if (pattern[i] != '-' && pattern[i] - '0' != bit)
            {
                match = false;
                break;
            }
        }

        if (match)
            result.push_back(m);
    }

    return result;
}

void findPrimeTerms()
{
    vector<string> allTerms;

    int total = 1;

    for (int i = 0; i < n; i++)
        total *= 3;

    for (int number = 0; number < total; number++)
    {
        int x = number;
        string pattern = "";

        for (int i = 0; i < n; i++)
        {
            int digit = x % 3;
            x /= 3;

            if (digit == 0)
                pattern += '0';
            else if (digit == 1)
                pattern += '1';
            else
                pattern += '-';
        }

        reverse(pattern.begin(), pattern.end());

        if (validTerm(pattern))
            allTerms.push_back(pattern);
    }

    for (string p : allTerms)
    {
        bool prime = true;

        for (string q : allTerms)
        {
            if (p != q && moreGeneral(q, p))
            {
                prime = false;
                break;
            }
        }

        if (prime)
        {
            Term t;
            t.pattern = p;
            t.minterms = getMinterms(p);

            primeTerms.push_back(t);
        }
    }
}

int bestTerms = 100;
int bestLiterals = 100;
vector<vector<int>> answers;

void findCovers(vector<bool> covered, vector<int> selected)
{
    bool complete = true;

    for (int i = 0; i < (int)ones.size(); i++)
    {
        if (!covered[i])
        {
            complete = false;
            break;
        }
    }

    if (complete)
    {
        int termCount = selected.size();
        int literals = 0;

        for (int index : selected)
        {
            for (char c : primeTerms[index].pattern)
            {
                if (c != '-')
                    literals++;
            }
        }

        if (termCount < bestTerms ||
            (termCount == bestTerms && literals < bestLiterals))
        {
            bestTerms = termCount;
            bestLiterals = literals;
            answers.clear();
            answers.push_back(selected);
        }
        else if (termCount == bestTerms &&
                 literals == bestLiterals)
        {
            answers.push_back(selected);
        }

        return;
    }

    if ((int)selected.size() >= bestTerms)
        return;

    int target = -1;

    for (int i = 0; i < (int)ones.size(); i++)
    {
        if (!covered[i])
        {
            target = i;
            break;
        }
    }

    int targetMinterm = ones[target];

    for (int i = 0; i < (int)primeTerms.size(); i++)
    {
        if (find(selected.begin(), selected.end(), i) != selected.end())
            continue;

        bool covers = false;

        for (int m : primeTerms[i].minterms)
        {
            if (m == targetMinterm)
            {
                covers = true;
                break;
            }
        }

        if (!covers)
            continue;

        vector<bool> newCovered = covered;

        for (int j = 0; j < (int)ones.size(); j++)
        {
            for (int m : primeTerms[i].minterms)
            {
                if (ones[j] == m)
                    newCovered[j] = true;
            }
        }

        selected.push_back(i);

        findCovers(newCovered, selected);

        selected.pop_back();
    }
}

int main()
{
    ifstream file("input.txt");

    if (!file)
    {
        cout << "Error: Could not open input.txt\n";
        return 0;
    }

    file >> n;

    if (n < 1 || n > 4)
    {
        cout << "Number of variables must be between 1 and 4.\n";
        return 0;
    }

    int rowVariables = n / 2;
    int colVariables = n - rowVariables;

    rows = 1 << rowVariables;
    cols = 1 << colVariables;

    vector<vector<int>> kmap(rows, vector<int>(cols));

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            file >> kmap[i][j];
        }
    }

    file.close();

    for (int i = 0; i < rows; i++)
    {
        for (int j = 0; j < cols; j++)
        {
            if (kmap[i][j] == 1)
            {
                int rowValue = gray(i);
                int colValue = gray(j);

                int minterm = (rowValue << colVariables) | colValue;

                ones.push_back(minterm);
            }
        }
    }

    if (ones.size() == 0)
    {
        cout << "Minimized expression: 0\n";
        return 0;
    }

    if (ones.size() == (1 << n))
    {
        cout << "Minimized expression: 1\n";
        return 0;
    }

    findPrimeTerms();

    vector<bool> covered(ones.size(), false);
    vector<int> selected;

    findCovers(covered, selected);

    cout << "Possible minimized Boolean expressions:\n\n";

    for (int i = 0; i < (int)answers.size(); i++)
    {
        cout << i + 1 << ". ";

        for (int j = 0; j < (int)answers[i].size(); j++)
        {
            if (j > 0)
                cout << " + ";

            cout << patternToExpression(
                primeTerms[answers[i][j]].pattern
            );
        }

        cout << "\n";
    }

    return 0;
}
