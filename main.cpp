#include <iostream>
#include <memory>
#include "LinkedList.hpp"

using std::string;
using std::cout;
using std::endl;

int main() {
    cout << "LINKED LIST IMPLEMENTATION" << endl;
    cout << "------------------------\nSTRINGS\n------------------------" << endl;
    cout << "std::string linked list: ";
    LinkedList<string> ll; // stack allocated, therefore memory is automatically managed (RAII)
    std::string hello = "Hello";
    ll.add(hello); // copy semantics (lvalue)
    ll.add("World"); // move semantics (rvalue)
    ll.print();
    cout << "amount of elements: " << ll.count << endl;
    cout << "index of 'Hello' " << std::to_string(ll.find("Hello")) << endl;
    cout << "element at index 1: \"" << ll.at(1) << "\" (via subscript: \"" << ll[1] << "\")" << endl;
    cout << "concatenated string list, separated by spaces: " << ll.join(' ') << endl;


    cout << "------------------------\nINTS\n------------------------" << endl;
    cout << "int linked list: ";
    LinkedList<int> lli;
    lli.add(11);
    lli.add(12);
    lli.add(13);
    lli.print();
    try {
        const auto removed_node = lli.pop();
        cout << "Popped value " << removed_node->data << " (ptr: " << removed_node.get() << ")" << " from int list" << endl;
    } catch (const std::exception& e) {
        cout << "Pop failed!!!" << e.what() << endl;
    }
    cout << "new int list: ";
    lli.print();

    cout << "------------------------\nCHARS\n------------------------" << endl;
    cout << "char linked list: ";

    LinkedList<char> llc;
    llc.add('h');
    llc.add('i');
    llc.print();

    cout << "concatenated char list: " << llc.join() << endl;

    cout << "------------------------\nMEMORY CLEANUP\n------------------------" << endl;

    cout << "MANUAL\n------------------------" << endl;
    cout << "Manually removing nodes from char LinkedList (clear())" << endl;
    llc.clear();
    cout << "\nAUTOMATIC (RAII)\n------------------------" << endl;

    // since memory is auto-managed (RAII + we didn't call clear()), de-constructors will fire here
    // and since the defined de-constructors feature debug statements, they will also fire here.
    return 0;
}