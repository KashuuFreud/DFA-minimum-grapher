#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <set>
#include <queue>
#include <sstream>
#include <algorithm>

using namespace std;

class DFA {
private:
    string alphabet;                  // 字母表，如 "ab"
    int numStates;                   // 状态数，状态编号为 1..numStates
    int startState;                  // 开始状态
    set<int> acceptStates;           // 接受状态集合
    vector<vector<int>> trans;       // 转移表 trans[state][charIndex]

public:
    DFA() : numStates(0), startState(-1) {}

    void clear() {
        alphabet.clear();
        numStates = 0;
        startState = -1;
        acceptStates.clear();
        trans.clear();
    }

    bool loadFromFile(const string& filename) {
        clear();

        ifstream fin(filename);
        if (!fin.is_open()) {
            cerr << "无法打开文件: " << filename << endl;
            return false;
        }

        if (!(fin >> alphabet)) {
            cerr << "读取字母表失败。" << endl;
            return false;
        }

        if (!checkAlphabetValid()) return false;

        if (!(fin >> numStates)) {
            cerr << "读取状态数失败。" << endl;
            return false;
        }
        if (numStates <= 0) {
            cerr << "状态数必须大于 0。" << endl;
            return false;
        }

        if (!(fin >> startState)) {
            cerr << "读取开始状态失败。" << endl;
            return false;
        }

        fin.ignore();
        string acceptLine;
        if (!getline(fin, acceptLine)) {
            cerr << "读取接受状态集合失败。" << endl;
            return false;
        }

        {
            stringstream ss(acceptLine);
            int x;
            while (ss >> x) {
                acceptStates.insert(x);
            }
        }

        trans.assign(numStates + 1, vector<int>(alphabet.size(), -1));

        for (int state = 1; state <= numStates; ++state) {
            for (int j = 0; j < (int)alphabet.size(); ++j) {
                if (!(fin >> trans[state][j])) {
                    cerr << "读取状态 " << state << " 的转移表失败。" << endl;
                    return false;
                }
            }
        }

        fin.close();
        return true;
    }

    bool inputFromKeyboard() {
        clear();

        cout << "请输入字母表（例如 ab ）: ";
        cin >> alphabet;

        if (!checkAlphabetValid()) return false;

        cout << "请输入状态数: ";
        cin >> numStates;
        if (numStates <= 0) {
            cerr << "状态数必须大于 0。" << endl;
            return false;
        }

        cout << "请输入开始状态: ";
        cin >> startState;

        int acceptCount;
        cout << "请输入接受状态个数: ";
        cin >> acceptCount;
        if (acceptCount <= 0) {
            cerr << "接受状态集合不能为空。" << endl;
            return false;
        }

        cout << "请输入接受状态（用空格分隔）: ";
        for (int i = 0; i < acceptCount; ++i) {
            int st;
            cin >> st;
            acceptStates.insert(st);
        }

        trans.assign(numStates + 1, vector<int>(alphabet.size(), -1));

        cout << "\n请依次输入每个状态在各字符下的转移。" << endl;
        cout << "格式说明：如果字母表是 " << alphabet
             << " ，那么每一行输入 " << alphabet.size() << " 个整数。" << endl;
        cout << "例如状态 1 输入：2 3 表示 1 --" << alphabet[0]
             << "--> 2, 1 --" << alphabet[1] << "--> 3" << endl << endl;

        for (int state = 1; state <= numStates; ++state) {
            cout << "请输入状态 " << state << " 的转移: ";
            for (int j = 0; j < (int)alphabet.size(); ++j) {
                cin >> trans[state][j];
            }
        }

        return true;
    }

    bool validate() const {
        if (alphabet.empty()) {
            cerr << "字母表不能为空。" << endl;
            return false;
        }

        if (numStates <= 0) {
            cerr << "状态数必须大于 0。" << endl;
            return false;
        }

        if (startState < 1 || startState > numStates) {
            cerr << "开始状态不在状态集中。" << endl;
            return false;
        }

        if (acceptStates.empty()) {
            cerr << "接受状态集合不能为空。" << endl;
            return false;
        }

        for (int st : acceptStates) {
            if (st < 1 || st > numStates) {
                cerr << "接受状态 " << st << " 不在状态集中。" << endl;
                return false;
            }
        }

        if ((int)trans.size() != numStates + 1) {
            cerr << "转移表大小不正确。" << endl;
            return false;
        }

        for (int state = 1; state <= numStates; ++state) {
            if ((int)trans[state].size() != (int)alphabet.size()) {
                cerr << "状态 " << state << " 的转移数与字母表大小不一致。" << endl;
                return false;
            }
            for (int j = 0; j < (int)alphabet.size(); ++j) {
                int nextState = trans[state][j];
                if (nextState < 1 || nextState > numStates) {
                    cerr << "状态 " << state << " 在字符 '" << alphabet[j]
                         << "' 下转移到非法状态 " << nextState << endl;
                    return false;
                }
            }
        }

        return true;
    }

    void printDFA() const {
        cout << "\n========== DFA 信息 ==========" << endl;

        cout << "字母表: { ";
        for (char c : alphabet) cout << c << " ";
        cout << "}" << endl;

        cout << "状态集: { ";
        for (int i = 1; i <= numStates; ++i) cout << i << " ";
        cout << "}" << endl;

        cout << "开始状态: " << startState << endl;

        cout << "接受状态集: { ";
        for (int st : acceptStates) cout << st << " ";
        cout << "}" << endl;

        cout << "状态转移表:" << endl;
        cout << "State";
        for (char c : alphabet) cout << "\t" << c;
        cout << endl;

        for (int i = 1; i <= numStates; ++i) {
            cout << i;
            for (int j = 0; j < (int)alphabet.size(); ++j) {
                cout << "\t" << trans[i][j];
            }
            cout << endl;
        }

        cout << "==============================" << endl;
    }

    bool containsOnlyAlphabet(const string& s) const {
        for (char c : s) {
            if (alphabet.find(c) == string::npos) {
                return false;
            }
        }
        return true;
    }

    bool recognize(const string& s, bool showProcess = true) const {
        if (!containsOnlyAlphabet(s)) {
            cout << "字符串中包含不属于字母表的字符。" << endl;
            return false;
        }

        int currentState = startState;

        if (showProcess) {
            cout << "识别过程：" << endl;
            cout << "初始状态: " << currentState << endl;
        }

        for (char c : s) {
            int idx = alphabet.find(c);
            int nextState = trans[currentState][idx];

            if (showProcess) {
                cout << "读取字符 '" << c << "' : "
                     << currentState << " -> " << nextState << endl;
            }

            currentState = nextState;
        }

        if (showProcess) {
            cout << "最终状态: " << currentState << endl;
        }

        return acceptStates.count(currentState) > 0;
    }

    void generateAcceptedStrings(int maxLen) const {
        cout << "\n长度 <= " << maxLen << " 的所有被接受字符串如下：" << endl;

        bool found = false;
        queue<pair<string, int>> q;
        q.push({"", startState});

        while (!q.empty()) {
            auto [str, state] = q.front();
            q.pop();

            if ((int)str.size() <= maxLen && acceptStates.count(state)) {
                cout << (str.empty() ? "ε" : str) << endl;
                found = true;
            }

            if ((int)str.size() == maxLen) continue;

            for (int i = 0; i < (int)alphabet.size(); ++i) {
                char c = alphabet[i];
                int nextState = trans[state][i];
                q.push({str + c, nextState});
            }
        }

        if (!found) {
            cout << "无。" << endl;
        }
    }

private:
    bool checkAlphabetValid() const {
        if (alphabet.empty()) {
            cerr << "字母表不能为空。" << endl;
            return false;
        }
        set<char> alphaSet(alphabet.begin(), alphabet.end());
        if ((int)alphaSet.size() != (int)alphabet.size()) {
            cerr << "字母表中不能有重复字符。" << endl;
            return false;
        }
        return true;
    }
};

int main() {
    DFA dfa;
    int choice;

    cout << "========== DFA 模拟器 ==========" << endl;
    cout << "请选择输入方式：" << endl;
    cout << "1. 从文件读取 DFA" << endl;
    cout << "2. 在终端手动输入 DFA" << endl;
    cout << "请输入你的选择(1 或 2): ";
    cin >> choice;

    bool ok = false;

    if (choice == 1) {
        string filename;
        cout << "请输入 DFA 文件名: ";
        cin >> filename;
        ok = dfa.loadFromFile(filename);
    } else if (choice == 2) {
        ok = dfa.inputFromKeyboard();
    } else {
        cout << "输入选项无效，程序结束。" << endl;
        return 1;
    }

    if (!ok) {
        cout << "DFA 输入失败，程序结束。" << endl;
        return 1;
    }

    if (!dfa.validate()) {
        cout << "DFA 非法，程序结束。" << endl;
        return 1;
    }

    cout << "\nDFA 读取并检查成功。" << endl;
    dfa.printDFA();

    int N;
    cout << "\n请输入要生成的最大字符串长度 N: ";
    cin >> N;

    if (N < 0) {
        cout << "N 不能为负数，程序结束。" << endl;
        return 1;
    }

    dfa.generateAcceptedStrings(N);

    while (true) {
        string inputStr;
        cout << "\n请输入要判定的字符串（输入 quit 退出）: ";
        cin >> inputStr;

        if (inputStr == "quit") {
            cout << "程序结束。" << endl;
            break;
        }

        bool accepted = dfa.recognize(inputStr, true);

        if (accepted) {
            cout << "结果：该字符串属于该 DFA 的语言。" << endl;
        } else {
            cout << "结果：该字符串不属于该 DFA 的语言。" << endl;
        }
    }

    return 0;
}
