#include <stdio.h>
#include "commandParser.h"

int main(void)
{
    struct graph * graph = initialiser_Graph();

    analyse_cmd(graph, "load test_files/test.gv");
    analyse_cmd(graph, "show topology");
    analyse_cmd(graph, "add link N1 N4 5");
    analyse_cmd(graph, "del link N1 N4");
    analyse_cmd(graph, "disconnect N3");
    analyse_cmd(graph, "update link N1 N2 1");
    analyse_cmd(graph, "save Topology2.txt");

    free_graph(graph);

    return 0;
}
