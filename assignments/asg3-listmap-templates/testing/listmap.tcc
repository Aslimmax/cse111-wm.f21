// $Id: listmap.tcc,v 1.15 2019-10-30 12:44:53-07 - - $
// Cody Yiu (coyiu@ucsc.edu)
// Andrew Lim (ansclim@ucsc.edu)

#include "listmap.h"
#include "debug.h"

//
/////////////////////////////////////////////////////////////////
// Operations on listmap.
/////////////////////////////////////////////////////////////////
//

/**
 * listmap::~listmap()
 * listmap's destructor, which deletes every node in the listmap.
 * Input: none
 * Output: none
 */
template <typename key_t, typename mapped_t, class less_t>
listmap<key_t,mapped_t,less_t>::~listmap() {
   DEBUGF ('l', reinterpret_cast<const void*> (this));
   // Initialize an iterator to the beginning of the list
   iterator iter = begin();

   // Loop through the listmap and delete every node
   while (iter != end()) {
      // Set iter to a temporary iterator for deletion
      iterator tempIter = iter;

      // Move to the next node in listmap and delete tempIter's node
      // (the node that iter was previously pointing to)
      ++iter;
      delete tempIter.where;
   }
}

/**
 * iterator listmap::insert (const value_type&)
 * Insertion takes a pair as a single arguments. If the key is already 
 * in listmap, replace the value, otherwise insert a new entry into the 
 * list.
 * Input: value_type pair
 * Output: interator pointing at the inserted item
 */ 
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::insert (const value_type& pair) {
   DEBUGF ('l', &pair << "->" << pair);

   // Determine if an item with pair's key already exists
   iterator foundNode = find(pair.first);
   if (foundNode != end()) { // Found an item
      // Update the value of the node found with pair.value
      (*foundNode).second = pair.second;
   } else { // An item with pair.key does not exist, insert a new node
      // Initialize a new node to insert into listmap
      node* newNode = new node(anchor(), anchor(), pair);

      // Check if list is empty. If so, set anchor's next value to the 
      // newNode
      if (anchor()->next == anchor()) {
         anchor_.next = newNode;

         // Set anchor's prev to the newly inserted Node
         anchor_.prev = newNode;

         // Update foundNode's where_member
         foundNode.where = newNode;
      }
      else { // Otherwise, list is not empty.
         iterator listIter = begin();
         for (; listIter != end(); ++listIter) {
            // Compare the current Node's key to the newNode's key
            if (!less((*listIter).first, newNode->value.first)) {
               break;
            }
         }

         // Check if listIter has reached the end of the list
         if (listIter == end()) {
            // Get the last node in the listmap
            listIter = anchor_.prev;

            // Set the current node's next to the newNode
            listIter.where->next = newNode;
            // Set newNode's prev to the currentNode
            newNode->prev = listIter.where;
            // Update anchor's prev to newNOde
            anchor_.prev = newNode;
         } else {            
            // Update newNode's prev to the current node's prev
            newNode->prev = listIter.where->prev;     
            // Update newNode's next to the current node
            newNode->next = listIter.where;
            // Update the current node's prev next to point to 
            // the newNode
            listIter.where->prev->next = newNode;    
            // Update current node prev to newNode
            listIter.where->prev = newNode;
         }

         foundNode.where = newNode;
      }
   }

   return foundNode;
}

/**
 * listmap::find(const key_type&)
 * Searches and returns an iterator to the specified key_type. If find
 * fails to find the item, it returns the end() iterator.
 * Input: key_type that
 * Output: iterator pointing at the item in listmap or the end iterator
 */
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::find (const key_type& that) {
   DEBUGF ('l', that);

   // Store return iterator
   iterator returnIter{};
   // Iniitialize list iterator to iterate through listmap
   iterator listIter{};

   // Loop through listmap and search for item wtih key "that"
   for (listIter = begin(); listIter != end(); ++listIter) {
      // Check if the item's key is that
      if (!less((*listIter).first, that) && 
         !less(that, (*listIter).first)) {
         returnIter = listIter;
         break;
      }
   }

   // Check if item was not found
   if (listIter == end()) {
      returnIter = end();
   }

   return returnIter;
}

/**
 * listmap::erase (iterator position)
 * The item pointed at by the argument iterator is deleted from the
 * list.
 * Input: iterator pointing at Node to be deleted
 * Output: iterator that points at the position immediately following
 * that which was erased.
 */
template <typename key_t, typename mapped_t, class less_t>
typename listmap<key_t,mapped_t,less_t>::iterator
listmap<key_t,mapped_t,less_t>::erase (iterator position) {
   DEBUGF ('l', &*position);
   // Initialize iterator object to store the position immediately
   // following that which was erased (if it exists)
   iterator listIter{};

   // Search for node with key from position
   listIter = find((*position).first);

   // If node not found, return iterator to end()
   if (listIter == end()) {
      return iterator(end());
   }

   // Store prev and next of position
   node* back = (listIter.where)->prev;
   node* forward = (listIter.where)->next;

   // Reassign their prev and next
   back->next = forward;
   forward->prev = back;

   delete listIter.where;

   return iterator(forward);
}

/**
 * listmap::printMap()
 * Output all elements in the listmap to the standard output. Starts
 * from the beginning of the listmap.
 * Input: none
 * Output: none
 */
template <typename key_t, typename mapped_t, class less_t>
void listmap<key_t, mapped_t, less_t>::printMap() {
   for (iterator listIter = begin(); listIter != end(); ++listIter) {
      cout << (*listIter).first << " = " << (*listIter).second << endl;
   }
}

