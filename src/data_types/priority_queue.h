#pragma once
#include <vector>
#include <algorithm>
#include <cmath>

enum class QueueProtocol {HIGHEST_PRIORITY, LOWEST_PRIORITY, OLDEST, NEWEST};
template <typename TypeOfData>
struct QueueNode{
    int priority;
    int order;
    TypeOfData data;
    bool operator==(const QueueNode& other) const {
        return (order == other.order);
    }
};
template <typename TypeOfData>
class PriorityQueue{
    public:
        PriorityQueue() = default;
        ~PriorityQueue() = default;
        void enqueue(TypeOfData data, int priority){
            QueueNode<TypeOfData> node;
            node.priority = priority;
            node.order = get_id();
            node.data = data;

            tree.push_back(node);
            size_t index = tree.size()-1;
            bubble_up(index);
        };
        QueueNode<TypeOfData> dequeue(QueueProtocol protocol){
            QueueNode<TypeOfData> node;
            if(protocol==QueueProtocol::HIGHEST_PRIORITY){
                
                if(tree.size()<2){
                    if (tree.empty()) return node;
                    else{
                        node = tree[0];
                        remove_and_correct(0);
                        return node;
                    }
                }
                else if(tree.size()==2){
                    node = tree[1];
                    remove_and_correct(1);
                }
                else if(tree[1].priority > tree[2].priority){
                    node=tree[1];
                    remove_and_correct(1);
                }
                else{
                    node = tree[2];
                    remove_and_correct(2);
                }
            }
            else if(protocol==QueueProtocol::LOWEST_PRIORITY){
                if(tree.size()==0){
                    
                }
                else{
                    node=tree[0];
                    remove_and_correct(0);
                }
            }
            else if(protocol==QueueProtocol::OLDEST){
                int order = 10000000;
                for (const auto& val : tree) {
                    if(val.order < order){
                        node = val;
                        order = val.order;
                    }
                }
                auto it = find(tree.begin(), tree.end(), node);
                remove_and_correct(it -tree.begin());
            }
            else if(protocol==QueueProtocol::NEWEST){
                int order=0;
                for(const auto& val : tree){
                    if(val.order > order){
                        node = val;
                        order = val.order;
                    }
                }
                auto it = find(tree.begin(), tree.end(), node);
                remove_and_correct(it -tree.begin());
            }
            return node;
        };
        QueueNode<TypeOfData> peek(QueueProtocol protocol){
            QueueNode<TypeOfData> node;
            if(protocol==QueueProtocol::HIGHEST_PRIORITY){
                
                if(tree.size()<2){
                    if(tree.empty()) return node;
                    else{
                        node = tree[0];
                        return node;
                    }
                }
                else if(tree.size()==2){
                    node = tree[1];
                }
                else if(tree[1].priority > tree[2].priority){
                    node=tree[1];
                }
                else{
                    node = tree[2];
                }
            }
            else if(protocol==QueueProtocol::LOWEST_PRIORITY){
                if(tree.size()==0){
                    
                }
                else{
                    node=tree[0];
                }
            }
            else if(protocol==QueueProtocol::OLDEST){
                int order = 10000000;
                for (const auto& val : tree) {
                    if(val.order < order){
                        node = val;
                        order = val.order;
                    }
                }
            }
            else if(protocol==QueueProtocol::NEWEST){
                int order=0;
                for(const auto& val : tree){
                    if(val.order > order){
                        node = val;
                        order = val.order;
                    }
                }
            }
            return node;
        };
    private:
        std::vector<QueueNode<TypeOfData>> tree;
        int get_id(){
            static int id = 0;
            return id++;
        };
        void bubble_up(int index){
            if (index==0) return;
            size_t parent_index = (index - 1) / 2;
            int level = static_cast<int>(31 - __builtin_clz(index + 1));
            if(level%2==0){
                if(tree[index].priority > tree[parent_index].priority){
                    std::swap(tree[index],tree[parent_index]);
                    bubble_up_max(parent_index);
                }
                else{
                    bubble_up_min(index);
                }
            }
            else{
                if(tree[index].priority < tree[parent_index].priority){
                    std::swap(tree[index],tree[parent_index]);
                    bubble_up_min(parent_index);
                }
                else{
                    bubble_up_max(index);
                }
            }
        };
        void bubble_up_max(int index){
            if(index<3){
                return;
            }
            int gramps = (((index - 1) / 2) - 1) / 2;
            if(tree[gramps].priority < tree[index].priority){
                std::swap(tree[gramps],tree[index]);
                bubble_up_max(gramps);
            }
        };
        void bubble_up_min(int index){
            if(index<3){
                return;
            }
            int gramps = (((index - 1) / 2) - 1) / 2;
            if(tree[gramps].priority > tree[index].priority){
                std::swap(tree[gramps],tree[index]);
                bubble_up_min(gramps);
            }
        };
        void remove_and_correct(int index){
            if (index >= tree.size()) return;
            if (index == tree.size() - 1) {
                tree.pop_back();
                return;
            }
            tree[index] = tree.back();
            tree.pop_back();

            int level = static_cast<int>(31 - __builtin_clz(index + 1));
            if (level % 2 == 0) push_down_min(index);
            else push_down_max(index);
            bubble_up(index); 
        };
        void push_down_min(int index){
            int size = tree.size();
            if (2 * index + 1 >= size) return; 

            int smallest_child = 2 * index + 1;
            if (2 * index + 2 < size && tree[2 * index + 2].priority < tree[smallest_child].priority) smallest_child = 2 * index + 2;
            
            
            for (int g = 0; g < 4; ++g) {
                int grandchild = 4 * index + 3 + g;
                if (grandchild < size && tree[grandchild].priority<tree[smallest_child].priority) smallest_child = grandchild;
            }

            if (smallest_child >= 4 * index+ 3) { //is grandchild
                if (tree[smallest_child].priority<tree[index].priority) {
                    std::swap(tree[smallest_child], tree[index]);
                    int parent_of_grandchild = (smallest_child - 1) / 2;
                    if (tree[parent_of_grandchild].priority<tree[smallest_child].priority) {
                        std::swap(tree[smallest_child], tree[parent_of_grandchild]);
                    }
                    push_down_min(smallest_child);
                }
            } 
            
            else if (tree[smallest_child].priority<tree[index].priority) {//is child
                std::swap(tree[smallest_child], tree[index]);
            }
        };
        void push_down_max(int index){
            int size = tree.size();
            if (2 * index + 1 >= size) return;

            
            int biggest_descendant = 2 * index + 1;
            if (2 * index + 2 < size && tree[biggest_descendant].priority<tree[2*index+2].priority) biggest_descendant = 2 * index + 2;
            
            for (int g = 0; g < 4; ++g) {
                int grandchild = 4 * index + 3 + g;
                if (grandchild < size && tree[biggest_descendant].priority<tree[grandchild].priority) biggest_descendant = grandchild;
            }

            if (biggest_descendant >= 4 * index+ 3) { //grandchild
                if (tree[index].priority<tree[biggest_descendant].priority) {
                    std::swap(tree[biggest_descendant], tree[index]);
                    int parent_of_grandchild = (biggest_descendant - 1) / 2;
                    if (tree[biggest_descendant].priority<tree[parent_of_grandchild].priority) { 
                        std::swap(tree[biggest_descendant], tree[parent_of_grandchild]);
                    }
                    push_down_max(biggest_descendant);
                }
            } 
            // child
            else if (tree[index].priority<tree[biggest_descendant].priority) {
                std::swap(tree[biggest_descendant], tree[index]);
            }
        };
};