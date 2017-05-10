#include "parser.h"
#include <cstdio>
#include <stdlib.h>
#include <cassert>
#include <fstream>
#include <string>
#include <cstring>
#include <iostream>
#include <climits>

#define INF INT_MAX;
// self created functions
void DijkstraFind(edge_S e, node_S n);


//----------------------------------------------------------------------------------------------------------------------
parser_S::parser_S(const char* p_caCase) : _caCase(p_caCase), _caSol(0), _dMMC(0), _vCycle(0)
{
    if (!_isFileExist(_caCase)) { assert(0); }
}
//----------------------------------------------------------------------------------------------------------------------
parser_S::parser_S(const char* p_caCase, const char* p_caSol) : _caCase(p_caCase), _caSol(p_caSol), _dMMC(0), _vCycle(0)
{
    if (!_isFileExist(_caCase)) { assert(0); }
    if (!_isFileExist(_caSol))  { assert(0); }
}
//----------------------------------------------------------------------------------------------------------------------
bool parser_S::_isFileExist(const char* p_caFile)
{
    _input.open(p_caFile, ios::in);
    if(!_input){
        printf("Warning: can not find file: %s !\n", p_caFile);
        _input.close();
        return false;
    }

    printf("File %s Open Success\n", p_caFile);
    _input.close();
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool parser_S::parse()
{
    parseInput();
    //calculate the min as _dMMC and path as _vCycle
    findCycle();
    if (_caSol) {
        return parseSol();
    }
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
bool parser_S::parseSol()
{
    _input.open(_caSol, ios::in);
    _getLine();
    if (0 == strcmp(_buffer, "No cycle")) {
        printf("[parse - solution] No cycle\n");
        return false;
    } else {
        sscanf(_buffer, "%lf", &_dMMC);
        char* pCh;
        while (_getLine()) {
            pCh = strtok(_buffer, " \n");
            while (pCh) {
                _vCycle.push_back(atoi(pCh));
                pCh = strtok(NULL, " \n");
            }
        }
        printf("[parse - solution] MMC: %.2f\n", _dMMC);
        printf("[parse - solution] Cycle: ");
        for (auto n : _vCycle) {
            printf("%d ", n);
        }
        printf("\n");
    }
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
void parser_S::parseInput()
{
    _input.open(_caCase, ios::in);
    //src
    _getLine();
    int numNode(0), numEdge(0), from, to, weight;
    set<int> n;
    sscanf(_buffer, "%d %d", &numNode, &numEdge);
    while (_getLine()) {
        sscanf(_buffer, "%d %d %d", &from, &to, &weight);
        edge_S* pEdge = new edge_S(from, to, weight);
        _vEdge.push_back(pEdge);
        if (n.count(from) == 0) {
            node_S* pFrom = new node_S(from);
            _vNode.push_back(pFrom);
            n.insert(from);
        }
        if (n.count(to) == 0) {
            node_S* pTo = new node_S(to);
            _vNode.push_back(pTo);
            n.insert(to);
        }
    }
    if ((int) _vEdge.size() != numEdge) {
        printf("[parser] Warning: edge inconsistency, there should be %d edges, but only %d edges are parsed\n", numEdge, (int) _vEdge.size());
        assert(0);
    }
    if ((int) _vNode.size() != numNode) {
        printf("[parser] Warning: node inconsistency, there should be %d nodes, but only %d nodes are parsed\n", numNode, (int) _vNode.size());
    }
    _input.close();
}
//----------------------------------------------------------------------------------------------------------------------
bool parser_S::_getLine()
{
    string buffer;
    if (_input.eof()) {return false;}
    getline(_input,buffer);
    while (buffer.empty()) {
        if (_input.eof()) {return false;}
        getline(_input,buffer);
    }
    strcpy(_buffer, buffer.c_str());
    return true;
}
//----------------------------------------------------------------------------------------------------------------------
void parser_S::dump()
{
    FILE *pFile;
    pFile = fopen("dumpParser.txt", "w");
    //edge
    fprintf(pFile,"//#edge %d\n", (int) _vEdge.size());
    for(auto & edge: _vEdge) {
        fprintf(pFile,"%d %d %d\n", edge->_fromNode, edge->_toNode, edge->_weight);
    }
    //node
    fprintf(pFile,"\n//#node %d\n", (int) _vNode.size());
    for(auto & node : _vNode) {
        fprintf(pFile,"%d\n", node->_id);
    }
    fclose(pFile);
}
//----------------------------------------------------------------------------------------------------------------------
void parser_S::solution() {
    FILE *pFile;

    Dijkstra();


    pFile = fopen("solution.txt","w");
    fprintf(pFile,"//min value:");
    fprintf(pFile,"//the cycle:");
    fclose(pFile);
}

void parser_S::findCycle(){
    //_vCycle store the cycle path
    //method 1: Floyd-Warshall
    int numNode = _vNode.size();
    int path[numNode][numNode];
    int predecessor[numNode][numNode];

    // initialize the path array
    for(int u = 0; u < numNode; u++){
        for(int v = 0; v < numNode; v++){
            edge_S *Edge = doesEdgeExist(u+1,v+1);
            if(u==v) {
                path[u][v] = 0;
                predecessor[u][v] = 0; // initialize the pi
            }else if(Edge->_weight != 0){// found the element
                path[u][v] = Edge->_weight;
                predecessor[u][v] = Edge->_fromNode;
            }else{
                path[u][v] = INF;
                predecessor[u][v] = 0; // initialize the pi
            }

        }
    }
    // Floyd-Warshall

    for(int u = 0; u < numNode; u++){
        for(int v = 0; v < numNode; v++){
            for(int k = 0; k < numNode; k++){
                if(path[u][v] > path[u][k] + path[k][v]){
                    path[u][v] = path[u][k] + path[k][v];
                    predecessor[u][v] = predecessor[k][v];
                }
            }
        }
    }
    cout << "Floyd–Warshall's Algorithm : " << endl;
    for(int i = 0 ; i < numNode; i ++){
        for(int j = 0; j <  numNode; j ++){
            cout << path[i][j] << " ";
        }
        cout << endl;
    }
    cout << "Predecessor list is : " << endl;
    for(int i = 0 ; i < numNode; i ++){
        for(int j = 0; j <  numNode; j ++){
            cout << predecessor[i][j] << " ";
        }
        cout << endl;
    }
}


void parser_S::Dijkstra()
{
    // simple solution
    //find the cycle start from Node n and end to Node n

}
void DijkstraFind(edge_S e, node_S n)
{

}
edge_S* parser_S::doesEdgeExist(int start, int end) {
    //find the start node element index with binary sort (if I have more time)
    //ref: http://en.cppreference.com/w/cpp/algorithm/lower_bound
    for(auto i : _vEdge){
        if(i->_fromNode == start && i->_toNode == end){
            return i;
        }
    }
    edge_S *returnEdge = new edge_S(0,0,0);
    return returnEdge;
}