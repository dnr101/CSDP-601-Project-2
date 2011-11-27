/*
Maximum Flow Project
UMES CSDP 601
D. Raizen
*/


#include <iostream>
#include <iomanip>
#include <fstream>
#include <string>
#include <ctype.h>
#include <limits>



using namespace std;

const char outC = 'c';
const char outF = 'f';
const int INF = std::numeric_limits<int>::max();

//Class Headers
class menu
{
    public:
        menu(int = 0, char** = NULL, void(**)(void) = NULL);        //size, "words", funcs
        ~menu();
        void display(void)const;
        char getChoice(void);
        bool done(void)const;
        void doChoice(char &)const;
        void typicalQuit(void);
    protected:
        int numItems;
        char** menuItems;
        void(** theChoices)(void);
        char theChoice;
};

//Element of a graph...
class edge
{
    public:
        edge(int = 0, int = 0);
        edge(const edge &);
        edge & operator= (const edge &);
        ~edge();
        void setCap(int);
        int getCap();
        void setFlow(int);
        void incFlow();
        int getFlow();
    private:
        int cap;
        int flow;
    friend class graph;
};

//2D array of edges...
class graph
{
    public:
        graph(int);
        ~graph();
        void inputGraph();
        void outputGraph(char);
        int flow();
        int getSource();
        int getSink();
    private:
        int numVerts;
        int * oil;
        int * height;
        int * current;
        edge ** theGraph;
        int source;
        int sink;
        int min(int, int);
        void push(int, int);
        void relabel(int);
        void discharge(int);
    friend class edge;
};

//Class edge methods
edge::edge(const int newCap, int newFlow)
{
    if (newCap == 0) cap = 0;
    else cap = newCap;
    if (newFlow == 0) flow = 0;
    else flow = newFlow;
}

edge::edge(const edge & otherEdge)
{
    cap = otherEdge.cap;
    flow = otherEdge.flow;
}

edge::~edge()
{

}

edge & edge::operator= (const edge & otherEdge)
{
    cap = otherEdge.cap;
    flow = otherEdge.flow;
    return * this;
}
void edge::setCap(int newCap)
{
    cap = newCap;
}

int edge::getCap()
{
    return cap;
}

void edge::setFlow(int newFlow)
{
    flow = newFlow;
}

void edge::incFlow()
{
    flow++;
}

int edge::getFlow()
{
    return flow;
}

//Class graph methods
graph::graph(int verts)
{
    source = 0;
    numVerts = verts;
    oil = new int[numVerts];
    height = new int[numVerts];
    current = new int[numVerts];
    theGraph = new edge* [numVerts];
    for (int i = 0; i < numVerts; i++)
    {
        oil[i] = 0;
        height[i] = 0;
        current[i] = 0;
        theGraph[i] = new edge[numVerts];
    }
};

graph::~graph()
{
    for (int i = 0; i < numVerts; i++)
    {
        delete[] theGraph[i];
    }
    delete[] theGraph;
    delete[] oil;
    delete[] height;
    delete[] current;
};

void graph::outputGraph(char capOrFlow)
{
    if (capOrFlow == 'c')
    {
        for (int i = 0; i < numVerts; i++)
        {
            for (int j = 0; j < numVerts; j++)
            {
                if (i >= j) continue;
                cout<<setw(20)<<"Edge ("<<i<<", "<<j<<") has capacity: "<<theGraph[i][j].getCap()<<endl;
            }
        }
    }
    else
    {
        cout<<"Flow from source vertex: "<<source<<" to sink: "<<sink<<"\n"<<endl;
    }
}
void graph::inputGraph()
{
    int capacity;
    for (int i = 0; i < numVerts; i++)
    {
        for (int j = 0; j < numVerts; j++)
        {
            if (i >= j) continue;
            cin.ignore();
            cout<<"Enter capacity of edge ("<<i<<", "<<j<<") (or 0 if none): ";
            cin>>capacity;
            theGraph[i][j].setCap(capacity);
        }
    }
    sink = numVerts - 1;
//    cin.ignore();
//    cout<<"Enter the source vertex: ";
//    cin>>source;
//    cin.ignore();
//    cout<<"Enter the sink vertex: ";
//    cin>>sink;
}

int graph::getSource()
{
    return source;
}

int graph::getSink()
{
    return sink;
}


//"Lift-to-front"flow method...
int graph::flow()
{
    for (int i = 0; i < numVerts; i++)
    {
        height[i] = 0;
        oil[i] = 0;
        current[i] = 0;
    }
    int list[numVerts-2];
    for (int i = 0; i < numVerts-2; i++)
    {
        int next = 0;
        if (next == source || next == sink) next++;
        list[i] = next;
    }
    height[source] = numVerts;
    oil[source] = INF;
    for (int v = 0; v < numVerts; v++)
    {
        push(source, v);
    }
    int p = 0;
    while (p < numVerts - 2)
    {
        int u = list[p];
        int oldHeight = height[u];
        discharge(u);
        if (height[u] > oldHeight)
        {
            int temp = list[0];
            list[0] = list[p];
            list[p] = temp;
            p = 0;
        }
        p++;
    }
    int flow = 0;
    for (int i = 0; i < numVerts; i++)
    {
        flow = flow + theGraph[source][i].getFlow();
    }
    return flow;
}

int graph::min(int a, int b)
{
    if (a <= b) return a;
    else return b;
}

//push helper method
void graph::push(int u, int v)
{
    int send = min(oil[u], theGraph[u][v].getCap() - theGraph[u][v].getFlow());
    theGraph[u][v].setFlow(theGraph[u][v].getFlow() + send);
    theGraph[v][u].setFlow(theGraph[u][v].getFlow() * -1);
    oil[u] = oil[u] - send;
    oil[v] = oil[v] + send;
}

//relabel helper method (performs the lift)
void graph::relabel(int u)
{
    int minHeight = height[u];
    for (int v = 0; v < numVerts; v++)
    {
        if (theGraph[u][v].getCap() - theGraph[u][v].getFlow() > 0)
        {
            minHeight = min(minHeight, height[v]);
            height[u] = minHeight + 1;
        }
    }
}

//discharge helper (reverse flow, sort of...)
void graph::discharge(int u)
{
    while (oil[u] > 0)
    {
        int v = current[u];
        if (v > numVerts)
        {
            relabel(u);
            current[u] = 0;
        }
        else if ((theGraph[u][v].getCap() - theGraph[u][v].getFlow() > 0) && (height[u] == height[v]+1))
        {
            push(u, v);
        }
        else
        {
            current[u]++;
        }
    }
}

//Menu funcs...
void enterGraph(void);
void displayGraph(void);
void displayFlow(void);
void quit(void);

//Create menu and graph
char* menuWords[] = {"Enter Graph", "Display Graph", "Display Maximum Flow", "Quit"};
void(* menuChoices[])(void) = {enterGraph, displayGraph, displayFlow, quit};
menu * theMenu = new menu(4, menuWords, menuChoices);
graph * myGraph;
int verts = 0;

//Standard main method...
int main(void)
{
    char choiceIn = '0';
    do
    {
        theMenu->display();
        choiceIn = theMenu->getChoice();
        theMenu->doChoice(choiceIn);
    }while(!(theMenu->done()));
};


//Details for Class: Menu
menu::menu(int howMany, char** theMenu, void(** theFunctions)(void))
{
    numItems = howMany;
    menuItems = theMenu;
    theChoices = theFunctions;
    theChoice = '0';
};

menu::~menu()
{
    //nothing created, nothing to destroy...
};

void menu::display() const
{
    cout << endl << endl << setw(45) << "MAIN MENU" << endl << endl;
    for (int i = 0; i <numItems; i++)
    {
        cout << setw(20) << (i+1) << ". " << menuItems[i] << endl;
    };
    cout << endl;
};

char menu::getChoice(void)
{
    const char maxChoice = ('0' + numItems);        //numItems cast as char...
    char aChoice;                                   //input is char to minimize error
    cout << "Enter the NUMBER of your choice: ";
    cin >> aChoice;
    while((aChoice < '1')||(aChoice > maxChoice))
    {
        cout << "Please select a number between 1 and " << maxChoice << ": ";
        cin >> aChoice;
    };
    theChoice = aChoice;
    return aChoice;
};

bool menu::done(void) const
{
    return(theChoice == ('0' + numItems));
};

void menu::doChoice(char & thisChoice) const
{
    (*theChoices[thisChoice - '0' - 1])();
};

void menu::typicalQuit(void)
{
    char yesNo;
    cout << endl << endl << setw(45) << "Quitting..." << endl << endl;
    cout << "Are you sure...? (Y/N)";
    cin >> yesNo;
    yesNo = toupper(yesNo);
    while((yesNo != 'Y')&&(yesNo != 'N'))
    {
        cout << "Please enter Y or N: ";
        cin >> yesNo;
        yesNo = toupper(yesNo);
    };
    if (yesNo == 'N')
    {
        theChoice = '0';
        cout << "Press the [ENTER] key to continue: ";
    }
    else
    {
        cout << endl << endl << "Goodbye. Press the [ENTER] key to exit: ";
    };
    cin.ignore();
    cin.get();
    system("cls");
};

//Menu functions..
void enterGraph(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "Enter Graph" << endl << endl;
    cout<<"Enter the number of vertices: ";
    cin>>verts;
    myGraph = new graph(verts);
    myGraph->inputGraph();
    cout << endl << "Press the [ENTER] key to continue...";
    cin.ignore();
    cin.get();
    system("cls");
};

void displayGraph(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "Display Graph" << endl << endl;
    myGraph->outputGraph(outC);
    cout<<"Press [Enter] to continue..."<<endl;
    cin.ignore();
    cin.get();
    system("cls");
};

void displayFlow(void)
{
    system("cls");
    cout << endl << endl << setw(45) << "Display Flow" << endl << endl;
    int theFlow = myGraph->flow();
    myGraph->outputGraph(outF);
    cout << "\nThe total flow is: " << theFlow << "\n" << endl;
    cout<<"Press [Enter] to continue..."<<endl;
    cin.ignore();
    cin.get();
    system("cls");
};

void quit(void)
{
    system("cls");
    theMenu->typicalQuit();
    system("cls");
};
