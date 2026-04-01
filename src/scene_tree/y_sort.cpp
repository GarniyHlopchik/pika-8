#include "y_sort.h"

void YSort::_update(float delta) {
    std::sort(children.begin(), children.end(), 
        [](const std::unique_ptr<Node>& a, const std::unique_ptr<Node>& b) {
            // 1. Get raw pointers for casting
            Node2D* left = dynamic_cast<Node2D*>(a.get());
            Node2D* right = dynamic_cast<Node2D*>(b.get());

            // 2. Both are Node2D: Sort by Y
            if (left && right) {
                Vector2 pos_left = left->get_global_position();
                Vector2 pos_right = right->get_global_position();
                return pos_left.y < pos_right.y;
            }

            // 3. Only 'a' is Node2D: 'a' comes first
            if (left) return true;

            // 4. Only 'b' is Node2D: 'b' comes first (so 'a' is false)
            if (right) return false;

            // 5. Neither is Node2D: Keep original relative order or sort by pointer
            return a.get() < b.get();
        }
    );

    unsigned int order = 0;
    for (auto& child : children) {
        child->set_sort_index(order++);
    }

    // Continue the update chain
    Node2D::_update(delta);
}