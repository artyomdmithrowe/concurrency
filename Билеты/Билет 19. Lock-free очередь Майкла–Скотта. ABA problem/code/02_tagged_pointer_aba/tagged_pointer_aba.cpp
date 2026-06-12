#include <iostream>

struct Node {
  const char* name;
};

struct TaggedPointer {
  Node* ptr;
  unsigned long long version;
};

bool SameRawPointer(Node* expected, Node* actual) { return expected == actual; }

bool SameTaggedPointer(TaggedPointer expected, TaggedPointer actual) {
  return expected.ptr == actual.ptr && expected.version == actual.version;
}

int main() {
  Node a{"A"};
  Node b{"B"};

  Node* raw_expected = &a;
  Node* raw_current = &a;

  std::cout << "raw pointer ABA:\n";
  std::cout << "  expected = " << raw_expected->name << '\n';
  std::cout << "  current  = " << raw_current->name << '\n';
  std::cout << "  CAS compares only address: " << std::boolalpha
            << SameRawPointer(raw_expected, raw_current) << "\n\n";

  TaggedPointer tagged_expected{&a, 1};

  TaggedPointer after_pop_a{&b, 2};
  TaggedPointer after_push_a_again{&a, 3};

  std::cout << "tagged pointer ABA:\n";
  std::cout << "  expected = (" << tagged_expected.ptr->name << ", "
            << tagged_expected.version << ")\n";
  std::cout << "  after pop A = (" << after_pop_a.ptr->name << ", "
            << after_pop_a.version << ")\n";
  std::cout << "  current = (" << after_push_a_again.ptr->name << ", "
            << after_push_a_again.version << ")\n";
  std::cout << "  CAS compares address and version: "
            << SameTaggedPointer(tagged_expected, after_push_a_again) << '\n';
}
