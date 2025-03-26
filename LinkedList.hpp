//
// Created by michael on 24/03/25.
//

#ifndef LINKEDLIST_H
#define LINKEDLIST_H
#include <memory>
#include <sstream>


// I opted for a template version which keeps the types homogenous,
// because using void* to vary types in a single list adds a lot of runtime complexity.
// This should be a lot faster, and should keep it simple when making the visualiser.
template<typename T>
struct TypeName {
    static std::string get() { return typeid(T).name(); } // Fallback
};

template<>
struct TypeName<std::string> {
    static std::string get() { return "std::string"; }
};

template<>
struct TypeName<char> {
    static std::string get() { return "char"; }
};

template<>
struct TypeName<int> {
    static std::string get() { return "int"; }
};


template<class T>
class LinkedList {
    // private:
    struct node {
        T data;
        std::unique_ptr<node> next;
        node* prev;

        explicit node(T&& data) : data(std::move(data)), next(nullptr), prev(nullptr) {} // move
        explicit node(const T& data) : data(data), next(nullptr), prev(nullptr) {} // copy

        ~node() {
            // tab indentation is to make LinkedList clearing more clear & segmented
            std::cout << "\t* node freed (value: ";
            if (TypeName<T>::get() == "std::string") {
                std::cout << "\"" << this->data << "\"";

            } else if (TypeName<T>::get() == "char") {
                std::cout << "\'" << this->data << "\'";

            } else {
                std::cout << this->data;
            }
            std::cout << ") (address: " << this << ")" << std::endl;
        }
    };


    std::unique_ptr<node> first;
    node* last;

    public:
    int count = 0;

    LinkedList() : first(nullptr), last(nullptr) {};

    ~LinkedList() {

        std::cout << "LinkedList of type " << TypeName<T>::get() << " being freed" << std::endl;
    }

    // convenience methods
    void print() {
        std::cout << this->to_format_string() << std::endl;
    }

    std::string to_string(const bool include_format) {
        if (include_format) {
            return this->to_format_string();
        }
        return this->join();
    }

    // methods left:
    // reverse

    /// 
    /// @param index The index to look for data at
    /// @return The data contained at @code index@endcode, if possible.
    /// Throws an out of range exception if the index is over @code this.count@endcode.
    T at(const int& index) {
        int idx = 0;
        if (index >= this->count) {
            throw std::out_of_range("Index out of range -> LinkedList.at(idx)");
        }
        node* current = this->first.get();
        while (current != nullptr) {
            if (idx == index) {
                return current->data;
            }
            current = current->next.get();
            idx++;
        }
        return nullptr;
    }
    T operator[](const int& index) {
        return this->at(index);
    }


    ///
    /// @param val The value to look for in the list
    /// @return The index of the first instance of @code val@endcode found. Returns @code -1@endcode if not found.
    int find(T val) {
        int idx = 0;
        node* current = this->first.get();
        while (current != nullptr) {
            if (current->data == val) {
                return idx;
            }
            current = current->next.get();
            idx++;
        }
        return -1;
    }


    void clear() {
        if (!this->first) {
            std::cout << "List is already empty!" << std::endl;
            return;
        }
        this->first.reset();
        this->last = nullptr;
        this->count = 0;
    }


    /// 
    /// @return A string consisting of all linked list elements concatenated.
    std::string join() {
        if (this->first.get() == nullptr) {
            return "";
        }
        node* current = this->first.get();
        std::string joined;
        while (current != nullptr) {
            joined += current->data;
            current = current->next.get();
        }
        return joined;
    }

    /// 
    /// @param c Value to add between elements
    /// @return A string consisting of all linked list elements concatenated, with @code c@endcode separating them.
    std::string join(const char c) {
        if (this->first.get() == nullptr) {
            return "";
        }
        node* current = this->first.get();
        std::string joined;
        while (current != nullptr) {
            joined += current->data;
            joined += c;
            current = current->next.get();
        }
        return joined;
    }

    ///
    /// @param data The data to add to the list
    void add(T&& data) {
        std::unique_ptr<node> new_node(new node(std::forward<T>(data)));
        if (this->last == nullptr) { // empty list
            this->first = std::move(new_node);
            this->last = this->first.get();
            return;
        }
        // goal:
        // last -> penultimate node
        // new_node -> last

        // set new node's tail
        new_node->prev = this->last;
        // move our new node into the 'next' position of the last
        this->last->next = std::move(new_node);

        // push the 'last' status down the line
        this->last = this->last->next.get();
        ++this->count;
    }

    void add(const T& data) {
        std::unique_ptr<node> new_node(new node(data));
        if (this->last == nullptr) { // empty list
            this->first = std::move(new_node);
            this->last = this->first.get();
            this->count = 1;
            return;
        }
        // goal:
        // last -> penultimate node
        // new_node -> last

        // set new node's tail
        new_node->prev = this->last;
        // move our new node into the 'next' position of the last
        this->last->next = std::move(new_node);

        // push the 'last' status down the line
        this->last = this->last->next.get();
        ++this->count;
    }

    /// 
    /// @return The last element of the list, if possible.
    std::unique_ptr<node> pop() {
        if (!this->last) {
            throw std::runtime_error("Attempted pop from empty array!");
        }
        std::unique_ptr<node> result;
        if (this->last->prev) { // >1 node
            node* new_last = this->last->prev; // temp variable
            result = std::move(new_last->next); // detach last node

            new_last->next.reset(); // cleanup new last
            result->prev = nullptr; // clean up result

            this->last = new_last; // update last node
        } else { // 1 node only
            result = std::move(this->first);
            this->last = nullptr;
        }
        --this->count;
        return result;
    }

    private:

    std::string to_format_string() {
        std::string formatted_string;
        std::ostringstream oss; // to keep polymorphic types flowing into string easily
        if (this->first.get() == nullptr) {
            oss << "[]";
            formatted_string = oss.str();
            return formatted_string;
        }
        node* current = this->first.get();
        oss << "[";
        // this one is done differently, not (while current != nullptr)
        // because we need to know if this is the last iteration or not,
        // and end early if it is (so we don't add an extra comma & space)
        while (true) {
            bool isString = false;
            bool isChar = false;
            if (typeid(T) == typeid(std::string)) { // compare to string
                isString = true;
                oss << "\"";

            } else if (typeid(T) == typeid('c')) { // compare to char
                isChar = true;
                oss << "\'";
            }
            oss << current->data;

            if (isString) {
                oss << "\"";
            } else if (isChar) {
                oss << "\'";
            }

            if (current->next.get() == nullptr) { // close off the list
                oss << "]";
                break;
            }
            oss << ", "; // separate elements
            current = current->next.get(); // keep it moving up the list
        }
        formatted_string = oss.str(); // bake string
        return formatted_string;
    }
};


#endif //LINKEDLIST_H
