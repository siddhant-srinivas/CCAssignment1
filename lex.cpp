#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <vector>
#include <set>
#include <unordered_set>
#include <unordered_map>
#include <map>
#include <stack>
#include <queue>

using namespace std;
typedef vector<vector<vector<int> > > vvvi;
typedef vector<vector<int> > vvi;
class Node{
public:
    char data;
    Node* left;
    Node* right;
    Node(){
        left = NULL;
        right = NULL;
    }
    Node(char val){
        data = val;
        left = NULL;
        right = NULL;
    }
};

struct Transition{
    char inputChar;
    vector<struct State *> destinationStates;
};

struct State{
    vector<struct Transition> transitions;
};

struct NFA{
    State* startState;
    State* finalState;
};

struct finalNFA{
    int numStates;
    vvvi NFAtransitions;
    vvi finalStates;
};

NFA * eval(Node *);
NFA * eval_char(Node *);
NFA * eval_concat(Node *);
NFA * eval_union(Node *);
NFA * eval_kleene(Node *);
NFA * eval_plus(Node *);
NFA * eval_qmark(Node *);

NFA* eval(Node* root){
    char symbol = root -> data;
    switch(symbol){
        case 'a':
        {
            return eval_char(root);
            break;
        }
        case 'b':
        {
            return eval_char(root);
            break;
        }
        case '.':
        {
            return eval_concat(root);
            break;
        }
        case '|':
        {
            return eval_union(root);
            break;
        }
        case '*':
        {
            return eval_kleene(root);
            break;
        }
        case '+':
        {
            return eval_plus(root);
            break;
        }
        case '?':
        {
            return eval_qmark(root);
            break;
        }
    }
}

NFA *eval_char(Node* root){
    NFA *nfa = new struct NFA;
    nfa->startState = new struct State;
    nfa->finalState = new struct State;

    Transition t;
    t.inputChar = root -> data;
    t.destinationStates.push_back(nfa->finalState);
    nfa->startState->transitions.push_back(t);
    return nfa;
}

NFA* eval_concat(Node* root){
    NFA *nfa = new struct NFA;
    NFA *lNfa = eval(root->left);
    NFA *rNfa = eval(root->right);

    Transition t1;
    t1.inputChar = 'e';
    t1.destinationStates.push_back(rNfa->startState);
    lNfa->finalState->transitions.push_back(t1);

    nfa->startState = lNfa->startState;
    nfa->finalState = rNfa->finalState;
    return nfa;
}

NFA* eval_union(Node* root){
    NFA *nfa = new struct NFA;
    NFA *uNfa = eval(root->left);
    NFA *lNfa = eval(root->right);

    nfa->startState = new struct State;
    nfa->finalState = new struct State;

    struct Transition t1;
    t1.inputChar = 'e';
    t1.destinationStates.push_back(uNfa->startState);
    t1.destinationStates.push_back(lNfa->startState);
    nfa->startState->transitions.push_back(t1);

    struct Transition t2Unfa;
    t2Unfa.inputChar = 'e';
    t2Unfa.destinationStates.push_back(nfa->finalState);
    uNfa->finalState->transitions.push_back(t2Unfa);

    struct Transition t2Lnfa;
    t2Lnfa.inputChar = 'e';
    t2Lnfa.destinationStates.push_back(nfa->finalState);
    lNfa->finalState->transitions.push_back(t2Lnfa);
    
    return nfa;
}

NFA* eval_kleene(Node* root){
    NFA* nfa = new struct NFA;
    NFA* lNfa = eval(root->left);
    nfa->startState = new struct State;
    nfa->finalState = new struct State;

    struct Transition t1;
    t1.inputChar = 'e';
    t1.destinationStates.push_back(lNfa->startState);
    t1.destinationStates.push_back(nfa->finalState);
    nfa->startState->transitions.push_back(t1);

    struct Transition t2;
    t2.inputChar = 'e';
    t2.destinationStates.push_back(nfa->finalState);
    lNfa->finalState->transitions.push_back(t2);

    struct Transition t3;
    t3.inputChar = 'e';
    t3.destinationStates.push_back(nfa->startState);
    nfa->finalState->transitions.push_back(t3);

    return nfa;
}

NFA* eval_plus(Node* root){
    NFA* nfa = new struct NFA;
    NFA* lNfa = eval(root->left);
    nfa->startState = new struct State;
    nfa->finalState = new struct State;

    struct Transition t1;
    t1.inputChar = 'e';
    t1.destinationStates.push_back(lNfa->startState);
    nfa->startState->transitions.push_back(t1);

    struct Transition t2;
    t2.inputChar = 'e';
    t2.destinationStates.push_back(nfa->finalState);
    lNfa->finalState->transitions.push_back(t2);

    struct Transition t3;
    t3.inputChar = 'e';
    t3.destinationStates.push_back(nfa->startState);
    nfa->finalState->transitions.push_back(t3);

    return nfa;
}

NFA* eval_qmark(Node* root){
    NFA *nfa = new struct NFA;
    NFA *lNfa = eval(root->left);
    State *start = new struct State;

    struct Transition t1;
    t1.inputChar = 'e';
    t1.destinationStates.push_back(lNfa->startState);
    t1.destinationStates.push_back(lNfa->finalState);
    start->transitions.push_back(t1);

    nfa->startState = start;
    nfa->finalState = lNfa->finalState;
    return nfa;
}

void assignStateNumbers(State* state, int& nextNumber, unordered_map<State*, int> &stateNumbers) {
    if (stateNumbers.find(state) == stateNumbers.end()) {
        stateNumbers[state] = nextNumber++;
        for (Transition& t : state->transitions) {
            for (State* dest : t.destinationStates) {
                assignStateNumbers(dest, nextNumber, stateNumbers);
            }
        }
    }
}

int findNumberOfStates(NFA* nfa, unordered_map<State*, int> &stateNumbers){
    int nextNumber = 0;
    // Assign numbers to all states
    assignStateNumbers(nfa->startState, nextNumber, stateNumbers);
    assignStateNumbers(nfa->finalState, nextNumber, stateNumbers);
    return stateNumbers.size();
}

int converteNFAToTable(NFA* nfa, vvvi &transitionVector, unordered_map<State*, int> &stateNumbers) {
    int finalSt = 0;
    int numStates = stateNumbers.size();
    for (auto& entry : stateNumbers) {
        State* state = entry.first;
        if(state == nfa->finalState){
            finalSt = entry.second;
        }
        int row = entry.second;
        for(Transition t : state->transitions){
            char c = t.inputChar;
            int column;
            if(c == 'a') column = 0;
            else if(c == 'b') column = 1;
            else if(c == 'e') column = 2;
            for (State* dest : t.destinationStates) {
                transitionVector[row][column].push_back(stateNumbers[dest]);
            }
        }
    }
    return finalSt;
}

vector<int> epsilonClosure(int current, const vvvi &transitions)
{
    set<int> epsSet;
    stack <int> s;
    s.push(current);
    epsSet.insert(current);

    while(!s.empty()){
        int newstate = s.top();
        s.pop();

        for(int k: transitions[newstate][2]){
            if(k!= -1){
                if (epsSet.find(k) == epsSet.end()){
                    epsSet.insert(k);
                    s.push(k);
                }
            }
        }
    }
    vector<int> epsVec;
    for(int k: epsSet){
        epsVec.push_back(k);
    }
    return epsVec;
}

vector<int> findTransitions(vector<int> states, const vvvi &transitions, int input)
{
    set<int> temp;
    for(int state: states){
        vector<int> temp1;
        temp1 = epsilonClosure(state, transitions);
        for(int j: temp1){
            if(j!=-1){
                temp.insert(j);
            }
        }
    }
    set<int> temp2 = temp;
    temp.clear();
    for(int state: temp2){
        for (int k : transitions[state][input])
        {
            if (k != -1)
            {
                temp.insert(k);
            }
        }
    }
    temp2 = temp;
    temp.clear();
    for (int state : temp2)
    {
        vector<int> temp1;
        temp1 = epsilonClosure(state, transitions);
        for (int j : temp1)
        {
            if (j != -1)
            {
                temp.insert(j);
            }
        }
    }
    vector<int> finalTrans;
    for(int k:temp){
        finalTrans.push_back(k);
    }
    return finalTrans;
}

bool isUnique(vector<int> state, vvvi newtran)
{
    for(int i = 0;i<newtran.size();i++){
        if(newtran[i][0]==state){
            return false;
        }
    }
    return true;
}

vvvi convertNfa(const vvvi &transitions, int n, int &toVisit){
    vvvi newtran(20, vector<vector<int> >(3, vector<int>()));
    vector<int> current;

    current = epsilonClosure(0, transitions);

    newtran[0][0] = current;

    int index = 0;
    toVisit = 1;
    current = newtran[0][0];

    while(index<toVisit){
        current = newtran[index][0];
        vector<int> zeroTran = findTransitions(current, transitions, 0);
        vector<int> oneTran = findTransitions(current, transitions, 1);
        newtran[index][1] = zeroTran;
        newtran[index][2] = oneTran;
        if(isUnique(zeroTran, newtran)){
            newtran[toVisit++][0] = zeroTran;
        }
        if (isUnique(oneTran, newtran))
        {
            newtran[toVisit++][0] = oneTran;
        }
        index++;
    }
    return newtran;
}

vvi newFinalStates(const vector <int> &finalStates, const vvvi &newtran, int n){
    set<vector<int> > temp;
    for(int k:finalStates){
        for(int i = 0;i<n;i++){
            for(int j:newtran[i][0]){
                if(j==k){
                    temp.insert(newtran[i][0]);
                }
            }
        }
    }

    vvi newtemp;
    for(vector<int> k: temp){
        newtemp.push_back(k);
    }

    return newtemp;
}

string convertToPostfix(string inputstr){
    string regex = "";
	for(int i = 0; i<inputstr.size(); i++){
		regex+=inputstr[i];
		if(i!=inputstr.size()-1 && inputstr[i]==')' && inputstr[i+1]=='(')
			regex+='.';
	}
	stack<char> s;
	string postfixStr = "";
	for(int i = 0; i<regex.size(); i++){
		switch(regex[i]){
			case 'a': postfixStr+=regex[i];
				break;
			case 'b': postfixStr+=regex[i];
				break;
			case '(': s.push('(');
				break;
			case ')':
				while(s.top()!='('){
					postfixStr+=s.top();
					s.pop();
				}   
				s.pop();
				break;
			default: 
				while(!s.empty() && s.top()!='('){
					postfixStr+=s.top();
					s.pop();
				}
				s.push(regex[i]);
				break;
		}
	}
	while(!s.empty()){
		postfixStr+=s.top();
		s.pop();
	}
	return postfixStr;
}

Node* syntaxTree(const string &postfix){
    stack<Node*>st;
    for(char c : postfix){
        if((int)(c) == 97 || (int)(c) == 98){
            st.push(new Node(c));
        }
        else if(c == '?' || c == '+' || c == '*'){
            Node* child = st.top();
            st.pop();
            Node* curr = new Node(c);
            curr -> left = child;
            st.push(curr);
        }
        else{
            Node* rchild = st.top();
            st.pop();
            Node* curr = new Node(c);
            if(st.empty()){
                curr -> left = rchild;
                st.push(curr);
            }
            else{
                Node* lchild = st.top();
                st.pop();
                curr -> left = lchild;
                curr -> right = rchild;
                st.push(curr);
            }
        }
    }
    return st.top();
}

/*void printEpsilonNFA(vvvi eNFAtrans){
    cout << "State\t0trans\t1trans\tetrans\n";
    for (size_t i = 0; i < eNFAtrans.size(); ++i) {
        cout << i << "\t";
        for (size_t j = 0; j < eNFAtrans[i].size(); ++j) {
            for (size_t k = 0; k < eNFAtrans[i][j].size(); ++k) {
                if(eNFAtrans[i][j][k] != -1) cout << eNFAtrans[i][j][k] << " ";
            }
            cout << "\t";
        }
        cout << endl;
    }
}*/

finalNFA* finalConversion(NFA* nfa, finalNFA* finalNFA){
    State* nfaStartState = nfa->startState;
    State* nfaFinalState = nfa->finalState;

    unordered_map<State*, int> stateNumbers;
    int numStates = findNumberOfStates(nfa, stateNumbers);
    vvvi myTransitionVector(numStates, vvi(3, vector<int>(numStates, -1)));
    vector<int> NFAfinalStates;
    int finalVal = converteNFAToTable(nfa, myTransitionVector, stateNumbers);
    NFAfinalStates.push_back(finalVal);

    vvvi newtran(numStates, vector<vector<int> >(3, vector<int>()));
    int toVisit = 1;
    newtran = convertNfa(myTransitionVector,numStates, toVisit);
    numStates = toVisit;

    vvi newAcceptingStates = newFinalStates(NFAfinalStates, newtran, numStates);
    finalNFA->NFAtransitions = newtran;
    finalNFA->finalStates = newAcceptingStates;
    finalNFA->numStates = numStates;

    return finalNFA;
}

/*void printFinalNFA(finalNFA* nfa){
    int numStates = nfa->numStates;
    vvvi transitions = nfa->NFAtransitions;
    vvi newAcceptingStates = nfa->finalStates;

    cout << "The new NFA is:" << endl;

    for(int i = 0;i<numStates;i++){
        for(int j = 0;j<3;j++){
            std::cout << "{";
            for(int k: transitions[i][j]){
                std::cout << k << " ";
            }
            std::cout << "}\t";
        }
        std::cout << endl;
    }

    cout << "The new accepting states are: ";
    for(auto state: newAcceptingStates){
        cout<<"{";
        for(int j: state){
            cout<<j<<" ";
        }
        cout<<"}  ";
    }
    cout << endl;
}*/

bool isFinal(vector<int> state, const vvi &finalStates)
{
    for (int i = 0; i < finalStates.size(); i++)
    {
        if (state == finalStates[i])
        {
            return true;
        }
    }
    return false;
}

int findIndex(vector <int> state, const vvvi &transitions, const int &n){
    for(int i = 0;i<n;i++){
        if(state == transitions[i][0]){
            return i;
        }
    }
    return -1;
}

bool isAccepted(const vvvi &transitions, vvi finalStates, string str, const int &n)
{
    vector <int> currentState = transitions[0][0];
    int index = 0;
    for(char c: str){
        int x = 0;
        if(c == 'a') x = 1;
        else if(c == 'b') x = 2;
        vector<int> temp = transitions[index][x];
        index = findIndex(temp, transitions, n);
        if(index == -1){
            return false;
        }
    }
    currentState = transitions[index][0];
    if(isFinal(currentState, finalStates)){
        return true;
    }
    return false;
}

int main(){
    ifstream f("input5.txt");
    string inputstr;
    getline(f, inputstr);
    int strlength = inputstr.length();
    vector<string> regExpressions;
    string temp;
    while(getline(f,temp)){
        regExpressions.push_back(temp);
    }
    vector<string> postfixStrings;
    for(int i = 0; i < regExpressions.size(); i++){
        postfixStrings.push_back(convertToPostfix(regExpressions[i]));
    }
    vector<Node*> expressionTreeNodes;
    for(int i = 0; i < postfixStrings.size(); i++){
        expressionTreeNodes.push_back(syntaxTree(postfixStrings[i]));
    }
    vector<NFA*> ENFASet;
    for(int i = 0; i < expressionTreeNodes.size(); i++){
        ENFASet.push_back(eval(expressionTreeNodes[i]));
    }
    vector<finalNFA*> NFASet;
    for(int i = 0; i < ENFASet.size(); i++){
        finalNFA* finNFA = new finalNFA;
        NFASet.push_back(finalConversion(ENFASet[i], finNFA));
    }
    vector<string> lexemes;
    vector<int> indices;
    int leftptr = 0;
    int subSize = strlength;
    int count = 0;
    while(leftptr<strlength){
        string subString = inputstr.substr(leftptr, subSize);
        int flag = 0;
        for(int i = 0; i < NFASet.size(); i++){
            if(isAccepted(NFASet[i]->NFAtransitions, NFASet[i]->finalStates, subString, NFASet[i]->numStates) == true){
                int oldleftptr = leftptr;
                leftptr = oldleftptr + subSize;
                subSize = strlength - subSize - oldleftptr;
                lexemes.push_back(subString);
                indices.push_back(i+1);
                flag = 1;
                break;
            }
        }
        if(leftptr == strlength) break;
        if(flag == 0){
            subSize--;
        }
        if(subSize == 0){
            leftptr = leftptr + 1;
            subSize = strlength - leftptr;
            lexemes.push_back(subString);
            indices.push_back(0);
        }
    }
    string outputStr = "";
    for(int i = 0; i < lexemes.size(); i++){
        string index = to_string(indices[i]);
        outputStr.append('<' + lexemes[i] + ',' + index + '>');
    }
    ofstream outputFile("output.txt");
    outputFile << outputStr;
    outputFile.close();
    return 0;
}
