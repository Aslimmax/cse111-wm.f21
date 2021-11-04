// $Id: sorted-list.cpp,v 1.36 2021-11-03 16:40:53-07 - - $

//
// List insertion algorithm.
// Insert nodes into a singly-linked list using only operator<
// to form comparisons.  Do not insert elements that already
// exist.
//

#include <cmath>
#include <iostream>
#include <stdexcept>
#include <string>
#include <vector>
using namespace std;

template <typename Type>
struct xless {
   bool operator() (const Type& left, const Type& right) const {
      return left < right;
   }
};

template <typename Type>
struct xgreater {
   bool operator() (const Type& left, const Type& right) const {
      return left > right;
   }
};


template <typename item_t, class less_t=xless<item_t>>
struct sorted_list {
   struct node {
      item_t item;
      node* link;
      node (const item_t& item_, node* link_):
                  item(item_),   link(link_) {
      }
   };
   less_t less;
   node* head = nullptr;

   sorted_list() = default; // Needed because default is suppressed.
   sorted_list (const sorted_list&) = delete;
   sorted_list& operator= (const sorted_list&) = delete;
   sorted_list (sorted_list&&) = delete;
   sorted_list& operator= (sorted_list&&) = delete;
   ~sorted_list();

   void insert (const item_t& newitem);
   const item_t& front() const { return head->item; }
   void pop_front();
};

template <typename item_t, class less_t>
sorted_list<item_t,less_t>::~sorted_list() {
   while (head != nullptr) pop_front();
}

template <typename item_t, class less_t>
void sorted_list<item_t,less_t>::insert (const item_t& newitem) {
   node** curr = &head;
   while (*curr != nullptr and less ((*curr)->item, newitem)) {
      curr = &(*curr)->link;
   }
   if (*curr == nullptr or less (newitem, (*curr)->item)) {
      *curr = new node (newitem, *curr);
   }
}

template <typename item_t, class less_t>
void sorted_list<item_t,less_t>::pop_front() {
   if (head == nullptr) throw underflow_error (__PRETTY_FUNCTION__);
   node* old = head;
   head = head->link;
   delete old;
}

template <typename item_t, class less_t=xless<item_t>>
void process (size_t argc, const char* const* const argv,
              const string& label) {
   sorted_list<item_t,less_t> list;
   cout << label << ": insert:" << endl;
   for (const char* const* argp = &argv[1];
        argp != &argv[argc]; ++argp) {
      cout << " " << *argp;
      list.insert (*argp);
   }
   cout << endl;
   cout << label << ": sorted:" << endl;
   for (auto itor = list.head; itor != nullptr; itor = itor->link) {
      cout << " " << itor->item;
   }
   cout << endl << endl;
}

template <typename container, class less_t=xless<item_t>>
void process (const container& data, const string& label) {
   sorted_list<double> list;
   cout << label << ": insert:" << endl;
   for (double num: data) { cout << " " << num; list.insert (num); }
   cout << endl;
   cout << label << ": sorted:" << endl;
   for (auto itor = list.head; itor != nullptr; itor = itor->link) {
      cout << " " << itor->item;
   }
   cout << endl;
   cout << endl;
}

const char* const sdata[] {
   "sortedlist", "Foo", "Bar", "Baz", "qux", "zxcvbnm", "asdfg",
   "qwerty", "Bar", "Baz", "Foo", "qwerty", "hello", "HELLO",
};
constexpr size_t sdata_size = sizeof sdata / sizeof *sdata;

const vector<double> num_data {
   1./0., 1./3., 1.024e6, acos(-1.), exp(1.), log(0.5), sqrt(2.),
};

struct name {
   string first_name;
   string last_name;
};
vector<name> names {
   {"Ada", "Lovelace"},
   {"Charles", "Babage"},
   {"Alan", "Turing"},
   {"Alonzo", "Church"},
};
ostream& operator<< (ostream& out, const name& who) {
   return out << who.first_name << " " << who.last_name;
}
struct less_name {
   bool operator() (const name& one, const name& two) {
      if (one.last_name < two.last_name) return true;
      if (two.last_name < one.last_name) return false;
      return one.first_name < two.first_name;
   }
};

int main() {
   process<const char*> (sdata_size, sdata, "char*: Char*");
   process<string> (sdata_size, sdata, "string: Default");
   process<string,xgreater<string>> (sdata_size, sdata,
               "string: Greater");
   process (num_data, "double");
// process (names); // error: 'item_t' was not declared in this scope
   process<vector<name>>,less_name> (names, "names");
   return 0;
}

/*
//TEST// alias grind='valgrind --leak-check=full --show-reachable=yes'
//TEST// grind --log-file=sortedlist.out.log >sortedlist.out 2>&1
//TEST// mkpspdf sortedlist.ps sortedlist.cpp* sortedlist.out*
*/
