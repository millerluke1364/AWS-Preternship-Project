#ifndef TREE_H
#define TREE_H

#include <stack>
#include <queue>
#include <string>
#include "Graph.h"
#include "Taxon.h"

#define uint unsigned int

class Tree: public Graph<Taxon> {
protected:
    std::queue<uint> vacancies;

    
    bool is_child_of(uint child, uint parent) {
        std::stack<uint> next_node;
        uint curr_node;

        next_node.push(parent);

        while (!next_node.empty()) {
            curr_node = next_node.top();
            next_node.pop();

            // returns node value if requested taxon is found
            if (curr_node == child) {
                return true;
            }

            // iterates through all edges in the current node
            for (uint i = 0; i < verts[curr_node].get_num_edges(); i++) {
                // adds each to the stack
                next_node.push(verts[curr_node].get_edge(i));
            }
        }

        return false;
    }


    

    void error(std::string msg, const char* func) {
        std::cout << "Error: Tree." << func << "() -> " << msg << std::endl;
    }


public:
    Tree() : vacancies() {
        Taxon root("root", 0);
        root.set_id(0);
        add_vert(root);
    }

    // virtual destructor
    virtual ~Tree() {}

    // depth first search
    int DFS(std::string name, uint start=0) {
        std::stack<uint> next_node;
        uint curr_node;

        next_node.push(start);

        while (!next_node.empty()) {
            curr_node = next_node.top();
            next_node.pop();

            // returns node value if requested taxon is found
            if (verts[curr_node].get_name() == name) {
                return curr_node;
            }

            // iterates through all edges in the current node
            for (uint i = 0; i < verts[curr_node].get_num_edges(); i++) {
                // adds each to the stack
                next_node.push(verts[curr_node].get_edge(i));
            }
        }

        std::cout << "DFS Error: Requested node \"" << name << "\" does not exist." << std::endl; 
        return -1;
    }


    int add_taxon(std::string name, int parent_id = 0) {
        int child_id;

        // creates new child taxon
        Taxon child(name, parent_id);

        // checks for any vacancies from lazy deletion to fill
        if (vacancies.empty()) {
            // child id will be the next element in verts
            child_id = (int) verts.size();
            child.set_id(child_id);
            add_vert(child);
        } else {
            // fills vacancy
            child_id = (int) vacancies.front();
            child.set_id(child_id);
            // std::cout << "using " << child_id << std::endl;
            vacancies.pop();

            // overrides deleted taxon
            verts[child_id] = child;
        }

        //  connects child to tree
        add_edge(parent_id, child_id);

        return child_id;
    }

    void move_taxon(int child_id, int parent_id) {
        if (child_id == 0) { error("cannot move root node", __FUNCTION__); return; }
        if (is_child_of(parent_id, child_id)) { error("cannot move parent into child node", __FUNCTION__); return; }

        Taxon* child = &verts[child_id];
        Taxon* old_parent = &verts[child->get_parent()];
        Taxon* new_parent = &verts[parent_id];

        // moving edge from old parent to new parent
        old_parent->del_edge(child_id);
        new_parent->add_edge(child_id);

        // updating child's parent's id
        child->set_parent(parent_id);

    }

    virtual void del_taxon(int id) {
        uint curr_edge;
        uint parent_id;

        parent_id = verts[id].get_parent();
        
        if (id == -1) {
            std::cout << "Error: Taxon does not exist." << std::endl;
            return;
        }

        // lazy deletion
        verts[id].del();
        vacancies.push(id);

        // removes connection from parent
        verts[parent_id].del_edge(id);

        // iteratively moves all child taxons to be children of the deleted taxon's parent
        for (uint i = 0; i < verts[id].get_num_edges(); i++) {
            curr_edge = verts[id].get_edge(i);

            verts[curr_edge].set_parent(parent_id);
            verts[parent_id].add_edge(curr_edge);
        }
        

    }

    int get_taxon(std::string name) {
        return DFS(name);
    }

    // prints out adjacency list representation of tree
    void print_tree_list() {
        std::cout << "Tree size: " << verts.size() << " verts (" << vacancies.size() << " vacancies)" << std::endl;

        for (uint i = 0; i < verts.size(); i++) {
            if (verts[i].alive()) {
                std::cout << i << ": " << "[" << verts[i] << " " << verts[i].get_id() << "]" << " -> ";
                verts[i].print_edges();
            } else {
                std::cout << i << ": *DELETED*" << std::endl;
            }
        }
    }

    // prints out subtree starting at given taxon (defaults to root node)
    virtual void print_tree(uint curr=0, uint depth=0, bool last=false, std::string head="") {
        Taxon* node = &verts[curr];
        std::string disp_str = "";
        std::string send_str = "";

        if (depth) {
            disp_str = head + (last ? "└── " : "├── ");
        }

        std::cout << disp_str << node->get_name() << std::endl;

        for (uint i = 0; i < node->get_num_edges(); i++) {
            if (depth) {
                send_str = head + (last ? "    " : "│   ");
            }

            print_tree(
                node->get_edge(i),              // passes in the next node
                depth + 1,                      // increases tree (and recursion) depth by one
                i == node->get_num_edges() - 1, // passes true if next call is the last child
                send_str                        // passes in the head string for the next line
            );
        }
    }

};

#endif