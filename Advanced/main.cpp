#include <iostream>
#include <fstream>
#include <vector>
#include <set>
#include <map>
#include <queue>
#include <string>
#include <algorithm>
#include <numeric>
#include <functional>
#include <cstdlib>
#include <ctime>

using namespace std;

class DFA {
private:
    vector<char> alphabet;
    int stateCount;
    int startState;
    set<int> acceptStates;
    vector<vector<int>> trans;

public:
    DFA() {
        stateCount = 0;
        startState = 1;
    }

    int getCharIndex(char c) const {
        for (int i = 0; i < (int)alphabet.size(); i++) {
            if (alphabet[i] == c) return i;
        }
        return -1;
    }

    bool readFromFile(const string& filename) {
        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "Error: cannot open file " << filename << endl;
            return false;
        }

        string alpha;
        int acceptCount;

        fin >> alpha;
        alphabet.assign(alpha.begin(), alpha.end());

        fin >> stateCount;
        fin >> startState;
        fin >> acceptCount;

        acceptStates.clear();
        for (int i = 0; i < acceptCount; i++) {
            int s;
            fin >> s;
            acceptStates.insert(s);
        }

        trans.assign(stateCount + 1, vector<int>(alphabet.size(), -1));

        for (int s = 1; s <= stateCount; s++) {
            for (int j = 0; j < (int)alphabet.size(); j++) {
                fin >> trans[s][j];
            }
        }

        fin.close();
        return true;
    }

    void readFromTerminal() {
        string alpha;
        int acceptCount;

        cout << "Enter alphabet, such as ab: ";
        cin >> alpha;
        alphabet.assign(alpha.begin(), alpha.end());

        cout << "Enter number of states: ";
        cin >> stateCount;

        cout << "Enter start state: ";
        cin >> startState;

        cout << "Enter number of accept states: ";
        cin >> acceptCount;

        acceptStates.clear();
        cout << "Enter accept states: ";
        for (int i = 0; i < acceptCount; i++) {
            int s;
            cin >> s;
            acceptStates.insert(s);
        }

        trans.assign(stateCount + 1, vector<int>(alphabet.size(), -1));

        cout << "Enter transition table." << endl;
        for (int s = 1; s <= stateCount; s++) {
            for (int j = 0; j < (int)alphabet.size(); j++) {
                cout << "State " << s << " with '" << alphabet[j] << "' -> ";
                cin >> trans[s][j];
            }
        }
    }

    bool validate() const {
        if (alphabet.empty()) {
            cerr << "Invalid DFA: alphabet is empty." << endl;
            return false;
        }

        set<char> seen;
        for (char c : alphabet) {
            if (seen.count(c)) {
                cerr << "Invalid DFA: duplicated symbol '" << c << "' in alphabet." << endl;
                return false;
            }
            seen.insert(c);
        }

        if (stateCount <= 0) {
            cerr << "Invalid DFA: number of states must be positive." << endl;
            return false;
        }

        if (startState < 1 || startState > stateCount) {
            cerr << "Invalid DFA: start state is not in state set." << endl;
            return false;
        }

        if (acceptStates.empty()) {
            cerr << "Invalid DFA: accept state set is empty." << endl;
            return false;
        }

        for (int s : acceptStates) {
            if (s < 1 || s > stateCount) {
                cerr << "Invalid DFA: accept state " << s << " is not in state set." << endl;
                return false;
            }
        }

        for (int s = 1; s <= stateCount; s++) {
            for (int j = 0; j < (int)alphabet.size(); j++) {
                int nextState = trans[s][j];
                if (nextState < 1 || nextState > stateCount) {
                    cerr << "Invalid DFA: state " << s
                         << " with symbol '" << alphabet[j]
                         << "' goes to illegal state " << nextState << "." << endl;
                    return false;
                }
            }
        }

        return true;
    }

    void printInfo() const {
        cout << endl;
        cout << "========== DFA Information ==========" << endl;

        cout << "Alphabet: { ";
        for (char c : alphabet) cout << c << " ";
        cout << "}" << endl;

        cout << "States: { ";
        for (int i = 1; i <= stateCount; i++) cout << i << " ";
        cout << "}" << endl;

        cout << "Start state: " << startState << endl;

        cout << "Accept states: { ";
        for (int s : acceptStates) cout << s << " ";
        cout << "}" << endl;

        cout << "Transition table:" << endl;
        cout << "State";
        for (char c : alphabet) cout << "\t" << c;
        cout << endl;

        for (int s = 1; s <= stateCount; s++) {
            cout << s;
            for (int j = 0; j < (int)alphabet.size(); j++) {
                cout << "\t" << trans[s][j];
            }
            cout << endl;
        }
    }

    bool judgeString(const string& input, bool showTrace = true) const {
        int current = startState;

        if (showTrace) {
            cout << "Start state: " << current << endl;
        }

        for (char c : input) {
            int index = getCharIndex(c);

            if (index == -1) {
                if (showTrace) {
                    cout << "Read '" << c << "': symbol is not in alphabet. Reject." << endl;
                }
                return false;
            }

            int nextState = trans[current][index];

            if (showTrace) {
                cout << "Read '" << c << "': "
                     << current << " -> " << nextState << endl;
            }

            current = nextState;
        }

        if (showTrace) {
            cout << "Final state: " << current << endl;
            if (acceptStates.count(current)) {
                cout << "Result: ACCEPT" << endl;
            } else {
                cout << "Result: REJECT" << endl;
            }
        }

        return acceptStates.count(current);
    }

    void generateAcceptedStrings(int maxLength) const {
        cout << endl;
        cout << "========== Accepted Strings With Length <= "
             << maxLength << " ==========" << endl;

        queue<pair<string, int>> q;
        q.push({"", startState});

        bool found = false;

        while (!q.empty()) {
            pair<string, int> item = q.front();
            q.pop();

            string str = item.first;
            int state = item.second;

            if ((int)str.length() <= maxLength && acceptStates.count(state)) {
                if (str.empty()) cout << "ε" << endl;
                else cout << str << endl;
                found = true;
            }

            if ((int)str.length() == maxLength) {
                continue;
            }

            for (int i = 0; i < (int)alphabet.size(); i++) {
                char c = alphabet[i];
                int nextState = trans[state][i];
                q.push({str + c, nextState});
            }
        }

        if (!found) {
            cout << "No accepted string." << endl;
        }
    }

    vector<int> getReachableStates() const {
        vector<int> visited(stateCount + 1, 0);
        queue<int> q;

        q.push(startState);
        visited[startState] = 1;

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int nextState : trans[u]) {
                if (!visited[nextState]) {
                    visited[nextState] = 1;
                    q.push(nextState);
                }
            }
        }

        vector<int> result;
        for (int i = 1; i <= stateCount; i++) {
            if (visited[i]) result.push_back(i);
        }

        return result;
    }

    vector<int> getProductiveStates() const {
        vector<vector<int>> reverseGraph(stateCount + 1);

        for (int s = 1; s <= stateCount; s++) {
            for (int nextState : trans[s]) {
                reverseGraph[nextState].push_back(s);
            }
        }

        vector<int> productive(stateCount + 1, 0);
        queue<int> q;

        for (int s : acceptStates) {
            productive[s] = 1;
            q.push(s);
        }

        while (!q.empty()) {
            int u = q.front();
            q.pop();

            for (int prev : reverseGraph[u]) {
                if (!productive[prev]) {
                    productive[prev] = 1;
                    q.push(prev);
                }
            }
        }

        vector<int> result;
        for (int i = 1; i <= stateCount; i++) {
            if (productive[i]) result.push_back(i);
        }

        return result;
    }

    void analyzeStates() const {
        cout << endl;
        cout << "========== Reachability and Dead State Analysis ==========" << endl;

        vector<int> reachable = getReachableStates();
        vector<int> productive = getProductiveStates();

        vector<int> isReachable(stateCount + 1, 0);
        vector<int> isProductive(stateCount + 1, 0);

        for (int s : reachable) isReachable[s] = 1;
        for (int s : productive) isProductive[s] = 1;

        cout << "Reachable states: ";
        for (int s : reachable) cout << s << " ";
        cout << endl;

        cout << "Unreachable states: ";
        bool hasUnreachable = false;
        for (int i = 1; i <= stateCount; i++) {
            if (!isReachable[i]) {
                cout << i << " ";
                hasUnreachable = true;
            }
        }
        if (!hasUnreachable) cout << "None";
        cout << endl;

        cout << "States that can reach an accept state: ";
        for (int s : productive) cout << s << " ";
        cout << endl;

        cout << "Dead states among reachable states: ";
        bool hasDead = false;
        for (int i = 1; i <= stateCount; i++) {
            if (isReachable[i] && !isProductive[i]) {
                cout << i << " ";
                hasDead = true;
            }
        }
        if (!hasDead) cout << "None";
        cout << endl;
    }

    void minimizeDFA() const {
        cout << endl;
        cout << "========== DFA Minimization Analysis ==========" << endl;

        vector<vector<int>> marked(stateCount + 1, vector<int>(stateCount + 1, 0));

        for (int i = 1; i <= stateCount; i++) {
            for (int j = 1; j < i; j++) {
                bool iAccept = acceptStates.count(i);
                bool jAccept = acceptStates.count(j);

                if (iAccept != jAccept) {
                    marked[i][j] = 1;
                }
            }
        }

        bool changed = true;

        while (changed) {
            changed = false;

            for (int i = 1; i <= stateCount; i++) {
                for (int j = 1; j < i; j++) {
                    if (marked[i][j]) continue;

                    for (int a = 0; a < (int)alphabet.size(); a++) {
                        int p = trans[i][a];
                        int q = trans[j][a];

                        if (p == q) continue;

                        int x = max(p, q);
                        int y = min(p, q);

                        if (marked[x][y]) {
                            marked[i][j] = 1;
                            changed = true;
                            break;
                        }
                    }
                }
            }
        }

        vector<int> parent(stateCount + 1);
        iota(parent.begin(), parent.end(), 0);

        function<int(int)> findRoot = [&](int x) {
            if (parent[x] == x) return x;
            parent[x] = findRoot(parent[x]);
            return parent[x];
        };

        auto mergeSet = [&](int a, int b) {
            int ra = findRoot(a);
            int rb = findRoot(b);
            if (ra != rb) parent[rb] = ra;
        };

        for (int i = 1; i <= stateCount; i++) {
            for (int j = 1; j < i; j++) {
                if (!marked[i][j]) {
                    mergeSet(i, j);
                }
            }
        }

        map<int, vector<int>> groups;

        for (int i = 1; i <= stateCount; i++) {
            groups[findRoot(i)].push_back(i);
        }

        int classId = 0;
        map<int, int> stateToClass;

        for (auto& item : groups) {
            classId++;

            cout << "C" << classId << " = { ";
            for (int s : item.second) {
                cout << s << " ";
                stateToClass[s] = classId;
            }
            cout << "}" << endl;
        }

        cout << "Number of states after minimization: " << groups.size() << endl;
        cout << "Start class: C" << stateToClass[startState] << endl;

        cout << "Accept classes: ";
        set<int> acceptClasses;
        for (int s : acceptStates) {
            acceptClasses.insert(stateToClass[s]);
        }

        for (int c : acceptClasses) {
            cout << "C" << c << " ";
        }
        cout << endl;
    }

    void exportDOT(const string& filename) const {
        ofstream fout(filename);

        fout << "digraph DFA {" << endl;
        fout << "  rankdir=LR;" << endl;
        fout << "  node [shape=circle];" << endl;
        fout << "  start [shape=point];" << endl;
        fout << "  start -> q" << startState << ";" << endl;

        for (int s = 1; s <= stateCount; s++) {
            fout << "  q" << s << " [label=\"" << s << "\"";

            if (acceptStates.count(s)) {
                fout << ", shape=doublecircle";
            }

            fout << "];" << endl;
        }

        map<pair<int, int>, string> edgeLabels;

        for (int s = 1; s <= stateCount; s++) {
            for (int j = 0; j < (int)alphabet.size(); j++) {
                int nextState = trans[s][j];
                edgeLabels[{s, nextState}] += string(1, alphabet[j]) + ",";
            }
        }

        for (auto& item : edgeLabels) {
            int from = item.first.first;
            int to = item.first.second;
            string label = item.second;

            if (!label.empty()) {
                label.pop_back();
            }

            fout << "  q" << from << " -> q" << to
                 << " [label=\"" << label << "\"];" << endl;
        }

        fout << "}" << endl;
        fout.close();

        cout << "DOT file exported: " << filename << endl;
    }

    string randomString(int maxLength) const {
        if (alphabet.empty()) return "";

        int len = rand() % (maxLength + 1);
        string result;

        for (int i = 0; i < len; i++) {
            int index = rand() % alphabet.size();
            result.push_back(alphabet[index]);
        }

        return result;
    }

    void randomTest(int count, int maxLength) const {
        cout << endl;
        cout << "========== Random String Test ==========" << endl;

        for (int i = 0; i < count; i++) {
            string s = randomString(maxLength);
            bool accepted = judgeString(s, false);

            cout << (s.empty() ? "ε" : s)
                 << " -> "
                 << (accepted ? "ACCEPT" : "REJECT")
                 << endl;
        }
    }
};

int main() {
    srand((unsigned)time(nullptr));

    DFA dfa;

    cout << "========== DFA Simulator with Advanced Analysis ==========" << endl;
    cout << "1. Read DFA from file" << endl;
    cout << "2. Input DFA from terminal" << endl;
    cout << "Choose input mode: ";

    int mode;
    cin >> mode;

    if (mode == 1) {
        string filename;
        cout << "Enter DFA file name: ";
        cin >> filename;

        if (!dfa.readFromFile(filename)) {
            return 1;
        }
    } else if (mode == 2) {
        dfa.readFromTerminal();
    } else {
        cout << "Invalid mode." << endl;
        return 1;
    }

    if (!dfa.validate()) {
        cout << "DFA validation failed. Program stopped." << endl;
        return 1;
    }

    cout << "DFA validation passed." << endl;

    dfa.printInfo();

    int maxLength;
    cout << endl;
    cout << "Enter max length N for accepted strings: ";
    cin >> maxLength;

    dfa.generateAcceptedStrings(maxLength);

    dfa.analyzeStates();

    dfa.minimizeDFA();

    dfa.exportDOT("dfa.dot");

    dfa.randomTest(5, maxLength);

    cout << endl;
    cout << "========== Continuous String Recognition ==========" << endl;

    while (true) {
        string input;
        cout << "Enter a string to judge, or input quit to exit: ";
        cin >> input;

        if (input == "quit") {
            break;
        }

        dfa.judgeString(input, true);
    }

    cout << "Program finished." << endl;

    return 0;
}
