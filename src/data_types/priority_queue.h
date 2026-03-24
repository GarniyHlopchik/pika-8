#include <vector>
#include <algorithm>
#include <cmath>
template <typename TypeOfData>
enum class QueueProtocol {HIGHEST_PRIORITY, LOWEST_PRIORITY, OLDEST, NEWEST}
struct QueueNode{
    int priority;
    int order;
    TypeOfData data;
    bool operator==(const QueneNode& other) const {
        return (order == other.order);
    }
};
class PriorityQueue{
    public:
        PriorityQueue() = default;
        ~PriorityQueue() = default;
        void enqueue(TypeOfData data, int priority);
        QueueNode dequeue(QueueProtocol protocol);
        QueneNode peek(QueueProtocol protocol);
    private:
        std::vector<QueueNode> tree;
        int get_id();
        void bubble_up(int index);
        void bubble_up_max(int index);
        void bubble_up_min(int index);
        void remove_and_correct(int index);
        void push_down_min(int index);
        void push_down_max(int index);
};